#pragma once

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    short *buffer;
    size_t sizesamples; // Totaal aantal shorts in de buffer
    size_t writeptr;
    size_t readptr;
    int channels;
} RINGBUFFER;

RINGBUFFER *RINGBUFFER_CREATE(short *storage, size_t sizesamples, int channels);
void RINGBUFFER_DESTROY(RINGBUFFER *rb);

// Hoeveel samples (shorts) zijn er beschikbaar om te lezen?
size_t RINGBUFFER_AVAILABLEREAD(RINGBUFFER *rb);

// Hoeveel ruimte (shorts) is er nog vrij om te schrijven?
size_t RINGBUFFER_AVAILABLEWRITE(RINGBUFFER *rb);

// Schrijf samples naar de buffer. Retourneert aantal geschreven samples.
size_t RINGBUFFER_WRITE(RINGBUFFER *rb, const short *data, size_t numsamples);

// Lees samples uit de buffer. Retourneert aantal gelezen samples.
size_t RINGBUFFER_READ(RINGBUFFER *rb, short *data, size_t numsamples);


