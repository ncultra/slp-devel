#ifndef _LSLP_LINUX_INCLUDE_
#define _LSLP_LINUX_INCLUDE_

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h> 
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <assert.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <net/if.h>
typedef char int8;
typedef	unsigned char uint8;
typedef uint8 byte;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef uint32 BOOL;

#ifdef __cplusplus
extern "C" {
#endif

void _lslp_term(int sig) ;
void  num_to_ascii(uint32 val, int8 *buf, int32 radix, BOOL is_neg);
void  hug_num_to_ascii(uint64 val, int8 *buf, int32 radix, BOOL is_neg);

typedef int SOCKETD;

#define MALLOC(x) malloc(x) 
#define FREE(x) free(x)
#define INLINE inline  

#define LSLP_DEFAULT_WAIT 100
#define LSLP_EXTRA_WAIT 250
#define _LSLP_CREATE_MUTEX(h) ((sem_init((sem_t *)&(h), 0, 1)) ? -1 : 0)
#define CREATE_MUTEX(h) _LSLP_CREATE_MUTEX((h))
#define _LSLP_WAIT_MUTEX(h, i, c)  lslp_linux_wait_mutex(((sem_t *)&(h)), (i), (c))
#define WAIT_MUTEX(h, i, c) _LSLP_WAIT_MUTEX((h), (i), (c))
#define _LSLP_CLOSE_MUTEX(h) sem_destroy((sem_t *)&(h))
#define CLOSE_MUTEX(h) _LSLP_CLOSE_MUTEX((h))

#define _LSLP_RELEASE_MUTEX(h) sem_post((sem_t *)&(h))
#define RELEASE_MUTEX(h) _LSLP_RELEASE_MUTEX((h))
#define _LSLP_DUP_MUTEX(h) ((sem_t *)&(h))

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

#define _LSLP_SLEEP(m) \
  { if(m) { \
      struct timespec wait_time , actual_time; \
      wait_time.tv_sec = (m / 1000); ; wait_time.tv_nsec = (((m % 1000) * 1000) * 1000);  \
      nanosleep(&wait_time, &actual_time); \
  } }
  

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SD_BOTH SHUT_RDWR

#define FAR

#define LSLP_SEM_T sem_t
#define LSLP_THREAD_T pthread_t

/** void *(*start)(void *), ustacksize, void *arg                   **/
#define _LSLP_BEGINTHREAD(start, stacksize, arg) \
          lslp_linux_begin_thread((start),(stacksize),(arg))

#define _LSLP_STRTOK(n, d, s) strtok_r((n), (d), (s))

#define _LSLP_ENDTHREAD(handle, code)  pthread_exit((handle))
#define _LSLP_SIG_ACTION() _lslp_sig_action()
#define _LSLP_SIG_ACTION_THREAD() _lslp_thread_sig_action()
#define _LSLP_PUSH_HANDLER(a, b) pthread_cleanup_push((a), (b))
#define _LSLP_POP_HANDLER(a) pthread_cleanup_pop((a))


#define LSLP_MAX_PATH _POSIX_PATH_MAX
#define LSLP_NEWLINE \r

#define SOCKADDR_IN struct sockaddr_in
#define SOCKADDR struct sockaddr
#define SOCKET int


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define _ultoa(v, b, r) num_to_ascii((uint32)(v), (b), (r), FALSE)
#define _itoa(v, b, r) num_to_ascii((uint32)(v), (b), (r), (((r) == 10) && ((v) < 0)))
#define _ltoa(v, b, r) num_to_ascii((uint32)(v), (b), (r), (((r) == 10) && ((v) < 0)))
#define _ul64toa(v, b, r) huge_num_to_ascii((uint64)(v), (b), (r), FALSE)
#define _i64toa(v, b, r) huge_num_to_ascii((uint64)(v), (b), (r), (((r) == 10) && ((v) < 0)))



#define LSLP_HEXDUMP(c) ((((c) > 31) && ((c) < 128)) ? (c) : '.')
#define LSLP_MSG_STRINGS		4
#define LSLP_STRINGS_HEXDUMP 	1
#define LSLP_STRINGS_WORKDUMP   2
#define LSLP_STRINGS_NADUMP     3



#define _LSLP_CLOSESOCKET(s) { \
		if ((s) >= 0); \
		close(s);      \
	}		       \
		
#define _LSLP_SOCKET(a, b, c) socket((int)(a), (int)(b), (int)(c))
#define _LSLP_BIND(a, b, c) bind((int)(a), (const struct sockaddr *)(b), (socklen_t)(c))
#define _LSLP_CONNECT(a, b, c) connect((int)(a), (const struct sockaddr *)(b), (socklen_t)(c))
#define _LSLP_LISTEN(a, b) listen((int)(a), (int)(b))
#define _LSLP_ACCEPT(a, b, c) accept((int)(a), (struct sockaddr *)(b), (socklen_t *)(c))
#define _LSLP_SEND(a, b, c, d) send((int)(a), (const void *)(b), (size_t)(c), (int)(d))
#define _LSLP_SENDTO(a, b, c, d, e, f) \
           sendto((int)(a), (const void *)(b), (size_t)(c), (int)(d), \
                  (const struct sockaddr *)(e), (socklen_t)(f)) 
#define _LSLP_RECV(a, b, c, d) recv((int)(a), (void *)(b), (size_t)(c), (int)(d))
#define _LSLP_RECV_FROM(a, b, c, d, e, f) \
           recvfrom((int)(a), (void *)(b), (size_t)(c), (int)(d), \
                    (struct sockaddr *)(e), (socklen_t *)(f))
#define _LSLP_GETHOSTBYNAME(a) gethostbyname((const char *)(a))
#define _LSLP_GETHOSTBYADDR(a, b, c) gethostbyaddr( (const void *)(a), (socklen_t)(b), (int)(c))
#define _LSLP_SETSOCKOPT(a, b, c, d, e) \
           setsockopt((int)(a), (int)(b), (int)(c), (const void *)(d), (socklen_t)(e))
#define _LSLP_GETSOCKOPT(a, b, c, d, e) \
           getsockopt((int)(a), (int)(b), (int)(c), (void *)(d), (socklen_t)(e))
#define _LSLP_SET_TTL(s, t)  setsockopt((s), IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&(t), sizeof((t))) 

#define _LSLP_ABORT(a) { shutdown((int)(a), SHUT_RDWR) ; close((int)(a)); }
#define LSLP_FD_SET fd_set

#define _LSLP_SELECT(a, b, c, d, e) \
           select((int)(a), (fd_set *)(b), (fd_set *)(c), (fd_set *)(d), (struct timeval *)(e))
#define _LSLP_FD_ISSET(a, b) FD_ISSET((int)(a), (fd_set *)(b))
#define _LSLP_FD_SET(a, b) FD_SET((int)(a), (fd_set *)(b))
#define _LSLP_FD_ZERO(a) FD_ZERO((fd_set *)(a))
#define _LSLP_FD_CLR(a, b) FD_CLEAR((int)(a), (fd_set *)(b))

#define _LSLP_IOCTLSOCKET ioctl
#define _LSLP_GETLASTERROR() errno

#define _LSLP_DEINIT_NETWORK()

#define _LSLP_INIT_NETWORK()
#define _LSLP_DEINIT_NETWORK()

//#define LSLP_MTU 4096
	

/* ascii and char tests and conversions */

#define _LSLP_ISASCII(a) isascii(a)

#ifdef __cplusplus
}
#endif

#endif /* _LSLP_LINUX_INCLUDE_ */
