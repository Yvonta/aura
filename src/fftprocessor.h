#pragma once

#include <complex.h>
#include <math.h>
#include <stdlib.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

// Recursive FFT: Transforms samples into complex frequency data
void FFTPROCESSOR_TRANSFORM(float complex* buf, int n);


