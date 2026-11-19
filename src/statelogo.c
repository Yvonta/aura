#include "statelogo.h"

#define STATELOGO_STEPMEMORY 0
#define STATELOGO_STEPCAMERA 20
#define STATELOGO_STEPYVONTA 50
#define STATELOGO_STEPMARLON 70

typedef struct {
    Camera camera;
    CUBESPLASH *yvontasplash;
    CUBESPLASH *marlonsplash;
} STATELOGO;

STATELOGO *statelogo;

CRESULT STATELOGO_START(int step)
{
    switch(step)
    {
        case STATELOGO_STEPMEMORY:
        {
            statelogo = (STATELOGO *)malloc(sizeof(STATELOGO));
            if(!statelogo)
            {
                return CFAILED;
            }
        } break;
        case STATELOGO_STEPCAMERA:
        {
            statelogo->camera.position   = (Vector3){ 0.0f, -0.16f, 25.0f };
            statelogo->camera.target     = (Vector3){ 0.0f, -0.4f, 0.0f };
            statelogo->camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
            statelogo->camera.fovy       = 90.0f;
            statelogo->camera.projection = CAMERA_PERSPECTIVE;
            return CSUCCESS;
        } break;
        case STATELOGO_STEPYVONTA:
        {
            statelogo->yvontasplash = CUBESPLASH_CREATE();
            if(!statelogo->yvontasplash)
            {
                return CFAILED;
            }
            int datasize = 0;
            unsigned char *filedata = LoadFileData("images/yvontac.png", &datasize);
            if (filedata == NULL)
            {
                CUBESPLASH_DESTROY(statelogo->yvontasplash);
                statelogo->yvontasplash = NULL;
                return CFAILED;
            } 
            if(CUBESPLASH_FROMMEMORY(statelogo->yvontasplash, ".png", filedata, datasize, 2.0f, 52, 0.f) == CFAILED)
            {
                CUBESPLASH_DESTROY(statelogo->yvontasplash);
                statelogo->yvontasplash = NULL;
                UnloadFileData(filedata);
                return CFAILED;
            }
            UnloadFileData(filedata);
        } break;
        case STATELOGO_STEPMARLON:
        {
            /*statelogo->marlonsplash = cubesplash_create();
            if(!statelogo->marlonsplash)
            {
                return CFAILED;
            }
            int datasize = 0;
            unsigned char *filedata = LoadFileData("images/marlon.png", &datasize);
            if (filedata == NULL)
            {
                cubesplash_destroy(statelogo->marlonsplash);
                statelogo->marlonsplash = NULL;
                return CFAILED;
            }
            if(cubesplash_frommemory(statelogo->marlonsplash, ".png", filedata, datasize, 2.0f, 64, -2.f) == CFAILED)
            {
                cubesplash_destroy(statelogo->marlonsplash);
                statelogo->marlonsplash = NULL;
                UnloadFileData(filedata);
                return CFAILED;
            }
            UnloadFileData(filedata);*/
        } break;
    }
    return CSUCCESS;
}

CRESULT STATELOGO_FINISH(int step)
{
    switch(step)
    {
        case STATELOGO_STEPMEMORY:
        {
            free(statelogo);
            statelogo = NULL;
        } break;
        case STATELOGO_STEPCAMERA:
        {
            
        } break;
        case STATELOGO_STEPYVONTA:
        {  
            CUBESPLASH_DESTROY(statelogo->yvontasplash);
            statelogo->yvontasplash = NULL;
        } break;
        case STATELOGO_STEPMARLON:
        {
            //cubesplash_destroy(statelogo->marlonsplash);
            //statelogo->marlonsplash = NULL;
        } break;
    }
    return CSUCCESS;
}

BOOL STATELOGO_RENDER(float dt)
{
  CUBESPLASH_UPDATE(statelogo->yvontasplash, dt);
  BeginMode3D(statelogo->camera);
  CUBESPLASH_DRAW(statelogo->yvontasplash, statelogo->camera);
  EndMode3D();
    
  if(statelogo->yvontasplash->timeElapsed > 5.f)
  {
    return FALSE;
  }
    
  return TRUE;
}
