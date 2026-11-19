#include "voweldetector.h"
#include <complex.h>
#include <math.h>
#include <stdlib.h>

VOWEL VOWELDETECTOR_ANALIZEFROMPCM(const short *pcmSamples, int sampleCount, int sampleRate) 
{
  // 1. Prepare buffer (Must be power of 2, e.g., 512 or 1024)
  float complex* fftBuf = malloc(sampleCount * sizeof(float complex));
  for (int i = 0; i < sampleCount; i++)
  {
    fftBuf[i] = (float)pcmSamples[i] / 32768.0f;
  }
  
  // 2. Run FFT
  FFTPROCESSOR_TRANSFORM(fftBuf, sampleCount);

  // 3. Find the dominant frequency
  float maxAmp = 0;
  int maxIndex = 0;
  for(int i = 1; i < sampleCount / 2; i++) 
  {
      /* hypotf(re,im): avoids cabsf (not always declared in NDK complex.h) */
      const float *reim = (const float *)&fftBuf[i];
      float amp = hypotf(reim[0], reim[1]);
      if (amp > maxAmp) 
      {
          maxAmp = amp;
          maxIndex = i;
      }
  }

  float dominantFreq = (float)maxIndex * sampleRate / sampleCount;
  free(fftBuf);

  // Threshold: Ignore silence
  if(maxAmp < 0.01f) 
  {
    return VOWEL_NONE;
  }
  // 4. Map Frequency to Vowel (Approximate Formants)
  if(dominantFreq < 600) 
  {
    return VOWEL_O;      // Deep/Closed
  }
  if(dominantFreq < 1200) 
  {
    return VOWEL_A;     // Open/Mid
  }
  if(dominantFreq >= 1200) 
  {
    return VOWEL_E;    // Sharp/High
  }
  return VOWEL_NONE;
}
