#pragma once

#include "commons.h"
#include "config.h"
#include "linkedlist.h"
#include "log.h"

int AUDIOGAIN_FILTER(short *frame, int len, int framecount, double *max, int NUMFRAMES);
