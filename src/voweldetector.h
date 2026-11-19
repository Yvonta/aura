#pragma once
#include "fftprocessor.h"

typedef enum { VOWEL_NONE, VOWEL_A, VOWEL_E, VOWEL_O } VOWEL;

VOWEL VOWELDETECTOR_ANALIZEFROMPCM(const short *pcmSamples, int sampleCount, int sampleRate);
