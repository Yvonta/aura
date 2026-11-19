#include "ringbuffer.h"

RINGBUFFER *RINGBUFFER_CREATE(short *storage, size_t sizesamples, int channels) {
    RINGBUFFER *v = malloc(sizeof(RINGBUFFER));
    if(!v)
    {
      return NULL;
    }
    memset(v, 0, sizeof(RINGBUFFER));
    v->buffer = storage;
    v->sizesamples = sizesamples;
    v->channels = channels;
    v->writeptr = 0;
    v->readptr = 0;
    if(v->buffer) 
    {
       memset(v->buffer, 0, sizesamples * sizeof(short));
    }
    return v;
}

void RINGBUFFER_DESTROY(RINGBUFFER *v) 
{
    if(!v)
    {
      return;
    }
    v->writeptr = 0;
    v->readptr = 0;
    free(v);
}

size_t RINGBUFFER_AVAILABLEREAD(RINGBUFFER *v) 
{
    if(v->writeptr >= v->readptr) 
    {
        return v->writeptr - v->readptr;
    }
    return v->sizesamples - (v->readptr - v->writeptr);
}

size_t RINGBUFFER_AVAILABLEWRITE(RINGBUFFER *v) 
{
    // We laten altijd 1 sample leeg om read/write pointer onderscheid te maken
    return v->sizesamples - RINGBUFFER_AVAILABLEREAD(v) - 1;
}

size_t RINGBUFFER_WRITE(RINGBUFFER *v, const short *data, size_t numsamples) 
{
    size_t available = RINGBUFFER_AVAILABLEWRITE(v);
    if(numsamples > available)
    {
      numsamples = available;
    }
    for(size_t i = 0; i < numsamples; i++) 
    {
        v->buffer[v->writeptr] = data[i];
        v->writeptr = (v->writeptr + 1) % v->sizesamples;
    }
    return numsamples;
}

size_t RINGBUFFER_READ(RINGBUFFER *v, short *data, size_t numsamples) 
{
    size_t available = RINGBUFFER_AVAILABLEREAD(v);
    if(numsamples > available) 
    {
      numsamples = available;
    }
    for(size_t i = 0; i < numsamples; i++) 
    {
        data[i] = v->buffer[v->readptr];
        v->readptr = (v->readptr + 1) % v->sizesamples;
    }
    return numsamples;
}


