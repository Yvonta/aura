#pragma once

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"


#include "commons.h"
#include "config.h"
#include "linkedlist.h"
#include "log.h"
#include "statelogo.h"
#include "statetycho.h"
#include "statemanager.h"

typedef struct {
  int serial;
  int sw;
  int sh;
  STATEMANAGER *sm;
} APP;

APP *APP_CREATE(int sw, int sh);
void APP_DESTROY(APP *v);
BOOL APP_TICK(APP *v);
