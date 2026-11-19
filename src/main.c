#include "commons.h"
#include "config.h"
#include "linkedlist.h"
#include "log.h"
#include "app.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif

BOOL loop = TRUE;
APP *app = NULL;

int logfun(char *msg)
{
  struct timespec ts;

  clock_gettime(CLOCK_REALTIME, &ts);

  struct tm *tminfo = localtime(&ts.tv_sec);

  char timebuf[32];
  strftime(timebuf, sizeof(timebuf), "[%Y-%m-%d %H:%M:%S", tminfo);

  int subsec = (int)(ts.tv_nsec / 100000);

#ifdef __ANDROID__
  __android_log_print(ANDROID_LOG_INFO, "personaclient", "%s:%04d] %s", timebuf, subsec, msg);
#else
  printf("%s:%04d] ", timebuf, subsec);
  printf("%s\n", msg);
#endif

  return 0;
}

static void siginthandler(int sig)
{
  (void)sig;
  loop = FALSE;
}

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  LOG_CREATE(FALSE, FALSE, FALSE, logfun);

#ifdef __ANDROID__
  __android_log_print(ANDROID_LOG_INFO, "personaclient", "main gestart via android_main");
#endif

  app = APP_CREATE(800, 600);
  if (!app)
  {
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_ERROR, "personaclient", "APP_CREATE gaf NULL terug");
#endif
    LOG_DESTROY();
    return 1;
  }

#ifndef __ANDROID__
  signal(SIGINT, siginthandler);
#endif

  while (loop && APP_TICK(app))
  {
  }

  APP_DESTROY(app);
  LOG_DESTROY();
  return 0;
}
