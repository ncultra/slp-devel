
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
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#include <winbase.h>
#include <tchar.h>
#include <sys/types.h>
#include <sys/timeb.h> 
#include <errno.h>
#include <assert.h>
#include <io.h>

#define HAVE_STRING_H 1		/* hacks so gnu libc fnmatch will work  */
#define STDC_HEADERS 1
#define HAVE_WCTYPE_H 1
#define HAVE_WCHAR_H 1
#define isascii __isascii

typedef char int8;
typedef	unsigned char uint8;
typedef uint8 byte;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

typedef struct _sock_address {
  SOCKADDR_IN *sock_addr;
  int addr_len;
} SOCK_ADDRESS;

typedef struct _SOCK_ADDRESS_LIST {
    int addr_count;
    SOCK_ADDRESS addr[1];
  } SOCK_ADDRESS_LIST ;


void WindowsStartNetwork(void);
int32 gettimeofday(struct timeval *tv, struct timezone *tz);
int gethostbyname_r(const char *name, 
		    struct hostent *resultbuf, 
		    char *buf, 
		    size_t bufsize, 
		    struct hostent **result, 
		    int *errnop);

BOOL windows_report_event(uint32 code, void *parm, uint32 parmIndex, va_list va) ;

VOID WINAPI _lslp_windows_main(void);
VOID WINAPI _lslp_service_start(DWORD argc, LPTSTR *argv) ;
VOID WINAPI _lslp_service_ctrl_handler(DWORD opcode) ;
DWORD _lslp_service_initialization(DWORD argc, LPTSTR *argv, DWORD *specificError) ;
uint32 _lslp_install_nt_service(   ) ;
uint32 _lslp_remove_nt_service(void) ;
  /*  WindowsSetTTL(SOCKET sock, int ttl ) ; */
void windows_get_transport_configuration(void) ;
int ReturnNTVersion() ;
/* WIN 32 macros for mutex semaphore */
#define _LSLP_CREATE_MUTEX(h) \
	(((h) = CreateMutex(NULL, FALSE, NULL)) ? 0 : -1)
#define _LSLP_CLOSE_MUTEX(h) \
	if((h) != 0){ReleaseMutex((HANDLE)(h));CloseHandle((HANDLE)(h)); (h) = 0;}
#define LSLP_DEFAULT_WAIT 100
#define LSLP_EXTRA_WAIT   250
/* h = handle, m = milliseconds, c = pointer to completion code */
#define _LSLP_WAIT_MUTEX(h, m, c) \
	if((h) != 0){(*(c)) = WaitForSingleObject((HANDLE)(h), INFINITE);}
#define _LSLP_RELEASE_MUTEX(h) \
	if((h) != 0) {ReleaseMutex((HANDLE)(h));}
#define _LSLP_DUP_MUTEX(h, n) \
	if ((h) != 0)		 \
	{DuplicateHandle(GetCurrentProcess(), (h), GetCurrentProcess(), (n),  \
		0, TRUE, DUPLICATE_SAME_ACCESS | MUTEX_ALL_ACCESS | SYNCHRONIZE);}
#define	_LSLP_CREATE_SEM(h, i) \
	(((h) = CreateSemaphore(NULL, (i), 0x0000ffff, NULL)) ? 0 : -1)
/* h = handle, m = milliseconds, c = pointer to completion code */
#define _LSLP_WAIT_SEM(h, m, c) \
	if((h) != 0){(*(c)) = WaitForSingleObject((HANDLE)(h), (m)) ;}
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
#define _LSLP_CLOSESOCKET closesocket
#define _LSLP_IOCTLSOCKET ioctlsocket
#define _LSLP_INIT_NETWORK WindowsStartNetwork
#define _LSLP_DEINIT_NETWORK WSACleanup
  //#define _LSLP_SET_TTL(s, ttl) WSAIoctl((s), SIO_MULTICAST_SCOPE, &(ttl), sizeof((ttl))  , NULL, 0 , NULL, NULL, NULL)
#define _LSLP_SET_TTL(s, t)  setsockopt((s), IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&(t), sizeof((t))) 

/* void *(*start)(void *), ustacksize, void *arg           */
#define _LSLP_BEGINTHREAD(start, stacksize, arg) _beginthread( ((void (__cdecl *)(void *))(start)), (unsigned)(stacksize), (void *)(arg)) 
#define _LSLP_ENDTHREAD(c)  _endthreadex(c)

#define strcasecmp _stricmp

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
#ifndef EINVAL
#define EINVAL WSAEINVAL
#endif
  /* to start as a service */
#define _LSLP_SERVICE_MAIN() _lslp_windows_main()

#define _LSLP_GETLASTERROR() WSAGetLastError()


#define _LSLP_STRTOK(n, d, s) strtok((n), (d) )
#define _LSLP_SIG_ACTION()
#define _LSLP_SIG_ACTION_THREAD()
#define _LSLP_PUSH_HANDLER(a, b)
#define _LSLP_POP_HANDLER(a, b)

#define strncasecmp strnicmp

#define _LSLP_GET_TRANSPORT windows_get_transport_configuration


/* maximum path length */
#define LSLP_MAXPATH _MAX_PATH
#define LSLP_NEWLINE \r\n


  /* logging functions */

#define _LSLP_CTIME(t, b) memcpy(((char *)(b)), ctime(((const time_t *)(t))), 26)

#define LSLP_SYSLOG_T HANDLE
#define _LSLP_OPEN_SYSLOG(h, i)  \
     { *((LSLP_SYSLOG_T *)(h)) = RegisterEventSource(NULL, "SLP" ); } 

#define _LSLP_CLOSE_SYSLOG(h) DeregisterEventSource(h)

#define _LSLP_REPORT_EVENT_SYSLOG(c, p, i, v) \
                                  windows_report_event(((uint32)(c)), \
                                  ((void *)(p)), \
                                  ((uint32)(i)), \
                                  ((va_list)(v)))

#define LSLP_IN_DEBUGGER  IsDebuggerPresent()
#define _LSLP_DEBUG_OUT(b) OutputDebugString(b)




/* ascii and char tests and conversions */

#define _LSLP_ISASCII(a) __isascii(a)

#endif	 /* WIN 32 */

#ifdef	__cplusplus
}
#endif

#endif /*_LSLP_WINDOWS_INCL_*/

