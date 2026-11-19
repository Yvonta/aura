#pragma once

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdlib.h>
#include <string.h>

#include "commons.h"
#include "config.h"
#include "linkedlist.h"
#include "log.h"

typedef struct {
    unsigned int vaoId;
    unsigned int instanceVboId;
    Mesh cubeMesh;
    Shader shader;
    int cubeCount;
    float timeElapsed;
    float duration;
    void *instanceData;  /* InstanceData[] for per-cube draw fallback */
    int pixelsx;
} CUBESPLASH;

CUBESPLASH *CUBESPLASH_CREATE();
void CUBESPLASH_DESTROY(CUBESPLASH *v);
CRESULT CUBESPLASH_FROMMEMORY(CUBESPLASH *v, const char *fileType, const unsigned char *fileData, int dataSize, float duration, int pixelsx, float z);
CRESULT CUBESPLASH_FROMIMAGE(CUBESPLASH *v, const Image *img, float duration, int pixelsx);
void CUBESPLASH_UPDATE(CUBESPLASH *v, float deltaTime);
void CUBESPLASH_DRAW(CUBESPLASH *v, Camera3D camera);
