#include "statetycho.h"

#define STREAM_CHUNK_SIZE (4096 / 2)
#define SAMPLE_RATE 16000
#define PREBUFFER_MIN_FRAMES 4096 // Pre-buffer ~0.5 seconds before starting playback
#define VAD_THRESHOLD 0.4f
#define SILENCE_TIMEOUT_SEC 2.0f

#define GAINNUMFRAMES 512

typedef struct {
    WEBSOCKETCLIENTCONFIG wsc;
    WEBSOCKETCLIENT *ws;
    AudioStream audio;
    ma_device mic;
    short *storage;
    short *micstorage;
    RINGBUFFER *audiorb;
    RINGBUFFER *micrb;
    BOOL isprebuffered;
    BOOL isspeaking;
    BOOL once;
    float silenceduration;
    VOWEL vowel;
    VOWEL lastvowel;
    float time;
    float goal;
    BOOL isassistanttalking;
    int outframecount;
    int inframecount;
    double outmax[GAINNUMFRAMES];
    double inmax[GAINNUMFRAMES];
    SUBTITLES *subtitles;
    BOOL micpermissiongranted;
    BOOL micinitialized;
} STATETYCHO;

static STATETYCHO *v = NULL;

#ifdef __ANDROID__
#include <jni.h>

static JavaVM *g_vm = NULL;
static jobject g_activity = NULL;

// Forward declaration so STATETYCHO_ON_MIC_PERMISSION_RESULT can call it
static CRESULT STATETYCHO_INIT_MIC(void);

void STATETYCHO_SET_ANDROID_ACTIVITY(JNIEnv *env, jobject activity)
{
    if (!env) return;
    (*env)->GetJavaVM(env, &g_vm);
    if (g_activity)
    {
        (*env)->DeleteGlobalRef(env, g_activity);
        g_activity = NULL;
    }
    if (activity)
    {
        g_activity = (*env)->NewGlobalRef(env, activity);
    }
}

static BOOL statetycho_get_jni_env(JNIEnv **out_env, BOOL *did_attach)
{
    *did_attach = FALSE;
    if (!g_vm || !out_env) return FALSE;

    jint status = (*g_vm)->GetEnv(g_vm, (void **)out_env, JNI_VERSION_1_6);
    if (status == JNI_OK)
    {
        return TRUE;
    }
    if (status == JNI_EDETACHED)
    {
        if ((*g_vm)->AttachCurrentThread(g_vm, out_env, NULL) == JNI_OK)
        {
            *did_attach = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

static void ANDROID_REQUEST_MIC_PERMISSION(void)
{
    JNIEnv *env = NULL;
    BOOL did_attach = FALSE;
    if (!statetycho_get_jni_env(&env, &did_attach) || !g_activity) return;

    jclass cls = (*env)->GetObjectClass(env, g_activity);
    if (cls)
    {
        jmethodID mid = (*env)->GetMethodID(env, cls, "requestMicPermission", "()V");
        if (mid)
        {
            (*env)->CallVoidMethod(env, g_activity, mid);
        }
        (*env)->DeleteLocalRef(env, cls);
    }

    if (did_attach && g_vm)
    {
        (*g_vm)->DetachCurrentThread(g_vm);
    }
}

void STATETYCHO_ON_MIC_PERMISSION_RESULT(BOOL granted)
{
    if (!v) return;
    v->micpermissiongranted = granted;
    if (granted)
    {
        STATETYCHO_INIT_MIC();
    }
}

JNIEXPORT void JNICALL
Java_com_yvonta_personaclient_MainActivity_nativeSetActivity(JNIEnv *env, jobject thiz)
{
    STATETYCHO_SET_ANDROID_ACTIVITY(env, thiz);
}

JNIEXPORT void JNICALL
Java_com_yvonta_personaclient_MainActivity_nativeOnMicPermissionResult(JNIEnv *env, jobject thiz, jboolean granted)
{
    (void)env;
    (void)thiz;
    STATETYCHO_ON_MIC_PERMISSION_RESULT(granted ? TRUE : FALSE);
}
#endif

void STATETYCHO_MICCALLBACK(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
  STATETYCHO *state = (STATETYCHO *)pDevice->pUserData;
  if (!state || !pInput) return;
  
  if(v->isassistanttalking)
  {
     return;
  }

  const short *inputsamples = (const short *)pInput;
  
  float sum = 0.0f;
  for (int i = 0; i < frameCount; i++)
  {
      float sample = (float)inputsamples[i] / 32768.0f;
      sum += sample * sample;
  }
  float rms = sqrtf(sum / frameCount);

  float chunkduration = (float)frameCount / 16000.0f;

  if (rms >= VAD_THRESHOLD)
  {
     v->isspeaking = TRUE;
     v->silenceduration = 0.0f;
     v->once = TRUE;
  } 
  else
  {
    v->silenceduration += chunkduration;    
    if (v->silenceduration > SILENCE_TIMEOUT_SEC)
    {
      v->isspeaking = FALSE;
      v->silenceduration = 0.0f;
      if(v->once)
      {
        v->once = FALSE;
        WEBSOCKETCLIENT_SENDEND(v->ws);
      }
    }
  }
  
  if(v->isspeaking && RINGBUFFER_AVAILABLEWRITE(state->micrb) >= (int)frameCount)
  {
    short samples[STREAM_CHUNK_SIZE];
    int len = frameCount;
    if(len > STREAM_CHUNK_SIZE)
    {
      return;
    }
    memcpy(samples, inputsamples, len);
    AUDIOGAIN_FILTER(samples, len, v->inframecount++, v->inmax, GAINNUMFRAMES);

    RINGBUFFER_WRITE(state->micrb, samples, len);
  }
}

// 1. Dedicated Audio Callback running on Raylib's internal audio thread (Rock-solid timing)
void STATETYCHO_AUDIOCALLBACK(void *buffer, unsigned int frames)
{
    short *out = (short *)buffer;

    if (!v || !v->audiorb) 
    {
        memset(buffer, 0, frames * sizeof(short));
        return;
    }

    // Check if we have enough pre-buffered data to start/continue playing
    if (!v->isprebuffered)
    {
        if (RINGBUFFER_AVAILABLEREAD(v->audiorb) >= PREBUFFER_MIN_FRAMES)
        {
            v->isprebuffered = TRUE;
        }
        else
        {
            // Still waiting to fill buffer: output silence
            memset(buffer, 0, frames * sizeof(short));
            v->vowel = VOWEL_NONE;
            v->isassistanttalking = FALSE;
            return;
        }
    }

    int available = RINGBUFFER_AVAILABLEREAD(v->audiorb);
    v->isassistanttalking = (available > 0);

    if (available >= frames)
    {
        // Read the exact number of frames Raylib's audio thread is asking for
        RINGBUFFER_READ(v->audiorb, out, frames);
        v->vowel = VOWELDETECTOR_ANALIZEFROMPCM((const short *)out, (int)frames, SAMPLE_RATE);
        AUDIOGAIN_FILTER(out, frames, v->outframecount++, v->outmax, GAINNUMFRAMES);
    }
    else
    {
        // Buffer Underrun: We ran out of data from the network
        // Zero out the buffer to prevent static/garbage noise, and trigger re-buffering
        memset(buffer, 0, frames * sizeof(short));
        if (available > 0)
        {
            RINGBUFFER_READ(v->audiorb, out, available);
            v->vowel = VOWELDETECTOR_ANALIZEFROMPCM((const short *)out, available, SAMPLE_RATE);
            AUDIOGAIN_FILTER(out, available, v->outframecount++, v->outmax, GAINNUMFRAMES);
        }
        v->isprebuffered = FALSE; 
    }
}

static CRESULT STATETYCHO_INIT_MIC(void)
{
    if (!v || v->micinitialized)
    {
        return CSUCCESS;
    }

    int samplesnum = 16000 * 120;
    v->micstorage = malloc(samplesnum * sizeof(short));
    if (!v->micstorage)
    {
        return CFAILED;
    }
    v->micrb = RINGBUFFER_CREATE(v->micstorage, samplesnum, 1);

    ma_device_config micconfig = ma_device_config_init(ma_device_type_capture);
    micconfig.capture.format   = ma_format_s16;
    micconfig.capture.channels = 1;
    micconfig.sampleRate       = 16000;
    micconfig.dataCallback     = STATETYCHO_MICCALLBACK;
    micconfig.pUserData        = v;

    ma_result res = ma_device_init(NULL, &micconfig, &v->mic);
    if (res != MA_SUCCESS)
    {
        E("[Audio Error]: ma_device_init failed (%d)", res);
        return CFAILED;
    }

    res = ma_device_start(&v->mic);
    if (res != MA_SUCCESS)
    {
        E("[Audio Error]: ma_device_start failed (%d)", res);
        ma_device_uninit(&v->mic);
        return CFAILED;
    }

    v->micinitialized = TRUE;
    return CSUCCESS;
}


void STATETYCHO_ONRECEIVE(struct lws *wsi, BUSPACKAGE *bp)
{
    if(!wsi || !bp) 
    {
        return;
    }
    
    if (bp->type == 1) 
    { 
        E("[Text Received]: %.*s", (int)bp->payloadlen, bp->payload);
        bp->payload[bp->payloadlen] = '\0';
        SUBTITLES_PUSH(v->subtitles, bp->payload); 
    } 
    else if (bp->type == 2) 
    { 
        int incomingsamples = bp->payloadlen / sizeof(short);
        
        // Push incoming websocket audio packets straight into the ring buffer
        if(RINGBUFFER_AVAILABLEWRITE(v->audiorb) >= incomingsamples) 
        {
            RINGBUFFER_WRITE(v->audiorb, (const short *)bp->payload, incomingsamples);
        }
        else
        {
            E("[Audio Warning]: Ring buffer full, dropping %d samples!", incomingsamples);
        }
    }
}

CRESULT STATETYCHO_START(int step)
{
    switch(step)
    {
        case 0:
        {
            v = malloc(sizeof(STATETYCHO));
            if(!v)
            {
                return CFAILED;
            }
            memset(v, 0, sizeof(STATETYCHO));
        } break;
        case 10:
        { 
            v->wsc.host = "192.168.1.196";
            v->wsc.port = 8079;
            v->wsc.path = "";
            v->wsc.use_ssl = 0; 
            v->wsc.user_data = v;
            v->wsc.onreceivefun = STATETYCHO_ONRECEIVE;
            v->ws = WEBSOCKETCLIENT_CREATE(&v->wsc);
            if(!v->ws)
            {
                return CFAILED;
            }
            
            WEBSOCKETCLIENT_SERVICE(v->ws, -1);
        } break; 
        case 20:
        {
            v->isprebuffered = FALSE;
            int samplesnum = 16000 * 120; // 30 min capacity
            v->storage = malloc(samplesnum * sizeof(short));
            if(!v->storage)
            {
                return CFAILED;
            }
            v->audiorb = RINGBUFFER_CREATE(v->storage, samplesnum, 1);
            
            v->audio = LoadAudioStream(SAMPLE_RATE, 16, 1);
                        
            // Bind the hardware-timed audio callback
            SetAudioStreamCallback(v->audio, STATETYCHO_AUDIOCALLBACK);
            
            PlayAudioStream(v->audio);
        } break;
        case 30:
        { 
#ifdef __ANDROID__
            if (v->micpermissiongranted)
            {
                return STATETYCHO_INIT_MIC();
            }
            else
            {
                ANDROID_REQUEST_MIC_PERMISSION();
            }
#else
            return STATETYCHO_INIT_MIC();
#endif
        } break;
        case 40:
        {
           v->subtitles = SUBTITLES_CREATE();
        }
    }
    return CSUCCESS;
}

CRESULT STATETYCHO_FINISH(int step)
{
    switch(step)
    {
        case 0:
        {
            free(v);
        } break;
        case 10:
        {
            WEBSOCKETCLIENT_DESTROY(v->ws);
            v->ws = NULL;
        } break;
        case 20:
        {
            // Note: RemoveAudioStreamCallback isn't strictly required before unloading in Raylib, 
            // but unloading safely tears down the stream.
            UnloadAudioStream(v->audio);
            RINGBUFFER_DESTROY(v->audiorb);
            v->audiorb = NULL;
            free(v->storage);
        } break;
        case 30:
        {
          ma_device_uninit(&v->mic);    
          RINGBUFFER_DESTROY(v->micrb);
          v->micrb = NULL;
          free(v->micstorage);
        } break;
        case 40:
        {
          SUBTITLES_DESTROY(v->subtitles);
          v->subtitles = NULL;
        }
    }
    return CSUCCESS;
}

BOOL STATETYCHO_RENDER(float dt)
{
    WEBSOCKETCLIENT_SERVICE(v->ws, -1);

    v->time += dt;
    CLONESANTA_DRAWBACKGROUND(GetScreenWidth(), GetScreenHeight(), v->time);
    CLONESANTA_DRAWCLONE((Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f + 20 }, 4.0f, v->lastvowel);
    if(v->lastvowel != v->vowel && v->time >= v->goal)
    {
        v->lastvowel = v->vowel;
        v->goal = v->time + 0.05f;
    }    
    SUBTITLES_UPDATE(v->subtitles, dt);
    SUBTITLES_DRAW(v->subtitles, GetScreenWidth(), GetScreenHeight());
    
    if(IsKeyPressed(KEY_SPACE))
    {
        WEBSOCKETCLIENT_SENDCHAT(v->ws, "What is the capital of France?");
    }
    
    int micavailable = RINGBUFFER_AVAILABLEREAD(v->micrb);
    if(micavailable >= STREAM_CHUNK_SIZE)
    {
        short sendbuffer[STREAM_CHUNK_SIZE];
        memset(sendbuffer, 0, sizeof(sendbuffer));
    
        if(micavailable > STREAM_CHUNK_SIZE)
        {
           micavailable = STREAM_CHUNK_SIZE;
        }
        RINGBUFFER_READ(v->micrb, sendbuffer, micavailable); 
        WEBSOCKETCLIENT_SENDPCM(v->ws, (char*)sendbuffer, micavailable * sizeof(short));
    }
  
    return TRUE;
}
