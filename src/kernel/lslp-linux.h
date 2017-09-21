
/*                                                    */
/* .                             */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                        */
/*                                                                        */
/*                                       */
/*                                                                        */ 
/*                                               */
/*                                                                        */
/*                                                                        */
/*           */
/*                                                     */
/*                                                                        */
/*          */
/*            */
/* .                              */
/*                                                                        */
/*                                                      */




#ifndef _LSLP_LINUX_INCLUDE_
#define _LSLP_LINUX_INCLUDE_

#ifdef __cplusplus
extern "c" {
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h> 
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
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



/**** some older linux systems (Cobalt in particular) may not have the header needed 
      to perform ioctl's on network interfaces. For those systems, declare the necessary
      kernel structures right here. Otherwise use the include file. 
****/
#ifdef HAVE_IF_H
#include <linux/if.h>
#else 

#define	IFF_UP		0x1		/* interface is up		*/
#define	IFF_BROADCAST	0x2		/* broadcast address valid	*/
#define	IFF_DEBUG	0x4		/* turn on debugging		*/
#define	IFF_LOOPBACK	0x8		/* is a loopback net		*/
#define	IFF_POINTOPOINT	0x10		/* interface is has p-p link	*/
#define	IFF_NOTRAILERS	0x20		/* avoid use of trailers	*/
#define	IFF_RUNNING	0x40		/* resources allocated		*/
#define	IFF_NOARP	0x80		/* no ARP protocol		*/
#define	IFF_PROMISC	0x100		/* receive all packets		*/
#define	IFF_ALLMULTI	0x200		/* receive all multicast packets*/

#define IFF_MASTER	0x400		/* master of a load balancer 	*/
#define IFF_SLAVE	0x800		/* slave of a load balancer	*/

#define IFF_MULTICAST	0x1000		/* Supports multicast		*/

#define IFF_VOLATILE	(IFF_LOOPBACK|IFF_POINTOPOINT|IFF_BROADCAST|IFF_MASTER|IFF_SLAVE|IFF_RUNNING)

#define IFF_PORTSEL	0x2000          /* can set media type		*/
#define IFF_AUTOMEDIA	0x4000		/* auto media select active	*/
#define IFF_DYNAMIC	0x8000		/* dialup device with changing addresses*/

struct ifmap 
{
	unsigned long mem_start;
	unsigned long mem_end;
	unsigned short base_addr; 
	unsigned char irq;
	unsigned char dma;
	unsigned char port;
	/* 3 bytes spare */
};

struct ifreq 
{
#define IFHWADDRLEN	6
#define	IFNAMSIZ	16
	union
	{
		char	ifrn_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	} ifr_ifrn;
	
	union {
		struct	sockaddr ifru_addr;
		struct	sockaddr ifru_dstaddr;
		struct	sockaddr ifru_broadaddr;
		struct	sockaddr ifru_netmask;
		struct  sockaddr ifru_hwaddr;
		short	ifru_flags;
		int	ifru_ivalue;
		int	ifru_mtu;
		struct  ifmap ifru_map;
		char	ifru_slave[IFNAMSIZ];	/* Just fits the size */
		char	ifru_newname[IFNAMSIZ];
		char *	ifru_data;
	} ifr_ifru;
};

#define ifr_name	ifr_ifrn.ifrn_name	/* interface name 	*/
#define ifr_hwaddr	ifr_ifru.ifru_hwaddr	/* MAC address 		*/
#define	ifr_addr	ifr_ifru.ifru_addr	/* address		*/
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-p lnk	*/
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address	*/
#define	ifr_netmask	ifr_ifru.ifru_netmask	/* interface net mask	*/
#define	ifr_flags	ifr_ifru.ifru_flags	/* flags		*/
#define	ifr_metric	ifr_ifru.ifru_ivalue	/* metric		*/
#define	ifr_mtu		ifr_ifru.ifru_mtu	/* mtu			*/
#define ifr_map		ifr_ifru.ifru_map	/* device map		*/
#define ifr_slave	ifr_ifru.ifru_slave	/* slave device		*/
#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface	*/
#define ifr_ifindex	ifr_ifru.ifru_ivalue	/* interface index	*/
#define ifr_bandwidth	ifr_ifru.ifru_ivalue    /* link bandwidth	*/
#define ifr_qlen	ifr_ifru.ifru_ivalue	/* Queue length 	*/
#define ifr_newname	ifr_ifru.ifru_newname	/* New name		*/

struct ifconf 
{
	int	ifc_len;			/* size of buffer	*/
	union 
	{
		char *			ifcu_buf;
		struct	ifreq 		*ifcu_req;
	} ifc_ifcu;
};
#define	ifc_buf	ifc_ifcu.ifcu_buf		/* buffer address	*/
#define	ifc_req	ifc_ifcu.ifcu_req		/* array of structures	*/

#endif

typedef char int8;
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
#define _LSLP_OPEN_SYSLOG(h, i)  \
                         { (*((LSLP_SYSLOG_T *)(h)) = NULL); \
                           openlog(((__const char *)(i)), 0, LOG_LOCAL0) ; }
#define _LSLP_CLOSE_SYSLOG(h) closelog()

#define _LSLP_REPORT_EVENT_SYSLOG linux_report_event 

#define LSLP_IN_DEBUGGER FALSE
#define _LSLP_DEBUG_OUT(b) 


/* ascii and char tests and conversions */

#define _LSLP_ISASCII(a) isascii(a)



#ifdef __cplusplus
}
#endif

#endif /* _LSLP_LINUX_INCLUDE_ */
