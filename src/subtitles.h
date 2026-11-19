#pragma once

#include "raylib.h"
#include <string.h>
#include <stdlib.h>

#define MAX_SUBTITLE_LENGTH 256
#define SUBTITLE_DURATION_SEC 6.0f

typedef struct {
    char text[MAX_SUBTITLE_LENGTH];
    float timeremaining;
    bool active;
} SUBTITLES;

SUBTITLES *SUBTITLES_CREATE();
void SUBTITLES_DESTROY(SUBTITLES *v);
void SUBTITLES_PUSH(SUBTITLES *v, char *text); 
void SUBTITLES_UPDATE(SUBTITLES *v, float dt);
void SUBTITLES_DRAW(SUBTITLES *v, int screenwidth, int screenheight);


