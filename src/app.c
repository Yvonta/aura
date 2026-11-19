#include "app.h"

#if defined(PLATFORM_ANDROID)
    #include <android/native_activity.h>
    #include <android_native_app_glue.h>
    #include <android/window.h>
    #include <jni.h>
#endif

#if defined(PLATFORM_ANDROID)
#include <sys/system_properties.h>
#include <stdlib.h>

// Deze functie moet BOVEN StartAudioService staan
int GetAndroidSdkVersion(void) {
    char sdk_ver_str[PROP_VALUE_MAX];
    int len = __system_property_get("ro.build.version.sdk", sdk_ver_str);
    if (len <= 0) return 0;
    return atoi(sdk_ver_str);
}
#endif


#if defined(PLATFORM_ANDROID)
#include <android/native_activity.h>
#include <android_native_app_glue.h>
#include <jni.h>
#include <android/log.h>

#define LOG_TAG "DonkeyJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void StartAudioService(struct android_app* app) {
    if (!app || !app->activity) return;

    __android_log_write(ANDROID_LOG_INFO, "DonkeyJNI", "StartAudioService start...");

    ANativeActivity* activity = app->activity;
    JavaVM* jvm = activity->vm;
    JNIEnv* env = NULL;

    // Attach thread
    (*jvm)->AttachCurrentThread(jvm, &env, NULL);

    // 1. Get the Activity class and find getClassLoader
    jclass activityClass = (*env)->GetObjectClass(env, activity->clazz);
    jmethodID getClassLoader = (*env)->GetMethodID(env, activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    
    // 2. Call activity.getClassLoader()
    jobject classLoader = (*env)->CallObjectMethod(env, activity->clazz, getClassLoader);
    
    // 3. Find the loadClass method in the ClassLoader
    jclass classLoaderClass = (*env)->FindClass(env, "java/lang/ClassLoader");
    jmethodID loadClass = (*env)->GetMethodID(env, classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    // 4. Use the ClassLoader to find your specific class
    jstring className = (*env)->NewStringUTF(env, "com.yvonta.donkeyaicloning.AudioService");
    jclass audioServiceClass = (jclass)(*env)->CallObjectMethod(env, classLoader, loadClass, className);

    // CHECK FOR EXCEPTION IMMEDIATELY
    if ((*env)->ExceptionCheck(env)) {
        __android_log_write(ANDROID_LOG_ERROR, "DonkeyJNI", "AudioService class NOT FOUND via ClassLoader!");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        // Do not proceed!
    } else if (audioServiceClass != NULL) {
        // 5. Look for the STATIC method "start"
        // Note: Ensure the Java method is: public static void start(Activity activity)
        jmethodID startMethod = (*env)->GetStaticMethodID(env, audioServiceClass, "start", "(Landroid/app/Activity;)V");

        if (startMethod == NULL || (*env)->ExceptionCheck(env)) {
            __android_log_write(ANDROID_LOG_ERROR, "DonkeyJNI", "Static method 'start' not found!");
            (*env)->ExceptionClear(env);
        } else {
            // 6. Call the method
            (*env)->CallStaticVoidMethod(env, audioServiceClass, startMethod, activity->clazz);
            __android_log_write(ANDROID_LOG_INFO, "DonkeyJNI", "AudioService started successfully!");
        }
    }

    // Cleanup
    (*env)->DeleteLocalRef(env, className);
    (*env)->DeleteLocalRef(env, activityClass);
    // Don't detach if this thread is managed by native_app_glue and needs to stay attached
}
#endif

#if defined(PLATFORM_ANDROID)
struct android_app; // Forward declaration
struct android_app *GetAndroidApp(void);


void RequestAndroidPermissions(struct android_app* app) {
    // Temporarily disabled to avoid runtime permission UI stealing focus
    // and leaving the app on a black screen behind it. We rely on the
    // manifest-declared RECORD_AUDIO permission instead.
    (void)app;
}

#endif

#include <time.h>
float stime = 0;

void ANIMATIONSTART(int step, float dt)
{
  stime += dt;
  CLONESANTA_DRAWBACKGROUND(GetScreenWidth(), GetScreenHeight(), stime);
}

void ANIMATIONFINISH(int step, float dt)
{
  ANIMATIONSTART(step, dt);
}



/*
void ANIMATIONSTART(int step, float dt)
{
  int u = (100-step)*0xff/100;
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){u, u, u, 0xff});
  int ms =  8000 - (int)dt;
  //usleep(ms);
}

void ANIMATIONFINISH(int step, float dt)
{
  for(int y = 0; y < GetScreenHeight(); y+=8) for(int x = 0; x < GetScreenWidth(); x+=8)
  {
    int u = rand()%255;
    DrawRectangle(x, y, 8, 8, (Color){u, u, u, 0xff});
  }
}*/

/*static float ClampStep01(int step)
{
    if (step < 0) return 0.0f;
    if (step > 100) return 1.0f;
    return (float)step / 100.0f;
}

void ANIMATIONSTART(int step, float dt)
{
    float t = ClampStep01(step);
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Festive night sky (0) blending toward warm holiday crimson/gold glow (100)
    unsigned char r = (unsigned char)(12 + t * 145);
    unsigned char g = (unsigned char)(24 + t * 18);
    unsigned char b = (unsigned char)(20 + t * 12);
    DrawRectangle(0, 0, w, h, (Color){ r, g, b, 255 });

    // Twinkling garland lights & golden aura scaled by progress
    int lightCount = 12;
    float spacing = (float)w / (float)(lightCount + 1);
    float wireY = (float)h * 0.14f;

    DrawLineEx((Vector2){ 0.0f, wireY }, (Vector2){ (float)w, wireY }, 3.0f, (Color){ 34, 94, 48, 255 });

    for (int i = 1; i <= lightCount; i++)
    {
        float lx = spacing * (float)i;
        float sway = sinf((float)GetTime() * 3.0f + (float)i * 0.9f + dt * 0.001f) * 6.0f * t;
        float ly = wireY + 12.0f + sway;
        float radius = (6.0f + 5.0f * t) * (0.8f + 0.2f * sinf((float)GetTime() * 6.0f + (float)i));

        Color bulbColor = (i % 3 == 0) ? (Color){ 255, 215, 0, 255 } :
                          (i % 2 == 0) ? (Color){ 220, 38, 38, 255 } :
                                         (Color){ 34, 197, 94, 255 };

        DrawCircle((int)lx, (int)ly, radius + 4.0f * t, Fade(bulbColor, 0.25f * t));
        DrawCircle((int)lx, (int)ly, radius, bulbColor);
    }

    // Central holiday star/tree emblem growing with step
    int cx = w / 2;
    int cy = h / 2;
    int treeSize = (int)(fminf((float)w, (float)h) * 0.22f * t);
    if (treeSize > 8)
    {
        // Tree tiers using raylib triangles
        DrawTriangle(
            (Vector2){ (float)cx, (float)(cy - treeSize) },
            (Vector2){ (float)(cx - treeSize), (float)(cy + treeSize / 2) },
            (Vector2){ (float)(cx + treeSize), (float)(cy + treeSize / 2) },
            (Color){ 21, 128, 61, 255 }
        );
        DrawTriangle(
            (Vector2){ (float)cx, (float)(cy - treeSize / 2) },
            (Vector2){ (float)(cx - treeSize * 4 / 5), (float)(cy + treeSize) },
            (Vector2){ (float)(cx + treeSize * 4 / 5), (float)(cy + treeSize) },
            (Color){ 22, 163, 74, 255 }
        );

        // Trunk & star topper
        DrawRectangle(cx - treeSize / 8, cy + treeSize, treeSize / 4, treeSize / 4, (Color){ 120, 53, 15, 255 });
        DrawCircle(cx, cy - treeSize - 6, 6.0f + 6.0f * t, (Color){ 250, 204, 21, 255 });
    }
}

void ANIMATIONFINISH(int step, float dt)
{
    float t = ClampStep01(step);
    if (t <= 0.0f) return;

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    int cell = 16;

    Color palette[] = {
        (Color){ 185, 28, 28, 255 },   // Holly red
        (Color){ 21, 128, 61, 255 },   // Pine green
        (Color){ 234, 179, 8, 255 },   // Gold tinsel
        (Color){ 248, 250, 252, 255 }   // Fresh snow
    };

    for (int y = 0; y < h; y += cell)
    {
        for (int x = 0; x < w; x += cell)
        {
            // Deterministic holiday tile pattern with subtle shimmer
            int hash = (x / cell) * 37 + (y / cell) * 17 + (int)(dt * 0.05f) + (rand() % 4);
            Color base = palette[abs(hash) % 4];
            unsigned char alpha = (unsigned char)(255.0f * t);
            Color tileColor = Fade(base, (float)alpha / 255.0f);

            DrawRectangle(x, y, cell, cell, tileColor);

            // Diagonal candy-cane / ornament accent inside larger cells
            if ((abs(hash) % 5) == 0 && t > 0.25f)
            {
                DrawCircle(x + cell / 2, y + cell / 2, (float)(cell / 4) * t, Fade((Color){ 255, 255, 255, 255 }, t));
            }
            else if ((abs(hash) % 7) == 0 && t > 0.4f)
            {
                DrawLine(x, y + cell, x + cell, y, Fade((Color){ 220, 38, 38, 255 }, t));
            }
        }
    }
}
*/

/*
static float ClampStep01(int step)
{
    if (step < 0) return 0.0f;
    if (step > 100) return 1.0f;
    return (float)step / 100.0f;
}

static void DrawGiftBox(int x, int y, int size, Color boxColor, Color ribbonColor, float alpha)
{
    if (size < 6 || alpha <= 0.0f) return;

    int half = size / 2;
    int left = x - half;
    int top = y - size;

    // Box body & lid
    DrawRectangle(left, top + 4, size, size - 4, Fade(boxColor, alpha));
    DrawRectangle(left - 2, top, size + 4, 6, Fade(boxColor, alpha));

    // Vertical & horizontal ribbons
    int ribW = (size / 6 < 2) ? 2 : size / 6;
    DrawRectangle(x - ribW / 2, top, ribW, size, Fade(ribbonColor, alpha));
    DrawRectangle(left - 2, top + size / 2, size + 4, ribW, Fade(ribbonColor, alpha));

    // Bow loops
    DrawCircle(x - ribW, top - 2, (float)ribW, Fade(ribbonColor, alpha));
    DrawCircle(x + ribW, top - 2, (float)ribW, Fade(ribbonColor, alpha));
}

static void DrawSparkleStar(float cx, float cy, float outerR, Color color)
{
    float innerR = outerR * 0.35f;
    DrawLineEx((Vector2){ cx - outerR, cy }, (Vector2){ cx + outerR, cy }, 2.0f, color);
    DrawLineEx((Vector2){ cx, cy - outerR }, (Vector2){ cx, cy + outerR }, 2.0f, color);
    DrawLineEx((Vector2){ cx - innerR, cy - innerR }, (Vector2){ cx + innerR, cy + innerR }, 1.5f, color);
    DrawLineEx((Vector2){ cx + innerR, cy - innerR }, (Vector2){ cx - innerR, cy + innerR }, 1.5f, color);
    DrawCircle((int)cx, (int)cy, outerR * 0.25f, color);
}

void ANIMATIONSTART(int step, float dt)
{
    float t = ClampStep01(step);
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float timeNow = (float)GetTime() + dt * 0.0005f;

    // Deep winter midnight shifting toward warm cozy hearth/pine ambiance
    unsigned char r = (unsigned char)(10 + t * 55);
    unsigned char g = (unsigned char)(18 + t * 42);
    unsigned char b = (unsigned char)(38 + t * 18);
    DrawRectangle(0, 0, w, h, (Color){ r, g, b, 255 });

    // Soft snow ground along the bottom
    float snowHeight = (float)h * (0.08f + 0.10f * t);
    DrawRectangle(0, (int)((float)h - snowHeight), w, (int)snowHeight + 2, Fade((Color){ 240, 248, 255, 255 }, 0.35f + 0.65f * t));

    // Falling snowflakes across the scene
    int flakeCount = 45;
    for (int i = 0; i < flakeCount; i++)
    {
        float seed = (float)(i * 97 + 13);
        float speed = 28.0f + (float)(i % 5) * 14.0f;
        float fx = fmodf(seed * 19.7f + sinf(timeNow * 1.4f + seed) * (18.0f + 22.0f * t) + (float)w, (float)w);
        float fy = fmodf(timeNow * speed + seed * 31.3f, (float)h);
        float fRadius = (1.8f + (float)(i % 3) * 1.3f) * (0.4f + 0.6f * t);

        DrawCircle((int)fx, (int)fy, fRadius, Fade((Color){ 255, 255, 255, 255 }, (0.35f + 0.55f * (float)(i % 3) * 0.5f) * (0.25f + 0.75f * t)));
    }

    // String of Christmas lights along the top
    int lightCount = 14;
    float spacing = (float)w / (float)(lightCount + 1);
    float wireY = (float)h * 0.10f;
    DrawLineEx((Vector2){ 0.0f, wireY }, (Vector2){ (float)w, wireY }, 3.0f, Fade((Color){ 34, 120, 58, 255 }, 0.3f + 0.7f * t));

    for (int i = 1; i <= lightCount; i++)
    {
        float lx = spacing * (float)i;
        float sway = sinf(timeNow * 2.6f + (float)i * 0.85f) * (3.0f + 5.0f * t);
        float ly = wireY + 12.0f + sway;
        float pulse = 0.75f + 0.25f * sinf(timeNow * 5.5f + (float)i * 1.3f);
        float rad = (5.0f + 4.0f * t) * pulse;

        Color bulb = (i % 4 == 0) ? (Color){ 250, 204, 21, 255 } :
                     (i % 3 == 0) ? (Color){ 56, 189, 248, 255 } :
                     (i % 2 == 0) ? (Color){ 239, 68, 68, 255 } :
                                    (Color){ 34, 197, 94, 255 };

        DrawLine((int)lx, (int)wireY, (int)lx, (int)ly, Fade((Color){ 80, 80, 80, 255 }, 0.4f + 0.6f * t));
        DrawCircle((int)lx, (int)ly, rad * 2.2f, Fade(bulb, 0.22f * t));
        DrawCircle((int)lx, (int)ly, rad, Fade(bulb, 0.2f + 0.8f * t));
    }

    // Centerpiece Christmas tree + presents
    float cx = (float)w * 0.5f;
    float baseY = (float)h - snowHeight + 6.0f;
    float scale = (0.25f + 0.75f * t) * (fminf((float)w, (float)h) / 520.0f);

    if (scale > 0.12f)
    {
        float trunkW = 28.0f * scale;
        float trunkH = 36.0f * scale;
        DrawRectangle((int)(cx - trunkW * 0.5f), (int)(baseY - trunkH), (int)trunkW, (int)trunkH, Fade((Color){ 101, 50, 20, 255 }, t));

        // Three lush pine bough tiers
        float tierBaseY[3] = { baseY - trunkH + 6.0f * scale, baseY - trunkH - 42.0f * scale, baseY - trunkH - 88.0f * scale };
        float tierHalfW[3] = { 105.0f * scale, 80.0f * scale, 54.0f * scale };
        float tierH[3]     = { 72.0f * scale, 64.0f * scale, 58.0f * scale };
        Color tierCol[3]   = { (Color){ 20, 110, 52, 255 }, (Color){ 22, 134, 62, 255 }, (Color){ 26, 158, 74, 255 } };

        for (int i = 0; i < 3; i++)
        {
            DrawTriangle(
                (Vector2){ cx, tierBaseY[i] - tierH[i] },
                (Vector2){ cx - tierHalfW[i], tierBaseY[i] },
                (Vector2){ cx + tierHalfW[i], tierBaseY[i] },
                Fade(tierCol[i], t)
            );

            // Ornaments along each tier
            int ornaments = 4 - i;
            for (int o = 0; o < ornaments; o++)
            {
                float frac = (ornaments == 1) ? 0.5f : (float)o / (float)(ornaments - 1);
                float ox = (cx - tierHalfW[i] * 0.65f) + (tierHalfW[i] * 1.3f) * frac;
                float oy = tierBaseY[i] - tierH[i] * (0.22f + 0.25f * (float)(o % 2));
                Color ballCol = ((i + o) % 2 == 0) ? (Color){ 220, 38, 38, 255 } : (Color){ 250, 204, 21, 255 };
                DrawCircle((int)ox, (int)oy, 5.0f * scale, Fade(ballCol, t));
            }
        }

        // Shining star at the top of the tree
        float topY = tierBaseY[2] - tierH[2] - 8.0f * scale;
        DrawCircle((int)cx, (int)topY, (18.0f + 6.0f * sinf(timeNow * 4.0f)) * scale, Fade((Color){ 250, 204, 21, 255 }, 0.3f * t));
        DrawSparkleStar(cx, topY, 14.0f * scale, Fade((Color){ 254, 240, 138, 255 }, t));

        // Gift boxes tucked beside the trunk
        DrawGiftBox((int)(cx - 64.0f * scale), (int)baseY, (int)(34.0f * scale), (Color){ 220, 38, 38, 255 }, (Color){ 250, 204, 21, 255 }, t);
        DrawGiftBox((int)(cx + 62.0f * scale), (int)baseY, (int)(30.0f * scale), (Color){ 37, 99, 235, 255 }, (Color){ 248, 250, 252, 255 }, t);
    }
}

void ANIMATIONFINISH(int step, float dt)
{
    float t = ClampStep01(step);
    if (t <= 0.0f) return;

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float timeNow = (float)GetTime() + dt * 0.0005f;
    int cell = 24;

    Color red = (Color){ 195, 32, 38, 255 };
    Color green = (Color){ 21, 128, 61, 255 };
    Color gold = (Color){ 245, 190, 25, 255 };
    Color snow = (Color){ 248, 250, 252, 255 };

    for (int y = 0; y < h; y += cell)
    {
        for (int x = 0; x < w; x += cell)
        {
            int gx = x / cell;
            int gy = y / cell;
            int pattern = (gx + gy) % 4;

            // Staggered reveal so step 0->100 sweeps in and 100->0 clears out
            float wave = 0.5f + 0.5f * sinf((float)gx * 0.35f + (float)gy * 0.25f - timeNow * 2.0f);
            float localT = ClampStep01((int)((t * 1.25f - wave * 0.25f) * 100.0f));
            if (localT <= 0.0f) continue;

            // Base festive quilt/wrapping-paper tile
            Color base = (pattern == 0) ? red :
                         (pattern == 1) ? green :
                         (pattern == 2) ? snow : gold;

            DrawRectangle(x, y, cell, cell, Fade(base, localT * 0.92f));

            // Candy-cane diagonal stripe motif
            if (pattern == 0 || pattern == 2)
            {
                Color stripe = (pattern == 0) ? snow : red;
                DrawLineEx(
                    (Vector2){ (float)x, (float)(y + cell) },
                    (Vector2){ (float)(x + cell), (float)y },
                    4.0f * localT,
                    Fade(stripe, localT)
                );
            }

            // Hanging bauble or twinkling star accent
            if ((gx * 7 + gy * 13) % 5 == 0)
            {
                DrawCircle(x + cell / 2, y + cell / 2, (cell * 0.28f) * localT, Fade(gold, localT));
                DrawCircle(x + cell / 2 - 2, y + cell / 2 - 2, (cell * 0.09f) * localT, Fade(snow, localT));
            }
            else if ((gx * 11 + gy * 3) % 7 == 0)
            {
                DrawSparkleStar((float)(x + cell / 2), (float)(y + cell / 2), (cell * 0.32f) * localT, Fade(gold, localT));
            }
        }
    }
}*/
/*

static float ClampStep01(int step)
{
    if (step < 0) return 0.0f;
    if (step > 100) return 1.0f;
    return (float)step / 100.0f;
}

void ANIMATIONSTART(int step, float dt)
{
    float t = ClampStep01(step);
    if (t <= 0.0f) return;

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float timeNow = (float)GetTime() + dt * 0.0005f;
    int pitch = 36;

    Color crimson = (Color){ 215, 38, 56, 255 };
    Color emerald = (Color){ 22, 163, 74, 255 };
    Color gold    = (Color){ 250, 204, 21, 255 };
    Color frost   = (Color){ 241, 245, 249, 255 };

    for (int y = -pitch; y < h + pitch; y += pitch)
    {
        for (int x = -pitch; x < w + pitch; x += pitch)
        {
            int gx = x / pitch;
            int gy = y / pitch;

            // Radial wave from center so step 0->100 assembles and 100->0 disperses
            float dx = (float)(x - w / 2) / fmaxf(1.0f, (float)w * 0.5f);
            float dy = (float)(y - h / 2) / fmaxf(1.0f, (float)h * 0.5f);
            float dist = sqrtf(dx * dx + dy * dy) * 0.65f;
            float wave = 0.5f + 0.5f * sinf(dist * 5.0f - timeNow * 2.5f + (float)(gx ^ gy) * 0.2f);

            float localT = ClampStep01((int)((t * 1.3f - dist * 0.35f - wave * 0.15f) * 100.0f));
            if (localT <= 0.0f) continue;

            float cx = (float)x + (float)pitch * 0.5f;
            float cy = (float)y + (float)pitch * 0.5f;
            float facetSize = ((float)pitch * 0.46f) * localT;

            Color primary = ((gx + gy) % 2 == 0) ? crimson : emerald;
            Color accent  = ((gx * 3 + gy) % 3 == 0) ? gold : frost;

            // Abstract diamond facet (two triangles forming a rotated square)
            DrawTriangle(
                (Vector2){ cx, cy - facetSize },
                (Vector2){ cx - facetSize, cy },
                (Vector2){ cx + facetSize, cy },
                Fade(primary, 0.55f * localT)
            );
            DrawTriangle(
                (Vector2){ cx, cy + facetSize },
                (Vector2){ cx + facetSize, cy },
                (Vector2){ cx - facetSize, cy },
                Fade(primary, 0.35f * localT)
            );

            // Crossed geometric diagonals inside each cell
            float half = (float)pitch * 0.35f * localT;
            DrawLineEx(
                (Vector2){ cx - half, cy - half },
                (Vector2){ cx + half, cy + half },
                1.5f,
                Fade(accent, 0.65f * localT)
            );
            DrawLineEx(
                (Vector2){ cx + half, cy - half },
                (Vector2){ cx - half, cy + half },
                1.5f,
                Fade(accent, 0.45f * localT)
            );

            // Glowing bokeh node at cell center
            DrawCircle((int)cx, (int)cy, 3.0f * localT, Fade(gold, 0.85f * localT));
        }
    }
}

void ANIMATIONFINISH(int step, float dt)
{
    float t = ClampStep01(step);
    if (t <= 0.0f) return;

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float timeNow = (float)GetTime() + dt * 0.0005f;
    int pitch = 36;

    Color crimson = (Color){ 215, 38, 56, 255 };
    Color emerald = (Color){ 22, 163, 74, 255 };
    Color gold    = (Color){ 250, 204, 21, 255 };
    Color frost   = (Color){ 241, 245, 249, 255 };

    for (int y = -pitch; y < h + pitch; y += pitch)
    {
        for (int x = -pitch; x < w + pitch; x += pitch)
        {
            int gx = x / pitch;
            int gy = y / pitch;

            // Radial wave from center so step 0->100 assembles and 100->0 disperses
            float dx = (float)(x - w / 2) / fmaxf(1.0f, (float)w * 0.5f);
            float dy = (float)(y - h / 2) / fmaxf(1.0f, (float)h * 0.5f);
            float dist = sqrtf(dx * dx + dy * dy) * 0.65f;
            float wave = 0.5f + 0.5f * sinf(dist * 5.0f - timeNow * 2.5f + (float)(gx ^ gy) * 0.2f);

            float localT = ClampStep01((int)((t * 1.3f - dist * 0.35f - wave * 0.15f) * 100.0f));
            if (localT <= 0.0f) continue;

            float cx = (float)x + (float)pitch * 0.5f;
            float cy = (float)y + (float)pitch * 0.5f;
            float facetSize = ((float)pitch * 0.46f) * localT;

            Color primary = ((gx + gy) % 2 == 0) ? crimson : emerald;
            Color accent  = ((gx * 3 + gy) % 3 == 0) ? gold : frost;

            // Abstract diamond facet (two triangles forming a rotated square)
            DrawTriangle(
                (Vector2){ cx, cy - facetSize },
                (Vector2){ cx - facetSize, cy },
                (Vector2){ cx + facetSize, cy },
                Fade(primary, 0.55f * localT)
            );
            DrawTriangle(
                (Vector2){ cx, cy + facetSize },
                (Vector2){ cx + facetSize, cy },
                (Vector2){ cx - facetSize, cy },
                Fade(primary, 0.35f * localT)
            );

            // Crossed geometric diagonals inside each cell
            float half = (float)pitch * 0.35f * localT;
            DrawLineEx(
                (Vector2){ cx - half, cy - half },
                (Vector2){ cx + half, cy + half },
                1.5f,
                Fade(accent, 0.65f * localT)
            );
            DrawLineEx(
                (Vector2){ cx + half, cy - half },
                (Vector2){ cx - half, cy + half },
                1.5f,
                Fade(accent, 0.45f * localT)
            );

            // Glowing bokeh node at cell center
            DrawCircle((int)cx, (int)cy, 3.0f * localT, Fade(gold, 0.85f * localT));
        }
    }
}
*/
APP *APP_CREATE(int sw, int sh)
{
  APP *v = malloc(sizeof(APP));
  if(!v)
  {
    return NULL;
  }
  memset(v, 0, sizeof(APP));
  
  v->serial = 0;
  v->sw = sw;
  v->sh = sh;
  
  v->sm = STATEMANAGER_CREATE(
    STATELOGO_START, 
    STATELOGO_FINISH, 
    STATELOGO_RENDER,
    ANIMATIONSTART,
    ANIMATIONFINISH,
    1
  );
  if(!v->sm)
  {
    free(v);
    return NULL;
  }
  
  // raylib
#if defined(PLATFORM_ANDROID)
    // Let raylib manage the Android native window/surface lifecycle.
    // Manually waiting for app->window and querying ANativeWindow sizes can
    // race/conflict with raylib's internal Android glue and result in black
    // screens on resume.
    struct android_app *app = GetAndroidApp();
    if (app) RequestAndroidPermissions(app);

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(0, 0, "Donkey AI Cloning");
#else
    /* OpenGL context exists only after InitWindow — LoadTextureFromImage in InitDeusCloud needs it */
    SetConfigFlags(FLAG_VSYNC_HINT |FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Yvonta AI Cloning");
#endif

#if defined(PLATFORM_ANDROID)
    // Stuur een commando naar de Android window manager om het scherm aan te houden
    //ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
#endif  
  
  InitAudioDevice();
  SetMasterVolume(1.0f);
   
  return v;
}

void APP_DESTROY(APP *v)
{
  if(!v)
  {
    
    CloseAudioDevice();
    CloseWindow();

    STATEMANAGER_DESTROY(v->sm);
    v->sm = NULL;

    free(v);
    return;
  }
}

BOOL APP_TICK(APP *v)
{
  if(!v || !v->sm)
  {
    return FALSE;
  }
  
  // raylib
  BeginDrawing();
  ClearBackground(BLACK);

  STATEMANAGER_RENDER(v->sm, GetFrameTime());
  
  EndDrawing();
  
  v->serial++;
  if(v->serial == 200)
  {
    STATEMANAGER_SWITCH(
      v->sm,
      STATETYCHO_START, 
      STATETYCHO_FINISH, 
      STATETYCHO_RENDER
    );
  }
  return !WindowShouldClose();
}

