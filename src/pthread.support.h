#ifndef WIN32
  #ifndef WIN64
    #include <unistd.h> // I think this is universal to UNIX/ LINUX plantofrms and should define _POSIX_THREADS
  #endif
#endif