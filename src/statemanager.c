#include "statemanager.h"

STATEMANAGER *STATEMANAGER_CREATE(
  CRESULT (*statestartfun)(int),
  CRESULT (*statefinishfun)(int),
  BOOL (*staterenderfun)(float),
  void (*amimatestart)(int, float),
  void (*amimatefinish)(int, float),
  int block
)
{
  STATEMANAGER *v = malloc(sizeof(STATEMANAGER));
  if(!v)
  {
    return NULL;
  }
  memset(v, 0, sizeof(STATEMANAGER));
  
  v->block = block;
  v->amimatestart = amimatestart;
  v->amimatefinish = amimatefinish;
  v->statestartfun = statestartfun;
  v->statefinishfun = statefinishfun;
  v->staterenderfun = staterenderfun;
    
  v->state = 1;
  
  return v;
}

void STATEMANAGER_DESTROY(STATEMANAGER *v)
{
   if(!v)
   {
     return;
   }
   
   for(v->step = 99; v->step <= 0; v->step-=v->block)
   {
     v->statefinishfun(v->step);
   }
   
   free(v);
}

CRESULT STATEMANAGER_SWITCH(
  STATEMANAGER *v,
  CRESULT (*statestartfun)(int),
  CRESULT (*statefinishfun)(int),
  BOOL (*staterenderfun)(float)
)
{
  if(!v)
  {
    return CFAILED;
  }
      
  v->statestartfunwait = statestartfun;
  v->statefinishfunwait = statefinishfun;
  v->staterenderfunwait = staterenderfun;
  v->state = 3;
  return CSUCCESS;
}

BOOL STATEMANAGER_RENDER(STATEMANAGER *v, float dt)
{
   BOOL ret;

   if(!v)
   {
     return FALSE;
   }
   
   switch(v->state)
   {
     case 3:
     {
       ret = v->staterenderfun(dt);
       if(!ret)
       {
         v->state = 2;
       }
     } break;
     case 1:
     {
       if(v->statestartfun(v->step) == CFAILED)
       {
         v->state = 2;
         v->step-=v->block;
       }
       else
       {
         v->step+=v->block;
         if(v->step >= 99)
         {
            v->state = 3;
         }
       }
       
       if(v->amimatestart)
       {
         v->amimatestart(v->step, dt);
       }
     } break;
     case 2:
     {
       v->statefinishfun(v->step); 
       v->step-=v->block;
       if(v->step < 0)
       {
         v->state = 1;
         v->step = 0;
         if(
           v->statestartfunwait &&
           v->statefinishfunwait &&
           v->staterenderfunwait
         )
         {
           v->statestartfun = v->statestartfunwait;
           v->statefinishfun = v->statefinishfunwait;
           v->staterenderfun = v->staterenderfunwait;
           v->statestartfunwait = NULL;
           v->statefinishfunwait = NULL;
           v->staterenderfunwait = NULL;
         }
         else
         {
           return FALSE;
         }
       }
       if(v->amimatefinish)
       {
         v->amimatefinish(v->step, dt);
       }
     } break;
   }
  
  return TRUE; 
}
