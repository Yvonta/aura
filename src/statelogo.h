#pragma once

#include "raylib.h"

#include "commons.h"
#include "config.h"
#include "linkedlist.h"
#include "log.h"
#include "cubesplash.h"

CRESULT STATELOGO_START(int step);
CRESULT STATELOGO_FINISH(int step);
BOOL STATELOGO_RENDER(float dt);
