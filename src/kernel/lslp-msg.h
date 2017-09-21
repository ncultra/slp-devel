/* .                             */
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



/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                        */
/*                                                                        */
/*                                       */
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
 *  Description: slp message handling headers
 *
 *****************************************************************************/




#ifndef _LSLP_MSGHANDLER_INCLUDE
#define _LSLP_MSGHANDLER_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_BUILD
#define DEBUG_ALLOC
#endif

#include "lslp-common-defs.h"

#ifndef __linux__
#pragma pack(push, lslp_dasa)
#pragma pack(1)
#endif

typedef struct {
    lslpStream *mesh;
    int8 action;
} meshCtrlWork;


#define LSLP_TYPE_DA 1
#define LSLP_TYPE_SA 0
void *_lslpMeshCtrlThread(void *work) ;

void _mesh_link_ret(lslpStream *, void *);
void *_stream_txerr_thread(void *) ;
void stream_txerr(lslpWork *w ); 
/* mesh synchronization states */

#define MESH_MAX_STATES    8
#define MESH_START_STATE     0
#define MESH_CONNECT_STATE   1
#define MESH_ACCEPT_STATE    2
#define MESH_TX_PEER_STATE   3
#define MESH_RX_PEER_STATE   4
#define MESH_TX_DA_STATE     5
#define MESH_RX_DA_STATE     6

/* #define MESH_TX_DATA_STATE   7 */
/* #define MESH_RX_DATA_STATE   8 */

#define MESH_SYNC_STATE      7

/* initiator state transition routines */
void _mesh_start_connect(lslpStream *, void *);
void _mesh_connect_txpeer(lslpStream *, void *);
void _mesh_txpeer_txda(lslpStream *, void *);
void _mesh_txda_rxda(lslpStream *, void *);

void _mesh_rxdata_sync(lslpStream *, void *);

/* initiatee state transition routines */

void _mesh_start_accept(lslpStream *, void *);
void _mesh_accept_rxpeer(lslpStream *, void *);
void _mesh_rxpeer_rxda(lslpStream *, void *);
void _mesh_rxda_txda(lslpStream *, void *);
void _mesh_txdata_sync(lslpStream *, void *);

  /* protocol race condition */
void _mesh_connect_rxpeer(lslpStream *, void *);
void _mesh_txpeer_rxpeer(lslpStream *, void *);
void _mesh_txda_rxpeer(lslpStream *, void *);
void _mesh_txdata_rxpeer(lslpStream *, void *);


/* protocol restart */
void _mesh_sync_start(lslpStream *, void *);

/* invalid state transition or error */
void _mesh_error(lslpStream *, void *);
void _mesh_invalid(lslpStream *, void *);
void _mesh_race(lslpStream *mi, void *parm) ;

/* null transition */
void _mesh_start_start(lslpStream *, void *);

void lslpCleanPendingQ(lslpWork *rply) ;
void lslpHandleRemoteSrvRply(lslpWork *work) ;
void  lslpHandleSrvRply(lslpWork *work) ;
void lslpHandleRemoteSrvReq(lslpWork *work) ;
void lslpHandleSimpleSrvReg(lslpWork *work) ;
void lslpHandleSimpleSrvReq(lslpWork *work) ;
void lslpHandleSrvReq(lslpWork *work) ;
lslpWork *lslpHandleSpecialSrvReq(struct lslp_srv_req *r, int type, lslpWork *work) ;
  int8 *lslpMakeRply(lslpWork *work, void *buf, int16 bufsize, uint32 next_ext) ;
int8 * lslpReInitWorkBuf(lslpWork *work, int16 newSize) ;
struct lslp_srv_rply *_lslpProcessSrvReq(struct lslp_srv_req *msg, int16 errCode) ;
BOOL lslpPreviousResponder(int16 len, int8 *list ) ;
void lslpHandleDAAdvert(lslpWork *w) ;
void _lslpRegisterLocalServices(lslpStream *list) ;
lslpWork *lslpCloneRequest(lslpWork *w, SOCKADDR_IN *rmt, SOCKADDR_IN *my) ;
void _lslpMeshFwdSrvReg(lslpWork *work) ;
lslpWork *lslpCreateSrvReg(lslpSrvRegList *reg, SOCKADDR_IN *rmtAddr) ;
void lslpHandleSrvReg(lslpWork *work ) ;
int8 *lslpMakeSrvACK(lslpWork *work, int16 errCode) ;
uint32 _lslpCheckScopeAndLink(  lslpSrvRegList *reg,  lslpDirAgent *h) ;
void lslpHandleSrvACK(lslpWork *work ) ;
void lslpHandleSrvDereg(lslpWork *work) ;
BOOL lslpCompareURL(lslpURL *a, lslpURL *b) ;
uint16 _lslp_get_xid(void) ;
BOOL lslp_scope_intersection(lslpScopeList *a, lslpScopeList *b) ;
int lslp_string_compare(int8 *s1, int8 *s2) ;
int lslp_isscope(int c) ;
BOOL lslp_islegal_scope(int8 *s) ;
int8 *lslp_foldString(int8 *s) ;
BOOL lslp_pattern_match(const int8 *string, const int8 *pattern, BOOL case_sensitive);
lslpScopeList *lslpScopeStringToList(int8 *s, int16 len) ;
BOOL lslp_predicate_match(lslpAttrList *a, int8 *b);
BOOL lslp_srvtype_match(const int8 *s, int8 *r);
struct nameAddr *lslpGetAddrFromURL(lslpAtomizedURL *url);
void lslpHandleAttrReq(lslpWork *work);
  



void *lslpMeshServiceThread(void *parm) ;
void _lslpMeshFwdDAAdv(lslpDirAgent *list, lslpStream *peers) ;
uint32 _lslpMeshConnectPeers( lslpStream *connectList, lslpStream *peerList) ;
uint32 _lslpMeshInitTXPeers(lslpStream *list, lslpStream *connectList) ;
uint32 _lslpMeshConnectInitPeers( lslpStream *list) ;
int8 *lslpStreamIntersectingDAList(int8 action,
				  lslpDirAgent *list, 
				  lslpDirAgent *us, 
				  lslpStream *rmtDA, 
				  int32 *retLen) ;
uint32 lslpMeshCtrl(lslpStream *mi, int8 action);
void _lslpMeshKeepAlive(lslpStream *list) ;
void _lslpMeshDispatchSocket(lslpStream *mi) ;
void  _lslpMeshReadPeers(lslpStream *list) ;
lslpStream *lslpAllocStream(SOCKADDR_IN *ra) ;
void lslpFreeStream(lslpStream *mi) ;
void lslpDestroyStreamList(lslpStream *l, int32 flag) ;
uint32 _lslpStreamCreateAndLinkIn(lslpStream *l, 
				  SOCKADDR_IN *rmtAddr, 
				  uint32 status ) ;
uint32 _lslpMeshCreateAndLinkIn(lslpStream *l, 
				SOCKADDR_IN *rmtAddr, 
				uint32 status, 
				int8 state,
				int8 next_state,
				int8 *scopes) ;
lslpStream *_lslpFindUnlinkStream(lslpStream *l, SOCKADDR_IN *rmtAddr) ; 
void _lslpMeshHandleDAIndication(lslpWork *w) ;
void _lslpMeshCtrlInHandler(lslpWork *w) ;
BOOL _lslpIsMeshPeerSock(SOCKET s) ;
BOOL _lslpIsClientSock(SOCKET s) ;
BOOL _lslpIsStreamSock(lslpStream *list, SOCKET s, uint32 status) ;
int8 * lslpGetNextExtension(lslpWork *work, int32 *offset) ;
lslpStream * _lslpStreamLockFindSock (lslpStream *list, SOCKET sock) ;
lslpStream * _lslpStreamLockFind(lslpStream *list, SOCKADDR_IN *addr) ;
BOOL _lslpMeshTestSetScope(lslpStream *list, SOCKADDR_IN *addr, int8 *scopes) ;
#define LSLP_CLEAR_BITS 0
#define LSLP_SET_BITS 1
BOOL _lslpMeshSetStatusBits(lslpStream *list, SOCKADDR_IN *addr, uint32 bits, int32 op) ;
BOOL _lslpMeshTestStatusBits(lslpStream *list, SOCKADDR_IN *addr, uint32 bits) ;
#define LSLP_RX_TIME 0
#define LSLP_TX_TIME 1
BOOL _lslpMeshSetTime(lslpStream *list, SOCKADDR_IN *addr, int32 op) ;

#ifndef __linux__
#pragma pack( pop, lslp_dasa )
#endif

#ifdef __cplusplus
}
#endif

#endif /* _LSLP_MSGHANDLER_INCLUDE */

