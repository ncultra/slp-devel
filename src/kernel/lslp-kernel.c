/*                                                    */
/* This an automatically generated prolog.                             */
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



/*******************************************************************     
 *  Description: portable kernel routines for SLP v2   
 *
 *****************************************************************************/



#include "lslp-common-defs.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#ifdef _WIN32_WINNT
#include <winsock2.h>
#else
#include <netinet/tcp.h>
#endif
#ifdef _AIX			// JSS
#include <netdb.h>		// JSS need struct hostent_data for gethostbyname_r()
#include "os400_qslputil.h"	// JSS
#endif				// JSS
/* DA and SA lists */
extern lslpDirAgentHead lslpDAs;
extern lslpDirAgentHead lslpSAs;
extern lslpDirAgentHead lslpRemoteDAs;
extern lslpDirAgentHead lslpRemoteSAs;
extern LSLP_CONFIG_ADV lslpConfigAdv; /* da adv configuration */
extern LSLP_CONFIG_ADV lslpConfigDisc; /* da disc configuration */
extern LSLP_CONFIG_ADV lslpConfigListen; /* listener configuration */
extern LSLP_CONFIG_ADV lslpConfigDontListen ; /* exclude from listening */
extern LSLP_CONFIG_ADV lslpConfigTransport; /* transport configuration */
extern BOOL is_nt_service ;
extern LSLP_SEM_T overload_sem;

/* listeners list */
static struct in_socks listeners = { &listeners, &listeners, TRUE } ;

/* current tcp connections */
lslpStream streamInHead = { &streamInHead, &streamInHead, TRUE } ;
/* incoming mesh list */
/* << Tue Jun 29 16:56:52 2004 mdd >> remove this list as part of rfc 3528 implementation */

/* list of all our addresses */

LSLP_SEM_T xid_sem;
LSLP_SEM_T sessionSem;
struct lslp_ses sessions[LSLP_SESSIONS];

/* semaphores to control work ueue access */
LSLP_SEM_T listSems[LSLP_QS];
lslpWork workHeads[LSLP_QS];
LSLP_SEM_T dit_presence_sem ; /* for use by the LDAP module */
/* semaphores to control threads */
LSLP_SEM_T lslpPrintSem;  /* mutex for the debug output routines */
LSLP_SEM_T urlParseSem;
LSLP_SEM_T attrParseSem;
LSLP_SEM_T opaqueParseSem;
LSLP_SEM_T filterParseSem;

/* thread handles */

LSLP_THREAD_T dispatchHandle;
LSLP_THREAD_T slpOutHandle;
LSLP_THREAD_T slpInHandle;
LSLP_THREAD_T meshHandle;
LSLP_THREAD_T pipeHandle;
int pipe_fd;
BOOL pipe_open;


/* sockets that we need to close during shutdown */



BOOL fwd_DA_necessary = FALSE;

/* die flag */
#ifdef __linux
volatile sig_atomic_t dieNow = 0;
#else
uint32 dieNow = 0;
#endif


uint32 waitInterval = 3;

int8 lslp_default_config[LSLP_MAXPATH];

/* logging variables */

int8 lslp_logPath[LSLP_MAXPATH];
FILE *lslp_logFile = NULL;
int32 lslp_logLevel = (0xffffffff & (~LSLP_LOG_FILE));

/* up time */
time_t up_time;

extern struct timeval peerWait ; /* semi-random peer wait interval*/

/* dispatch table */
static void invalidIn(lslpWork *w);
static void srvrqstIn(lslpWork *w);
static void srvrplyIn(lslpWork *w);
static void srvregIn(lslpWork *w);
static void srvderegIn(lslpWork *w);
static void srvackIn(lslpWork *w);
static void attrreqIn(lslpWork *w);
static void attrrepIn(lslpWork *w);
static void daadvertIn(lslpWork *w);
static void srvtyperqstIn(lslpWork *w);
static void srvtyperplyIn(lslpWork *w);
static void saadvertIn(lslpWork *w);
static void meshctrlIn(lslpWork *w); 



void (*dpt[16])(lslpWork *w) = 
{
  invalidIn, srvrqstIn, srvrplyIn, srvregIn, srvderegIn,  srvackIn, 
  attrreqIn, attrrepIn, daadvertIn, srvtyperqstIn, srvtyperplyIn, saadvertIn, 
  meshctrlIn, invalidIn, invalidIn, invalidIn
};



uint32 _startKernel(void)
{

  _LSLP_SIG_ACTION();
  _LSLP_INIT_NETWORK();  
  if(LSLP_OK == _initWorkLists())
    if (0 == (_LSLP_CREATE_SEM(urlParseSem, 1)))
      	if(0 == (_LSLP_CREATE_SEM(attrParseSem, 1)))
	  if(0 == (_LSLP_CREATE_SEM(opaqueParseSem, 1)))
	    if(0 == (_LSLP_CREATE_SEM(filterParseSem, 1)))
	      if (0 == (_LSLP_CREATE_SEM(sessionSem, 1)))
		if (0 == (_LSLP_CREATE_SEM(lslpPrintSem, 1)))
		  if(0 ==( _LSLP_CREATE_SEM(xid_sem, 1))) 
/* 		    if(0 ==( _LSLP_CREATE_SEM(connectHead.sem, 1)))  << Tue Jun 29 16:56:40 2004 mdd >>*/
		      if(0 ==( _LSLP_CREATE_SEM(streamInHead.sem, 1))) {
			if( -1 == _lslpParseConfigFile(lslp_default_config)) {
			  _report_event(LSLP_SEV_IDX(78 , LSLP_SEVERITY_ERROR), NULL, 0);
			  return(LSLP_NOT_INITIALIZED);
			}
			_LSLP_GET_TRANSPORT() ; 
			/* start the uptime clock */
			time(&up_time);
			gettimeofday(&peerWait, NULL);
			peerWait.tv_sec = ( peerWait.tv_usec % 10)  + 1;
			if (-1 != (int32)(pipeHandle = (LSLP_THREAD_T)_LSLP_BEGINTHREAD(pipeThread , 0, NULL)))
			  if (-1 != (int32)(dispatchHandle = (LSLP_THREAD_T)_LSLP_BEGINTHREAD(_dispatchThread, 0, NULL)))
			    if (-1 != (int32)(slpInHandle = (LSLP_THREAD_T)_LSLP_BEGINTHREAD(slpInThread, 0, NULL)))
			      /* << Tue May 25 11:16:20 2004 mdd >> note - mesh thread disabled while working on rfc 3528 support */ 
			      if (-1 != (int32)(meshHandle = (LSLP_THREAD_T)_LSLP_BEGINTHREAD(lslpMeshServiceThread, 0, &streamInHead))) {
				_report_event( LSLP_SEV_IDX(4 , LSLP_SEVERITY_INFO) , NULL, 0);
				return(LSLP_OK);
			      }
		      }
  _report_event( LSLP_SEV_IDX(5,  LSLP_SEVERITY_ERROR), NULL, 0);
  return(LSLP_NOT_INITIALIZED);
}	

void _stopKernel(void)
{

  /*   dieNow = 1; */
  /* cancel threads here */
  _report_event(LSLP_SEV_IDX(6 , LSLP_SEVERITY_INFO), NULL, 0);
  _LSLP_SLEEP(waitInterval * 4); /* sleep until other threads have had a chance to awaken */
  _LSLP_CLOSE_SEM(lslpPrintSem);
  _LSLP_CLOSE_SEM(xid_sem);
  _LSLP_CLOSE_SEM(opaqueParseSem);
  _LSLP_CLOSE_SEM(attrParseSem);
  _LSLP_CLOSE_SEM(urlParseSem);
  _LSLP_CLOSE_SEM(filterParseSem);
/*   _LSLP_CLOSE_SEM(connectHead.sem); << Tue Jun 29 16:56:29 2004 mdd >> */
  _LSLP_CLOSE_SEM(streamInHead.sem);

  _destroyWorkLists();
  _lslpDestroyDAList((lslpDirAgent *)&lslpSAs, FALSE);
  _lslpDestroyDAList((lslpDirAgent *)&lslpDAs, FALSE);
  _lslpDestroyDAList((lslpDirAgent *)&lslpRemoteDAs, FALSE);
  _lslpDestroyDAList((lslpDirAgent *)&lslpRemoteSAs, FALSE);
  lslpDestroyStreamList(&streamInHead, 0);
  _lslpDestroyConfigAdvertList(&lslpConfigAdv, 0);
  _lslpDestroyConfigAdvertList(&lslpConfigDisc, 0);
  /* close listening sockets */
  _LSLP_DEINIT_NETWORK();	

   if(syslog_is_open == TRUE) {
    _LSLP_CLOSE_SYSLOG(lslp_syslog_handle);
  }
  if(pipe_open == TRUE) {
    close(pipe_fd);
  }
}	

/* work list routines */
uint32 _initWorkLists(void)
{
  int32 i;

  memset(listSems, 0x00, sizeof(LSLP_SEM_T) * LSLP_QS);
  for (i = 0; i < LSLP_QS; i++)
    {
      if(0 != (_LSLP_CREATE_SEM(listSems[i], 1)))
	return(LSLPERR_INVALID_SEM);
    }

  memset(workHeads, 0x00, sizeof(lslpWork) * LSLP_QS);
  for (i = 0; i < LSLP_QS; i++)
    {
      workHeads[i].next = &(workHeads[i]);
      workHeads[i].prev = &(workHeads[i]);
      workHeads[i].isHead = TRUE;
      workHeads[i].type = LSLP_Q_API_IN + i;
    }


  if (_lslpInitSAList()) {
    if (_lslpInitDAList())
      return(LSLP_OK);
  }
  return(LSLPERR_INVALID_SEM);
}	

uint32 _destroyWorkLists(void)
{
  int32 i;
  lslpWork *temp;
  for (i = 0; i < LSLP_QS; i++)
    {
      _LSLP_CLOSE_SEM(listSems[i]);
      while ((temp = workHeads[i].next) && (! _LSLP_IS_HEAD(temp)))
	{	
	  /*	  assert(temp->status == 0L); */
	  _LSLP_UNLINK(temp);
	  if (temp->status & LSLP_STATUS_TCP)
	    if (temp->ses_idx != -1)
	      {
		_LSLP_CLOSESOCKET(temp->ses_idx);	
	      }

	  if (temp->apiSock != 0)
	    {
	      _LSLP_CLOSESOCKET(temp->apiSock);
	    }
	  freeWorkNode(temp);
	}
    }	
  return(LSLP_OK);
}	

lslpWork *allocWorkNode(void)
{
  lslpWork *temp;
  temp = (lslpWork *)calloc(1, sizeof(lslpWork));
  temp->ses_idx = -1;
  temp->apiSock = -1;
  temp->version = LSLP_WORKNODE_VERSION;
  temp->rply.isHead = TRUE;
  temp->rply.next = temp->rply.prev = (lslpWork *)&(temp->rply);
  temp->txerr = stream_txerr;
  return(temp);
}	

void freeWorkNode(lslpWork *w)
{
  assert(w != NULL);

  if((w->hdr.data != NULL ) && (w->hdr.data != &(w->na.static_data[0]))) {
    if(w->hdr.data != w->na.dyn_data) {
      if(w->na.dyn_data != NULL)
	free(w->na.dyn_data);
    }
    free(w->hdr.data);
    w->hdr.data = NULL;
  } else {
    w->hdr.data = NULL;
  }

  if (w->hdr.data != NULL)
    free(w->hdr.data);
  /* mesh-enhanced */
  
  if((w->ses_idx != -1) && (!(w->status & LSLP_STATUS_KEEP_SOCKET)) && (w->status & LSLP_STATUS_TCP)) {
    _LSLP_CLOSESOCKET(w->ses_idx);   
  }
  if(w->apiSock != -1 && (w->status & LSLP_STATUS_TCP)) {
    _LSLP_CLOSESOCKET(w->apiSock);
  }
  while(! _LSLP_IS_EMPTY((lslpWork *)&(w->rply))) {
    lslpWork *temp = w->rply.next;
    _LSLP_UNLINK(temp);
    freeWorkNode(temp);
  }
  free(w);
}	

#if defined( _WIN32 ) || defined( BSD )  
 int gethostbyname_r(const char *name, 
		    struct hostent *resultbuf, 
		    char *buf, 
		    size_t bufsize, 
		    struct hostent **result, 
		    int *errnop) 
{
  name = name;
  resultbuf = resultbuf;
  buf = buf;
  bufsize = bufsize;
  
  if(NULL == (*result = gethostbyname(name))) {
#if defined( _WIN32 ) 
    *errnop = WSAGetLastError();
#else
    *errnop = errno;
#endif 
    return(-1);
  } 
  return(0);
}

#endif

/****************************************************************
 *  GetHostAddresses
 *
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: obtains all network addresses for a multi-homed 
 *				 host. allocates and returns an array of nameAddr 
 *				 structures. The last structure in the array is 
 *				 zeroed out.
 *
 *  RETURNS:    Array of nameAddr structures; last struct in array 
 *				is zeroed out. 
 *
 ***************************************************************/
struct nameAddr *GetHostAddresses(int8 *name)
{
  struct hostent *host;
  struct nameAddr *sockNames = NULL;
  int32 numIfs;
  struct hostent hostbuf;
  uint8 *temp, *tempName;
  uint32 result, err;
  size_t hostbuflen = 1024;

#if 0	// JSS
  /*
   * AIX prototype:  <netdb.h>
   * 
   *   gethostbyname_r(char *hostname, 
   *                   struct hostent *host, 
   *                   struct hostent_data *hostdata);
   *
   * After a successful call to gethostbyname_r()/gethostbyaddr_r(), the
   * structure hostent_data will contain the data to which pointers in
   * the hostent structure will point to.
   */
  struct hostent_data {
        struct  in_addr host_addr;       /* host address pointer */
        char    *h_addr_ptrs[_MAXADDRS + 1];    /* host address */
        char    hostaddr[_MAXADDRS];
        char    hostbuf[_HOSTBUFSIZE + 1];      /* host data */
        char    *host_aliases[_MAXALIASES];
        char    *host_addrs[2];
        FILE    *hostf;
        int     stayopen;               /* AIX addon */
        ulong   host_addresses[_MAXADDRS];
        int     this_service;
        char    domain[256];
        char    *current;
        int     currentlen;
        void    *_host_reserv1;         /* reserved for future use */
        void    *_host_reserv2;         /* reserved for future use */
  } hostdata;                           /*    Actual Addresses. */
#endif	// JSS

#ifdef _AIX 			// JSS
  struct hostent_data hostdata; // JSS
#endif				// JSS

  numIfs = 0;
  host = NULL;
  temp = NULL;
  tempName = NULL;
  temp = malloc(hostbuflen); 

  if(temp != NULL) {
    if(name != NULL) 
      tempName = strdup(name);
    else {
      if(0 == gethostname(temp, hostbuflen)) {
	tempName = strdup(temp);
      } else {tempName = NULL; }
    }
    if(tempName != NULL){
      host = NULL;
#ifdef _AIX 	// JSS
      memset(&hostdata, 0x00, sizeof(hostdata));
      result = gethostbyname_r(tempName, &hostbuf, &hostdata);
      host = &hostbuf;
#else		// JSS
      while(temp != NULL && (result = gethostbyname_r(tempName, &hostbuf, temp, hostbuflen, 
						      &host, &err)) == ERANGE){
	hostbuflen *= 2;
	temp = realloc(temp, hostbuflen);
      }
#endif		// JSS
    }
    if(host != NULL){
      int32 numIfs = 0;
      struct in_addr *ptr;
      int8 **listPtr = host->h_addr_list;
      while ((ptr = (struct in_addr *)*listPtr++)  != NULL) {
	  numIfs++;
      }
      sockNames = (struct nameAddr *)calloc(numIfs + 1, sizeof(struct nameAddr));
      if (sockNames != NULL) {
	struct nameAddr *na = sockNames;
	while (((ptr = (struct in_addr *)*(host->h_addr_list)++) != NULL )) {
	  na->myAddr.sin_addr.s_addr = ptr->s_addr;
	  na->version = LSLP_NAMEADDR_VERSION;
	  na++;
	}	
      }
    }
  } 
  if(temp != NULL)
    free(temp);
  if(tempName != NULL)
    free(tempName);

  return(sockNames);
}	

/****************************************************************
 *  CloseStream
 *
 *
 *  PARAMETERS: IN SOCKET sock
 *
 *  DESCRIPTION: closes the socket 
 *
 *  RETURNS:
 *
 ***************************************************************/
void CloseStream(SOCKET sock)
{
  _LSLP_CLOSESOCKET(sock);
}	

/****************************************************************
 *  JoinMulticast
 *
 *
 *  PARAMETERS: IN in_addr multi the IP multicast group to join
 *				IN SOCKADDR_IN myAddr intface which should join
 *				   the group.
 *			    IN uint16 port the port upon which to join 
 *
 *  DESCRIPTION: Allocates a socket and prepares that socket to
 *               RECIEVE multicast datagrams (not to send). Joins
 *				 multicast group, but DOES NOT set TTL. 
 *
 *  RETURNS: bound socket upon success, zero otherwise
 *
 ***************************************************************/

SOCKET JoinMulticast(struct in_addr multi, struct nameAddr *temp, uint16 port)
{
  struct ip_mreq mreq;

  mreq.imr_multiaddr.s_addr = multi.s_addr;
  mreq.imr_interface.s_addr = temp->myAddr.sin_addr.s_addr;

  if(SOCKET_ERROR == setsockopt(temp->sock,IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq)))
    return(SOCKET_ERROR);
    // its now ready to receive data
  return(0);
}	

SOCKET PrepSendBcast(struct sockaddr_in *iface) 
{
  SOCKET sock;
  int32 i = 1;

  if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    return(INVALID_SOCKET);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&i, sizeof(i) );
  if(SOCKET_ERROR == bind(sock, (struct sockaddr *)iface, sizeof(struct sockaddr_in))) {
    _LSLP_CLOSESOCKET(sock);
    return(INVALID_SOCKET);
  }

  if(SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&i, sizeof(i))) {
    _LSLP_CLOSESOCKET(sock);
    return(INVALID_SOCKET);
  }

  return(sock);
}

/****************************************************************
 *  LeaveMulticast
 *
 *
 *  PARAMETERS: IN SOCKET sock - a bound socket that is a member of 
 *				a multicast group.
 *
 *  DESCRIPTION: leaves the group by closing the socket. 
 *			     Socket is destroyed
 *
 *  RETURNS:
 *
 ***************************************************************/

void LeaveMulticast(SOCKET sock)
{
  shutdown(sock, 0x02 );

  _LSLP_CLOSESOCKET(sock);
}	

/****************************************************************
 *  PrepSendMulticast
 *
 *
 *  PARAMETERS: IN in_addr intface - IP address used to send to the 
 *			     multicast group
 *				IN int32 ttl - the Time To Live value for multicast datagrams
 *				   sent by this socket. 
 *
 *  DESCRIPTION: prepares a datagram socket for sending to 
 *				 multicast or broadcast addresses. Sets the multicast 
 *				 sending interface to in_addr.
 *
 *  RETURNS: SOCKET sock - a socket ready for sending to the 
 *			 multicast group address or to a broadcast address, 
 *			 or zero if error. 
 *
 ***************************************************************/
SOCKET PrepSendMulticast(struct sockaddr_in *iface, int32 ttl)
{
  SOCKET sock;
  struct sockaddr_in ma;
  int32 ccode = 1;

  if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    return(INVALID_SOCKET);

  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&ccode, sizeof(ccode) );

  if(SOCKET_ERROR == bind(sock, (struct sockaddr *)iface, sizeof(struct sockaddr_in))) {
    _LSLP_CLOSESOCKET(sock);
    return(INVALID_SOCKET);
  }

  if(lslp_mcast_int != INADDR_ANY ) {
    memset(&ma, 0x00, sizeof(ma));
    ma.sin_addr.s_addr = 	lslp_mcast_int ;
    if(SOCKET_ERROR == setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&ma, sizeof(struct sockaddr_in))) {
      ccode = errno;
      _LSLP_CLOSESOCKET(sock);
      return(INVALID_SOCKET);
    }
  }

  bind(sock, (struct sockaddr *)iface, sizeof(struct sockaddr_in));
  if(SOCKET_ERROR == _LSLP_SET_TTL(sock, ttl)) {
    _LSLP_CLOSESOCKET(sock);
    return(INVALID_SOCKET);
  }
  return(sock);
}	

/****************************************************************
 *  connectedRead
 *
 *
 *  PARAMETERS: SOCKET fd - a socket that is connected.
 *				void *pv  - buffer to read into
 *				size_t n - number of bytes to read 
 *
 *  DESCRIPTION: waits 2 seconds for a connected socket to have
 *				 data - timesout and returns zero if no data
 *
 *  RETURNS: num bytes or <0 if error
 *
 ***************************************************************/
int32 connectedRead(SOCKET fd, void *pv, size_t n)
{

  size_t nleft = n;
  int32    nread = 0, iErr;
  int8   *pc   = (int8 *) pv;
  fd_set fds;

  while (nleft > 0) {
    /* i can get rid of this select later */
    struct timeval tv = { 1, 0 };
    FD_ZERO(&fds);
    FD_SET(fd,&fds);
    iErr = select(FD_SETSIZE ,&fds,NULL,NULL,&tv);
    if (iErr < 0) {
      if(errno != EINTR)
	return(iErr);
    } 
    else if (iErr == 0) { /* time expired */
      _report_event(LSLP_SEV_IDX(8 , LSLP_SEVERITY_ERROR), NULL, 0, fd);
	  break;
    } 
    else {
      if ((nread = recv(fd, pc, nleft, 0)) < 0) {  /* socket error */
	if (errno == EINTR)   /* blocking call was interrupted */
	  nread = 0; /* and call read() again */
	else {
	  _report_event(LSLP_SEV_IDX(10 , LSLP_SEVERITY_ERROR), NULL, 0, fd);
	  return -1;	
	}   
      } 
      else if (nread == 0) 
	break;   /* EOF  - connection closed */
      nleft -= nread;
      pc    += nread;
    }
  }
  _report_event(LSLP_SEV_IDX(9 , LSLP_SEVERITY_DEBUG), NULL, 0, n - nleft, "tcp", fd);
   { 
     lslpStream *peer = _lslpStreamLockFindSock(&streamInHead, fd); 
     if(peer != NULL) { 
       time(&(peer->last_read)); 
       _LSLP_SIGNAL_SEM(streamInHead.sem); 
     } 
   } 
  return (n - nleft); /* return >= 0 */
}


/****************************************************************
 *  connectedWrite
 *
 *
 *  PARAMETERS: SOCKET fd - a connected socket
 *				void *pv - buffer to write
 *				size_t n - number of bytes to write

 *  DESCRIPTION:
 *
 *  RETURNS: 
 *
 ***************************************************************/
int32 connectedWrite(SOCKET fd, void *pv, size_t n)
{

  size_t nleft = n;
  int32 nwritten = 0;
  const int8 *pc = (int8 *) pv;
  while(nleft > 0) {
    if ((nwritten = send(fd, pc, nleft, 0)) < 0) {
      if (errno == EINTR) 
	nwritten = 0; /* call write() again */
      else {
	_report_event(LSLP_SEV_IDX(12 , LSLP_SEVERITY_ERROR), NULL, 0, fd);
	return -1;                   /* error              */
      }
    }
    nleft -= nwritten;
    pc    += nwritten;
  }
  _report_event(LSLP_SEV_IDX(11 , LSLP_SEVERITY_DEBUG), NULL, 0, n, "tcp", fd);
   { 
     lslpStream *peer = _lslpStreamLockFindSock(&streamInHead, fd); 
     if(peer != NULL) { 
       time(&(peer->last_write)); 
       _LSLP_SIGNAL_SEM(streamInHead.sem); 
     } 
   } 
  return(n);
}

/****************************************************************
 *  dgramWrite
 *
 *
 *  PARAMETERS: IN OUT lslpWork *work contains status info
 *				IN buf contains the data to be transmitted
 *				IN bufsize contains the number of bytes to be 
 *			    	transmitted. 
 *
 *  DESCRIPTION: Opens and binds a socket, then transmits data. 
 *				 alters work->status flags if data is transmitted. 
 *
 *  RETURNS:	 number of bytes transmitted. 
 *
 ***************************************************************/
int32 dgramWrite(lslpWork *work, void *buf, int32 bufSize, SOCKADDR_IN *bind_addr)
{
  SOCKET sock;
  int32 bytes;
  struct sockaddr_in local_addr;
  int ccode;
  assert(work != NULL);
  assert( work->status & LSLP_STATUS_UDP ) ;
  if (! (work->status & (LSLP_STATUS_MCAST | LSLP_STATUS_BCAST)))
    assert(work->ses_idx >= 0 && work->ses_idx < LSLP_SESSIONS);

  if ((work->status & LSLP_STATUS_CONVERGE) || 
      (work->status & LSLP_STATUS_MCAST)) {
    if (INVALID_SOCKET ==  (sock = PrepSendMulticast(&(work->na.myAddr), lslp_config_ttl ))){
      _report_event(LSLP_SEV_IDX(13 , LSLP_SEVERITY_ERROR), NULL, 0);
	  return(-1);
    }
  } else {
    if (work->status & LSLP_STATUS_BCAST) { 
      if(INVALID_SOCKET == (sock = PrepSendBcast(&(work->na.myAddr))))
	return(-1);
    } else {
      /* really I should change this to use the address that is stored */
      /* in the session structure */
      local_addr.sin_family = AF_INET;
      local_addr.sin_port =  htons(lslp_config_opport);
      local_addr.sin_addr.s_addr = INADDR_ANY;
      if (INVALID_SOCKET == (sock = socket(AF_INET, SOCK_DGRAM, 0))) {
	_report_event(LSLP_SEV_IDX(14 , LSLP_SEVERITY_ERROR), NULL, 0);
	perror(NULL);
	assert(0);
	return(-1);
      } 
      bytes = 1;
      ccode = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&bytes, sizeof(bytes) );
      if (SOCKET_ERROR == bind(sock, (struct sockaddr *)&local_addr, 
			     sizeof(struct sockaddr_in))) {
	_report_event(LSLP_SEV_IDX(15 , LSLP_SEVERITY_ERROR), NULL, 0);	
	_LSLP_CLOSESOCKET(sock);
	return(-1);
      }
    }
  }
  bytes = 1;

  if(work->status & LSLP_STATUS_MCAST) {
        SOCKADDR_IN mca;
      mca.sin_family = AF_INET;  
      mca.sin_port =  htons(lslp_config_opport);    
      mca.sin_addr.s_addr = _LSLP_MCAST;
      setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (const char *)&bytes, sizeof(bytes));
      do {
	bytes = sendto(sock, buf, bufSize, 0,  (struct sockaddr *)&mca , sizeof(SOCKADDR_IN ) );
      } while((bytes == -1) && (errno == EINTR));
  } else {
    if (work->status & LSLP_STATUS_BCAST) {
      do {
	bytes = sendto(sock, buf, bufSize, 0, (struct sockaddr *)&work->na.rmtAddr, sizeof(SOCKADDR_IN)) ;
      } while((bytes == -1) && (errno == EINTR)) ;
    } else {
      do{
	bytes = sendto(sock, buf, bufSize, 0,  
		       (struct sockaddr *)&(sessions[work->ses_idx].rmtAddr),
		       sizeof(SOCKADDR_IN) );
      }while((bytes == -1) && (errno == EINTR));
    } 
  }
  if (bytes && (bytes != SOCKET_ERROR)) {
    work->na.data_len = bytes;
    _report_event(LSLP_SEV_IDX(11 , LSLP_SEVERITY_DEBUG), NULL, 0, bytes, "udp", sock);  
      sessions[work->ses_idx].xid = work->hdr.xid;
      /* if this is a first-try request dgram, reset the session timer */
      if ((work->status & LSLP_STATUS_RQ) && ! (work->status & LSLP_STATUS_RETX))
	rtt_newpack(&(sessions[work->ses_idx]).timer);
      /* if this is a request dgram, update the session timer */
      if (work->status & LSLP_STATUS_RQ) {
	  work->status |= LSLP_STATUS_TX;
	  rtt_start(&(sessions[work->ses_idx]).timer);
	  /* timeout for this dgram is now in timer.currto */
      }
  } 
  _LSLP_CLOSESOCKET(sock);
  return(bytes);
}	

/****************************************************************
 *  _getSession
 *
 *
 *  PARAMETERS:	IN struct nameAddr *na contains sending and receiving
 *					interface information. 
 *
 *  DESCRIPTION: Sessions are for calculating the retry interval for
 *				 Udp requests from one interface to another. The retry
 *               interval is recalculated every time a req-resp round
 *				 trip is successful. 
 *
 *  RETURNS: Index to an allocated session or -1 if no sessions can
 *			 be allocated. 
 *
 ***************************************************************/
int32 _getSession(struct nameAddr *na)
{
  int32 ccode;
  assert(na != NULL);
  
  _LSLP_WAIT_SEM(sessionSem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if (ccode == LSLP_WAIT_OK) {
    ccode = getSession(na);
    _LSLP_SIGNAL_SEM(sessionSem);
  } else {
    _report_event(LSLP_SEV_IDX(17, LSLP_SEVERITY_ERROR), NULL, 0);
    ccode = -1;
  }
  return(ccode);
}	

/****************************************************************
 *  getSession
 *
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: interally called routine - does not lock session 
 *              semaphore. First attempts to allocate an unused session.
 *				If all sessions have been used, allocates the least
 *				recently used session. If all sessions are currently
 *				in use, returns -1 which indicates no sessions are 
 *				available for allocation. 
 *				
 *
 *  RETURNS: session index or -1.
 *
 ***************************************************************/
int32 getSession(struct nameAddr *na)
{
  int32 i, oldestIndex, orphanedIndex;
  int8 *bptr;
  struct timeval oldest;
  assert(na != NULL);
  /* initialize the 'oldest' timer and index */
  gettimeofday(&oldest, NULL);
  oldestIndex = -1, orphanedIndex = -1;
  /* extract the ip address */
  bptr = inet_ntoa(*(struct in_addr *) &na->rmtAddr.sin_addr.s_addr);

  for (i = 0; i < LSLP_SESSIONS; i++) {
    if (sessions[i].rmtAddr.sin_port  == 0L ) {
      /* we went beyond the end of all allocated sessions without */
      /* finding a match - start a new session */
      gettimeofday(&(sessions[i].last), NULL);
      memcpy(&(sessions[i].rmtAddr), &(na->rmtAddr), sizeof(SOCKADDR_IN));
      /*       memcpy(&(sessions[i].myAddr), &(na->myAddr), sizeof(SOCKADDR_IN)); */
      rtt_init(&(sessions[i].timer));
      _report_event(LSLP_SEV_IDX(18, LSLP_SEVERITY_DEBUG), NULL, 0, i);
      return(i);
    }
    /* do we need to update the oldest info ? */
    if ((sessions[i].last.tv_sec < oldest.tv_sec) || 
	((sessions[i].last.tv_sec == oldest.tv_sec) &&     
	 (sessions[i].last.tv_usec < oldest.tv_usec))) {
      /* find out if we need to orphan this session */
      if(orphanedIndex == -1) {
       	struct timeval now;
	gettimeofday(&now, NULL);
	if (now.tv_sec - sessions[i].last.tv_sec >= LSLP_ORPHAN_WAIT) {
	  _report_event(LSLP_SEV_IDX(19, LSLP_SEVERITY_DEBUG), NULL, 0, i, now.tv_sec - sessions[i].last.tv_sec);
	  orphanedIndex = i;
	  /* save the index of this orphaned session so we can use it if we need to */
	  /* but only save the index of the first orphaned session we see -- */
	  /* that way the orphaned session will always be as close to the front of */
	  /* the session array as possible. */
	}
      } else {        /* yes we do need to update the oldest ! */
	oldest.tv_sec = sessions[i].last.tv_sec;
	oldest.tv_usec = sessions[i].last.tv_usec;
	oldestIndex = i;
      }
    }
    if (sessions[i].rmtAddr.sin_port != 0 && 
	sessions[i].rmtAddr.sin_addr.s_addr == na->rmtAddr.sin_addr.s_addr) {
      /* this target might use a different port each time he sends us something */ 
      sessions[i].rmtAddr.sin_port = na->rmtAddr.sin_port;
      /* this is a previously initialized session for the remote address */
      gettimeofday(&(sessions[i].last), NULL);
      /* don't init the session timer - this is an existing session */
      _report_event(LSLP_SEV_IDX(20 , LSLP_SEVERITY_DEBUG), NULL, 0, i);
      return(i);
    }
  } /* for the number of sessions */

  /* there are no free sessions and no existing sessions we can use */
  /* we first try to use an orphaned session. If that doesn't work, */
  /* we steal the oldest session from whomever is using it now. */
  /* either we allocate the oldest session for ourselves */
  /* or we return a -1, which will cause the work node to */
  /* be put on the waiting list. */
  if (orphanedIndex != -1 || oldestIndex != -1 ) {
    int32 tempIndex;
    if(orphanedIndex != -1)
      tempIndex = orphanedIndex;
    else 
      tempIndex = oldestIndex;
    gettimeofday(&(sessions[tempIndex].last), NULL);
    memcpy(&(sessions[tempIndex].rmtAddr), &(na->rmtAddr), sizeof(SOCKADDR_IN));
    /*    memcpy(&(sessions[tempIndex].myAddr), &(na->myAddr), sizeof(SOCKADDR_IN)); */
    /* init the session timer */
    rtt_init(&(sessions[tempIndex].timer));
    _report_event(LSLP_SEV_IDX(21, LSLP_SEVERITY_DEBUG), NULL, 0, i);
    return(tempIndex);
  }
  return(oldestIndex);
}	

/****************************************************************
 *  _insertWorkNode
 *
 *
 *  PARAMETERS: IN OUT lslpWork *node 
 *
 *  DESCRIPTION: node->type determines which list the node will be
 *              inserted in. node is always inserted at the end
 *  		of the list. In order to insert the node, routine 
 *		must successfully lock the listhead semaphore. 
 *
 *  RETURNS: LSLP_WAIT_OK if successful, LSLP_WAIT_FAILED otherwise. 
 *
 ***************************************************************/
uint32 _insertWorkNode(lslpWork *node)
{
  uint32 ccode;
  assert(node != NULL);	
  assert(node->type >= 0 && node->type < LSLP_QS);
  _LSLP_WAIT_SEM((listSems[node->type]), LSLP_Q_WAIT, &ccode); 
  assert(ccode == LSLP_WAIT_OK);
  /* insert at the end of the list */
  if (ccode == LSLP_WAIT_OK) {
    int q = node->type;
    gettimeofday(&node->timer, NULL);
    _LSLP_INSERT_BEFORE(node, (&workHeads[node->type]));
    _LSLP_SIGNAL_SEM((listSems[node->type]));
    _report_event(LSLP_SEV_IDX(24, LSLP_SEVERITY_DEBUG), NULL, 0, q);
  } else {
    _report_event(LSLP_SEV_IDX(23, LSLP_SEVERITY_ERROR), node,
		 (LSLP_PARM_WORK | sizeof(node)), node->type);
    node->hdr.errCode = -1;
  }
  return(ccode);
}	

/****************************************************************
 *  _lockList
 *
 *
 *  PARAMETERS: IN int32 index determines the list which will be 
 *				locked.
 *
 *  DESCRIPTION: attempts to gain ownership of the list's semaphore. 
 *
 *  RETURNS: LSLP_WAIT_OK if successful, LSLP_WAIT_FAILED otherwise. 
 *
 ***************************************************************/
uint32 _lockList(int32 index)
{
  uint32 ccode;
  assert(index >= 0 && index < LSLP_QS);
  _LSLP_WAIT_SEM(listSems[index], LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  return(ccode);
}	

/****************************************************************
 *  _unlockList
 *
 *
 *  PARAMETERS: IN int32 index determines the list which will be unlocked. 
 *
 *  DESCRIPTION: relinquishes ownership of the listhead semaphore. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void _unlockList(int32 index )
{
  assert(index >= 0 && index < LSLP_QS);
  _LSLP_SIGNAL_SEM(listSems[index]);
}

/****************************************************************
 *  _unlinkWorkNode
 *
 *
 *  PARAMETERS: IN OUT lslpWork *node is the node to be unlinked from
 *				its list. 
 *
 *  DESCRIPTION:  Locks listhead, unlinks node, unlocks listhead. Does
 *				  not care about the location of the node in the list, 
 *				  except that node MUST NOT be the listhead. 
 *
 *  RETURNS:  LSLP_WAIT_OK if successful, LSLP_WAIT_FAILED otherwise
 *
 ***************************************************************/
uint32 _unlinkWorkNode(lslpWork *node)
{
  uint32 ccode;
  assert(node != NULL);
  assert(node->isHead == FALSE);	
  assert(node->type >= 0 && node->type < LSLP_QS);
  _LSLP_WAIT_SEM((listSems[node->type]), LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if (ccode == LSLP_WAIT_OK) {
    _LSLP_UNLINK(node);
    _LSLP_SIGNAL_SEM((listSems[node->type]));
    _report_event(LSLP_SEV_IDX(25, LSLP_SEVERITY_DEBUG), node, 
		 (LSLP_PARM_WORK | sizeof(node)), node->type);
  } else {
    _report_event(LSLP_SEV_IDX(23, LSLP_SEVERITY_ERROR), node, 
		 (LSLP_PARM_WORK | sizeof(node)), node->type);
  }
  return(ccode);
}	

/****************************************************************
 *  _unlinkFirstWorkNode
 *
 *
 *  PARAMETERS: IN int32 index
 *
 *  DESCRIPTION: Unlinks the first node from the list indicated by index.
 *				 attempts to lock the listhead, if successful unlinks the 
 *				 first node, unlocks the listhead, and returns.  If the 
 *				 list is empty, simply returns NULL.
 *
 *  RETURNS: pointer to a worknode or NULL. 
 *
 ***************************************************************/
lslpWork *_unlinkFirstWorkNode(int32 index)
{
	
  uint32 ccode;
  lslpWork *work = NULL;
  assert(index >= 0 && index < LSLP_QS);
  _LSLP_WAIT_SEM((listSems[index]), LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  /* unlink from the front of the list */
  if (ccode == LSLP_WAIT_OK) {
    if (! _LSLP_IS_EMPTY(&(workHeads[index]))) {
      work = workHeads[index].next;
      _LSLP_UNLINK(work);
    }
    _LSLP_SIGNAL_SEM((listSems[index]));
    if(work != NULL) {
      _report_event(LSLP_SEV_IDX(26, LSLP_SEVERITY_DEBUG), work, 
		   (LSLP_PARM_WORK | sizeof(work)), index);
    }
  } else {
    _report_event(LSLP_SEV_IDX(23, LSLP_SEVERITY_ERROR), NULL, 0, index);
  }
  return(work);
}	

/****************************************************************
 *  setRQRPStatus
 *
 *
 *  PARAMETERS: IN OUT lslpWork *work is a work node that has an 
 *			   initialized SLP header. 
 *
 *  DESCRIPTION: reads work->hdr.msgid and decides if the worknode is
 *				 for an SLP request or an SLP reply. sets work->status 
 *				 accordingly. 
 *
 *  RETURNS: modifies work->status. 
 *
 ***************************************************************/
void setRQRPStatus(lslpWork *work)
{
  assert(work != NULL);
  
  if (work->hdr.msgid == 2 || 
      work->hdr.msgid == 5 ||
      work->hdr.msgid == 7 || 
      work->hdr.msgid == 10)
    work->status |= LSLP_STATUS_RP;
  else if(work->hdr.msgid == 12)
    work->status |= (LSLP_STATUS_RQ | LSLP_STATUS_RP);
  else
    work->status |= LSLP_STATUS_RQ;
  return;
}	

/****************************************************************
 *  setHdrAndRqRpStatus
 *
 *
 *  PARAMETERS: IN OUT lslpWork *work is a work node that contains 
 *					the SLP header and data buff that need to be initialized
 *
 *  DESCRIPTION: work->hdr MUST be initialized, and work->hdr.data MUST 
 *				 point to an allocated buffer. 
 *
 *  RETURNS:  work->hdr.flags and work->hdr.data modified
 *
 ***************************************************************/
void setHdrAndRqRpStatus(lslpWork *work)
{
  assert(work != NULL);
  assert(work->hdr.data != NULL);
  /* finish initializing the header, then copy the data */
  work->hdr.ver = LSLP_PROTO_VER;
  work->hdr.msgid = _LSLP_GETFUNCTION((uint8 *)work->hdr.data);
  work->hdr.len = _LSLP_GETLENGTH((uint8 *)work->hdr.data);
  work->hdr.flags = _LSLP_GETFLAGS((uint8 *)work->hdr.data);
  work->hdr.nextExt = _LSLP_GETFIRSTEXT((uint8 *)work->hdr.data);
  work->hdr.xid = _LSLP_GETXID((uint8 *)work->hdr.data);
  work->hdr.langLen = _LSLP_GETLANLEN((uint8 *)work->hdr.data);
  memcpy(work->hdr.lang, &(((uint8 *)work->hdr.data)[LSLP_LAN]), _LSLP_MIN(18, work->hdr.langLen));
  setRQRPStatus(work);
	
}	

/****************************************************************
 *  kapiErr
 *
 *
 *  PARAMETERS: IN SOCKET sock - a connected socket upon which to send
 *						  an error response to the API client. 
 *
 *  DESCRIPTION: sends a tcp message to the API client indicating an 
 *					internal error occured. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void apiErr(SOCKET sock)
{
  lslpWork tempWork;
  memset(&tempWork, 0x00, sizeof(tempWork));
  tempWork.hdr.errCode = LSLP_INTERNAL_ERROR;
  _report_event(LSLP_SEV_IDX(30, LSLP_SEVERITY_ERROR), NULL, 0, sock);
  connectedWrite(sock, &tempWork, sizeof(tempWork));
  return;
}	

/****************************************************************
 *  formSLPMsg
 *
 *
 *  PARAMETERS: IN lslpHdr *hdr points to an initialized SLP header.
 *				IN void *buf contains the SLP msg data.
 *				IN int32 bufsize contains the size of the SLP msg data, in bytes.  
 *
 *  DESCRIPTION: Given an initialized lslpHdr struct and an initialized
 *				 SLP payload, allocates and stuffs a buffer with a correctly 
 *				 formed SLP message.
 *
 *  RETURNS:  An allocated buffer containing a fully formed SLP msg or NULL. 
 *
 ***************************************************************/
uint8 *formSLPMsg(lslpHdr *hdr, void *buf, int32 bufSize, BOOL allocFlag )
{
  int32 hdrLen;
  int8 *rBuf;

  assert(hdr != NULL);
  assert(buf != NULL);

  hdrLen = 14 + hdr->langLen;
  if(allocFlag == TRUE) {
    rBuf = (uint8 *)calloc(hdrLen + bufSize, sizeof(uint8));
  } else {
    rBuf = (hdr->data);
  }
  assert(rBuf != NULL);
  if (rBuf != NULL)
    {
      hdr->len = hdrLen + bufSize;
      _LSLP_SETVERSION(rBuf, hdr->ver);
      _LSLP_SETFUNCTION(rBuf, hdr->msgid);
      _LSLP_SETLENGTH(rBuf, hdr->len);	
      _LSLP_SETFLAGS(rBuf, hdr->flags);
      _LSLP_SETLAN(rBuf, hdr->lang);
      if( hdr->nextExt && rBuf + hdr->nextExt  < rBuf + bufSize ) {
	_LSLP_SETFIRSTEXT(rBuf, hdr->nextExt + hdrLen);
      } else {
	_LSLP_SETFIRSTEXT(rBuf, 0x00000000);
	hdr->nextExt = 0;
      }
      _LSLP_SETXID(rBuf, hdr->xid);
      memcpy(rBuf + _LSLP_HDRLEN(rBuf), buf, bufSize);

      /* normalize extension offsets. We assume that extension offsets */
      /* in the buffer need to be adjusted by because they were */
      /* calculated before the length of the header was known. */
      if(hdr->nextExt != 0 ) {
	int8* first_ext;
	int8* this_ext;
	
	int8* next_ext;
	int offset;
	int normal_offset;
	
	this_ext = first_ext = rBuf + hdr->nextExt + hdrLen;
	
	offset = _LSLP_GETNEXTEXT(this_ext, 2);
	while(offset && (offset + rBuf) < (hdr->len + rBuf) ) {
	  next_ext = first_ext + offset;
	  normal_offset = next_ext - rBuf;
	  if(rBuf + normal_offset < rBuf + hdr->len) {
	    _LSLP_SETNEXTEXT(this_ext, normal_offset, 2);
	    this_ext = rBuf + normal_offset;
	    offset = _LSLP_GETNEXTEXT(this_ext, 2);
	  } else {
	    offset = 0;
	  }
	  
	}
	
      }
    }
  return(rBuf);
}	

/****************************************************************
 *  internalErr
 *
 *
 *  PARAMETERS: IN int32 xid is the SLP msg xid to which this error 
 *				response belongs. 
 *
 *  DESCRIPTION: Forms an SLP SRVACK with the error code LSLP_INTERNAL_ERROR.
 *
 *  RETURNS:  a buffer with the correctly formed SRVACK or NULL. 
 *
 ***************************************************************/
uint8 *internalErr(int32 xid, int8 err)
{
  lslpHdr hdr;
  uint8 data = err;
  memset(&hdr, 0x00, sizeof(hdr));
  hdr.ver = LSLP_PROTO_VER;
  hdr.msgid = LSLP_SRVACK;
  /* called routine will set length */
  hdr.flags = 0;
  hdr.nextExt = 0;
  hdr.xid = xid;
  strcpy(hdr.lang, LSLP_EN_US);
  hdr.langLen = strlen(hdr.lang);
  hdr.data = &data;
  hdr.data = formSLPMsg(&hdr, &data, sizeof(data), FALSE);
  _report_event(LSLP_SEV_IDX(31, LSLP_SEVERITY_ERROR), hdr.data, 
	       (LSLP_PARM_MSG | _LSLP_GETLENGTH(((uint8 *)hdr.data))));
  return((uint8 *)hdr.data);
}	

/****************************************************************
 *  tcpInternalErr
 *
 *
 *  PARAMETERS:  IN SOCKET sock the connected socket upon which to 
 *					send the SRVACK msg. 
 *				IN int32 xid - the xid that corresponds the the slp
 *				msg that generated the internal error. 
 *
 *  DESCRIPTION: Sends an INTERNAL ERROR SRVACK over TCP. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void tcpInternalErr(SOCKET sock, int32 xid, int8 err)
{
  uint8 *buf = internalErr(xid, err);
  if (buf != NULL)
    connectedWrite(sock, buf, (_LSLP_GETLENGTH(buf)));
  return;
}	

/****************************************************************
 *  udpInternalErr
 *
 *
 *  PARAMETERS:	IN SOCKADDR_IN *rmt is the destination address
 *				IN SOCKADDR_IN *my is the local interface
 *				IN int32 xid is the xid of the SLP msg that generated
 *				the internal error 
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
 ***************************************************************/
void udpInternalErr(SOCKADDR_IN *rmt, SOCKADDR_IN *my, int32 xid, int8 err)
{
  SOCKET sock;
  BOOL reuse = TRUE;
  if (INVALID_SOCKET != (sock = socket(AF_INET, SOCK_DGRAM, 0))) {
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));
    if (SOCKET_ERROR != bind(sock, (struct sockaddr *)&my, sizeof(SOCKADDR_IN))) {
      uint8 *buf = internalErr(xid, LSLP_INTERNAL_ERROR);	
      if (buf != NULL) {
	do {
	  err = sendto ( sock, buf, (_LSLP_GETLENGTH(buf)), 
			 0, (struct sockaddr FAR *)rmt, sizeof(SOCKADDR_IN) );
	} while((err == -1) && (errno == EINTR));
      }
    }
  }
  return;
}	



/****************************************************************
 *  socketOpen
 *
 *
 *  PARAMETERS: IN SOCKET sock - the socket in question
 *
 *  DESCRIPTION:  Determines whether or not a stream socket is 
 *				 still connected. 
 *
 *  RETURNS: 0 if socket is closed, > 0 if socket is open, -1 if socket error
 *
 ***************************************************************/
int32 socketOpen(SOCKET sock)
{
  /* see if there are more messages available for reading */
  /* on this connected socket. */
  int err;
  fd_set fds;
  struct timeval tv = {0,0};
  assert(sock != SOCKET_ERROR );
  FD_ZERO(&fds);
  FD_SET(sock, &fds);
  do {
    err = select(FD_SETSIZE, &fds, NULL, NULL, &tv);
  } while((err < 0) && (errno == EINTR));
  if (0 < err ) {
      uint8 peekbuf[LSLP_MIN_HDR + 1  + sizeof(struct apiAddr)];
      int32 bytesRead;
      /* recv peek - if no data, session is closed */
      do {
	bytesRead = recv(sock, peekbuf, LSLP_MIN_HDR + sizeof(struct apiAddr), MSG_PEEK );
      }while((bytesRead < 0) && (errno == EINTR));
      return(bytesRead);
    } /* if the socket is ready to read */
  return(-1);
}	

/****************************************************************
 *  dispatchSLP
 *
 *
 *  PARAMETERS: IN lslpWork *newWork holds the incoming SLP msg. 
 *
 *  DESCRIPTION: Given an worknode with an initialized data buffer, 
 *				 set the worknode header and status and insert into
 *				the SLP_IN queue. Previously to making this call, 
 *				caller must have allocated newWork->hdr.data and read
 *				the msg into newWork->hdr.data. If an internal error
 *				prevents the worknode from being inserted into the SLP_IN
 *				Q, sends an internal error srvack back to the caller. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void dispatchSLP(lslpWork *newWork)
{
  assert(newWork != NULL);
  assert(newWork->hdr.data != NULL);
  
  /* short-circuit if the packet is not the correct version */
  if ( LSLP_PROTO_VER ==  _LSLP_GETVERSION((uint8 *)newWork->hdr.data)) {
    /* short circuit if the msg id is invalid */
    if ((_LSLP_GETFUNCTION((uint8 *)newWork->hdr.data)) && 
	(_LSLP_GETFUNCTION((uint8 *)newWork->hdr.data)) <= LSLP_MESHCTRL ) {
      
      /* finish initializing the header, then copy the data */
      setHdrAndRqRpStatus(newWork);


     /* check for a msg that attempts to make us overrun our buffers */
      if(newWork->hdr.len > (uint32) newWork->na.data_len ) {
	_report_event(LSLP_SEV_IDX(56 , LSLP_SEVERITY_ERROR), newWork, 
		     (LSLP_PARM_WORK | sizeof(newWork)), 
		     newWork->na.data_len, 
		     newWork->hdr.len);
	if(! ((newWork->status & LSLP_STATUS_BCAST)  ||
	      (newWork->status & LSLP_STATUS_MCAST) ||
	      (newWork->status & LSLP_STATUS_CONVERGE))) {
	  if (newWork->status & LSLP_STATUS_TCP )
	    tcpInternalErr(newWork->ses_idx, newWork->hdr.xid, LSLP_PARSE_ERROR);
	  else
	    udpInternalErr(&newWork->na.rmtAddr, 
			    &newWork->na.myAddr, 
			    newWork->hdr.xid, LSLP_PARSE_ERROR);
	}
	newWork->txerr(newWork);
	return;
      }
      _report_event(LSLP_SEV_IDX(16 , LSLP_SEVERITY_DEBUG), 
		   newWork->hdr.data, 
		   (LSLP_PARM_MSG | _LSLP_GETLENGTH(((uint8 *)newWork->hdr.data))));
      newWork->type = LSLP_Q_SLP_IN;
    } else {   /* if the hdr has a correct msg id */
      _report_event(LSLP_SEV_IDX(33, LSLP_SEVERITY_ERROR), newWork,
		   (LSLP_PARM_WORK | sizeof(newWork)), 
		   (_LSLP_GETFUNCTION((uint8 *)newWork->hdr.data)));
      newWork->status |= LSLP_STATUS_PARSE_ERROR;
    }
  } else {  /* if the protocol hdr has the correct version */
    _report_event(LSLP_SEV_IDX(32, LSLP_SEVERITY_ERROR), newWork,
		 (LSLP_PARM_WORK | sizeof(newWork)), 
		 (_LSLP_GETFUNCTION((uint8 *)newWork->hdr.data)));
    newWork->status |= LSLP_STATUS_PARSE_ERROR; 
  }
  if (LSLP_WAIT_OK !=  _insertWorkNode(newWork)) {
    if(! ((newWork->status & LSLP_STATUS_BCAST)  ||
	  (newWork->status & LSLP_STATUS_MCAST) ||
	  (newWork->status & LSLP_STATUS_CONVERGE))) {
      if (newWork->status & LSLP_STATUS_TCP) 
	tcpInternalErr(newWork->ses_idx, newWork->hdr.xid, LSLP_INTERNAL_ERROR);
      else 
	udpInternalErr(&newWork->na.rmtAddr, 
			&newWork->na.myAddr, 
			newWork->hdr.xid, LSLP_INTERNAL_ERROR);
    }
    newWork->txerr(newWork);
  }
  return;
}      


/****************************************************************
 *  dispatchTCP
 *
 *
 *  PARAMETERS: IN struct nameAddr *na contains the remote and local
 *				interfaces. 
 *				IN SOCKET sock is the socket to read. If sock is zero, 
 *			    obtains a new socket by accepting a connection upon na->tcpSock. 
 *
 *  DESCRIPTION: accepts a connection (or uses an existing connection) to 
 *				 read the next slp message into a work node. Will allocate
 *				 a new work node and a new work->hdr.data buffer to hold the
 *				message data. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void dispatchTCP(lslpStream *stream, SOCKET sock )
{

  int32 bytesRead;
  int32 fromLen = sizeof(SOCKADDR_IN);
  uint32 ccode;
  struct nameAddr tempName;
  SOCKET tempSock;
  lslpStream *in_stream = NULL;
  lslpWork *newWork = NULL;
  memset(&tempName, 0x00, sizeof(tempName));  
  if (stream == NULL) {
    /* incoming connection */
    do{
      tempSock = accept(sock, (struct sockaddr *)&(tempName.rmtAddr), &fromLen);
    } while((tempSock == INVALID_SOCKET) && (errno == EINTR));
      bytesRead = 51000;
      setsockopt(tempSock, SOL_SOCKET, SO_RCVBUF, (const char *)&bytesRead, sizeof(bytesRead));
      bytesRead = 51000;
      setsockopt(tempSock, SOL_SOCKET, SO_SNDBUF, (const char *)&bytesRead, sizeof(bytesRead));
      _report_event(LSLP_SEV_IDX(36, LSLP_SEVERITY_DEBUG), NULL, 
		    0, tempSock);
      if(NULL != (in_stream = (lslpStream *)calloc(1, sizeof(lslpStream)))) {
	in_stream->status = (LSLP_STATUS_CLIENT_SOCKET | LSLP_STATUS_KEEP_SOCKET);
	in_stream->peerSock = tempSock;
	in_stream->close_interval = lslp_config_close_conn;
	/* set the write stamp (we just connected) */
	in_stream->last_read = time(&in_stream->last_write);
	memcpy(&(in_stream->peerAddr), &(tempName.rmtAddr), sizeof(SOCKADDR_IN));
	/* don't finalize the creation of a new stream unless we know we can get */
	/* a work node to carry out the initial request. */
	if(NULL != (newWork = allocWorkNode())) {
	  _LSLP_WAIT_SEM(streamInHead.sem, LSLP_Q_WAIT, &ccode);
	  assert(ccode == LSLP_WAIT_OK);
	  if(ccode == LSLP_WAIT_OK) {
	    _LSLP_INSERT(in_stream, &streamInHead);
	    _LSLP_SIGNAL_SEM(streamInHead.sem);
	  } else { freeWorkNode(newWork); free(in_stream);  _LSLP_CLOSESOCKET(tempSock); return; } 
	} else { free(in_stream);  _LSLP_CLOSESOCKET(tempSock); return; } 
      } else { _LSLP_CLOSESOCKET(tempSock); return; } 
  } else { in_stream = stream ; }

  if (INVALID_SOCKET != (in_stream->peerSock)) {
    if((newWork != NULL) || (NULL != (newWork = allocWorkNode()))) {
      newWork->txerr = stream_txerr;
      memcpy(&(newWork->na.rmtAddr), &(in_stream->peerAddr), sizeof(SOCKADDR_IN));
      newWork->ses_idx = in_stream->peerSock;
      newWork->status = (LSLP_STATUS_TCP | LSLP_STATUS_RCVD | LSLP_STATUS_KEEP_SOCKET);
      /* first read just the header to see how long the msg is */

      bytesRead  = connectedRead(newWork->ses_idx, newWork->na.static_data, LSLP_MIN_HDR);
      if(bytesRead == LSLP_MIN_HDR) {
	newWork->na.data_len = _LSLP_GETLENGTH(newWork->na.static_data);
	if (newWork->na.data_len <= lslp_config_max_msg) {
	  /* the length looks good - now see if it can fit into the static data */
	  if(newWork->na.data_len < LSLP_LSL_MTU) 
	    newWork->hdr.data = newWork->na.static_data;
	  else {
	    newWork->hdr.data = calloc(newWork->na.data_len + 1, sizeof(int8));
	    if(NULL != (newWork->hdr.data = calloc(newWork->na.data_len + 1, sizeof(int8))))
	      memcpy(newWork->hdr.data, newWork->na.static_data, LSLP_MIN_HDR) ;
	  }
	  if(NULL != newWork->hdr.data ) {
	    bytesRead = connectedRead(newWork->ses_idx, 
				       ((int8 *)newWork->hdr.data) + LSLP_MIN_HDR, 
				       newWork->na.data_len - LSLP_MIN_HDR);
	    if(bytesRead == newWork->na.data_len- LSLP_MIN_HDR) {
	      dispatchSLP(newWork);
	      return;
	    } else {  /* if we read the expected number of bytes */
	      _report_event(LSLP_SEV_IDX(38, LSLP_SEVERITY_ERROR), NULL, 0, 
			   bytesRead, newWork->na.data_len, 
			   "tcp", newWork->ses_idx);
	    }
	  } 
	} else {  /* length is right */
	  _report_event(LSLP_SEV_IDX(57, LSLP_SEVERITY_DEBUG), newWork, 
		       (LSLP_PARM_WORK | sizeof(newWork)), 
		       newWork->na.data_len,
		       lslp_config_max_msg);
	  if(! ((newWork->status & LSLP_STATUS_BCAST)  ||
		(newWork->status & LSLP_STATUS_MCAST) ||
		(newWork->status & LSLP_STATUS_CONVERGE))) {
	    tcpInternalErr(tempSock, 0, LSLP_PARSE_ERROR);
	  }
	}
      } /* if we read the hdr */
      /* other side closed the connection ! */
      if(bytesRead == 0)
	bytesRead = -1;
      newWork->hdr.errCode = bytesRead;
      newWork->txerr(newWork);
    } /* if we allocated the work node */
    _LSLP_CLOSESOCKET(tempSock);
  }
  return;
}	


/****************************************************************
 *  dispatchUDP
 *
 *
 *  PARAMETERS: IN SOCKET sock is a udp socket ready to be read
 *				
 *
 *  DESCRIPTION: obtains a session and reads from na->udpSock into a 
 *				 work node. Will allocate a new work node and a new 
 *				 work->hdr.data buffer to hold the message data. 
 *
 *  RETURNS: SOCKET_ERROR if problem reading socket
 *
 ***************************************************************/

uint32 dispatchUDP(SOCKET sock, struct timeval *stamp)
{
  uint32 ccode = LSLP_OK;
  int32 fromLen = sizeof(SOCKADDR_IN), result;

  lslpWork *newWork = allocWorkNode();
  if (newWork != NULL) {
    newWork->na.version = LSLP_NAMEADDR_VERSION;
    newWork->na.sock = sock;
    do {(result = (newWork->na.data_len  = 
		   recvfrom(sock, newWork->na.static_data, 
			lslp_config_mtu, 0, 
			(struct sockaddr *)&(newWork->na.rmtAddr), 
			&fromLen)));}
    while((result < 0) && (EINTR == errno)); 

    /* if there is enough data to read, */
    if (newWork->na.data_len >= LSLP_MIN_HDR) {
      newWork->hdr.data = newWork->na.static_data;
      fromLen = sizeof(struct sockaddr_in);
      getsockname (sock, (struct sockaddr *)&(newWork->na.myAddr), &fromLen);
      newWork->status |= (LSLP_STATUS_RCVD | LSLP_STATUS_UDP) ;

      if (-1 == (newWork->ses_idx = _getSession(&newWork->na))) {
	_report_event(LSLP_SEV_IDX(39, LSLP_SEVERITY_ERROR), newWork, 
		     (LSLP_PARM_WORK | sizeof(newWork)));
	/* there are no sessions right now, link to the waiting list */
	newWork->type = LSLP_Q_WAITING;
	if (LSLP_WAIT_OK !=  _insertWorkNode(newWork)) {
	  _report_event(LSLP_SEV_IDX(40, LSLP_SEVERITY_ERROR), newWork,
		       (LSLP_PARM_WORK | sizeof(newWork)));
	  newWork->txerr(newWork);
	}
	return(ccode);
      } /* if we didn't get a session */
      _report_event(LSLP_SEV_IDX(9, LSLP_SEVERITY_DEBUG), newWork, 
		   (LSLP_PARM_WORK | sizeof(newWork)), 
		  newWork->na.data_len, "udp", 
		  newWork->na.sock);

      /* check for a bcast or mcast daadvert that originated here - then discard it */
      /* this means that in order to use mcast or bcast discovery to discover the  */
      /* the local host, send the request to the mcast or bcast address, but  */
      /* set the mcast TTL to 1, and clear the multicast bit in the slp header flag  */
      if( FALSE == lslp_respond_self_bcast) {
	if(_LSLP_GETFLAGS(newWork->na.static_data) & LSLP_FLAGS_MCAST) {   
	  LSLP_CONFIG_ADV *interfaces = lslpConfigTransport.next ;  
	  while(! _LSLP_IS_HEAD(interfaces)) {   
	    if((newWork->na.rmtAddr.sin_addr.s_addr == interfaces->iface) &&  
	       interfaces->iface != INADDR_ANY ) { 
	      newWork->txerr(newWork) ; 
	      return(ccode); 
	    } 
	    interfaces = interfaces->next; 
	  }   
	}
      }

      /* check for duplicate incoming messages due to listening on INADDR_ANY */
      /* also checks for the possiblity of lingering replies  */

      /* duplicates will have a different session - we need to find a session  */
      /* with the same remote addr, stamp, and xid, but it won't have the same local addr  */

      if(sessions[newWork->ses_idx].xid == _LSLP_GETXID(newWork->na.static_data)) {
	/* we got a message from this client with a duplicate xid it's either a  */
	/* retry or a duplicate  */
	if((stamp->tv_sec == sessions[newWork->ses_idx].stamp.tv_sec) && 
	   (stamp->tv_usec == sessions[newWork->ses_idx].stamp.tv_usec) ) {
	  /* duplicate - just waste it  */
	  newWork->txerr(newWork);
	  return(ccode);
	}

	/* probably a retry - see if it is on the lingering list  */

	
      } 
      /* update the session with xid and stamp  */
      sessions[newWork->ses_idx].xid = _LSLP_GETXID(newWork->na.static_data);
      sessions[newWork->ses_idx].stamp.tv_sec = stamp->tv_sec;
      sessions[newWork->ses_idx].stamp.tv_usec = stamp->tv_usec;
      dispatchSLP(newWork);
	
    } else { /* if there is at least an slp header */
      _report_event(LSLP_SEV_IDX(38, LSLP_SEVERITY_DEBUG), newWork, 
		   (LSLP_PARM_WORK | sizeof(newWork)), 
		  newWork->na.data_len, LSLP_MIN_HDR, 
		  "udp", newWork->na.sock);
      newWork->txerr(newWork);
      ccode = SOCKET_ERROR;
    } /* we got the work node but we weren't able to read the minimum amount of data */
  }  else { /* if we got the new work node */
    _report_event(LSLP_SEV_IDX(76, LSLP_SEVERITY_ERROR), NULL, 0);
  }
  return(ccode);	 
}	


/****************************************************************
 *  q_slp_in
 *
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: called by dispatch thread to handle the SLP IN Q
 *
 *  RETURNS:
 *
 ***************************************************************/
void q_slp_in(void)
{
  BOOL found = FALSE;
  lslpWork *newWork = _unlinkFirstWorkNode(LSLP_Q_SLP_IN);
  while(newWork != NULL)  {
    found = TRUE;
    if((newWork->status & LSLP_STATUS_PEER_SOCKET) ||( newWork->status & LSLP_STATUS_CLIENT_SOCKET) ) {
      newWork->status |= LSLP_STATUS_KEEP_SOCKET;
    }
    assert((newWork->status & LSLP_STATUS_RCVD) || (newWork->status & LSLP_STATUS_LOCALHOST));
    assert(newWork->hdr.len < (uint32)lslp_config_max_msg);
    /* this is just a last-chance attempt to catch any damaged or malicious packets */
    if(newWork->hdr.len >= (uint32)lslp_config_max_msg) {
      _report_event(LSLP_SEV_IDX(58, LSLP_SEVERITY_DEBUG), NULL, 0, newWork->hdr.len);
      newWork->txerr(newWork);
    } else {
      /* call the dispatch-in table */
      _report_event(LSLP_SEV_IDX(43, LSLP_SEVERITY_DEBUG), 
		   newWork, (LSLP_PARM_WORK | sizeof(newWork)),
		   newWork->hdr.msgid);

      dpt[newWork->hdr.msgid & 0x0f](newWork);
    }
    newWork = _unlinkFirstWorkNode(LSLP_Q_SLP_IN);
  }
  /* only sleep when there are no incoming messages to dispatch */
  if(found == FALSE) {
    _LSLP_SLEEP(waitInterval);
  }
  return;
}	

/****************************************************************
 *  q_waiting
 *
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: called by dispatch thread to handle the WAITING Q
 *
 *  RETURNS:
 *
 ***************************************************************/
void q_waiting(void)
{
  lslpWork *newWork = _unlinkFirstWorkNode(LSLP_Q_WAITING);
  if (newWork != NULL) {
    assert(newWork->status & LSLP_STATUS_UDP);
    assert(newWork->status & LSLP_STATUS_RCVD);

       /* if there is enough data to read, */
    if (newWork->na.data_len >= LSLP_MIN_HDR) {
      if (-1 != (newWork->ses_idx = _getSession(&newWork->na))) {
	/* if this works the node will be linked to slp_q_in */
	dispatchSLP(newWork);
      } else { 
	_report_event(LSLP_SEV_IDX(40, LSLP_SEVERITY_ERROR), newWork, 
		     (LSLP_PARM_WORK | sizeof(newWork)));
	/* we tried twice and failed */
	newWork->hdr.data =  newWork->na.static_data;
	setHdrAndRqRpStatus(newWork);
	if(! ((newWork->status & LSLP_STATUS_BCAST)  ||
	      (newWork->status & LSLP_STATUS_MCAST) ||
	      (newWork->status & LSLP_STATUS_CONVERGE))) {
	  udpInternalErr(&newWork->na.rmtAddr, 
			  &newWork->na.myAddr, 
			  newWork->hdr.xid, LSLP_INTERNAL_ERROR);
	}
	newWork->hdr.data = NULL;
	newWork->txerr(newWork);
      }
    } else {  /* if there is at least an slp header */
      _report_event(LSLP_SEV_IDX(37, LSLP_SEVERITY_ERROR), newWork,
		   (LSLP_PARM_WORK | sizeof(newWork)), "udp", 
		   newWork->na.sock);
      newWork->txerr(newWork);
    }
  } /* while there is waiting work */
}	

/****************************************************************
 *  q_lingering
 *
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: called by dispatch thread to handle the LINGERING Q
 *
 *  RETURNS:
 *
 ***************************************************************/

void q_lingering(void)
{
  lslpWork *newWork;
  time_t now;
  if (LSLP_WAIT_OK == _lockList(LSLP_Q_LINGERING)) {
    time(&now);
    newWork = workHeads[LSLP_Q_LINGERING].next;
    while (! _LSLP_IS_HEAD(newWork)) {
      if ( ! (newWork->status & LSLP_STATUS_RP) ||  (now - newWork->timer.tv_sec >= LSLP_LINGER_WAIT)   ) {  
	lslpWork *temp = newWork;
	newWork = newWork->next;
	_LSLP_UNLINK(temp);
	temp->scratch = NULL;
	temp->txerr(temp);
	continue;	
      } 
      newWork = newWork->next;
    }
    /* we've processed every node on the list */
    _unlockList(LSLP_Q_LINGERING);
  }
  return;
}
 	

/****************************************************************
 *  q_pending
 *
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: called by the dispatch thread to handle the PENDING Q
 *
 *  RETURNS: 
 *
 ***************************************************************/
void q_pending(void)
{
  struct timeval now;
  lslpWork *newWork, *temp;
  lslpWork sendList = {&sendList, &sendList, TRUE } ;
  gettimeofday(&now, NULL);
  if (LSLP_WAIT_OK == _lockList(LSLP_Q_PENDING)) {
    newWork = workHeads[LSLP_Q_PENDING].next;
    while (! _LSLP_IS_HEAD(newWork)) {
      BOOL timeout = FALSE;
      assert(newWork->status & LSLP_STATUS_RQ);

      /* should we test this work for a tcp timeout? */
      if (newWork->status & LSLP_STATUS_TCP) {
	if (now.tv_sec - newWork->timer.tv_sec >= LSLP_ORPHAN_WAIT) {
	  temp = newWork;
	  newWork = newWork->next;
	  _LSLP_UNLINK(temp);
	  _report_event(LSLP_SEV_IDX(49, LSLP_SEVERITY_ERROR), temp, 
		       (LSLP_PARM_WORK | sizeof(temp)), "tcp", 
		       now.tv_sec - newWork->timer.tv_sec);
	  temp->scratch = NULL;
	  temp->txerr(temp);
	  continue;
	}
      } else {		/* UDP request */

	/* process  convergence requests */
	if( (newWork->status & LSLP_STATUS_CONVERGE) ||
	    (newWork->status & LSLP_STATUS_MCAST) ||
	    (newWork->status & LSLP_STATUS_BCAST)) {

	  if((now.tv_sec - newWork->timer.tv_sec) > lslp_config_retry) {

	    /* re-transmit interval has expired */

	    timeout = TRUE;
	      /* maximum lifetime has expired for this convergence request */
	      temp = newWork;
	      newWork = newWork->next;
	      _LSLP_UNLINK(temp);
	      temp->scratch = NULL;
	      temp->txerr(temp);
	      continue;
	  }  /* time to look at this node again */

	} else { 
	  /* a normal udp request - evaulate it for timeout */
	  assert(newWork->ses_idx >= 0 && newWork->ses_idx < LSLP_SESSIONS);
	  if (now.tv_sec - newWork->timer.tv_sec >= 
	      sessions[newWork->ses_idx].timer.rtt_currto) {
	    timeout = TRUE;
	    if ( rtt_timeout(& sessions[newWork->ses_idx].timer) < 0) {
	      temp = newWork;
	      newWork = newWork->next;
	      /* this pending request is done for - */
	      _LSLP_UNLINK(temp);
	      temp->scratch = temp;
	      temp->txerr(temp);
	      continue;
	    }
	  } 
	}/* normal udp timeout evaluation */
      } /* udp */

      /* to timeout the node, insert him into a different list. if that fails, destroy him */
      if ( timeout == TRUE  ) {
	temp = newWork;
	newWork = newWork->next;
	
	_LSLP_UNLINK(temp);
	temp->scratch = NULL;
	temp->status |= LSLP_STATUS_RETX;
	temp->type = LSLP_Q_SLP_OUT;

	_report_event(LSLP_SEV_IDX(50, LSLP_SEVERITY_ERROR), temp,
		     (LSLP_PARM_WORK | sizeof(temp)), 
		     now.tv_sec - temp->timer.tv_sec);
	if(temp->rtx_count < lslp_config_retry_max) {
	  temp->rtx_count++;
	  _LSLP_INSERT(temp, &sendList);
	} else { temp->txerr(temp); }
	continue;
      }
      newWork = newWork->next; 
    }	/* while traversing the list 	*/

    _unlockList(LSLP_Q_PENDING);

  } /* if we locked the pending q */

  temp = sendList.next;
  while(! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    if(LSLP_WAIT_OK != _insertWorkNode(temp)) {
      temp->txerr(temp);
    }
    temp = sendList.next;
  }
  return;
}	
 

void *_dispatchThread(void *parm) 
{
  /* ignore termination signals */
  _LSLP_SIG_ACTION_THREAD();
  _report_event(LSLP_SEV_IDX(42, LSLP_SEVERITY_INFO), NULL, 0, "Dispatch");
  while ( ! dieNow ) {
    /* the following routine will sleep whenever there are no incoming messages */
    q_slp_in();
    /* the lingering queue contains responses that we keep just in case */
    /* the request gets retried. we only keep them around for LSLP_LINGER_WAIT seconds */
    /* the lingering queue also contains work nodes that have multiple msgs in them, */
    /* plus work nodes that have connected sockets that we need to try reading again */
    /* handle new requests and responses in from the net */
    q_lingering();
    /* waiting q has udp msgs waiting for a session */
    /* transmit all the msgs waiting to be sent */
    slpOutThread() ;
    /* take care of mesh business */
    /* << Tue Jun 29 16:57:45 2004 mdd >> rfc 3528 */
/*     _lslpMeshInitTXPeers(&streamInHead, &connectHead) ; */
    if(fwd_DA_necessary) { 
      _lslpMeshFwdDAAdv((lslpDirAgent *)&lslpRemoteDAs, &streamInHead) ;
      fwd_DA_necessary = FALSE;
    }
    /* service work nodes that are waiting for a session (rare ) */ 
    q_waiting() ;
    /* the pending queue contains requests that are waiting for a response */
    /* we either need to resend each request or time it out */
    q_pending();
    /* advertise ourself */ 
    lslp_daAdvertThread() ;
  } /* while we are supposed to stay alive */
  _report_event(LSLP_SEV_IDX(51, LSLP_SEVERITY_INFO), NULL, 0, "Dispatch");
  _LSLP_ENDTHREAD(0);
  return(NULL);
}	


void slpOutThread(void)
{
  /* to guarantee highest performace, do a select on each socket before trying to send */
  /* to go beyond that, make each socket non-blocking so that we never pause when   */
  /* waiting for socket buffer space to free up. of course, when we have to be able  */
  /* to process partial writes to a connected socket */
  lslpWork *newWork;
  /* get the new work from the slp out queue */
  newWork = _unlinkFirstWorkNode(LSLP_Q_SLP_OUT) ;
  while (newWork != NULL) {
    /* now form an slp message and transmit it */
    /* assumptions: work->hdr is correctly initialized */
    /* work->hdr.len has the correct txbuffer length */
    /* work->hdr.data has a correctly formed SLP msg. */
    assert(! (newWork->status & LSLP_STATUS_LOCALHOST));
    assert(newWork->hdr.len == _LSLP_GETLENGTH((int8*)newWork->hdr.data));
    
    if (newWork->status & LSLP_STATUS_TCP) {
      if(newWork->status & LSLP_STATUS_KEEP_SOCKET) {
	newWork->status |= LSLP_STATUS_DONT_LINGER;
      }
      newWork->hdr.errCode = connectedWrite(newWork->ses_idx, newWork->hdr.data, newWork->hdr.len);
    }  else {
      newWork->status |=  LSLP_STATUS_UDP;
      /* force the mcast flag to be set if appropriate */
      if(( newWork->status & LSLP_STATUS_MCAST) || (newWork->status & LSLP_STATUS_BCAST)) {
	int8 flags;
	flags = _LSLP_GETFLAGS(((int8 *)newWork->hdr.data));
	flags |= LSLP_FLAGS_MCAST;
	_LSLP_SETFLAGS(((int8 *)newWork->hdr.data), flags);
      }
      newWork->hdr.errCode = dgramWrite(newWork, newWork->hdr.data, newWork->hdr.len, NULL);
    }
    if(newWork->hdr.errCode <= 0 ) {
      newWork->txerr(newWork);
    } else {
      newWork->status |= LSLP_STATUS_TX;
      if ((newWork->status & LSLP_STATUS_RQ) && 
	   (!(newWork->status & LSLP_STATUS_RP))) { 
	if(newWork->status & LSLP_STATUS_TCP) {
	  newWork->txerr(newWork);
	  newWork = NULL;
	} else {
	    newWork->type = LSLP_Q_PENDING;
	}
      } else if ((newWork->status & LSLP_STATUS_RP)) {
	if(newWork->status & LSLP_STATUS_DONT_LINGER) { 
	  newWork->txerr(newWork);
	  newWork = NULL;
	}else {
	  newWork->type = LSLP_Q_LINGERING;
	}
      }
      if ((newWork != NULL) && (LSLP_WAIT_OK != _insertWorkNode(newWork))) {
	newWork->txerr(newWork);
      }
    }
    newWork = _unlinkFirstWorkNode(LSLP_Q_SLP_OUT);
  } /* if we unlinked some new work from the slp out queue */
  return ;
}	


BOOL prepareSock(SOCKET sock)
{
  int err = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(int)) >= 0) {
    /* set huge receive buffers */
    err = 51000;
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&err, sizeof(int)) ;
    err = 51000;
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&err, sizeof(int)) ;
    return(TRUE);
  }
  return(FALSE);
}

BOOL prepareMulticastSock(SOCKET sock, SOCKADDR_IN *addr) 
{
  struct ip_mreq mreq;
  
  mreq.imr_multiaddr.s_addr = _LSLP_MCAST;
  mreq.imr_interface.s_addr = addr->sin_addr.s_addr;
  
  if(SOCKET_ERROR == setsockopt(sock,IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq))) 
    return(FALSE);
  return(TRUE);
}

BOOL prepareUDPsock(SOCKET sock, SOCKADDR_IN *addr) 
{
  int err = 1;  
  if(TRUE == prepareSock(sock)) {
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&err, sizeof(err));
    if(SOCKET_ERROR != (err = bind(sock, (struct sockaddr *)addr, sizeof(SOCKADDR_IN)))) 
      return(TRUE);
  }  
  return(FALSE);
}

#ifdef __linux__
#ifndef TCP_NODELAY
#define TCP_NODELAY 1
#endif
#endif
BOOL prepareTCPSock(SOCKET sock, SOCKADDR_IN *addr) 
{

  if(TRUE == prepareSock(sock)) {
    int err = 1;  
  
     setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&err, sizeof(int)) ;  

    /* disable the nagle algorithm to keep things fast and simple */
    err = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &err, sizeof(int));
    
    if(SOCKET_ERROR != (err = bind(sock, (struct sockaddr *)addr, sizeof(SOCKADDR_IN)))) 
      return(TRUE);
  }
  return(FALSE);
}
  

int32 createListeners(LSLP_CONFIG_ADV *transport_list, 
		      struct in_socks *listen_socks) 
{  
  LSLP_CONFIG_ADV *this_config;
  LSLP_CONFIG_ADV *dont_need;
  struct in_socks *this_socks;
  SOCKADDR_IN addr;
  int8 *bptr;
  int32 count = 0;
  BOOL bcast_prepared = FALSE; 


/*****************************************************************
 *
 * http://www.landfield.com/faqs/unix-fsocket/
 *
 * 
 * It seems to be the case in the majority of implementations that a
 *  socket bound to a unicast address will not see incoming packets with
 * broadcast addresses as their destinations.
 *
 *  The approach I've taken is to use SIOCGIFCONF to retrieve the list of
 *  active network interfaces, and SIOCGIFFLAGS and SIOCGIFBRDADDR to
 *  identify broadcastable interfaces and get the broadcast addresses.
 *  Then I bind to each unicast address, each broadcast address, and to
 *  INADDR_ANY as well. That last is necessary to catch packets that are
 *  on the wire with INADDR_BROADCAST in the destination.  (SO_REUSEADDR
 *  is necessary to bind INADDR_ANY as well as the specific addresses.)
 *
 *  This gives me very nearly what I want. The wrinkles are:
 *
 *  o  I don't assume that getting a packet through a particular socket
 *     necessarily means that it actually arrived on that interface.
 *
 *  o  I can't tell anything about which subnet a packet originated on if
 *     its destination was INADDR_BROADCAST.
 *
 *  o  On some stacks, apparently only those with multicast support, I get
 *     duplicate incoming messages on the INADDR_ANY socket.
 *
 *****************************************************************/


  /* create our listening sockets */
  /* check first for an empty list. an empty list is technically a configuration error */
  /* however, doing this sloppy code allows us to start up the server with essentially no  */
  /* configuration  */
  if(_LSLP_IS_EMPTY(transport_list)) {
    LSLP_CONFIG_ADV *temp;
    /* the transport list is empty, create a dummy transport for INADDR_ANY  */
    if(NULL != (temp = (LSLP_CONFIG_ADV *)calloc(1, sizeof(LSLP_CONFIG_ADV *)))) {
      temp->addr = temp->iface = INADDR_ANY;
      _LSLP_INSERT(temp, transport_list);
    }
  }

  this_config = transport_list->next;
  while(! _LSLP_IS_HEAD(this_config)) { 
    bptr = inet_ntoa(*(struct in_addr *)&this_config->iface );
    /* bypass interfaces that are masked out from listening */
    if(this_config->mask == 0xffffffff) {
      this_config = this_config->next;
      continue;
    }

    /* listen for datagrams to this interface */
    if(NULL != (this_socks = (struct in_socks *)calloc(1, sizeof(struct in_socks)))) {
      this_socks->udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(lslp_config_opport);
      addr.sin_addr.s_addr = this_config->iface;
      if(FALSE == prepareUDPsock(this_socks->udp_sock, &addr)) {
	_report_event(LSLP_SEV_IDX(77, LSLP_SEVERITY_ERROR), NULL, 0, "udp");
	free(this_socks);
	return(count);
      }

      /* check to see if we will still need to listen on INADDR_ANY */
      if(this_config->iface == INADDR_ANY)
	bcast_prepared = TRUE;

      /* if this interface supports broadcast, listen on its broadcast address */      
      if( (this_config->flag & IFF_BROADCAST) && bcast_prepared == FALSE  ) {
	this_socks->bcast_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(lslp_config_opport);
	addr.sin_addr.s_addr = this_config->addr;; 
	if(FALSE == prepareUDPsock(this_socks->bcast_sock, &addr)) {
	  _report_event(LSLP_SEV_IDX(77, LSLP_SEVERITY_ERROR), NULL, 0, "udp");
	  _LSLP_CLOSESOCKET(this_socks->bcast_sock);
	  free(this_socks);
	  return(count);
	}
      } else { this_socks->bcast_sock = INVALID_SOCKET ;}
      if(this_socks->bcast_sock == INVALID_SOCKET) {
	_LSLP_CLOSESOCKET(this_socks->bcast_sock);
      }

      /* if this interface supports multicast AND if the configuration file */
      /* allows this interface to support multicast, bind it */
      if(this_config->flag & IFF_MULTICAST) {
	this_socks->mcast_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(lslp_config_opport);
	addr.sin_addr.s_addr = _LSLP_MCAST;
	if( prepareSock( this_socks->mcast_sock ) ) {
	  if( SOCKET_ERROR != bind(this_socks->mcast_sock, (struct sockaddr *)&addr, sizeof(SOCKADDR_IN))) {
	    addr.sin_family = AF_INET;
	    addr.sin_port = htons(lslp_config_opport);
	    addr.sin_addr.s_addr = this_config->iface;	   
	    if( FALSE == prepareMulticastSock(this_socks->mcast_sock, &addr) ) 
	      this_socks->mcast_sock = INVALID_SOCKET;
	  } else { 
	    /* on some machines, particularly windows nt/2000, */
	    /* binding to the multicast address doesnt work. */
	    addr.sin_family = AF_INET;
	    addr.sin_port = htons(lslp_config_opport);
	    addr.sin_addr.s_addr = INADDR_ANY;
	    if( SOCKET_ERROR != bind(this_socks->mcast_sock, (struct sockaddr *)&addr, sizeof(SOCKADDR_IN))) {
	      addr.sin_family = AF_INET;
	      addr.sin_port = htons(lslp_config_opport);
	      addr.sin_addr.s_addr = this_config->iface;	   
	      if( FALSE == prepareMulticastSock(this_socks->mcast_sock, &addr) ) 
		this_socks->mcast_sock = INVALID_SOCKET;
	      else 
		bcast_prepared = TRUE;
	    } else {   this_socks->mcast_sock = INVALID_SOCKET; }
	  }
	} else { this_socks->mcast_sock = INVALID_SOCKET; }
      } else { this_socks->mcast_sock = INVALID_SOCKET; }
      if(this_socks->mcast_sock == INVALID_SOCKET) {
	_LSLP_CLOSESOCKET(this_socks->mcast_sock);
      }

      /* listen for connections on this interface */
      addr.sin_family = AF_INET;
      addr.sin_port = htons(lslp_config_opport);
      addr.sin_addr.s_addr = this_config->iface;
      this_socks->tcp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if(FALSE == prepareTCPSock(this_socks->tcp_sock, &addr)) {
	_report_event(LSLP_SEV_IDX(77, LSLP_SEVERITY_ERROR), NULL, 0, "tcp");
	_LSLP_CLOSESOCKET(this_socks->udp_sock);
	_LSLP_CLOSESOCKET(this_socks->bcast_sock);
	free(this_socks);
	return(count);
      }
      listen(this_socks->tcp_sock, SOMAXCONN);

      /* listen for api connections on this interface */
      this_socks->api_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      addr.sin_family = AF_INET;
      addr.sin_port = htons(lslp_config_apiport);
      addr.sin_addr.s_addr = this_config->iface;
      if(FALSE == prepareTCPSock(this_socks->api_sock, &addr)) {
	_report_event(LSLP_SEV_IDX(77, LSLP_SEVERITY_ERROR), NULL, 0, "tcp");
	_LSLP_CLOSESOCKET(this_socks->udp_sock);
	_LSLP_CLOSESOCKET(this_socks->bcast_sock);
	_LSLP_CLOSESOCKET(this_socks->tcp_sock);
	free(this_socks);
	return(count);
      }
      listen(this_socks->api_sock, SOMAXCONN);
      _LSLP_INSERT(this_socks, listen_socks);
      count++;
    } /* if allocated the in_socks */
    this_config = this_config->next;
  } /* while traversing the config list */

  /* at this point, all configured interfaces are listening on udp, bcast, */
  /* mcast, and tcp. however, we may still need to listen on udp for INADDR_ANY. */
  if(bcast_prepared == FALSE) {
    if(NULL != (this_socks = (struct in_socks *)calloc(1, sizeof(struct in_socks)))) {
      this_socks->bcast_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(lslp_config_opport);
      addr.sin_addr.s_addr = INADDR_ANY;
      if(FALSE == prepareUDPsock(this_socks->bcast_sock, &addr)) {
	_report_event(LSLP_SEV_IDX(77, LSLP_SEVERITY_ERROR), NULL, 0, "udp");
	free(this_socks);
	return(count);
      }
      this_socks->udp_sock = this_socks->mcast_sock = this_socks->tcp_sock =
	this_socks->api_sock = INVALID_SOCKET;
      _LSLP_INSERT(this_socks, listen_socks);
      count++;
    }
  }

  /* free memory we no longer need */
  dont_need = lslpConfigListen.next; 
  while(! _LSLP_IS_HEAD(dont_need)) { 
    _LSLP_UNLINK(dont_need); 
    free(dont_need); 
    dont_need = lslpConfigListen.next; 
  } 

  dont_need = lslpConfigDontListen.next; 
  while(! _LSLP_IS_HEAD(dont_need)) { 
    _LSLP_UNLINK(dont_need); 
    free(dont_need); 
    dont_need = lslpConfigDontListen.next; 
  } 
  return(count);
}

/* start up a directory agent or sa  */
void startDASA(struct sockaddr_in *da_addr) 

{
  int8 urlBuf[80];
  int8 attrBuf[128] ;
  int32 flag;
  //  int8 tempBuf[] = "DEFAULT\0";
  int8 tempBuf2[] = "DSA\0";

  if(lslp_config_is_sa) 
    flag = LSLP_FLAG_SA;
  else
    flag = LSLP_FLAG_DA;
  strcpy(attrBuf, lslp_da_attr);
  strcat(attrBuf, ",(api-port = ");
  sprintf(&attrBuf[strlen(attrBuf)], "%d)", lslp_config_apiport);
  strcpy(urlBuf, lslp_da_url);
  if(! strlen(lslp_host_part)) 
      strncat(urlBuf, inet_ntoa(da_addr->sin_addr),  79 - strlen(urlBuf)) ;
  else 
    strncat(urlBuf, lslp_host_part, 79 - strlen(urlBuf));
  lslpCreateDA(1, lslp_logPath, urlBuf, da_addr, lslp_scope_string , tempBuf2, attrBuf, flag);
  return;
}

void *slpInThread(void *parm)
{
  struct in_socks *this_interface;
  int interfaces ;
  struct timeval stamp ;

  /* ignore termination signals */
  _LSLP_SIG_ACTION_THREAD();
  _report_event(LSLP_SEV_IDX(41, LSLP_SEVERITY_INFO), NULL, 0, "SLP Incoming");

  if( 0 < (interfaces = createListeners(&lslpConfigTransport, &listeners) )) {
    struct sockaddr_in da_addr;
    fd_set readfds;
    int32 sel;

    struct timeval tv = { 0, 0 };

    /* start up the da */
    memset(&da_addr, 0x00, sizeof(struct sockaddr_in));
    if(lslp_config_force_da_bind && (lslp_da_addr != NULL) ) 
      da_addr.sin_addr.s_addr = inet_addr(lslp_da_addr);
    else {
      /* find the first valid listener and use its interface */
      LSLP_CONFIG_ADV *t = lslpConfigTransport.next;
      while(! _LSLP_IS_HEAD(t)) {
	/* make certain its not a loopback interface, its not a dynamic interface,  */
	/* and its up and running */
	if((! (t->flag & IFF_LOOPBACK))  && 
	   ( !(t->flag & IFF_DYNAMIC)) && 
	   (t->flag & IFF_UP) && 
	   (t->mask != 0xffffffff) ) {
	  da_addr.sin_addr.s_addr = t->iface;
	  break;
	}
	t = t->next;
      }
    }
    
    startDASA(&da_addr) ;
    
    _report_event(LSLP_SEV_IDX(42, LSLP_SEVERITY_INFO), NULL, 0, "SLP Incoming");      

    while( ! dieNow ) {
      int count;

      this_interface = listeners.next;
      gettimeofday(&stamp, NULL);

      while (! _LSLP_IS_HEAD(this_interface)) {
	count = 0;

	/* test and dispatch the bcast listener */
	if(this_interface->bcast_sock != INVALID_SOCKET) {
	  FD_ZERO(&readfds) ;
	  FD_SET(this_interface->bcast_sock, &readfds); 
	  do{(sel = select(FD_SETSIZE, &readfds, NULL, NULL, &tv ));}
	  while((sel < 0) && ( errno == EINTR));
	  if (sel < 0 ) {  _report_event(LSLP_SEV_IDX(52, LSLP_SEVERITY_ERROR), NULL, 0);	} 
	  else if (sel > 0) { 
	    dispatchUDP(this_interface->bcast_sock, &stamp); 
	    count++ ;
	  } 
	}
	/* test and dispatch the mcast listener */
	if(this_interface->mcast_sock != INVALID_SOCKET) {
	  FD_ZERO(&readfds) ;
	  FD_SET(this_interface->mcast_sock, &readfds); 
	  do{(sel = select(FD_SETSIZE, &readfds, NULL, NULL, &tv ));}
	  while((sel < 0) && ( errno == EINTR));
	  if (sel < 0 ) {  _report_event(LSLP_SEV_IDX(52, LSLP_SEVERITY_ERROR), NULL, 0);	} 
	  else if (sel > 0) { 
	    dispatchUDP(this_interface->mcast_sock, &stamp); 
	    count++ ;
	  } 
	}
	if(this_interface->udp_sock != INVALID_SOCKET ) {
	  /* test and dispatch the udp listener */
	  FD_ZERO(&readfds) ;
	  FD_SET(this_interface->udp_sock, &readfds); 
	  do{(sel = select(FD_SETSIZE, &readfds, NULL, NULL, &tv ));}
	  while((sel < 0) && ( errno == EINTR));
	  if (sel < 0 ) {  _report_event(LSLP_SEV_IDX(52, LSLP_SEVERITY_ERROR), NULL, 0);	} 
	  else if (sel > 0) { dispatchUDP(this_interface->udp_sock, &stamp); count++ ;} 
	}
	if(this_interface->tcp_sock != INVALID_SOCKET ) {
	  /* test and dispatch the tcp listener */
	  FD_ZERO(&readfds) ;
	  FD_SET(this_interface->tcp_sock, &readfds); 
	  do{(sel = select(FD_SETSIZE, &readfds, NULL, NULL, &tv ));}
	  while((sel < 0) && ( errno == EINTR));
	  if (sel < 0 ) {  _report_event(LSLP_SEV_IDX(52, LSLP_SEVERITY_ERROR), NULL, 0);	} 
	  else if (sel > 0) {  dispatchTCP( NULL, this_interface->tcp_sock); count++; } 
	}
	if(this_interface->api_sock != INVALID_SOCKET) {
	  /* test and dispatch the api listener */
	  FD_ZERO(&readfds) ;
	  FD_SET(this_interface->api_sock, &readfds); 
	  do{(sel = select(FD_SETSIZE, &readfds, NULL, NULL, &tv ));}
	  while((sel < 0) && ( errno == EINTR));
	  if (sel < 0 ) {  _report_event(LSLP_SEV_IDX(52, LSLP_SEVERITY_ERROR), NULL, 0);	} 
	  else if (sel > 0) { dispatchAPI(this_interface->api_sock, LSLP_NEW_CONN); count++; } 
	}
	this_interface = this_interface->next;
      }
      if(count == 0) {
	_LSLP_SLEEP(waitInterval);

      }
      _lslpMeshReadPeers(&streamInHead);
    }
  }

  _report_event(LSLP_SEV_IDX(51, LSLP_SEVERITY_INFO), NULL, 0, "SLP Incoming");
  this_interface = listeners.next;
  while(! _LSLP_IS_HEAD(this_interface)) {
    _LSLP_CLOSESOCKET(this_interface->tcp_sock);
    _LSLP_CLOSESOCKET(this_interface->api_sock);
    LeaveMulticast(this_interface->udp_sock);  /* <---- also closes the socket */
    this_interface = this_interface->next;
  }
  _LSLP_ENDTHREAD(0);
  return(NULL);
}



/* returns a length-preceded escaped string compliant with an slp opaque value */
/* length is encoded as two-byte value at the beginning of the string */
/* size of returned buffer will be ((3 * length) + 3 + 2) */
int8 *encode_opaque(void *buffer, int16 length)
{
  static int8 transcode[16] = 
    { 
      '0', '1', '2', '3', '4', '5', '6', '7', 
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
  
  int8 *bptr, *srcptr, *buf;
  int32 encoded_length;
  
  if(buffer == NULL || length == 0)
    return(NULL);
  
  encoded_length = (length * 3) + 5;
  /* see if the encoded length will overflow the max size of an slp string */
  if( 0xffff0000 & encoded_length )
    return NULL;
  
  buf = malloc(encoded_length);
  if(buf == NULL)
    return NULL;
  bptr = buf;
  
  /* encode the length */
  _LSLP_SETSHORT(bptr, (int16)encoded_length, 0);
  bptr += 2;
  srcptr = (int8 *)buffer;
  *bptr = 0x5c; *(bptr + 1) = 0x66; *(bptr + 2) = 0x66;
  bptr += 3;
  
  while(length){
    *bptr = 0x5c;
    *(bptr + 1) = transcode[((*srcptr) & 0xf0) >> 4];
    *(bptr + 2) = transcode[(*srcptr) & 0x0f];
    bptr += 3;
    srcptr++;
    length--;
  }
  return buf;
}

/* returns an opaque buffer. size of opaque buffer will be */
/* ((size of escaped opaque string - 2) / 3) - 1  */
void *decode_opaque(int8 *buffer)
{
  
  int16 encoded_length, alloc_length;
  int8 *bptr, *srcptr, *retptr;

  if(buffer == NULL)
    return NULL;

  srcptr = buffer;
  retptr = bptr = NULL;
  /* get the length */
  encoded_length = _LSLP_GETSHORT(srcptr, 0);
  if(0 == encoded_length || 0 > encoded_length)
    return NULL;
  alloc_length = (encoded_length / 3) - 1;
  
  srcptr += 2;
  encoded_length -= 2; 
  
  /* check the header */
  if( *srcptr == 0x5c ){
    if( (*(srcptr + 1) == 0x46) || (*(srcptr + 1) == 0x66 ) ) {
      if( (*(srcptr + 2) == 0x46) || (*(srcptr + 2) == 0x66 ) ) {
	retptr = (bptr = malloc(alloc_length));
	if(bptr == NULL)
	  return NULL;
	/* adjust the encoded length to reflect that we consumed the header */
	encoded_length -= 3;
	srcptr += 3;
	
	while(encoded_length && alloc_length){
	  int8 accumulator = 0;
	  if(*srcptr == 0x5c){
	    /* encode the high nibble */
	    if( *(srcptr + 1) < 0x3a && *(srcptr + 1) > 0x2f){
	      /* asci decimal char */
	      accumulator = (*(srcptr + 1) - 0x30) * 0x10;
	    }
	    else if( *(srcptr + 1) < 0x47 && *(srcptr + 1) > 0x40){
	      accumulator = (*(srcptr + 1) - 0x37) * 0x10;
	    }
	    else if( *(srcptr + 1) < 0x67 && *(srcptr + 1) > 0x60){
	      accumulator = (*(srcptr + 1) - 0x57) * 0x10;
	    }
	    /* encode the low nibble */
	    if( *(srcptr + 2) < 0x3a && *(srcptr + 2) > 0x2f){
	      /* asci decimal char */
	      accumulator += *(srcptr + 2) - 0x30;
	    } 
	    else if( *(srcptr + 2) < 0x47 && *(srcptr + 2) > 0x40){
	      accumulator += *(srcptr + 2) - 0x37;
	    }
	    else if( *(srcptr + 2) < 0x67 && *(srcptr + 2) > 0x60){
	      accumulator += *(srcptr + 2) - 0x57;
	    }
	    /* store the byte */
	    *bptr = accumulator;
	  }
	  else
	    break;

	  /* update pointers and counters */
	  srcptr += 3;
	  bptr++;
	  encoded_length -= 3;
	  alloc_length--;
	}
	if( alloc_length || encoded_length) {
	  free(retptr);
	  retptr = NULL;
	}
      }
    }
  }
  return (void *)retptr;
}



void *pipeThread(void *parm)
{
#ifdef __linux__
  int32 ccode; 
  _LSLP_SIG_ACTION_THREAD();

  if( lslp_fifo != NULL ) {
    int8 * bptr = strstr(lslp_fifo, "\n");
    if(bptr != NULL)
      *bptr = 0x00;
    do { ccode = mkfifo(lslp_fifo, S_IRWXU ) ;} 
    while( ccode == EINTR );
    if( -1 != (pipe_fd = open( lslp_fifo, O_WRONLY ))) {
      /* blocks until a reader opens the pipe */
      pipe_open = TRUE;
    }
  }

#endif
  _LSLP_ENDTHREAD(0);
  return(NULL);
}



/****************************************************************
 *  processAPIIn
 *
 *
 *  PARAMETERS: IN OUT lslpWork *newWork is a work node that originated
 *				from the api socket. 
 *
 *  Description: Reads the api socket and initializes the worknode. Then
 *				inserts the worknode to either the SLP OUT or SLP IN
 *				Q depending on the target of the request. 
 *
 *  RETURNS:  LSLP_OK or LSLP_INTERNAL_ERROR
 *
 ***************************************************************/
uint32 processAPIIn(lslpWork *newWork)
{
  int8 apiHdr[39];
  int32 bytesRead;

  assert(newWork != NULL);
  memset(&apiHdr[0], 0x00, 39);
  bytesRead = connectedRead(newWork->apiSock, &apiHdr, 38 ); 
  if (bytesRead == 38) { 
    if(! strcmp(&apiHdr[4], LSLP_API_SIGNATURE)) {
      if(LSLP_API_HDR_VERSION == (_LSLP_GETLONG((int8 *)&apiHdr[0], 0))) {
	newWork->na.data_len = ((_LSLP_GETSHORT((int8 *)&apiHdr[32], 0)) - 38);
	if(   (newWork->na.data_len > 0 ) &&   
	   (NULL != (newWork->na.dyn_data = calloc(newWork->na.data_len + 2, sizeof(int8))))) {
	  newWork->hdr.data = newWork->na.dyn_data;
	  bytesRead = connectedRead(newWork->apiSock, newWork->na.dyn_data, newWork->na.data_len);
	  if(bytesRead == newWork->na.data_len) {
	    switch ( _LSLP_GETSHORT((int8 *)&apiHdr[36], 0) ) {
	    case LSLP_SIMPLE_SRVREQ:
	      lslpHandleSimpleSrvReq(newWork);
	      newWork->txerr(newWork);
	      return(LSLP_OK);
	    case LSLP_SIMPLE_SRVRPLY:
	      break;
	    case LSLP_SIMPLE_SRVREG:
	      lslpHandleSimpleSrvReg(newWork);
	      newWork->txerr(newWork);
	      return(LSLP_OK);
	    case LSLP_SIMPLE_SRVDEREG:
	      break;
	    case LSLP_API_ACK:
	      break;
	    case LSLP_REMOTE_SRVREQ:
	      lslpHandleRemoteSrvReq(newWork);
	      newWork->txerr(newWork);
	      break;
	    case LSLP_REMOTE_SRVRPLY:
	      break;
	    default:
	      break;
	    }
	  } /* if we read the buffer correctly */
	} /* if we allocated our buffer space */
      } /* if the version is correct */
    } /* if the signature matches */
  } /* if the version matches */
  /* other side closed the connection ! */
  if(bytesRead == 0)
    bytesRead = -1;
  newWork->hdr.errCode = bytesRead;
  newWork->txerr(newWork);
  return(LSLP_INTERNAL_ERROR);
}	


/****************************************************************
 *  dispatchAPI
 *
 *
 *  PARAMETERS: IN struct nameAddr *na holds info about the remote and
 *				local interfaces.
 *				IN SOCKET sock, when non-zero, contains the socket 
 *				to read.
 *
 *  DESCRIPTION: If socket is zero, accepts a connection and creates
 *				a new socket. allocates a work node and assigns the socket 
 *				to the new worknode. Calls processAPIIn to dispatch the
 *				new work. 
 *
 *  RETURNS:
 *
 ***************************************************************/
#define LSLP_NEW_CONN 1
#define LSLP_EXISTING_CONN 0

void dispatchAPI(SOCKET sock, int32 mode)
{
  SOCKADDR_IN rmtAddr;
  int32 fromLen = sizeof(SOCKADDR_IN);
  SOCKET tempSock;

  if (mode  == LSLP_NEW_CONN) {
    int32 size;
    tempSock = accept(sock, (struct sockaddr *)&rmtAddr, &fromLen);
    size = 51000;
    setsockopt(tempSock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(size));
    size = 51000;
    setsockopt(tempSock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(size));
  }
  else {
    tempSock = sock;
  }
  if(mode == LSLP_NEW_CONN)
    assert(tempSock != sock);
  if (INVALID_SOCKET != tempSock) {
    lslpWork *newWork = allocWorkNode();
    if (newWork != NULL) {
      newWork->status |= (LSLP_STATUS_TCP | LSLP_STATUS_API );
      newWork->na.version = LSLP_NAMEADDR_VERSION;
      memcpy(&(newWork->na.rmtAddr), &rmtAddr, sizeof(SOCKADDR_IN));
      fromLen = sizeof(SOCKADDR_IN);
      getsockname(tempSock, (struct sockaddr *)&(newWork->na.myAddr), &fromLen);
      newWork->apiSock = tempSock;
      if (  processAPIIn(newWork)) {
	_report_event(LSLP_SEV_IDX(35, LSLP_SEVERITY_ERROR), NULL, 0);
      }
    }
  }
    return;
}	

void meshctrlIn(lslpWork *w)
{
  /* mesh-enhanced */
  _lslpMeshCtrlInHandler(w) ;

  return;
}

void invalidIn(lslpWork *w)
{
  assert(FALSE);
}	

void srvrqstIn(lslpWork *w)
{

  lslpHandleSrvReq(w) ;

}	
void srvrplyIn(lslpWork *w)
{

  lslpHandleSrvRply(w);
	
}	
void srvregIn(lslpWork *w)
{
  int8 *extension;
  int16 ext_nbr;
  int32 offset = LSLP_NEXT_EX;
  extension = lslpGetNextExtension(w, &offset);
  while(NULL != extension) {
    ext_nbr = _LSLP_GETSHORT(extension, 0);
    
    switch(ext_nbr) {
    case LSLP_MESHCTRL_EXT: {
      if(LSLP_MESHCTRL_FORWARD  == (_LSLP_GETBYTE(extension, 5))) {
	/* delete the forward action code */
	_LSLP_SETBYTE(extension, 0, 5);
	/* we need to forward this srvreg to every peer that has the */
	/* intersecting registration scope */
	_lslpMeshFwdSrvReg(w);
      }
      break;
    }
    default:
      break;
    }
    extension = lslpGetNextExtension(w, &offset);
  }

  lslpHandleSrvReg( w ) ;
}
	
void srvderegIn(lslpWork *w)
{
  /* traverse the appropriate reg list and look */
  /* for a srvtype match. if found, unlink and free */
  /* then dereg with remote das. */

	
}	
void srvackIn(lslpWork *w)
{
    lslpHandleSrvACK( w ) ;
}
	
void attrreqIn(lslpWork *w)
{
  /* when txing the response, */
  /* copy the session to work->scratch, free the session */
  lslpHandleAttrReq(w);
}	
void attrrepIn(lslpWork *w)
{
	
}	

void daadvertIn(lslpWork *w) 
{

  lslpHandleDAAdvert(w);
}
	
void srvtyperqstIn(lslpWork *w)
{
  /* when txing the response, */
  /* copy the session to work->scratch, free the session */
	
}	
void srvtyperplyIn(lslpWork *w)
{
	
}	
void saadvertIn(lslpWork *w)
{
	
}	

/******************************************/
/* structure constructors and destructors */
/******************************************/

lslpHdr * lslpAllocHdr(void)
{
  return((lslpHdr *)calloc(1, sizeof(lslpHdr)));
}	

void lslpFreeHdr(lslpHdr *hdr)
{
  assert(hdr != NULL);
  if (hdr->data != NULL)
    free(hdr->data);
  free(hdr);
}	

lslpAuthBlock *lslpAllocAuthBlock(void)
{
  return((lslpAuthBlock *)calloc(1, sizeof(lslpAuthBlock)));
}	

lslpAuthBlock *lslpAllocAuthList(void)
{
  lslpAuthBlock *head;
  if (NULL != (head = lslpAllocAuthBlock()))
    {
      head->next = head->prev = head;
      head->isHead = TRUE;
      return(head);
    }
  return(NULL);
}

	
/* NOTE: be CERTAIN block is not linked to a list !!! */
void lslpFreeAuthBlock(lslpAuthBlock *auth)
{
  if (auth->spi != NULL)
    free(auth->spi);
  if (auth->block != NULL)
    free(auth->block); 
  free(auth);
  return;		
}	

void lslpFreeAuthList(lslpAuthBlock *list)
{								
  lslpAuthBlock *temp;

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  while(! (_LSLP_IS_EMPTY(list)))
    {
      temp = list->next;
      _LSLP_UNLINK(temp);
      lslpFreeAuthBlock(temp);
    }
  lslpFreeAuthBlock(list);
  return;
}	

/* for protected scopes, we need to change this routine (or add another one) */
/* that uses an SPI list as an input and stuffs only only stuffs  auth blocks*/
/* that match spis in the input list */

/* this routine will return FALSE AND alter the buffer and length */
BOOL lslpStuffAuthList(int8 **buf, int16 *len, lslpAuthBlock *list)
{
  int8 *bptr;
  int16 numAuths = 0;
  lslpAuthBlock *auths;
  BOOL ccode = TRUE;
  assert(buf != NULL);
  assert(len != NULL);
  /*   assert(list != NULL); */
  if (buf == NULL || *buf == NULL || len == NULL )
    return(FALSE);
  /* always return true on an empty list so we can continue to build the */
  /* msg buffer - an empty list is not an error! */
  bptr = *buf;
  _LSLP_SETBYTE(bptr, numAuths, 0);

  (*buf)+= 1;
  (*len) -= 1;
  if(list == NULL || _LSLP_IS_EMPTY(list))
    return(TRUE);
  /* auth list */
  auths  = list->next;
  memset(*buf, 0x00, *len);
  while (! _LSLP_IS_HEAD(auths) && auths->len <= *len)
    {
      /* store the auth bsd */ 
      _LSLP_SETSHORT(*buf, auths->descriptor, 0);
      /* store the the bsd size */
      _LSLP_SETSHORT(*buf, auths->len, 2);
      /* store the timestamp */
      _LSLP_SETLONG(*buf, auths->timestamp, 4);
      /* store spi string length */
      _LSLP_SETSHORT(*buf, auths->spiLen, 8);
      assert(strlen(auths->spi) + 1 == auths->spiLen );
      /* store the spi string */
      strcpy((*buf) + 10, auths->spi);
      /* store the authentication block */
      if (auths->block != NULL && (auths->len - (auths->spiLen + 10) ) > 0 )
	memcpy(((*buf) + 10 + auths->spiLen), auths->block, (auths->len - (auths->spiLen + 10)));
      (*buf) += auths->len; 
      (*len) -= auths->len;
      numAuths++;
      auths = auths->next;
    } /* while we are traversing the attr list */
  if (! _LSLP_IS_HEAD(auths))
    {
      /* we terminated the loop before copying all the auth blocks */
      ccode = FALSE;
    }
  else
    {
      _LSLP_SETBYTE(bptr, numAuths, 0);
    }
  return(ccode);
}	

lslpAuthBlock *lslpUnstuffAuthList(int8 **buf, int16 *len, int16 *err)
{
  int16 tempLen;
  lslpAuthBlock *list = NULL, *temp;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL);
  assert(err != NULL);
  *err = 0;
  tempLen = _LSLP_GETBYTE(*buf, 0);        /* get the number of auth-blocks */
  (*buf) += 1;                                /* advance to the auth blocks */
  (*len) -= 1;
  if((tempLen == 0) || (NULL != (list = lslpAllocAuthList()))) {
    if(list != NULL) {
      while((tempLen > 0) && (*len > 10) && (*err == 0)) {
	if(NULL != (temp = lslpAllocAuthBlock())) {
	  temp->descriptor = _LSLP_GETSHORT(*buf, 0); /* get the block structure descriptor */
	  (*buf) += 2;                                  /* advance to the block length */
	  temp->len = _LSLP_GETSHORT(*buf, 0);        /* get the block length */
	  (*buf) += 2;                                  /* advance to the timestamp */
	  temp->timestamp = _LSLP_GETLONG(*buf, 0);   /* get the timestamp */
	  (*buf) += 4;                                  /* advance to the spi length */
	  temp->spiLen = _LSLP_GETSHORT(*buf, 0);     /* get the spi length */
	  (*buf) += 2;                                  /* advance to the spi */ 
	  *len -= 10;
	  if(*len >= (temp->spiLen)) {
	    if(NULL != (temp->spi = (uint8 *)calloc(temp->spiLen + 1, sizeof(uint8)))) {
	      memcpy(temp->spi, *buf, temp->spiLen);  /* copy the spi */
	      (*buf) += temp->spiLen;                   /* advance to the next block */
	      (*len) -= temp->spiLen;
	      if(*len >= (temp->len - (10 + temp->spiLen))) {
		if (NULL != (temp->block = 
			     (uint8 *)calloc((temp->len - (10 + temp->spiLen)) + 1, 
					     sizeof(uint8)))) {
		  memcpy(temp->block, *buf, (temp->len - (10 + temp->spiLen)) );
		  _LSLP_INSERT(temp, list);                /* insert the auth block into the list */ 
		  (*buf) += (temp->len - (10 + temp->spiLen));
		  (*len) -= (temp->len - (10 + temp->spiLen));
		  temp = NULL;
		} else {*err = LSLP_INTERNAL_ERROR; } /* if we alloced the auth block buffer */
	      } else { *err = LSLP_PARSE_ERROR;}
	    } else { *err = LSLP_INTERNAL_ERROR ;}/* if we alloced the spi buffer */
	  } else { *err = LSLP_PARSE_ERROR;} 
	}else { *err = LSLP_INTERNAL_ERROR; } /* if we alloced the auth block */
	tempLen--; /* decrement the number of auth blocks */
      } /* while there is room and there are auth blocks to process */
    } else {
      if(tempLen == 0)
	*err = 0;
      else
	*err = LSLP_INTERNAL_ERROR ; 
    }
  }
  if(*err != 0 && list != NULL) {
    lslpFreeAuthList(list);
    list = NULL;
  }
  return(list);
}

lslpURL *lslpAllocURL(void)
{
  lslpURL *url;
  if (NULL != (url = (lslpURL *)calloc(1, sizeof(lslpURL))))
    {
      if (NULL != (url->authBlocks = lslpAllocAuthList()))
	return(url);
      free(url);
    }
  return(NULL);
}	

lslpURL *lslpAllocURLList(void)
{
  lslpURL *head;
  if (NULL != (head = lslpAllocURL()))
    {
      head->next = head->prev = head;
      head->isHead = TRUE;
      return(head);
    }
  return(NULL);	
}	

/* url MUST be unlinked from list */
void lslpFreeURL(lslpURL *url)
{
  assert(url != NULL);
  if (url->url != NULL)
    free(url->url);
  if ( url->authBlocks != NULL)
      lslpFreeAuthList(url->authBlocks);
  if(url->atomized != NULL)
    lslpFreeAtomizedURLList(url->atomized, TRUE);
  free(url);
  return;
}	

void lslpFreeURLList(lslpURL *list)
{								
  lslpURL *temp;

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  while(! (_LSLP_IS_EMPTY(list)))
    {
      temp = list->next;
      _LSLP_UNLINK(temp);
      lslpFreeURL(temp);
    }
  lslpFreeURL(list);
  return;
}	


/* for protected scopes, we need to change stuff URL so that it */
/* only stuffs url auth blocks that match spis in the srv req */
/* this routine may return FALSE AND alter the buffer and length */
BOOL  lslpStuffURL(int8 **buf, int16 *len, lslpURL *url) 
{
  assert((buf != NULL) && (*buf != NULL));
  assert((len != NULL) && (*len > 8));
  assert((url != NULL) && (! _LSLP_IS_HEAD(url)));
  if(_LSLP_IS_HEAD(url))
    return(FALSE);
  memset(*buf, 0x00, *len);
  /* advanced past the reserved portion */
  (*buf) += sizeof(int8);
  _LSLP_SETSHORT(*buf, url->lifetime - time(NULL), 0);
  (*buf) += sizeof(int16);
  url->len = strlen(url->url);
  /* url->url is a null terminated string, but we only stuff the non-null bytes */
  _LSLP_SETSHORT(*buf, url->len, 0);

  (*buf) += sizeof(int16);
  (*len) -= 5;
  if(*len < url->len - 1)
    return(FALSE);
  memcpy(*buf, url->url, url->len);
  (*buf) += url->len;
  (*len) -= url->len;
  return(lslpStuffAuthList(buf, len, url->authBlocks));
}

/* this routine may  return FALSE AND alter the buffer and length */
BOOL lslpStuffURLList(int8 **buf, int16 *len, lslpURL *list) 
{
  BOOL ccode = TRUE;
  assert((buf != NULL) && (*buf != NULL));
  assert((len != NULL) && (*len > 8));
  assert((list != NULL) && (_LSLP_IS_HEAD(list)));
  if(! _LSLP_IS_HEAD(list))
    return(FALSE);
  while((ccode == TRUE) && (! _LSLP_IS_HEAD(list->next))) {
    list = list->next;
    ccode = lslpStuffURL(buf, len, list);
  }
  return(ccode);
}

lslpURL *lslpUnstuffURL(int8 **buf, int16 *len, int16 *err) 
{
  lslpURL *temp;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL && *len > 8);
  assert(err != NULL);
  *err = 0;
  if(NULL != (temp = lslpAllocURL())) {
    temp->lifetime = _LSLP_GETSHORT((*buf), 1);
    temp->len = _LSLP_GETSHORT((*buf), 3);
    (*buf) += 5;
    (*len) -= 5;
    if(*len >= temp->len) { 
      if (NULL != (temp->url = (int8 *)calloc(1, temp->len + 1))) {
	int8 *tempurl;
	memcpy(temp->url, *buf, temp->len);
	*((temp->url) + temp->len) = 0x00;
	tempurl = temp->url;
	temp->atomized = _lslpDecodeURLs((int8 **)&tempurl, 1);
	(*buf) += temp->len;
	(*len) -= temp->len;
	if(temp->atomized != NULL) 
	  lslpFreeAuthBlock(temp->authBlocks);
	temp->authBlocks = lslpUnstuffAuthList(buf, len, err);
      } else {*err = LSLP_INTERNAL_ERROR;}
    } else {*err = LSLP_PARSE_ERROR; }
  } else {*err = LSLP_INTERNAL_ERROR;}
  if(*err != 0 && temp != NULL) {
    lslpFreeURL(temp);
    temp = NULL;
  }
  return(temp);
}

lslpExt *lslpAllocExt(void)
{
  return((lslpExt *)calloc(1, sizeof(lslpExt)));
}	

void lslpFreeExt(lslpExt *ext)
{
  assert(ext != NULL);
  if (ext->data != NULL)
    free(ext->data);
  free(ext);
  return;	
}	


lslpAttrList *lslpAllocAttr(int8 *name, int8 type, void *val, int16 len)
{	
  lslpAttrList *attr;
  if (NULL != (attr = (lslpAttrList *)calloc(1, sizeof(lslpAttrList))))
    {
      if (name != NULL)
	{
	  if (NULL == (attr->name = strdup(name)))
	    {
	      free(attr);
	      return(NULL);
	    }
	}
      attr->type = type;
      if (type == head)	/* listhead */
	return(attr);
      if (val != NULL)
	{
	  attr->attr_len = len;
	  switch (type)	    {
	    case string:
	      if ( NULL != (attr->val.stringVal = strdup((int8 *)val)))
		return(attr);
	      break;
	    case integer:
	      attr->val.intVal = *(uint32 *)val;
	      break;
	    case bool_type:
	      attr->val.boolVal = *(BOOL *)val;
	      break;
	    case opaque:
	      if ( NULL != (attr->val.opaqueVal = strdup((int8 *)val)))
		return(attr);
	      break;
	    default:
	      break;
	    }
	}
    }
  return(attr);
}	


lslpAttrList *lslpAllocAttrList(void)
{
  lslpAttrList *temp;
  if (NULL != (temp = lslpAllocAttr(NULL, head, NULL, 0)))
    {
      temp->next = temp->prev = temp;
      temp->isHead = TRUE;	
    }
  return(temp);
}	

/* attr MUST be unlinked from its list ! */
void lslpFreeAttr(lslpAttrList *attr)
{
  assert(attr != NULL);
  if (attr->name != NULL)
    free(attr->name);
  if(attr->attr_string != NULL)
    free(attr->attr_string);
  if (attr->type == string && attr->val.stringVal != NULL)
    free(attr->val.stringVal);
  else if (attr->type == opaque && attr->val.opaqueVal != NULL)
    free(attr->val.opaqueVal);
  free(attr);
}	

void lslpFreeAttrList(lslpAttrList *list, BOOL staticFlag)
{
  lslpAttrList *temp;

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  while(! (_LSLP_IS_EMPTY(list)))
    {
      temp = list->next;
      _LSLP_UNLINK(temp);
      lslpFreeAttr(temp);
    }
  if(staticFlag == TRUE)
    lslpFreeAttr(list);
  return;
	
}	

BOOL lslpStuffAttrList(int8 **buf, int16 *len, lslpAttrList *list, lslpAttrList *include)
{
		
  lslpAttrList *attrs, *included;
  int16 attrLen = 0, lenSave;
  int8 *bptr;
  BOOL ccode = FALSE, hit = TRUE;
  assert(buf != NULL);
  assert(len != NULL && *len > 3);
/*   assert(list != NULL); */
  if (buf == NULL || len == NULL || *len < 3 )
    return(FALSE);
  /* always return TRUE on an empty list so we can continue to build the */
  /* msg buffer - an empty list is not an error! */
  if (list == NULL || _LSLP_IS_EMPTY(list)) {
    _LSLP_SETSHORT((*buf), 0, 0);  
    (*buf) += 2;
    (*len) -= 2;
    return(TRUE);
  }
  /* attr list */
  lenSave = *len;
  attrs = list->next;
  bptr = *buf;
  
  /* <<< Fri May 14 17:13:22 2004 mdd >>> 
     on some platforms memset must be called with a 4-byte aligned address as the buffer */ 
  /*  memset(bptr, 0x00, *len); */
  (*buf) += 2;	 /* reserve space for the attrlist length short */
  (*len) -= 2;
  while (! _LSLP_IS_HEAD(attrs) && attrLen + 1 < *len) {
    assert(attrs->type != head);
    if(include != NULL && _LSLP_IS_HEAD(include) && (! _LSLP_IS_EMPTY(include))) {
      included = include->next;
      hit = FALSE;
      
      while(! _LSLP_IS_HEAD(included)){
	if( ! lslp_string_compare(attrs->name, included->name)){
	  hit = TRUE;
	  break;
	}
	included = included->next;
      }
    }
    
    if(hit == FALSE){
      attrs = attrs->next;
      continue;
    }
    
    if (attrLen + (int16)strlen(attrs->name) + 3 < *len)
      {
	/* << Wed Jun  9 14:07:54 2004 mdd >> properly encode multi-valued attributes */
	if( _LSLP_IS_HEAD(attrs->prev) || lslp_string_compare(attrs->prev->name, attrs->name)) {
	  

	  if (attrs->type != tag) {
	    **buf = '(';  
	    (*buf)++; 
	    attrLen++;
	    (*len)--;
	  }
	  
	  strcpy(*buf, attrs->name);
	  (*buf) += strlen(attrs->name); 
	  attrLen += strlen(attrs->name);
	  (*len) -= strlen(attrs->name);

	
	  if (attrs->type != tag)
	    {
	      **buf = '='; 
	      (*buf)++; 
	      attrLen++;
	      (*len)--;
	    }
	} /* if not a multi-val */ 
	switch (attrs->type)
	  {
	  case tag:
	    ccode = TRUE;
	    
	    break;
	  case string:
	    if (attrLen + (int16)strlen(attrs->val.stringVal) + 2 < *len)
	      {
		strcpy(*buf, (attrs->val.stringVal));
		(*buf) += strlen(attrs->val.stringVal); 
		attrLen +=  strlen(attrs->val.stringVal);
		(*len) -= strlen(attrs->val.stringVal);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case integer:
	    if (attrLen + 33 + 2 < *len)
	      {
		_itoa( attrs->val.intVal, *buf, 10 );
		attrLen += strlen(*buf);
		(*buf) += strlen(*buf);
		(*len) -= strlen(*buf);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case bool_type:
	    if (attrLen + 6 + 2 < *len)
	      {
		if (attrs->val.boolVal)
		  strcpy(*buf, "TRUE");
		else
		  strcpy(*buf, "FALSE");
		attrLen += strlen(*buf);
		(*buf) += strlen(*buf);
		(*len) -= strlen(*buf);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case opaque:
	    {
	      int16 opLen;
	      opLen = (_LSLP_GETSHORT(((int8 *)attrs->val.opaqueVal), 0));
	      if (attrLen + opLen + 3 < *len)
		{
		  memcpy(*buf, (((int8 *)attrs->val.opaqueVal) + 2), opLen);
		  (*buf) += opLen;
		  attrLen += opLen;
		  (*len) -= opLen;
		  ccode = TRUE;
		}
	      else
		ccode = FALSE;
	      break;
	    }
	  default:
	    ccode = FALSE;
	    break;
	  }
	if (ccode == TRUE && attrLen + 2 < *len && attrs->type != tag)
	  {
	    if( _LSLP_IS_HEAD(attrs->next) || lslp_string_compare(attrs->next->name, attrs->name  )) {
	      **buf = ')'; 
	      (*buf)++; 
	      attrLen++;
	      (*len)--;
	    }
	  }
	if (ccode == TRUE && ! _LSLP_IS_HEAD(attrs->next) && attrLen + 1 < *len)
	  {
	    **buf = ','; 
	    (*buf)++; 
	    attrLen++;
	    (*len)--;
	  }
      }  /* if room for the attr name */
    else
      {
	ccode = FALSE;
	break;
      }
    attrs = attrs->next;
  } /* while we are traversing the attr list */

  /* check for a trailing comma, which may end up in the buffer if we are */
  /* selecting attributes based upon a tag list */
  if(*buf && *(*buf - 1) == ',') {
    *(*buf - 1) = 0x00;
    attrLen -= 1;
    
  }
  

  /* set the length short */
  if (ccode == TRUE)
    {
      _LSLP_SETSHORT(bptr, attrLen, 0);
    }
  else
    {
      (*buf) = bptr; 
      (*len) = lenSave;
      memset(*buf, 0x00, *len);
    }
  return(ccode);
}	


lslpAttrList *lslpUnstuffAttr(int8 **buf, int16 *len, int16 *err)  
{
  int16 tempLen;
  lslpAttrList *temp = NULL;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL);
  assert(err != NULL);
  *err = 0;
  tempLen = _LSLP_GETSHORT(*buf, 0);
  if(tempLen > 0) {
    (*buf) += sizeof(int16);
    (*len) -= sizeof(int16);
    if(*len >= tempLen) {
      if(NULL != (temp = _lslpDecodeAttrString(*buf))) {
      (*buf) += tempLen;
      (*len) -= tempLen;
      } else {*err = LSLP_PARSE_ERROR; }
    } else {*err = LSLP_INTERNAL_ERROR; }
  }
  return(temp);
}


lslpLDAPFilter *lslpAllocFilter(int operator)
{
  lslpLDAPFilter *filter = (lslpLDAPFilter *)calloc(1, sizeof(lslpLDAPFilter));
  if(filter  != NULL) {
    filter->next = filter->prev = filter;
    if(operator == head) {
      filter->isHead = TRUE;
    } else {
      filter->children.next = filter->children.prev = &(filter->children);
      filter->children.isHead = 1;
      filter->attrs.next = filter->attrs.prev = &(filter->attrs);
      filter->attrs.isHead = 1;
      filter->operator = operator;
    }
  }
  return(filter);
}


void lslpFreeFilter(lslpLDAPFilter *filter)
{
  if(filter->children.next != NULL) {
    while(! (_LSLP_IS_EMPTY((lslpLDAPFilter *)&(filter->children)))) {
      lslpLDAPFilter *child = (lslpLDAPFilter *)filter->children.next;
      _LSLP_UNLINK(child);
      lslpFreeFilter(child);
    }
  }
  if(filter->attrs.next != NULL) {
    while(! (_LSLP_IS_EMPTY(&(filter->attrs)))) {
      lslpAttrList *attrs = filter->attrs.next;
      _LSLP_UNLINK(attrs);
      lslpFreeAttr(attrs);
    }
  }
}

void lslpFreeFilterList(lslpLDAPFilter *head, BOOL static_flag)
{
  assert((head != NULL) && (_LSLP_IS_HEAD(head)));
  while(! (_LSLP_IS_EMPTY(head))) {
    lslpLDAPFilter *temp = head->next;
    _LSLP_UNLINK(temp);
    lslpFreeFilter(temp);
  }
  
  if( static_flag == TRUE)
    lslpFreeFilter(head);
  return;
}

void lslpFreeFilterTree(lslpLDAPFilter *root)
{
  assert(root != NULL);
  if( ! _LSLP_IS_EMPTY( &(root->children) ) ) {
    lslpFreeFilterTree((lslpLDAPFilter *)root->children.next);
  }
  if( ! (_LSLP_IS_HEAD(root->next)) && (! _LSLP_IS_EMPTY(root->next)) )
    lslpFreeFilterTree(root->next);
  
  if(root->attrs.next != NULL) {
    while(! (_LSLP_IS_EMPTY(&(root->attrs)))) {
      lslpAttrList *attrs = root->attrs.next;
      _LSLP_UNLINK(attrs);
      lslpFreeAttr(attrs);
    }
  }
}


BOOL lslpEvaluateOperation(int compare_result, int operation)
{
  switch(operation) {
  case expr_eq:
    if(compare_result == 0)	/*  a == b */
      return(TRUE);
    break;
  case expr_gt:
    if(compare_result >= 0)	/*  a >= b  */
      return(TRUE);
    break;

  case expr_lt:			/* a <= b  */
    if(compare_result <= 0)
      return(TRUE);
    break; 
  case expr_present:
  case expr_approx:
  default:
    return(TRUE);
    break;
  }
  return(FALSE);
}


/* evaluates attr values, not names */
BOOL lslpEvaluateAttributes(const lslpAttrList *filter, const lslpAttrList *registered, int op)
{
  assert(filter != NULL && registered != NULL && (! _LSLP_IS_HEAD(filter)) && (! _LSLP_IS_HEAD(registered))) ;
  /* first ensure they are the same type  */
    switch(filter->type) {
    case opaque:
    case string:
      if(registered->type != string && registered->type != opaque)
	return FALSE;
      
      if( registered->type == opaque || filter->type == opaque ) {
	return(lslpEvaluateOperation(memcmp(registered->val.stringVal, 
					    filter->val.stringVal, 
					    _LSLP_MIN(registered->attr_len, 
						      strlen(filter->val.stringVal))), op));
      } else {
        if( TRUE == lslp_pattern_match(registered->val.stringVal, 
					 filter->val.stringVal, 
					 FALSE)) {
	    return( lslpEvaluateOperation(0, op)) ;
	  } else {
	    return (lslpEvaluateOperation(1, op));
	  }
      }
    
      break;
    case integer:
      return( lslpEvaluateOperation( filter->val.intVal - registered->val.intVal, op));
    case tag:			/* equivalent to a presence test  */
      return(TRUE);
    case bool_type:
      if((filter->val.boolVal != 0) && (registered->val.boolVal != 0))
	return(TRUE);
      if((filter->val.boolVal == 0) && (registered->val.boolVal == 0))
	return(TRUE);
      break;
    default: 
      break;
    }
  return(FALSE);

}



/* filter is a filter tree, attrs is ptr to an attr listhead */

BOOL lslpEvaluateFilterTree(lslpLDAPFilter *filter, const lslpAttrList *attrs)
{
  assert(filter != NULL);
  assert(attrs != NULL);
  assert(! _LSLP_IS_HEAD(filter));
  if(filter == NULL || (_LSLP_IS_HEAD(filter)) || attrs == NULL)
    return FALSE;
  
  if(! _LSLP_IS_HEAD(filter->children.next) ) {
    lslpEvaluateFilterTree((lslpLDAPFilter *)filter->children.next, attrs);
  }
  if( ! (_LSLP_IS_HEAD(filter->next)) && (! _LSLP_IS_EMPTY(filter->next)) ) {
    lslpEvaluateFilterTree(filter->next, attrs);
  }
  if(filter->operator == ldap_and || filter->operator == ldap_or || filter->operator == ldap_not) {
    /* evaluate ldap logical operators by evaluating filter->children as a list of filters */
    lslpLDAPFilter *child_list = (lslpLDAPFilter *)filter->children.next;
    /* initialize  the filter's logical value to TRUE */
    if(filter->operator == ldap_or)
      filter->logical_value = FALSE;
    else
      filter->logical_value = TRUE;
    while(! _LSLP_IS_HEAD(child_list)) {
      if(child_list->logical_value == TRUE)  {
	if(filter->operator == ldap_or) {
	  filter->logical_value = TRUE;
	  break;
	}
	if(filter->operator == ldap_not) {
	  filter->logical_value = FALSE;
	  break;
	}
	/* for an & operator keep going  */
      } else {
	/* child is FALSE */
	if(filter->operator == ldap_and) {
	  filter->logical_value = FALSE;
	  break;
	}
      }
      child_list = child_list->next;
    }
  } else  {
    /* find the first matching attribute and set the logical value */
    filter->logical_value = FALSE;
    if(! _LSLP_IS_HEAD(filter->attrs.next) ) {
      attrs = attrs->next;
      while( (! _LSLP_IS_HEAD(attrs )) && 
	     ( FALSE  == lslp_pattern_match(filter->attrs.next->name, attrs->name, FALSE)) ) {
	attrs = attrs->next ; 
      }
      /* either we have traversed the list or found the first matching attribute */
      if( ! _LSLP_IS_HEAD(attrs) ) {
	/* we found the first matching attribute, now do the comparison */
	if (filter->operator == expr_present || filter->operator == expr_approx) 
	  filter->logical_value = TRUE;
	else
	  filter->logical_value = lslpEvaluateAttributes(filter->attrs.next, attrs, filter->operator );
      }
    }
  }
  return(filter->logical_value);
}

lslpMsg *lslpAllocMessage(int8 type) 
{
  lslpMsg *temp = (lslpMsg *)calloc(1, sizeof(lslpMsg));
  if(temp != NULL) {
    temp->dynamic = LSLP_DESTRUCTOR_DYNAMIC;
    temp->type = type;
  }
  return(temp);
}


/********************************/
/***** SLP MSG destructors ******/
/********************************/

/* #define LSLP_DESTRUCTOR_DYNAMIC 1 */
/* #define LSLP_DESTRUCTOR_STATIC  0 */

void lslpDestroySrvReq(struct lslp_srv_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->srvcType != NULL)
    free(r->srvcType);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (r->predicate != NULL)
    free(r->predicate);
  if(r->spiList != NULL)
    lslpFreeSPIList(r->spiList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvRply(struct lslp_srv_rply *r, int8 flag) 
{
  assert(r != NULL);
  if (r->urlList != NULL)
    free(r->urlList);
  if(r->attr_ext_buf != NULL)
    free(r->attr_ext_buf);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	


void lslpDestroyAcceptIDEntry(accept_id_entry *entry, BOOL dyn_flag ) 
{
  if(entry->url_len && entry->url) free(entry->url);
  if(dyn_flag == TRUE)
    free(entry);
}

void lslpDestroyMeshFwdExt(mesh_fwd_ext *ext, BOOL dyn_flag)
{
  lslpDestroyAcceptIDEntry(&(ext->accept_entry), FALSE);
  if(dyn_flag == TRUE)
    free(ext);
}


void lslpDestroySrvReg(struct lslp_srv_reg *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    lslpFreeURLList(r->url);
  if (r->srvType != NULL)
    free(r->srvType);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (r->attrList != NULL)
    lslpFreeAttrList(r->attrList, TRUE);
  if (r->attrAuthList != NULL)
    lslpFreeAuthList(r->attrAuthList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvAck(struct lslp_srv_ack *r, int8 flag)
{
  assert(r != NULL);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroyDAAdvert(struct lslp_da_advert *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->spiList != NULL)
    free(r->spiList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySAAdvert(struct lslp_sa_advert *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvTypeReq(struct lslp_srvtype_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->nameAuth != NULL)
    free(r->nameAuth);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;
}	

void lslpDestroySrvTypeReply(struct lslp_srvtype_rep *r, int8 flag)
{
  assert(r != NULL);
  if (r->srvTypeList != NULL)
    free(r->srvTypeList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	

void lslpDestroyAttrReq(struct lslp_attr_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->tagList != NULL)
    free(r->tagList);
  if (r->spiList != NULL)
    lslpFreeSPIList(r->spiList);			
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	

void lslpDestroyAttrReply(struct lslp_attr_rep *r, int8 flag)
{
  assert(r != NULL);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
	
}	

void lslpDestroySrvDeReg(struct lslp_srv_dereg *r, int8 flag)
{
  assert(r != NULL);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->urlList != NULL)
    lslpFreeURLList(r->urlList);
  if (r->tagList != NULL)
    free(r->tagList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	

void lslpDestroySLPMsg(lslpMsg *msg, int8 flag)
{
  assert(msg != NULL);
  switch (msg->type)
    {
    case srvReq:
      lslpDestroySrvReq(&(msg->msg.srvReq), flag);
      break;
    case srvRply:
      lslpDestroySrvRply(&(msg->msg.srvRply), flag);
      break;
    case srvReg:
      lslpDestroySrvReg(&(msg->msg.srvReg), flag);
      break;
    case srvAck:
      lslpDestroySrvAck(&(msg->msg.srvAck), flag);
      break;
    case daAdvert:
      lslpDestroyDAAdvert(&(msg->msg.daAdvert), flag);
      break;
    case saAdvert:
      lslpDestroySAAdvert(&(msg->msg.saAdvert), flag);
      break;
    case srvTypeReq:
      lslpDestroySrvTypeReq(&(msg->msg.srvTypeReq), flag);
      break;
    case srvTypeRep:
      lslpDestroySrvTypeReply(&(msg->msg.srvTypeRep), flag);
      break;
    case attrReq:
      lslpDestroyAttrReq(&(msg->msg.attrReq), flag);
      break;
    case attrRep:
      lslpDestroyAttrReply(&(msg->msg.attrRep), flag);
      break;
    case srvDereg:
      lslpDestroySrvDeReg(&(msg->msg.srvDereg), flag);
      break;
    default:
      break;
    }
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(msg);
  return;
}	


void _report_event(uint32 code, void *parm, uint32 parmIndex, ...)
{

  if( lslp_log_level >= (int32)(LSLP_MSG_SEVERITY(code))) {
    va_list va;
    int32 ccode = LSLP_WAIT_OK;
    int8 buf[27];
    time_t tm;
    FILE *log_file = NULL;
    /*    _LSLP_WAIT_SEM(lslpPrintSem, LSLP_Q_WAIT, &ccode); Fri Apr  6 10:34:16 2001*/
    if(ccode == LSLP_WAIT_OK) {
      va_start(va, parmIndex);
      memset(&buf[0], 0x00, 27);
      time(&tm);
      _LSLP_CTIME(&tm, buf) ;
      buf[24] = 0x00;  /* get rid of the new line character */
      if(lslp_use_stderr) {
	fprintf(stderr, "%s%s%s", "SLP ", buf, " : ");
	vfprintf(stderr, lslp_Msgs[LSLP_MSG_CODE(code)].msg, va);
      }
      if(lslp_log_file != NULL) {
	log_file = fopen(lslp_log_file, "at");
	if(log_file != NULL) {
	  fprintf(log_file, "%s%s%s", "slp ", buf, " : ");
	  vfprintf(log_file, lslp_Msgs[LSLP_MSG_CODE(code)].msg, va);
	}
	fclose(log_file);
      }
      if(lslp_use_syslog) {
	if(syslog_is_open == FALSE) {
	  _LSLP_OPEN_SYSLOG(&lslp_syslog_handle, "SLP");
	  syslog_is_open = TRUE;
	}
	_LSLP_REPORT_EVENT_SYSLOG(code, parm, parmIndex, va);
      }
      if (((lslp_log_level != LSLP_SEVERITY_MESH) && (lslp_log_level >= 4) && (parm != NULL) && (parmIndex > 0))) {
	switch (parmIndex & 0xffff0000) {
	case LSLP_PARM_WORK:
	  lslp_workdump(parm);
	  break;
	case LSLP_PARM_MSG:
	  lslp_msgdump(parm);
	  break;
	case LSLP_PARM_NA:
	  lslp_nadump(parm);
	  break;
	default:    
	  lslp_hexdump(parm, LSLP_PARM_SIZE(parmIndex));
	  break;
	}
      }
      va_end(va);
      /*       _LSLP_SIGNAL_SEM(lslpPrintSem);  Fri Apr  6 10:34:32 2001 */
    }
  }
  return;
} 

void lslp_nadump(void *parm)
{
  int8 *temp1, *temp2;
  temp1 = strdup((int8 *)inet_ntoa(((struct nameAddr *)parm)->myAddr.sin_addr));
  temp2 = strdup((int8 *)inet_ntoa(((struct nameAddr *)parm)->rmtAddr.sin_addr));
  lslp_print(lslp_Msgs[LSLP_STRINGS_NADUMP].msg,
	     ((struct nameAddr *)parm)->sock, 
	     ((struct nameAddr *)parm)->sock, 
	     htons(((struct nameAddr *)parm)->myAddr.sin_port), 
	     (temp1 != NULL) ? temp1 : "error",
	     htons(((struct nameAddr *)parm)->rmtAddr.sin_port), 
	     (temp2 != NULL) ? temp2 : "error", 
	     ((struct nameAddr *)parm)->data_len,
	     ((struct nameAddr *)parm)->data_len);
  if(temp1 != NULL)
    free(temp1);
  if(temp2 != NULL)
    free(temp2);
}	

void lslp_msgdump(void *parm)
{
  assert(parm != NULL);
  lslp_hexdump(parm, 
	       (_LSLP_MAX((_LSLP_GETLENGTH((uint8 *)parm)), 
			  LSLP_MIN_HDR )));
}	
	
void lslp_workdump(void *parm)
{
  assert(parm != NULL);
  lslp_print(lslp_Msgs[LSLP_STRINGS_WORKDUMP].msg,
	     ((lslpWork *)parm)->type, 
	     ((lslpWork *)parm)->status,
	     ((lslpWork *)parm)->ses_idx,
	     ((lslpWork *)parm)->apiSock,
	     ((lslpWork *)parm)->timer.tv_sec,
	     ((lslpWork *)parm)->timer.tv_usec,
	     ((lslpWork *)parm)->hdr.ver,
	     ((lslpWork *)parm)->hdr.msgid,
	     ((lslpWork *)parm)->hdr.len,
	     ((lslpWork *)parm)->hdr.flags,
	     ((lslpWork *)parm)->hdr.nextExt,
	     ((lslpWork *)parm)->hdr.xid);

  lslp_nadump((void *)&((lslpWork *)parm)->na );
  if (((lslpWork *)parm)->hdr.data != NULL)
    lslp_msgdump(((lslpWork *)parm)->hdr.data);
}	

void lslp_hexdump(void *parm, int32 parmIndex)
{
  int8 *buf;
  int32 size = parmIndex + 0x10;
  size &= 0xfffffff0;
  buf = (uint8 *)calloc(size, sizeof(int8));
  if (buf != NULL)
    {
      int32 i, x;
      memcpy(buf, parm, size);
      for (i = 0, x = 0; i < size ; i += 16)
	{
	  lslp_print(lslp_Msgs[LSLP_STRINGS_HEXDUMP].msg, 
		     (uint8)buf[0 + i],
		     (uint8)buf[1 + i],
		     (uint8)buf[2 + i],
		     (uint8)buf[3 + i],
		     (uint8)buf[4 + i],
		     (uint8)buf[5 + i],
		     (uint8)buf[6 + i],
		     (uint8)buf[7 + i],
		     (uint8)buf[8 + i],
		     (uint8)buf[9 + i],
		     (uint8)buf[10 + i],
		     (uint8)buf[11 + i],
		     (uint8)buf[12 + i],
		     (uint8)buf[13 + i],
		     (uint8)buf[14 + i],
		     (uint8)buf[15 + i], 

		     LSLP_HEXDUMP((uint8)buf[0 + i]),
		     LSLP_HEXDUMP((uint8)buf[1 + i]),
		     LSLP_HEXDUMP((uint8)buf[2 + i]),
		     LSLP_HEXDUMP((uint8)buf[3 + i]),
		     LSLP_HEXDUMP((uint8)buf[4 + i]),
		     LSLP_HEXDUMP((uint8)buf[5 + i]),
		     LSLP_HEXDUMP((uint8)buf[6 + i]),
		     LSLP_HEXDUMP((uint8)buf[7 + i]),
		     LSLP_HEXDUMP((uint8)buf[8 + i]),
		     LSLP_HEXDUMP((uint8)buf[9 + i]),
		     LSLP_HEXDUMP((uint8)buf[10 + i]),
		     LSLP_HEXDUMP((uint8)buf[11 + i]),
		     LSLP_HEXDUMP((uint8)buf[12 + i]),
		     LSLP_HEXDUMP((uint8)buf[13 + i]),
		     LSLP_HEXDUMP((uint8)buf[14 + i]),
		     LSLP_HEXDUMP((uint8)buf[15 + i]));  
	}
      free(buf);
    }
}	

void lslp_print(int8 *msg, ...)
{

  va_list va;
  FILE *log_file = NULL;
  va_start(va, msg);

  if(lslp_use_stderr) {
    vfprintf(stderr, msg, va);
  }
  if(lslp_log_file != NULL) {
    log_file = fopen(lslp_log_file, "at");
    if(log_file != NULL) {
      vfprintf(log_file, msg, va);
    }
    fclose(log_file);
  }
  return;
}




/*************************************************************/

/* test harness -- undef for production version */

void TestMacros(void)
{
  int8 hdr[LSLP_LAN + 8];
  int32 ccode;


  memset(&hdr, 0x00, sizeof(hdr));
  _LSLP_SETVERSION(hdr, 2);
  ccode = _LSLP_GETVERSION(hdr);
  _LSLP_SETFUNCTION(hdr, LSLP_SRVRQST);
  ccode = _LSLP_GETFUNCTION(hdr);
  _LSLP_SETLENGTH(hdr, 65504);
  ccode = _LSLP_GETLENGTH(hdr);
  _LSLP_SETFLAGS(hdr, 3);
  ccode = _LSLP_GETFLAGS(hdr);
  _LSLP_SETLAN(hdr, "en-US");
  ccode = _LSLP_GETLANLEN(hdr);
  _LSLP_SETLANLEN(hdr, 6);
  ccode = _LSLP_GETLANLEN(hdr);
  _LSLP_SETFIRSTEXT(hdr, 65511);
  ccode = _LSLP_GETFIRSTEXT(hdr);
  _LSLP_SETXID(hdr, 461);
  ccode = _LSLP_GETXID(hdr);

}	




/* parameters - 
config file path
install service , windows NT
remove service, windows NT
run in terminal (not as a service), windows NT
*/

#define LSLP_CONFIG_FILE_NAME  "slp.conf"
#define LSLP_INSTALL_SERVICE 0x00000001
#define LSLP_REMOVE_SERVICE  0x00000002
#define LSLP_ATTACH_CONSOLE  0x00000004
#define LSLP_CMD_ERR         0x10000000
void print_slp_usage(void)  
{
#ifdef _WIN32
  printf("\a\nUsage: lslpkernel [-f<config-file>] [-i] [-r] [-c] [-h]");
#else
  printf("\nUsage: lslpkernel [-f<config-file>] [-c] [-h]");
#endif
  printf("\nWhere <config-file> is the relative path to the SLP configuration file;");
#ifdef _WIN32
  printf("\n\t -i installs SLP as a service on Windows NT and Windows 2000 computers;");
  printf("\n\t -r removes SLP as a service on Windows NT and Windows 2000 computers;");
#endif
  printf("\n\t -c causes SLP to attach itself to a console and run in interactive mode;");
  printf("\n\t -h displays this help message.\n");
  return;
}


static uint32 usage(int32 argc, int8 *argv[])
{
  int32 i, y;
  int8 *bptr;
  uint32 ccode = 0;

  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-' || *argv[i] == '/') {
      switch (*(argv[i] + 1)) {
      case 'c':
      case 'C':
	lslp_use_stderr = 1;
	ccode |= LSLP_ATTACH_CONSOLE;
	break;
      case 'h':
      case 'H':
	ccode |= LSLP_CMD_ERR;
	break;
      case 'f':
      case 'F':
	bptr = argv[i];
	bptr +=2;
	y = 0;
	while((*bptr != 0x00) && (y < (LSLP_MAXPATH - 1))) {
	  lslp_default_config[y] = *bptr;
	  y++;
	  bptr++;
	}
	lslp_default_config[y] = 0x00;
	break;
      case 's':
      case 'S':
      case 'i':
      case 'I':
	if(ccode) {
	  printf("\n Error: the -i flag should not be used in combination with any other flags");
	  ccode |= LSLP_CMD_ERR;
	} else {
	  ccode |= LSLP_INSTALL_SERVICE;
	}
	break;

      case 'r':
      case 'R':
	if(ccode) {
	  printf("\n Error: the -f flag should not be used in combination with any other flags");
	  ccode |= LSLP_CMD_ERR;
	} else {
	  ccode |= LSLP_REMOVE_SERVICE;
	}
	break ;
      default:
	printf("\n Unrecognized command-line parameter: %s.", argv[i]);
	ccode |= LSLP_CMD_ERR;
	break;	    	    
      }
    }
  }
  if(ccode & LSLP_CMD_ERR) {
    print_slp_usage();
  }
  return(ccode);
}	

int32 main(int32 argc, int8 *argv[])
{
  uint32 ccode = 0;

  ccode = usage(argc, argv);
  if(ccode & LSLP_CMD_ERR)
    return(1);
  if(ccode & LSLP_INSTALL_SERVICE)
    return((int32)_lslp_install_nt_service());
  if(ccode & LSLP_REMOVE_SERVICE) 
    return((int32) _lslp_remove_nt_service()) ;
  if(ccode & LSLP_ATTACH_CONSOLE) {
    if( LSLP_NOT_INITIALIZED != _startKernel() ) {
#ifdef _AIX					// JSS
      qslp_CreatePidFile(pszSlpDaPidFile);	// JSS
#endif						// JSS
      
      while (! dieNow) {
	int c = getchar();
	if(c == 'q') {
	  dieNow = 1;
	  break;
	}
	_LSLP_SLEEP(waitInterval );
      }

      _stopKernel();
    }

    return(0);
  }
  /* we've passed up every other option - we must be starting as a service */
  _LSLP_SERVICE_MAIN();
  return(0);
}	

