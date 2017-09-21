#ifndef _LSLP_WINDOWS_INCL_
#define _LSLP_WINDOWS_INCL_
#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _WIN32 
#ifndef _MT
#define _MT
#endif


#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>    /* _beginthread, _endthread */
#include <winbase.h>
#include <tchar.h>
#include <sys/types.h> 
#include <sys/timeb.h> 
#include <assert.h>
#include <errno.h>
#include <stdarg.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

  /* control import/export declarations */
#ifdef __DECLSPEC_IMPORT
#define __DECLSPEC DLL_IMPORT
#else 
#ifdef __DECLSPEC_EXPORT
#define __DECLSPEC DLL_EXPORT
#else
#define __DECLSPEC
#endif
#endif


typedef char int8;
typedef	unsigned char uint8;
typedef uint8 byte;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef SOCKET SOCKETD;

#define snprintf sprintf_s
#define INLINE __inline
  
void WindowsStartNetwork(void);
int32 gettimeofday(struct timeval *tv, struct timezone *tz);

/* WIN 32 macros for mutex semaphore */
#define _LSLP_CREATE_MUTEX(h) \
	(((h) = CreateMutex(NULL, FALSE, NULL)) ? 0 : -1)
#define _LSLP_CLOSE_MUTEX(h) \
	if((h) != 0){ReleaseMutex((HANDLE)(h));CloseHandle((HANDLE)(h)); (h) = 0;}
#define LSLP_DEFAULT_WAIT 100
#define LSLP_EXTRA_WAIT   250
/* h = handle, m = milliseconds, c = pointer to completion code */
#define _LSLP_WAIT_MUTEX(h, m, c) \
	if((h) != 0){(*(c)) = WaitForSingleObject((HANDLE)(h), (m));}
#define _LSLP_RELEASE_MUTEX(h) \
	if((h) != 0) {ReleaseMutex((HANDLE)(h));}
#define _LSLP_DUP_MUTEX(h, n) \
	if ((h) != 0)		 \
	{DuplicateHandle(GetCurrentProcess(), (h), GetCurrentProcess(), (n),  \
		0, TRUE, DUPLICATE_SAME_ACCESS | MUTEX_ALL_ACCESS | SYNCHRONIZE);}
#define	_LSLP_CREATE_SEM(h, i) \
	(((h) = (uint32)CreateSemaphore(NULL, (i), 0x0000ffff, NULL)) ? 0 : -1)
/* h = handle, m = milliseconds, c = pointer to completion code */
#define _LSLP_WAIT_SEM(h, m, c) \
	if((h) != 0){(*(c)) = WaitForSingleObject((HANDLE)(h), (m));}
#define _LSLP_SIGNAL_SEM(h)  \
	ReleaseSemaphore((HANDLE)(h), 1, NULL)
#define _LSLP_CLOSE_SEM(h) \
	{if((h) != 0){ReleaseSemaphore((HANDLE)(h), 0x0000fff0, NULL); CloseHandle((HANDLE)(h)); (h) = 0;}}

#define LSLP_SEM_T HANDLE
#define LSLP_THREAD_T HANDLE
#define LSLP_WAIT_FAILED	WAIT_FAILED
#define LSLP_WAIT_OK		WAIT_OBJECT_0
#define LSLP_WAIT_TIMEOUT   WAIT_TIMEOUT
#define LSLP_WAIT_ABANDONDED WAIT_ABANDONED

/* void SLEEP(int32 milliseconds) */
#define _LSLP_SLEEP Sleep

/* void *(*start)(void *), ustacksize, void *arg           */
#define _LSLP_BEGINTHREAD(start, stacksize, arg) _beginthread((start), (stacksize), (arg)) 
#define _LSLP_ENDTHREAD  _endthread 
#define strcasecmp _stricmp
#define _LSLP_STRTOK(n, d, s) strtok((n), (d) )
#define _LSLP_SIG_ACTION()
#define _LSLP_SIG_ACTION_THREAD()
#define _LSLP_PUSH_HANDLER(a, b)
#define _LSLP_POP_HANDLER(a, b)

#define strncasecmp strnicmp

/* maximum path length */
#define LSLP_MAXPATH _MAX_PATH
#define LSLP_NEWLINE \r\n
#endif	 /* WIN 32 */


#define ENOTSOCK WSAENOTSOCK
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL 
#define EAFNOSUPPORT WSAEAFNOSUPPORT 
#define EISCONN WSAEISCONN 
#define ETIMEDOUT WSAETIMEDOUT
#define ECONNREFUSED WSAECONNREFUSED
#define ENETUNREACH WSAENETUNREACH
#define EADDRINUSE WSAEADDRINUSE
#define EINPROGRESS WSAEINPROGRESS 
#define EALREADY WSAEALREADY 
#define EWOULDBLOCK WSAEWOULDBLOCK

#define _LSLP_GETLASTERROR() WSAGetLastError()

#define _LSLP_INIT_NETWORK WindowsStartNetwork
#define _LSLP_DEINIT_NETWORK WSACleanup

#define _LSLP_SOCKET(a, b, c) socket(((int)(a)), ((int)(b)), ((int)(c)))
#define _LSLP_CLOSESOCKET closesocket
#define _LSLP_BIND(a, b, c) bind(((SOCKET)(a)), ((const struct sockaddr *)(b)), ((int)(c)))
#define _LSLP_CONNECT(a, b, c) connect(((SOCKET)(a)), ((const struct sockaddr *)(b)), ((int)(c)))
#define _LSLP_LISTEN(a, b) listen(((SOCKET)(a)), ((int)(b)))
#define _LSLP_ACCEPT(a, b, c) accept(((SOCKET)(a)), ((struct sockaddr *)(b)), ((int *)(c)))
#define _LSLP_SEND(a, b, c, d) send(((SOCKET)(a)), ((const char *)(b)), ((int)(c)), ((int)(d)))
#define _LSLP_SENDTO(a, b, c, d, e, f) \
           sendto(((SOCKET)(a)), ((const char *)(b)), ((int)(c)), ((int)(d)), \
                  ((const struct sockaddr *)(e)), ((int)(f)))
#define _LSLP_RECV(a, b, c, d) recv(((SOCKET)(a)), ((const char *)(b)), ((int)(c)), ((int)(d)))
#define _LSLP_RECV_FROM(a, b, c, d, e, f) \
           recvfrom(((SOCKET)(a)), ((char *)(b)), ((int)(c)), ((int)(d)), \
                    ((struct sockaddr *)(e)), ((int *)(f)))

#define _LSLP_GETHOSTBYNAME(a, b) /* ATTN << Tue Dec 18 13:26:09 2001 mdd >> */
#define _LSLP_GETHOSTBYADDR(a, b, c, d) /* ATTN << Tue Dec 18 13:26:49 2001 mdd >> */
#define _LSLP_SET_TTL(s, t)  setsockopt((s), IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&(t), sizeof((t))) 
#define _LSLP_SETSOCKOPT(a, b, c, d, e) \
           setsockopt(((SOCKET)(a)), ((int)(b)), ((int)(c)), ((const char *)(d)), ((int)(e)))
#define _LSLP_GETSOCKOPT(a, b, c, d, e) \
           getsockopt(((SOCKET)(a)), ((int)(b)), ((int)(c)), ((char *)(d)), ((int *)(e)))
#define _LSLP_ABORT(a) { shutdown(((SOCKET)(a)), SD_BOTH ) ; closesocket((SOCKET)(a)) }
#define LSLP_FD_SET fd_set
#define _LSLP_SELECT(a, b, c, d, e) \
           select(((int)(a)), ((fd_set *)(b)), ((fd_set *)(c)), \
           ((fd_set *)(d)), ((const struct timeval *)(e)))
#define _LSLP_FD_ISSET(a, b)  FD_ISSET(((SOCKET)(a)), ((fd_set *)(b)))
#define _LSLP_FD_SET(a, b) FD_SET(((SOCKET)(a)), ((fd_set *)(b)))
#define _LSLP_FD_ZERO(a) FD_ZERO((fd_set *)(a))
#define _LSLP_FD_CLR(a, b) FD_CLR(((SOCKET)(a)), ((fd_set *)(b)))

#define _LSLP_IOCTLSOCKET ioctlsocket


/* ascii and char tests and conversions */

#define _LSLP_ISASCII(a) __isascii(a)


#ifdef	__cplusplus
}
#endif

#endif /*_LSLP_WINDOWS_INCL_*/

