#pragma once

#include "commons.h"
#include "config.h"
#include "linkedlist.h"
#include "log.h"

typedef struct {
  int (*statestartfunwait)(int);
  int (*statefinishfunwait)(int);
  short (*staterenderfunwait)(float);
  int (*statestartfun)(int);
  int (*statefinishfun)(int);
  short (*staterenderfun)(float);
  int state;
  int step;
  int block;
  void (*amimatestart)(int, float);
  void (*amimatefinish)(int, float);
} STATEMANAGER;

STATEMANAGER *STATEMANAGER_CREATE(
  int (*statestartfun)(int),
  int (*statefinishfun)(int),
  short (*staterenderfun)(float),
  void (*amimatestart)(int, float),
  void (*amimatefinish)(int, float),
  int block
);
void STATEMANAGER_DESTROY(STATEMANAGER *v);
CRESULT STATEMANAGER_SWITCH(
  STATEMANAGER *v,
  int (*statestartfun)(int),
  int (*statefinishfun)(int),
  short (*staterenderfun)(float)
);
BOOL STATEMANAGER_RENDER(STATEMANAGER *v, float dt);
