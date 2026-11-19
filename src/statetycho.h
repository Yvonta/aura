#pragma once

#include "raylib.h"
#include "miniaudio.h"


#include "commons.h"
#include "config.h"
#include "linkedlist.h"
#include "log.h"
#include "cubesplash.h"
#include "websocketclient.h"
#include "ringbuffer.h"
#include "voweldetector.h"
#include "audiogain.h"
#include "clonesanta.h"
#include "subtitles.h"

CRESULT STATETYCHO_START(int step);
CRESULT STATETYCHO_FINISH(int step);
BOOL STATETYCHO_RENDER(float dt);
