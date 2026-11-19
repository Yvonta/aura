#include "cubesplash.h"

typedef struct {
    Vector3 startPos;
    Vector3 targetPos;
    Vector4 color;
} InstanceData;
#define INSTANCE_STRIDE (sizeof(InstanceData))

#if defined(PLATFORM_ANDROID)
static const char *vsSource = 
    "#version 100\n"
    "attribute vec3 vertexPosition;\n"
    "attribute vec3 vertexNormal;\n"
    // VERANDERD: Deze zijn nu UNIFORMS, geen attributes
    "uniform vec3 u_startPos;\n" 
    "uniform vec3 u_targetPos;\n"
    "uniform vec4 u_color;\n"
    // ---
    "uniform mat4 u_pMatrix; uniform mat4 u_vMatrix;\n"
    "uniform float u_time; uniform float u_animDuration; uniform vec3 u_lightDir;\n"
    "varying vec4 v_litColor;\n"
    "float cubicEase(float t) { return t * t * (3.0 - 2.0 * t); }\n"
    "void main() {\n"
    "    float t = clamp(u_time, 0.0, u_animDuration) / u_animDuration;\n"
    "    float eased_t = cubicEase(t);\n"
    // VERANDERD: Gebruik de uniform namen
    "    vec3 animatedPos = mix(u_startPos, u_targetPos, eased_t);\n"
    "    vec3 transformedNormal = normalize((u_vMatrix * vec4(vertexNormal, 0.0)).xyz);\n"
    "    float intensity = 0.3 + max(dot(transformedNormal, u_lightDir), 0.0) * 0.7;\n"
    "    v_litColor = vec4(u_color.rgb * intensity, u_color.a);\n"
    "    gl_Position = u_pMatrix * u_vMatrix * vec4(vertexPosition + animatedPos, 1.0);\n"
    "}\n";

static const char *fsSource = 
    "#version 100\n"
    "precision highp float;\n"
    "varying vec4 v_litColor;\n"
    "void main() { gl_FragColor = v_litColor; }\n";
#else
static const char *vsSource =
    "#version 330 core\n"
    "in vec3 vertexPosition;\n"
    "in vec3 vertexNormal;\n"
    "in vec3 a_startPos;\n"
    "in vec3 a_targetPos;\n"
    "in vec4 a_color;\n"
    "uniform mat4 u_pMatrix;\n"
    "uniform mat4 u_vMatrix;\n"
    "uniform float u_time;\n"
    "uniform float u_animDuration;\n"
    "uniform vec3 u_lightDir;\n"
    "out vec4 v_litColor;\n"
    "float cubicEase(float t) { return t * t * (3.0 - 2.0 * t); }\n"
    "void main() {\n"
    "    float t = clamp(u_time, 0.0, u_animDuration) / u_animDuration;\n"
    "    float eased_t = cubicEase(t);\n"
    "    vec3 animatedPos = mix(a_startPos, a_targetPos, eased_t);\n"
    "    vec3 transformedNormal = normalize((u_vMatrix * vec4(vertexNormal, 0.0)).xyz);\n"
    "    float intensity = 0.3 + max(dot(transformedNormal, u_lightDir), 0.0) * 0.7;\n"
    "    v_litColor = vec4(a_color.rgb * intensity, a_color.a);\n"
    "    gl_Position = u_pMatrix * u_vMatrix * vec4(vertexPosition + animatedPos, 1.0);\n"
    "}\n";
static const char *fsSource =
    "#version 330 core\n"
    "in vec4 v_litColor;\n"
    "out vec4 fragColor;\n"
    "void main() { fragColor = v_litColor; }\n";
#endif


CUBESPLASH *CUBESPLASH_CREATE()
{
    CUBESPLASH *v = malloc(sizeof(CUBESPLASH));
    if(!v) return NULL;
    memset(v, 0, sizeof(CUBESPLASH));
    return v;
}

void CUBESPLASH_DESTROY(CUBESPLASH *v)
{
    if(!v)
    {
      return;
    }
    
    // 1. Free CPU-side instance data
    if(v->instanceData) 
    {
        free(v->instanceData);
        v->instanceData = NULL;
    }

    // 3. Free GPU-side Instance VBO (The one we made via rlLoadVertexBuffer)
    if(v->instanceVboId > 0) 
    {
        rlUnloadVertexBuffer(v->instanceVboId);
        v->instanceVboId = 0;
    }

    // 4. Free the Mesh (This handles all mesh VBOs and the VAO)
    // Note: We check .vboId because GenMeshCube allocates it on the heap
    if(v->cubeMesh.vboId) 
    {
        UnloadMesh(v->cubeMesh);
        // UnloadMesh handles the internal array, but let's clear our handle
        v->cubeMesh.vboId = 0; 
    }

    // 5. Free the Shader
    if(v->shader.id > 0) 
    {
        UnloadShader(v->shader);
        v->shader.id = 0;
    }

    // 6. Reset counter
    v->cubeCount = 0;
    v->timeElapsed = 0.0f;

    free(v);
}

CRESULT CUBESPLASH_FROMMEMORY(CUBESPLASH *v, const char *fileType, const unsigned char *fileData, int dataSize, float duration, int pixelsx, float z)
{
 
    if(!v) 
    {
        E("CUBESPLASH: Failed to create cubesplash");
        return CFAILED;
    }
    if (fileData == NULL || dataSize <= 0) 
    {
        E("CUBESPLASH: Failed to load file data");
        return CFAILED;
    }
    if (pixelsx <= 0) 
    {
        pixelsx = 42;
    }
    
    v->pixelsx = pixelsx;
    v->duration = duration;

    Image img = LoadImageFromMemory(fileType, fileData, dataSize);
    if(img.data == NULL || img.width <= 0 || img.height <= 0) 
    {
      return CFAILED;
    }
    
    float aspect = (float)img.height / (float)img.width;
    int targetW = (img.width > pixelsx) ? pixelsx : img.width;
    if (targetW <= 0) 
    {
      targetW = 1;
    }
    int targetH = (int)((float)targetW * aspect);
    if (targetH <= 0) 
    {
      targetH = 1;
    }
    
    if (img.width > pixelsx) ImageResize(&img, targetW, targetH);
    
    Color *pixels = LoadImageColors(img);
    if (pixels == NULL) { UnloadImage(img); return CFAILED; }

    int opaqueCount = 0;
    int totalPixels = img.width * img.height;
    for (int i = 0; i < totalPixels; i++) if (pixels[i].a > 0) opaqueCount++;

    if (opaqueCount == 0) {
        UnloadImageColors(pixels);
        UnloadImage(img);
        return CFAILED;
    }

    v->cubeCount = opaqueCount;
    InstanceData *instances = (InstanceData *)malloc(sizeof(InstanceData) * opaqueCount);
    if (instances == NULL) {
        UnloadImageColors(pixels);
        UnloadImage(img);
        return CFAILED;
    }

    float size = 0.4f;
    float stepSize = size * 1.2f;
    float offsetX = -(img.width * stepSize) / 2.0f + stepSize / 2.0f;
    float offsetY = (img.height * stepSize) / 2.0f - stepSize / 2.0f;
    float maxDist = 60.0f; // Iets minder extreem voor Android

    for (int i = 0, idx = 0; i < totalPixels; i++) {
        if (pixels[i].a == 0) continue;
        int x = i % img.width;
        int y = i / img.width;
        
        // Posities en Kleuren (Vanaf regel 205)
        instances[idx].targetPos.x = x * stepSize + offsetX;
        instances[idx].targetPos.y = -y * stepSize + offsetY;
        instances[idx].targetPos.z = z;

        instances[idx].startPos.x = ((float)rand()/(float)RAND_MAX - 0.5f) * maxDist;
        instances[idx].startPos.y = ((float)rand()/(float)RAND_MAX - 0.5f) * maxDist;
        instances[idx].startPos.z = ((float)rand()/(float)RAND_MAX - 0.5f) * maxDist;

        // Kleur (gebruik pixels[i] want 'p' bestaat hier niet)
        instances[idx].color.x = pixels[i].r/255.0f;
        instances[idx].color.y = pixels[i].g/255.0f;
        instances[idx].color.z = pixels[i].b/255.0f;
        instances[idx].color.w = pixels[i].a/255.0f;
        idx++;
    }

    // 1. Shader laden
    v->shader = LoadShaderFromMemory(vsSource, fsSource);
    if(v->shader.id == 0) 
    {
      E("CUBESPLASH: Failed to load shader!");
      return CFAILED;
    }

    // 2. Mesh genereren
    v->cubeMesh = GenMeshCube(size, size, size);
    
    // 3. EXPLICIETE UPLOAD (Belangrijk voor Android GLES)
    // We gebruiken 'false' voor dynamic omdat een cube mesh statisch is.
    UploadMesh(&v->cubeMesh, false); 

    // 4. VBO voor instanties laden
    // We gebruiken rlLoadVertexBuffer direct.
    v->instanceVboId = rlLoadVertexBuffer(instances, sizeof(InstanceData) * opaqueCount, false);

    // 5. Kopieer data voor CPU gebruik (indien nodig)
    v->instanceData = (InstanceData *)malloc(sizeof(InstanceData) * (size_t)opaqueCount);
    if (v->instanceData) memcpy(v->instanceData, instances, sizeof(InstanceData) * (size_t)opaqueCount);

    // Cleanup
    free(instances);
    UnloadImageColors(pixels);
    UnloadImage(img);

    // DEBUG: Als dit 0 print in logcat, is er iets mis met de GPU context
    if (v->instanceVboId == 0) 
    {
        E("CUBESPLASH: Failed to load VBO");
    }
    
    return CSUCCESS;
}

CRESULT CUBESPLASH_FROMIMAGE(CUBESPLASH *v, const Image *img, float duration, int pixelsx)
{
    if(!v) 
    {
        return CFAILED;
    }
    if (img == NULL || img->data == NULL) 
    {
        return CFAILED;
    }
    v->pixelsx = pixelsx;
    v->duration = duration;

    int w = img->width > v->pixelsx ? v->pixelsx : img->width;
    int h = img->height > v->pixelsx ? v->pixelsx : img->height;
    Color *pixels = LoadImageColors(*img);
    if (pixels == NULL) return CFAILED;

    int opaqueCount = 0;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            if (pixels[y * img->width + x].a > 0) opaqueCount++;
    v->cubeCount = opaqueCount;
    if (opaqueCount == 0) { UnloadImageColors(pixels); return CFAILED; }

    InstanceData *instances = (InstanceData *)malloc(sizeof(InstanceData) * (size_t)opaqueCount);
    if (instances == NULL) { UnloadImageColors(pixels); return CFAILED; }

    float size = 0.4f;
    float stepSize = size * 1.2f;
    float offsetX = -(w * stepSize) / 2.0f + stepSize / 2.0f;
    float offsetY = (h * stepSize) / 2.0f - stepSize / 2.0f;
    float maxDist = 52.0f;

    int totalPixels = w * h;
    for (int i = 0, idx = 0; i < totalPixels; i++) {
        if (pixels[i].a == 0) continue;
        int x = i % img->width;
        int y = i / img->width;
        
        // Target positie
        instances[idx].targetPos.x = x * stepSize + offsetX;
        instances[idx].targetPos.y = -y * stepSize + offsetY;
        instances[idx].targetPos.z = 0.0f;

        // Start positie
        instances[idx].startPos.x = ((float)rand()/(float)RAND_MAX - 0.5f) * maxDist;
        instances[idx].startPos.y = ((float)rand()/(float)RAND_MAX - 0.5f) * maxDist;
        instances[idx].startPos.z = ((float)rand()/(float)RAND_MAX - 0.5f) * maxDist;

        // Kleur (gebruik pixels[i])
        instances[idx].color.x = pixels[i].r/255.0f;
        instances[idx].color.y = pixels[i].g/255.0f;
        instances[idx].color.z = pixels[i].b/255.0f;
        instances[idx].color.w = pixels[i].a/255.0f;
        idx++;
    }
    UnloadImageColors(pixels);

    v->shader = LoadShaderFromMemory(vsSource, fsSource);
    if(v->shader.id == 0) 
    {
      E("CUBESPLASH: Failed to load shader!");
      return CFAILED;
    }

#if !defined(PLATFORM_ANDROID)
    if (v->shader.id == 0) {
        static const char *esVs = "#version 100\n"
            "attribute vec3 vertexPosition; attribute vec3 vertexNormal;\n"
            "attribute vec3 a_startPos; attribute vec3 a_targetPos; attribute vec4 a_color;\n"
            "uniform mat4 u_pMatrix; uniform mat4 u_vMatrix;\n"
            "uniform float u_time; uniform float u_animDuration; uniform vec3 u_lightDir;\n"
            "varying vec4 v_litColor;\n"
            "float cubicEase(float t) { return t * t * (3.0 - 2.0 * t); }\n"
            "void main() {\n"
            "    float t = clamp(u_time, 0.0, u_animDuration) / u_animDuration;\n"
            "    float eased_t = cubicEase(t);\n"
            "    vec3 animatedPos = mix(a_startPos, a_targetPos, eased_t);\n"
            "    vec3 transformedNormal = normalize((u_vMatrix * vec4(vertexNormal, 0.0)).xyz);\n"
            "    float intensity = 0.3 + max(dot(transformedNormal, u_lightDir), 0.0) * 0.7;\n"
            "    v_litColor = vec4(a_color.rgb * intensity, a_color.a);\n"
            "    gl_Position = u_pMatrix * u_vMatrix * vec4(vertexPosition + animatedPos, 1.0);\n"
            "}\n";
        static const char *esFs = "#version 100\n"
            "precision highp float;\n"
            "varying vec4 v_litColor;\n"
            "void main() { gl_FragColor = v_litColor; }\n";
        v->shader = LoadShaderFromMemory(esVs, esFs);
        if(v->shader.id == 0) 
        {
          E("CUBESPLASH: Failed to load shader!");
          return CFAILED;
        }
    }
#endif
    v->cubeMesh = GenMeshCube(size, size, size);
    UploadMesh(&v->cubeMesh, false);
    v->instanceVboId = rlLoadVertexBuffer(instances, sizeof(InstanceData) * (size_t)opaqueCount, false);
    v->instanceData = malloc(sizeof(InstanceData) * (size_t)opaqueCount);
    if (v->instanceData) memcpy(v->instanceData, instances, sizeof(InstanceData) * (size_t)opaqueCount);
    free(instances);
    
    return CSUCCESS;
}


void CUBESPLASH_UPDATE(CUBESPLASH *v, float deltaTime)
{
    v->timeElapsed += deltaTime;
}

void CUBESPLASH_DRAW(CUBESPLASH *v, Camera3D camera)
{
    if (v->cubeCount <= 0) return;

    rlDrawRenderBatchActive();

#if defined(PLATFORM_ANDROID) || defined(GRAPHICS_API_OPENGL_ES2)
    // --- ANDROID / GLES2: Handmatige loop (geen Hardware Instancing) ---
    InstanceData *instances = (InstanceData *)v->instanceData;
    if (instances == NULL) return;

    float t = v->timeElapsed / v->duration;
    if (t > 1.0f) t = 1.0f;
    if (t < 0.0f) t = 0.0f;
    float eased_t = t * t * (3.0f - 2.0f * t);

    Material mat = LoadMaterialDefault();

    for (int i = 0; i < v->cubeCount; i++) {
        Vector3 currentPos = {
            instances[i].startPos.x + (instances[i].targetPos.x - instances[i].startPos.x) * eased_t,
            instances[i].startPos.y + (instances[i].targetPos.y - instances[i].startPos.y) * eased_t,
            instances[i].startPos.z + (instances[i].targetPos.z - instances[i].startPos.z) * eased_t
        };

        mat.maps[MATERIAL_MAP_DIFFUSE].color = (Color){
            (unsigned char)(instances[i].color.x * 255),
            (unsigned char)(instances[i].color.y * 255),
            (unsigned char)(instances[i].color.z * 255),
            (unsigned char)(instances[i].color.w * 255)
        };

        DrawMesh(v->cubeMesh, mat, MatrixTranslate(currentPos.x, currentPos.y, currentPos.z));
    }
#else
    // --- MACOS / DESKTOP: Hardware Instancing (Supersnel) ---
    if (v->shader.id == 0) return;

    Matrix view = GetCameraMatrix(camera);
    Matrix projection = MatrixPerspective(camera.fovy * DEG2RAD, 
                        (float)GetScreenWidth()/(float)GetScreenHeight(), 0.01f, 1000.0f);
    Vector3 lightDir = Vector3Normalize((Vector3){-0.5f, 0.7f, 0.7f});

    rlEnableShader(v->shader.id);

    // Uniforms instellen voor desktop shader
    int locTime     = rlGetLocationUniform(v->shader.id, "u_time");
    int locDuration = rlGetLocationUniform(v->shader.id, "u_animDuration");
    int locLight    = rlGetLocationUniform(v->shader.id, "u_lightDir");
    int locView     = rlGetLocationUniform(v->shader.id, "u_vMatrix");
    int locProj     = rlGetLocationUniform(v->shader.id, "u_pMatrix");

    rlSetUniform(locTime, &v->timeElapsed, SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(locDuration, &v->duration, SHADER_UNIFORM_FLOAT, 1);
    rlSetUniform(locLight, &lightDir, SHADER_UNIFORM_VEC3, 1);
    rlSetUniformMatrix(locView, view);
    rlSetUniformMatrix(locProj, projection);

    // Mesh Attributes (Position & Normal)
    int vPos = rlGetLocationAttrib(v->shader.id, "vertexPosition");
    int vNorm = rlGetLocationAttrib(v->shader.id, "vertexNormal");

    rlEnableVertexArray(v->cubeMesh.vaoId);

    if (vPos >= 0) {
        rlEnableVertexAttribute(vPos);
        rlEnableVertexBuffer(v->cubeMesh.vboId[0]);
        rlSetVertexAttribute(vPos, 3, RL_FLOAT, false, 0, 0);
    }
    if (vNorm >= 0) {
        rlEnableVertexAttribute(vNorm);
        rlEnableVertexBuffer(v->cubeMesh.vboId[2]);
        rlSetVertexAttribute(vNorm, 3, RL_FLOAT, false, 0, 0);
    }

    // Instance Attributes (De startPos, targetPos en color uit de VBO)
    int aStart = rlGetLocationAttrib(v->shader.id, "a_startPos");
    int aTarget = rlGetLocationAttrib(v->shader.id, "a_targetPos");
    int aColor = rlGetLocationAttrib(v->shader.id, "a_color");

    rlEnableVertexBuffer(v->instanceVboId);

    if (aStart >= 0) {
        rlEnableVertexAttribute(aStart);
        rlSetVertexAttribute(aStart, 3, RL_FLOAT, false, (int)sizeof(InstanceData), 0);
        rlSetVertexAttributeDivisor(aStart, 1);
    }
    if (aTarget >= 0) {
        rlEnableVertexAttribute(aTarget);
        rlSetVertexAttribute(aTarget, 3, RL_FLOAT, false, (int)sizeof(InstanceData), sizeof(Vector3));
        rlSetVertexAttributeDivisor(aTarget, 1);
    }
    if (aColor >= 0) {
        rlEnableVertexAttribute(aColor);
        rlSetVertexAttribute(aColor, 4, RL_FLOAT, false, (int)sizeof(InstanceData), sizeof(Vector3) * 2);
        rlSetVertexAttributeDivisor(aColor, 1);
    }

    // Draw call
    if (v->cubeMesh.indices != NULL) {
        rlEnableVertexBufferElement(v->cubeMesh.vboId[6]);
        rlDrawVertexArrayElementsInstanced(0, v->cubeMesh.triangleCount * 3, 0, v->cubeCount);
    } else {
        rlDrawVertexArrayInstanced(0, v->cubeMesh.vertexCount, v->cubeCount);
    }

    // Cleanup state
    rlDisableVertexAttribute(vPos);
    rlDisableVertexAttribute(vNorm);
    rlDisableVertexAttribute(aStart);
    rlDisableVertexAttribute(aTarget);
    rlDisableVertexAttribute(aColor);
    rlSetVertexAttributeDivisor(aStart, 0);
    rlSetVertexAttributeDivisor(aTarget, 0);
    rlSetVertexAttributeDivisor(aColor, 0);
    rlDisableShader();
#endif
}

