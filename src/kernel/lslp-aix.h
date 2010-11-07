
/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* OBJECT CODE ONLY SOURCE MATERIALS                                      */
/*                                                                        */ 
/* SLP for Server Management                                              */
/*                                                                        */
/*                                                                        */
/* (C) COPYRIGHT International Business Machines Corp. 2000,2005          */
/* All Rights Reserved                                                    */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */




#ifndef _LSLP_AIX_INCLUDE_
#define _LSLP_AIX_INCLUDE_

#ifdef __cplusplus
extern "c" {
#endif

#include <as400_protos.h>
#include <as400_types.h>
#include <os400msg.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h> 
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include <netdb.h>
#include <errno.h>
#include <assert.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <syslog.h>
#include <net/if.h>

/* Thread safe syslog_r calls need syslog_data structure */
extern struct syslog_data kernel_syslog_data ;

typedef signed char int8;
typedef	unsigned char uint8;
typedef uint8 byte;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int int64;
typedef unsigned long long int uint64;
typedef uint32 BOOL;

void _lslp_term(int sig) ;
int32 _lslp_thread_sig_action(void);
int32 _lslp_sig_action(void) ;
void lslp_linux_wait_mutex(sem_t *h, uint32 m, uint32 *c);
void lslp_linux_sleep(int32 ms);
uint32 lslp_linux_begin_thread(void *(*start)(void *), uint32 ss, void *a);
void  num_to_ascii(uint32 val, int8 *buf, int32 radix, BOOL is_neg);
void  hug_num_to_ascii(uint64 val, int8 *buf, int32 radix, BOOL is_neg); 
BOOL linux_report_event(uint32 code, void *parm, uint32 parmIndex, va_list va) ;
uint32 _lslp_install_nt_service(void) ;
uint32 _lslp_remove_nt_service(void) ;
void linux_get_transport_configuration(void) ;

#define LSLP_DEFAULT_WAIT 100
#define LSLP_EXTRA_WAIT 250
#define _LSLP_CREATE_MUTEX(h) ((sem_init((sem_t *)&(h), 0, 1)) ? -1 : 0)
#undef CREATE_MUTEX
#define CREATE_MUTEX(h) _LSLP_CREATE_MUTEX((h))
#define _LSLP_WAIT_MUTEX(h, i, c)  lslp_linux_wait_mutex(((sem_t *)&(h)), (i), (c))
#undef WAIT_MUTEX
#define WAIT_MUTEX(h, i, c) _LSLP_WAIT_MUTEX((h), (i), (c))
#define _LSLP_CLOSE_MUTEX(h) sem_destroy((sem_t *)&(h))
#undef CLOSE_MUTEX
#define CLOSE_MUTEX(h) _LSLP_CLOSE_MUTEX((h))

#define _LSLP_RELEASE_MUTEX(h) sem_post((sem_t *)&(h))
#undef RELEASE_MUTEX
#define RELEASE_MUTEX(h) _LSLP_RELEASE_MUTEX((h))
#define _LSLP_DUP_MUTEX(h, n) ((sem_t *)&(h))
#undef DUP_MUTEX
#define DUP_MUTEX(h, n) _LSLP_DUP_MUTEX(h, n)
#define _LSLP_CREATE_SEM(h, i) ((sem_init((sem_t *)&(h), 0, (i))) ? -1 : 0)
#define _LSLP_WAIT_SEM _LSLP_WAIT_MUTEX
#define _LSLP_SIGNAL_SEM _LSLP_RELEASE_MUTEX
#define _LSLP_CLOSE_SEM _LSLP_CLOSE_MUTEX

#define LSLP_WAIT_FAILED 0xffffffff
#define WAIT_FAILED LSLP_WAIT_FAILED
#define LSLP_WAIT_OK  0x00000000
#define LSLP_WAIT_TIMEOUT 0xffffffff
#define WAIT_TIMEOUT LSLP_WAIT_TIMEOUT
#define LSLP_WAIT_ABANDONDED 0xffffffff

#define _LSLP_SLEEP(m) usleep((m) * 1000)

#define _LSLP_CLOSESOCKET close
#define _LSLP_IOCTLSOCKET ioctl
#define _LSLP_INIT_NETWORK()
#define _LSLP_DEINIT_NETWORK()

#define _LSLP_SET_TTL(s, t)  setsockopt((s), IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&(t), sizeof((t))) 

#ifndef EINVAL
#define EINVAL 0
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK 0
#endif
#define _LSLP_GETLASTERROR() errno

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SD_BOTH 2

#define FAR

#define LSLP_SEM_T sem_t
#define LSLP_THREAD_T pthread_t

/** void *(*start)(void *), ustacksize, void *arg                   **/
#define _LSLP_BEGINTHREAD(start, stacksize, arg) \
          lslp_linux_begin_thread((start),(stacksize),(arg))

#define _LSLP_STRTOK(n, d, s) strtok_r((n), (d), (s))

#define _LSLP_ENDTHREAD(c)  
#define _LSLP_SIG_ACTION() _lslp_sig_action()
#define _LSLP_SIG_ACTION_THREAD() _lslp_thread_sig_action()
#define _LSLP_PUSH_HANDLER(a, b) pthread_cleanup_push((a), (b))
#define _LSLP_POP_HANDLER(a) pthread_cleanup_pop((a))

#define _LSLP_SERVICE_MAIN() \
{ _startKernel(); while( ! dieNow ) {  _LSLP_SLEEP(waitInterval); } _stopKernel();}

#define LSLP_MAXPATH FILENAME_MAX
#define LSLP_NEWLINE \r

#define SOCKADDR_IN struct sockaddr_in
#define SOCKET int

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define _ultoa(v, b, r) num_to_ascii((uint32)(v), (b), (r), FALSE)
#define _itoa(v, b, r) num_to_ascii((uint32)(v), (b), (r), (((r) == 10) && ((v) < 0)))
#define _ltoa(v, b, r) num_to_ascii((uint32)(v), (b), (r), (((r) == 10) && ((v) < 0)))
#define _ul64toa(v, b, r) huge_num_to_ascii((uint64)(v), (b), (r), FALSE)
#define _i64toa(v, b, r) huge_num_to_ascii((uint64)(v), (b), (r), (((r) == 10) && ((v) < 0)))

#define _LSLP_GET_TRANSPORT linux_get_transport_configuration

/* logging functions */
#define _LSLP_CTIME(t, b)  ctime_r(((const time_t *)(t)), ((char *)(b)))
#define LSLP_SYSLOG_T void *

// Thread safe
#define _LSLP_OPEN_SYSLOG(h, i)  \
                         { ((* ((LSLP_SYSLOG_T *)(h)) = NULL)); \
                           openlog_r(((const char *)(i)), 0, LOG_LOCAL0, &kernel_syslog_data) ; }

// Thread safe
#define _LSLP_CLOSE_SYSLOG(h) closelog_r(&kernel_syslog_data)

// Thread safe
#define _LSLP_REPORT_EVENT_SYSLOG(c, p, i, v) \
                                  linux_report_event(((uint32)(c)), \
                                  ((void *)(p)), \
                                  ((uint32)(i)), \
                                  ((va_list)(v)))

#define LSLP_IN_DEBUGGER FALSE
#define _LSLP_DEBUG_OUT(b) 


/* ascii and char tests and conversions */

#define _LSLP_ISASCII(a) isascii(a)



#ifdef __cplusplus
}
#endif

#endif /* _LSLP_AIX_INCLUDE_ */
