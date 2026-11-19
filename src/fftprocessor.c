#include "fftprocessor.h"
#include <complex.h>



void FFTPROCESSOR_TRANSFORM(float complex* buf, int n) {
    if (n <= 1) return;

    float complex* even = malloc(n/2 * sizeof(float complex));
    float complex* odd  = malloc(n/2 * sizeof(float complex));

    for (int i = 0; i < n/2; i++) {
        even[i] = buf[2*i];
        odd[i]  = buf[2*i + 1];
    }

    FFTPROCESSOR_TRANSFORM(even, n/2);
    FFTPROCESSOR_TRANSFORM(odd, n/2);

    for (int i = 0; i < n/2; i++) {
        /* cexpf is often missing from Android NDK <complex.h>; cos/sin is portable */
        float angle = -2.0f * PI * (float)i / (float)n;
        float complex w = cosf(angle) + I * sinf(angle);
        float complex t = w * odd[i];
        buf[i] = even[i] + t;
        buf[i + n/2] = even[i] - t;
    }
    
    free(even);
    free(odd);
}
