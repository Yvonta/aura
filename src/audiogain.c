#include "audiogain.h"

int AUDIOGAIN_FILTER(short *frame, int len, int framecount, double *max, int NUMFRAMES)
{
    int i = 0;
    double maxmax = 0.0;
    double val = 0.0;

    max[framecount % NUMFRAMES] = 0;
    
    // 1. Find the peak value in the current frame
    for (i = 0; i < len; i++)
    {
        val = (double)frame[i] / 32768.0;
        
        if (val < 0) 
            val = -val; /* ABS */
            
        if (val > max[framecount % NUMFRAMES])
        {
            max[framecount % NUMFRAMES] = val;
        }
    }
    
    // 2. Determine global max across recent frames
    if (framecount <= NUMFRAMES)
    {
        maxmax = 1.0;
    }
    else
    {    
        for (i = 0; i < NUMFRAMES && i < framecount; i++)
        {
            if (max[i] > maxmax)
            {
                maxmax = max[i];
            }
        }
    }
    
    // 3. Safe Gain Limiting (AGC)
    // Prevent division by zero or near-zero silence
    if (maxmax < 0.001)
    {
        maxmax = 0.001;
    }

    // Calculate gain multiplier: 1.0 / maxmax
    double gain = 1.0 / maxmax;

    // Define your Maximum Allowable Gain (e.g., 3.0 = max 3x boost / ~9.5 dB)
    double max_allowed_gain = 3.0; 
    if (gain > max_allowed_gain)
    {
        gain = max_allowed_gain;
    }

    // 4. Apply gain and convert back to short samples in-place
    for (i = 0; i < len; i++)
    {
        val = (double)frame[i] / 32768.0; // Convert to float scale (-1.0 to 1.0)
        val = val * gain;                // Apply controlled gain

        // Scale back to 16-bit integer range
        val = val * 32767.0;

        // Clamp to prevent overflow/underflow distortion
        if (val > 32767.0)  val = 32767.0;
        if (val < -32768.0) val = -32768.0;

        frame[i] = (short)val;
    }

    return TRUE;
}

