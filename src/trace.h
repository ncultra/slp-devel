
#ifndef __LSLP_TRACE_H
#define __LSLP_TRACE_H


#ifdef TRACE_ENABLED 

 #ifdef _WIN32
  #define SLP_TRACE printf("\nSLP: (file=%s, line=%d) ", __FILE__, __LINE__); \
                        printf
 #else
  #define SLP_TRACE(fmt, va...) printf("SLP: (file=%s, line=%d) " fmt "\n", \
     __FILE__, __LINE__, ## va )

 #endif // win32

#else 
 #ifdef _WIN32
   #define SLP_TRACE
 #else
   #define SLP_TRACE(fmt, va...) ((void)0)
#endif
#endif // trace enabled

#endif // defined



