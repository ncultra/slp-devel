/******************************************************************* 
 *  Description: include file for directory agent and service 
 *	agent routines  
 *
 *****************************************************************************/




#ifndef _LSLP_DASA_INCLUDE
#define _LSLP_DASA_INCLUDE

#ifdef	__cplusplus
extern "C" {
#endif

/* defines for debugging code */
#ifdef DEBUG_BUILD
#define DEBUG_ALLOC
#endif

#include "lslp-common-defs.h"


#ifndef __linux__
#pragma pack(push, lslp_dasa)
#pragma pack(1)
#endif

#define LSLP_DESTRUCTOR_DYNAMIC 1
#define LSLP_DESTRUCTOR_STATIC  0

typedef struct lslp_srv_reg_instance {
  struct lslp_srv_reg_instance *next;
  struct lslp_srv_reg_instance *prev;
  BOOL isHead;
  lslpURL *url;   /* includes url auth block */
  int8 *srvType;
  lslpScopeList *scopeList;
  lslpAttrList *attrList;
  lslpAuthBlock *authList;	  /* signature(s) for attribute list  */
  time_t directoryTime;
  uint64 version_timestamp;       /* time submitted by registering SA */   
  accept_id_entry accept_entry;   /* url of accepting DA and accept timestamp */ 
  uint32 fwd_status;              /* control of fwd to mesh peers */
}lslpSrvRegList;
  
typedef struct lslp_srv_reg_head {
  struct lslp_srv_reg_instance *next;
  struct lslp_srv_reg_instance *prev;
  BOOL isHead;
}lslpSrvRegHead;			  /* holds the auth blocks for all attrs in the list */
  

  /* instead of storing a list of shared scopes in the da structure itself, */
  /* we will derive the list when we need to dump or forward srvregs */
  /* we can use the scope_intersection routine. */
typedef struct lslp_dir_agent {
  struct lslp_dir_agent *next;   /* if we want, we can instantiate multiple */
  struct lslp_dir_agent *prev;   /* das on one machine */
  BOOL isHead;
  uint32 status;
  LSLP_SEM_T writeSem;
  uint32 minRefresh;                          /* minimum refresh interval */
  uint32 activeRegs;
  uint32 timedOutRegs;
  uint32 incrementalRegs;
  uint32 authenticatedRegs;
  int8 *logPath;
  lslpAtomizedURL *url;                 /* service:directory-agent://<addr> */
  time_t statelessBoot;
  struct timeval  last_adv;                      /* last time we received an advert from this da */
  SOCKADDR_IN addr;	                /* the address this da resides at */
  uint32 socket;                        /* socket, if the da is a peer */ 
  lslpScopeList *scopeList;
  lslpSPIList *spiList; 
  lslpAttrList *attrList ;                  /* attributes of this da */
  lslpAuthBlock *authList;	            /* auth blocks for da adverts */
  lslpSrvRegHead *srvRegList;               /* service registrations active at this da */
  uint64 accept_timestamp;
  
}lslpDirAgent;

typedef struct lslp_dir_agent_head {
	struct lslp_dir_agent *next;   
	struct lslp_dir_agent *prev;   
	BOOL isHead;
	uint32 status;
	LSLP_SEM_T writeSem;
}lslpDirAgentHead;


lslpScopeList *lslpAllocScope(void);
lslpScopeList *lslpAllocScopeList(void);
void lslpFreeScope(lslpScopeList *s);
void lslpFreeScopeList(lslpScopeList *head);
#define LSLP_FLAG_SPI 0
#define LSLP_FLAG_SCOPE 1
BOOL lslpStuffScopeList(int8 **buf, int16 *len, lslpScopeList *list);
lslpScopeList *lslpUnstuffScopeList(int8 **buf, int16 *len, int16 *err);
lslpSPIList *lslpUnstuffSPIList(int8 **buf, int16 *len, int16 *err);
BOOL lslpStuffSPIList(int8 **buf, int16 *len, lslpSPIList *list);
lslpSrvRegList *lslpAllocSrvReg(void);
lslpSrvRegHead *lslpAllocSrvRegList(void);
void lslpFreeSrvReg(lslpSrvRegList *s);
void lslpFreeSrvRegList(lslpSrvRegHead *head);
uint32 _lslpInitSAList(void);
uint32 _lslpInitDAList(void);
BOOL lslpIsValidDAAddress(uint32 addr) ;
BOOL _lslpLinkDA(lslpDirAgent *da, lslpDirAgent *l);
BOOL _lslpUnlinkDA(lslpDirAgent *da, lslpDirAgent *l);
uint32 lslpCreateDA(uint32 minRefresh, 
		     int8 *logPath,
		     int8 *url,
		     SOCKADDR_IN *hostAddr,
		     int8 *scopes,
		     int8 *spis,
		     int8 *attrs, 
		     int32 flag);
void lslpFreeDA(lslpDirAgent *da);
BOOL _lslpDestroyDA(int8 *url); 
BOOL _lslpDestroyRemoteDA(int8 *url) ;
void _lslpDestroyDAList(lslpDirAgent *l, BOOL dynFlag);
BOOL lslpValidateDAURL(lslpDirAgent *da, int8 *url);
BOOL lslpValidateDAScopeList(lslpDirAgent *da, int8 *scopes);
BOOL lslpValidateDASPIList(lslpDirAgent *da, int8* spis);
BOOL lslpValidateDAAttrList(lslpDirAgent *da, int8 *attr);
BOOL lslpValidateDAAuthList(lslpDirAgent *da);
void lslp_daAdvertThread(void );
lslpWork *lslpCreateDASrvReq(SOCKADDR_IN *addr_to,
			      SOCKADDR_IN *addr_from,
			      lslpScopeList *scopeList, 
			      lslpSPIList *spiList );
lslpWork *lslpCreateDAAdvert(lslpDirAgent *da, 
			      int8 *url, 
			      lslpWork *r, 
			      LSLP_CONFIG_ADV *config) ;
time_t _lslpFindExistingDA(int8 *url, lslpDirAgent *l, BOOL lock) ;
  lslpDirAgent* _lslpFindLockExistingDA(uint8*, lslpDirAgent* );
  
BOOL _lslpCreateDirAgentFromAdvert(lslpWork *work);


#define _lslpLinkSA _lslpLinkDA
#define _lslpUnlinkSA _lslpUnlinkDA
#define LSLP_FLAG_SA	0
#define LSLP_FLAG_DA	1

#define lslpAllocSPI() (lslpSPIList *)lslpAllocScope()
#define lslpAllocSPIList() (lslpSPIList *)lslpAllocScopeList()
#define lslpFreeSPI(s) lslpFreeScope((lslpScopeList *)(s))
#define lslpFreeSPIList(s) lslpFreeScopeList((lslpScopeList *)(s))
  
  void lslpSetDAMeshEnhancedFlag(lslpDirAgent *l);
  lslpStream * _get_da_stream(lslpStream *, uint32 ) ;
  void set_da_stream(SOCKADDR_IN *, uint32, lslpDirAgent *);
  lslpDirAgent * _get_stream_da(lslpDirAgent *, uint32 );
  BOOL da_offline(lslpDirAgent *, uint32 ) ;
  BOOL da_online(lslpDirAgent *, uint32 , SOCKADDR_IN *);
  

#ifndef __linux__
#pragma pack( pop, lslp_dasa )
#endif
#ifdef	__cplusplus
}
#endif
#endif /* _LSLP_DASA_INCLUDE */

