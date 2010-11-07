/* This is an automatically generated prolog.                             */
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



/*                                                                        */
/*                                                                        */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* OBJECT CODE ONLY SOURCE MATERIALS                                      */
/*                                                                        */
/* (C) COPYRIGHT International Business Machines Corp. 2000,2004          */
/* All Rights Reserved                                                    */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */



#ifndef _LSLP_KERNEL_INCLUDE
#define _LSLP_KERNEL_INCLUDE
#ifdef	__cplusplus
extern "C" {
#endif
/* defines for debugging code */
#ifdef DEBUG_BUILD
#define DEBUG_ALLOC
#endif

#include "lslp-common-defs.h"
#include "opaque.h"
#include "lslp.h"
#include "rtt.h"
#include "url.h"
#include "lslp-config.h"
#ifndef __linux__
#pragma pack(push, lslp_kernel)
#pragma pack(1)
#endif
#define LSLP_SESSIONS 64
#define LSLP_SES_VERSION 1
struct lslp_ses {
  int32 version;
  SOCKADDR_IN myAddr;
  SOCKADDR_IN rmtAddr;
  uint32 xid;
  struct timeval stamp;
  struct timeval last;	   /* last time this sess was used */
  struct rtt_struct timer; /* timing info */
};

/* worknode status bits */
#define LSLP_STATUS_FREE		0x00000000

#define LSLP_STATUS_CONVERGE	        0x00000001  /* waiting for convergence */
#define LSLP_STATUS_TCP			0x00000002  /* if set, inAddr is a socket */
						    /* if clear, inAddr is an inet_addr */
#define LSLP_STATUS_API			0x00000004  /* work originated from API */
#define LSLP_STATUS_RCVD		0x00000008  /* work originated from network */
#define LSLP_STATUS_TX  		0x00000010  /* work has been transmitted over net*/
#define LSLP_STATUS_RETX 		0x00000020  /* work has been retried */

#define LSLP_DA_INITIATED_PEER_CONN     0x00000040  /* da that initated a peer connection with us */
#define LSLP_DA_NEEDS_REGS              0x00000080  /* we need to dump regs to this da */
#define LSLP_DA_MESH_ENHANCED		0x00000100  /* da is mesh-enhanced */ 
#define LSLP_DA_PEER_CONNECTED          0x00000200  /* da is a peer of us */ 
#define LSLP_STATUS_PARSE_ERROR         0x00000400  /* malformed msg */
#define LSLP_DA_FWD_DAADV               0x00000800  /* need to fwd da adv to this guy - he must be a new peer */ 
#define LSLP_STATUS_RQ			0x00001000  /* work is a request */
#define LSLP_STATUS_RP		  	0x00002000  /* work is a rply */
#define LSLP_DA_TO_RTX                  0x00004000
#define LSLP_STATUS_MCAST		0x00008000
#define LSLP_STATUS_BCAST 		0x00010000
#define LSLP_STATUS_LOCALHOST           0x00020000
#define LSLP_STATUS_TRUNCATED           0x00040000
#define LSLP_STATUS_DONT_LINGER         0x00080000  /* we don't want this reply to linger */
#define LSLP_STATUS_INTERNAL_ERROR      0x00100000  /* internal error during processing */
#define LSLP_STATUS_KEEP_SOCKET         0x00200000  /* don't close the TCP socket - mesh enhanced */
#define LSLP_DA_KEEP_SOCKET             LSLP_STATUS_KEEP_SOCKET
#define LSLP_STATUS_PEER_SOCKET         0x00400000  /* this is a mesh peer socket */
#define LSLP_STATUS_CLIENT_SOCKET       0x00800000  /* this is a stream client socket */
#define LSLP_STATUS_UDP                 0x01000000
#define LSLP_STATUS_API_SOCKET          0x02000000
#define LSLP_DA_POTENTIAL_PEER          0x04000000  /* a da that we need to peer with */
#define LSLP_DA_DOWN                    0x08000000
#define LSLP_CONNECT_CONNECTING         0x10000000 /* non-blocking connect underway */
#define LSLP_CONNECT_READING            0x20000000 /* stream is connect ed */ 
#define LSLP_CONNECT_CLOSED             0x40000000 /* stream has been closed */

#define LSLP_DA_NOT_PEER                0


  /* meta status macros */

#define _LSLP_CONNECT_TO_PEER(bits) (( ((bits) & LSLP_DA_POTENTIAL_PEER) && \
				       (! ((bits) & LSLP_CONNECT_CONNECTING))) ? TRUE : FALSE) 


#define LSLP_LSL_MTU 1432
#define LSLP_NAMEADDR_VERSION 1
struct nameAddr {
	int32 version;
	SOCKET sock;
	SOCKADDR_IN myAddr;
	SOCKADDR_IN rmtAddr;
	int16 data_len;
	uint8 static_data[LSLP_LSL_MTU + 1];
	uint8 *dyn_data;
} ;

#define LSLP_WORKBUF_AVAIL(w) \
  ((w)->hdr.data == (w)->na.static_data ? LSLP_LSL_MTU : (w)->na.data_len)
#define LSLP_WORKBUF_STATIC(w) \
  ((w)->hdr.data == (w)->na.static_data ? TRUE : FALSE)

struct lslp_work;

#define LSLP_WORKNODE_VERSION 1
typedef struct lslp_work_head {
  struct lslp_work *next;
  struct lslp_work *prev;
  BOOL isHead;
} lslpWorkHead;

typedef struct slp_stream {
  struct slp_stream *next;
  struct slp_stream *prev;
  BOOL isHead;
  LSLP_SEM_T sem;
  uint32 status;
  int16 state;
  int16 next_state;
  time_t in_time;
  SOCKET peerSock;
  time_t last_write;
  time_t last_read;
  time_t close_interval;
  time_t reconnect_interval;
  int32 reconnect_count;
  SOCKADDR_IN peerAddr;
  lslpScopeList *scopeList;
} lslpStream;


typedef struct lslp_work {
  struct lslp_work *next;
  struct lslp_work *prev;
  BOOL isHead;
  int32 version;
  int32 type;		   /* which queue is this node part of? */
  uint32 status;           /* must be zero to destroy this node */
  int32 ses_idx;	   /* for udp, index into lslp_ses array */
  int32 rtx_count;         /* retransmission count */
  /* for tcp, socket number */
  int32 apiSock;      	   /* connected socket for api ipc */
  struct timeval timer;
  lslpHdr hdr;		   /* the slp v2 header, plus pointer to data */
  struct nameAddr na;
  lslpWorkHead rply;
/*   struct slp_stream *stream;  */
  void (*txerr)(struct lslp_work *);
  void *scratch;         /* scratch memory ptr for use in processing msgs. */
}lslpWork;

  struct in_socks {
    struct in_socks *next;
    struct in_socks *prev;
    BOOL isHead;
    SOCKET udp_sock;
    SOCKET bcast_sock;
    SOCKET mcast_sock;
    SOCKET tcp_sock;
    SOCKET api_sock;
  };


#define LSLP_ADDRFLAGS_CONVERGE		LSLP_STATUS_CONVERGE
#define LSLP_ADDRFLAGS_TCP		LSLP_STATUS_TCP
#define LSLP_ADDRFLAGS_MCAST 		LSLP_STATUS_MCAST
#define LSLP_ADDRFLAGS_BCAST		LSLP_STATUS_BCAST
#define LSLP_ADDRFLAGS_LOCALHOST        LSLP_STATUS_LOCALHOST

struct apiAddr{
  int32 version;
  SOCKADDR_IN target;
  uint32 flags;
};


#define LSLP_API_HDR_VERSION 1
#define LSLP_API_SIGNATURE "legato slp stream interface\0"
#define LSLP_API_SIGNATURE_SIZE 28
#define LSLP_API_OFFSET_VER 0
#define LSLP_API_OFFSET_SIG 4
#define LSLP_API_OFFSET_LEN 32
#define LSLP_API_OFFSET_ERR 34
#define LSLP_API_OFFSET_ID  36
#define LSLP_API_HDR_SIZE 38


struct api_hdr {
  int32 version;
  int8 signature[28];
  int16 msgLen; /* including header */
  int16 errCode;
  int16 function_id;
};

#define LSLP_API_ACK                           0x2000 /* with err code in first three bits */
#define LSLP_SIMPLE_SRVREQ                     0x1001
#define LSLP_SIMPLE_SRVRPLY                    0x1002
#define LSLP_SIMPLE_SRVREG                     0x1003
#define LSLP_SIMPLE_SRVDEREG                   0x1004

#define LSLP_REMOTE_SRVREQ                     0x3001
#define LSLP_REMOTE_SRVRPLY                    0x3002

/************************************************************************
remote srvreq:
SOCKADDR_IN target
uint32 flags
int16 lang tag length
int8 *language tag
int16 prlistLen       <-- MUST be zero
int16 srvtype length
int8 *srvtype
int16 scope length
int8 *scope
int16 predicate length
int8 *predicate
int16 spi length
int8 *spi

remote srvrply:
int16 err
int16 count
url entries 


 
simple srvreg: 
struct api_hdr hdr; 
int8 *service-type;  null-terminated string


simple srvrply:
struct api_hdr hdr;
int16 errCode;
int16 num_urls;    number of urls
int8 *urls;         series of null-terminated urls, terminated by 2 null bytes


simple srvreg:
struct api_hdr hdr:
int16 lifetime;   number of seconds to cache registration
int8 *service-type; null-terminated service type string
int8 *url; null-terminated url
int8 *attrs; null-terminated attr string (terminated by 2 nulls)

simple srvdereg:
struct api_hdr hdr;
int8 *url

**************************************************************************/


/* work queue type definitions */
#define LSLP_QS		       	8	  /* how many work queues are there ? */
#define LSLP_Q_API_IN	        0	  /* brand new requests and replies submitted by API*/
#define LSLP_Q_API_OUT		1	  /* all done - ready to return to API and release */
#define LSLP_Q_SLP_IN 		2	  /* incoming msgs from SLP */
#define LSLP_Q_SLP_OUT		3	  /* outgoing msgs to SLP */
#define LSLP_Q_PENDING		4	  /* requests waiting for responses */
#define LSLP_Q_TIMEOUT          5	  /* timed-out requests - need to be re-sent */
#define LSLP_Q_LINGERING        6	  /* responses hanging around in case we have a duplicate request */
#define LSLP_Q_WAITING          7     /* SLP incoming but waiting for a session index */
#define LSLP_LINGER_WAIT        2     /* seconds that we cause responses to linger */
#define LSLP_ORPHAN_WAIT       15     /* seconds we need to wait before deciding a session is orphaned */

/* work queue semaphore definitions */
#define LSLP_Q_WAIT 	 960         /* milliseconds to wait for a queue to come free */
#define LSLP_Q_RETRIES   50           /* how many times to try waiting for a queue to come free */

/* logging definitions */
#define _lslpMsgOut	 lslp_msg_out
/* #define _lslpMsgOut   if (0) lslp_msg_out  */

#define LSLP_MSG_CODE(i)        (0x0000ffff & (i))
#define LSLP_MSG_FACILITY(i)    (((i) >> 16) & 0x0fff)
#define LSLP_MSG_SEVERITY(i)    ((i) >> 30)
#define LSLP_SEV_IDX(i, s)      ((i & 0x0fffffff) | ((s) << 30))
#define LSLP_SEVERITY_SUCCESS   0x00000000
#define LSLP_SEVERITY_INFO      0x00000002
#define LSLP_SEVERITY_WARN      0x00000003
#define LSLP_SEVERITY_ERROR     0x00000004 
#define LSLP_SEVERITY_DEBUG     0x00000005

#define LSLP_SEVERITY_MESH      0x00000001

#define LSLP_LOG_SCREEN		0x00000010   /* output to screen */
#define LSLP_LOG_FILE 		0x00000020   /* output to file */
#define LSLP_LOG_ERRORS		0x00000040   /* log errors */
#define LSLP_LOG_SLP_IO		0x00000080   /* log network I/O on slp port */
#define LSLP_LOG_API_IO		0x00000100   /*	log I/O on api port */
#define LSLP_LOG_Q_IO		0x00000200   /* log q inserts and unlinks */
#define LSLP_LOG_GENERAL	0x00000400   /* general messages */
#define LSLP_LOG_NIO		0x00000800   /* network io */
#define LSLP_LOG_KIO		0x00001000   /* kernel */
#define LSLP_LOG_DATA           0x00002000   /* log data parms */

#define LSLP_PARM_SIZE(i)       ((i) & 0x0000ffff) 
#define LSLP_PARM_NONE		0x00000000
#define LSLP_PARM_WORK 		0x00010000   /* parm is pointer to lslpWork */
#define LSLP_PARM_MSG		0x00020000   /* parm is a pointer to an SLP msg buffer */
#define LSLP_PARM_NA		0x00030000   /* parm is a pointer to a nameAddr struct */

/* command-line process macros */
#define LSLP_CONFIG_FILE_NAME  "slp.conf"
#define LSLP_INSTALL_SERVICE 0x00000001
#define LSLP_REMOVE_SERVICE  0x00000002
#define LSLP_ATTACH_CONSOLE  0x00000004
#define LSLP_CMD_ERR         0x10000000

struct nameAddr *GetHostAddresses(int8 *name);
void CloseStream(SOCKET sock);
SOCKET JoinMulticast(struct in_addr multi, struct nameAddr *temp, uint16 port);
SOCKET PrepSendBcast(struct sockaddr_in *iface) ;
void LeaveMulticast(SOCKET sock);
SOCKET PrepSendMulticast(struct sockaddr_in *iface, int32 ttl);
uint32 _insertWorkNode(lslpWork *node);
uint32 _unlinkWorkNode(lslpWork *node);
lslpWork *_unlinkFirstWorkNode(int32 index);
int32 getSession(struct nameAddr *na);
uint32 _lockList(int32 index);
void _unlockList(int32 index );

uint32 _startKernel(void);
void _stopKernel(void);
uint32 _initWorkLists(void);
uint32 _destroyWorkLists(void);
lslpWork *allocWorkNode(void);
void freeWorkNode(lslpWork *w);
int32 connectedRead(SOCKET fd, void *pv, size_t n);
int32 connectedWrite(SOCKET fd, void *pv, size_t n);
int32 _getSession(struct nameAddr *na);
uint32 _dispatchIn(struct nameAddr *na, SOCKET tcpSock);
int32 dgramWrite(lslpWork *work, void *buf, int32 bufSize, SOCKADDR_IN *bind_addr);
void dispatchSLP(lslpWork *newWork);
void dispatchTCP(lslpStream *stream, SOCKET sock ) ;
uint32 dispatchUDP(SOCKET sock, struct timeval *stamp);
#define LSLP_NEW_CONN 1
#define LSLP_EXISTING_CONN 0
void dispatchAPI(SOCKET sock, int32 mode);
void setRQRPStatus(lslpWork *work);
void apiErr(SOCKET sock);
void setHdrAndRqRpStatus(lslpWork *work);
int32 socketOpen(SOCKET sock);
void udpInternalErr(SOCKADDR_IN *rmt, SOCKADDR_IN *my, int32 xid, int8 err);
void tcpInternalErr(SOCKET sock, int32 xid, int8 err);
uint8 *internalErr(int32 xid, int8 err);
uint8 *formSLPMsg(lslpHdr *hdr, void *buf, int32 bufSize, BOOL allocFlag);

void q_slp_in(void);
void q_waiting(void);
void q_lingering(void);
void q_pending(void);
void slpOutThread(void );

/* our threads */
void *_dispatchThread(void *parm);

BOOL prepareSock(SOCKET sock) ;
BOOL prepareMulticastSock(SOCKET sock, SOCKADDR_IN *addr) ;
BOOL prepareUDPsock(SOCKET sock, SOCKADDR_IN *addr) ;
BOOL prepareTCPSock(SOCKET sock, SOCKADDR_IN *addr) ;

int32 createListeners( LSLP_CONFIG_ADV *transport, struct in_socks *listen_list) ;

void startDASA(struct sockaddr_in *da_addr) ;
void *slpInThread(void *parm);
int8 *encode_opaque(void *buffer, int16 length);
void *decode_opaque(int8 *buffer);
  
void *pipeThread(void *parm) ;
/* debugging messages */
void _report_event(uint32 code, void *parm, uint32 parmIndex, ...) ;
void lslp_msg_out(int8 *file, LSLP_SEM_T sem, uint32 flags, void *parm, int32 parmIndex, int8 *msg, ...);
void lslp_print(int8 *s, ...);
void lslp_hexdump(void *parm, int32 parmIndex);
void lslp_workdump(void *parm);
void lslp_msgdump(void *parm);
void lslp_nadump(void *parm);

/* structure constructors/destructors */
lslpHdr * lslpAllocHdr(void);
void lslpFreeHdr(lslpHdr *hdr);
lslpAuthBlock *lslpAllocAuthBlock(void);
lslpAuthBlock *lslpAllocAuthList(void);
void lslpFreeAuthBlock(lslpAuthBlock *auth);
void lslpFreeAuthList(lslpAuthBlock *list);
BOOL lslpStuffAuthList(int8 **buf, int16 *len, lslpAuthBlock *list);
lslpAuthBlock *lslpUnstuffAuthList(int8 **buf, int16 *len, int16 *err);

lslpURL *lslpAllocURL(void);
lslpURL *lslpAllocURLList(void);
void lslpFreeURL(lslpURL *url);
void lslpFreeURLList(lslpURL *list);
BOOL  lslpStuffURL(int8 **buf, int16 *len, lslpURL *url) ;
BOOL lslpStuffURLList(int8 **buf, int16 *len, lslpURL *list) ;
lslpURL *lslpUnstuffURL(int8 **buf, int16 *len, int16 *err) ;

lslpExt *lslpAllocExt(void);
void lslpFreeExt(lslpExt *ext);
lslpAttrList *lslpAllocAttr(int8 *name, int8 type, void *val, int16 len);
lslpAttrList *lslpAllocAttrList(void);
  BOOL lslpStuffAttrList(int8 **buf, int16 *len, lslpAttrList *list, lslpAttrList *include);
void lslpFreeAttr(lslpAttrList *attr);
void lslpFreeAttrList(lslpAttrList *list, BOOL staticFlag);
lslpAttrList *lslpUnstuffAttr(int8 **buf, int16 *len, int16 *err) ;

#define LSLP_DESTRUCTOR_DYNAMIC 1
#define LSLP_DESTRUCTOR_STATIC  0



  lslpLDAPFilter *lslpAllocFilter(int operator);
  void lslpFreeFilter(lslpLDAPFilter *filter);
  void lslpFreeFilterList(lslpLDAPFilter *head, BOOL static_flag);
  void lslpFreeFilterTree(lslpLDAPFilter *root) ;

  BOOL lslpEvaluateOperation(int compare_result, int operation);
  BOOL lslpEvaluateAttributes(const lslpAttrList *a, const lslpAttrList *b, int op);
  BOOL lslpEvaluateFilterTree(lslpLDAPFilter *filter, const lslpAttrList *attrs);
  lslpLDAPFilter *_lslpDecodeLDAPFilter(int8 *filter) ; /* defined in filter.y  */

void lslpDestroySrvReq(struct lslp_srv_req *r, int8 flag);
void lslpDestroySrvRply(struct lslp_srv_rply *r, int8 flag);
void lslpDestroyAcceptIDEntry(accept_id_entry *entry, BOOL dyn_flag ) ;
void lslpDestroyMeshFwdExt(mesh_fwd_ext *ext, BOOL dyn_flag);
  
  
void lslpDestroySrvReg(struct lslp_srv_reg *r, int8 flag);
void lslpDestroySrvAck(struct lslp_srv_ack *r, int8 flag);
void lslpDestroyDAAdvert(struct lslp_da_advert *r, int8 flag);
void lslpDestroySAAdvert(struct lslp_sa_advert *r, int8 flag);
void lslpDestroySrvTypeReq(struct lslp_srvtype_req *r, int8 flag);
void lslpDestroySrvTypeReply(struct lslp_srvtype_rep *r, int8 flag);
void lslpDestroyAttrReq(struct lslp_attr_req *r, int8 flag);
void lslpDestroyAttrReply(struct lslp_attr_rep *r, int8 flag);
void lslpDestroySrvDeReg(struct lslp_srv_dereg *r, int8 flag);
void lslpDestroySLPMsg(lslpMsg *msg, int8 flag);


  /* rfc 3528 implementation  - mslp.c */ 

  uint32 mslp_existing_da_init(lslpWork *, uint8 *);
  
  uint64 mslp_init_64bit_timestamp(void);
  void mslp_init_accept_id_entry(accept_id_entry *entry, uint8 *url);
  BOOL mslpStuffAcceptIDEntry(uint8 **buf, int16 *len, accept_id_entry *entry);
  BOOL mslpStuffFwdExt(uint8 **buf, int16 *len, mesh_fwd_ext *ext);
  
  mesh_fwd_ext *mslpUnstuffFwdExt(uint8 **buf, int16 *len);
  mesh_fwd_ext *mslpExtractFwdExt(lslpWork *w);
  

#ifdef	__cplusplus
}
#endif

#ifndef __linux__
#pragma pack( pop, lslp_kernel )
#endif
#endif /* _LSLP_KERNEL_INCLUDE */

