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




#ifndef SLP_CLIENT_INC
#define SLP_CLIENT_INC

#ifdef	__cplusplus
extern "C" {
#endif
  
#ifndef NUCLEUS
#include "../lslp-common-defs.h"
#include "../lslp.h"
#endif

#ifdef _AIX		// JSS
#include <netdb.h>	// JSS need gethostbyname_r() prototype
#endif			// JSS

#include <stddef.h>

#define DA_SRVTYPE "service:directory-agent"
#define DA_SRVTYPELEN 23
#define DA_SCOPE "DEFAULT"
#define DA_SCOPELEN 7

#define SA_SRVTYPE "service:service-agent"
#define SA_SRVTYPELEN 21
#define SA_SCOPE "DEFAULT"
#define SA_SCOPELEN 7

#define TYPE_UNKKNOWN 0
#define TYPE_DA_LIST 1
#define TYPE_RPLY_LIST 2

#undef LSLP_WAIT_OK
#define LSLP_WAIT_OK 0

#define slp_safe_free(a)  if(a != NULL) {free(a);}

#ifdef SLP_LIB_IMPORT
#define SLP_STORAGE_DECL DLL_IMPORT
#else
#define SLP_STORAGE_DECL DLL_EXPORT
#endif

#ifdef __linux__
  extern void setLslpMtu(int32 mtu);
  extern int32 getLslpMtu();
#else
  DLL_EXPORT void setLslpMtu(int32 mtu);
  DLL_EXPORT int32 getLslpMtu();
  DLL_EXPORT const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
  DLL_EXPORT int inet_pton(int af, const char *src, void *dst);
  DLL_EXPORT int ip6_addr_type(const char *addr);
  DLL_EXPORT int ip6_is_loopback(const char *addr);
  DLL_EXPORT char *ip6_normalize_addr(const char *, int);

  
#endif

#define LSLP_PATH_MAX 255

  union slp_sockaddr
  {
    struct sockaddr_storage store;
    struct sockaddr saddr;
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
  };
  
  typedef struct lslp_auth_block {
    struct lslp_auth_block *next;
    struct lslp_auth_block *prev;
    BOOL isHead;
    uint16 descriptor;
    uint16 len;
    time_t timestamp;
    uint16 spiLen;
    int8 *spi;
    int8 *block;
  }lslpAuthBlock;


  typedef struct lslp_atom_list {
    struct lslp_atom_list *next;
    struct lslp_atom_list *prev;
    BOOL isHead;
    int8 *str;
    uint32 hash;
  }lslpAtomList;

  typedef struct lslp_atomized_url {
    struct lslp_atomized_url *next;
    struct lslp_atomized_url *prev;
    BOOL isHead;
    int8 *url;
    uint32 urlHash;
    lslpAtomList srvcs;
    lslpAtomList site;
    lslpAtomList host;
    lslpAtomList user;
    lslpAtomList port;
    lslpAtomList path;
    lslpAtomList attrs;
  } lslpAtomizedURL;

  static INLINE void init_atomized_url(struct lslp_atomized_url *url) 
  {
    _LSLP_INIT_HEAD(url);
    _LSLP_INIT_HEAD(&url->srvcs);
    _LSLP_INIT_HEAD(&url->site);
    _LSLP_INIT_HEAD(&url->host);      
    _LSLP_INIT_HEAD(&url->user);
    _LSLP_INIT_HEAD(&url->port);
    _LSLP_INIT_HEAD(&url->path);
    _LSLP_INIT_HEAD(&url->attrs);
  }

  typedef struct lslp_url_entry {
    struct lslp_url_entry *next;
    struct lslp_url_entry *prev;
    BOOL isHead;
    time_t lifetime;
    uint16 len;
    int8 *url;
    uint8 auths;
    lslpAuthBlock *authBlocks;
    lslpAtomizedURL *atomized;
    lslpAtomList *attrs;
    
  }lslpURL ;

  typedef struct lslp_ext {
    uint16 id;
    uint32 nextOffset;
    void *data;
  }lslpExt;

  enum attrTypes {
    head = -1,
    string, 
    integer, 
    bool_type, 
    opaque, 
    tag
  }lslpTypes;

  /* opaque vals should always have the first four bytes */ 
  /* contain the length of the value */
  typedef union lslp_attr_value {
    int8 *stringVal;
    uint32 intVal;
    int32 boolVal;
    void *opaqueVal;
  }lslpAttrVal;

  typedef struct lslp_attr_list {
    struct lslp_attr_list *next;
    struct lslp_attr_list *prev;
    BOOL isHead;
    int16 attr_string_len;
    int8 *attr_string;
    int8 *name;
    int8 type;
    int32 attr_len;
    lslpAttrVal val;
  }lslpAttrList;

  enum ldap_operator_types{
    ldap_and = 260,    /* to match token values assigned in y_filter.h */ 
    ldap_or, 
    ldap_not, 
    expr_eq,
    expr_gt, 
    expr_lt, 
    expr_present, 
    expr_approx
  } ;

  struct ldap_filter_struct ;
  typedef struct ldap_filter_struct_head {
    struct ldap_filter_struct_head  *next;
    struct ldap_filter_struct_head  *prev;
    BOOL isHead;
    int ldap_operator; 
  } filterHead;

  typedef struct ldap_filter_struct {
    struct ldap_filter_struct *next;
    struct ldap_filter_struct *prev;
    BOOL isHead;
    int ldap_operator; 
    int nestingLevel;
    BOOL logical_value;
    filterHead children ;
    lslpAttrList attrs;
  } lslpLDAPFilter ;

  typedef struct lslp_scope_list {
    struct lslp_scope_list *next;
    struct lslp_scope_list *prev;
    BOOL isHead;
    BOOL isProtected;
    int8 *scope;
  }lslpScopeList;
  
  typedef lslpScopeList lslpSPIList;

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
  }lslpSrvRegList;
  
  typedef struct lslp_srv_reg_head {
    struct lslp_srv_reg_instance *next;
    struct lslp_srv_reg_instance *prev;
    BOOL isHead;
  }lslpSrvRegHead;			  /* holds the auth blocks for all attrs in the list */

#define LSLP_CONNECTION_RCV_MSG 0x00000001
#define LSLP_CONNECTION_SND_MSG 0x00000002
#define LSLP_CONNECTION_CLEAR_RCV(con) (con->c.flags &= ~LSLP_CONNECTION_RCV_MSG)
#define LSLP_CONNECTION_CLEAR_SND(con) (con->c.flags &= ~LSLP_CONNECTION_SND_MSG)
#define LSLP_CONNECTION_SET_RCV(con) (con->c.flags |= LSLP_CONNECTION_RCV_MSG)
#define LSLP_CONNECTION_SET_SND(con) (con->c.flags |= LSLP_CONNECTION_SND_MSG)
  /* TCP support */
  typedef struct lslp_connection 
  {
    uint32 flags;
    int8* _msg_buf;
    int8* _rcv_buf;
    int8* _scratch;
    int8* _err_buf;
    int rcv_data_len;
    int msg_data_len;
  } lslpConnection;
  
  typedef struct lslp_connection_head 
  {
    struct lslp_connection* next;
    struct lslp_connection* prev;
    BOOL isHead;
  } lslpConnectionHead;
  

  typedef struct lslp_listener 
  {
    int type;  /* SOCK_STREAM or SOCK_DGRAM */
  } lslpListener;

#define NET_TYPE_LISTENER 0
#define NET_TYPE_CONNECT  1
#define NET_TYPE_SENDTO   2
#define NET_TYPE_RCVFROM  4

  struct slp_net 
  {
    struct slp_net *next;
    struct slp_net *prev;
    BOOL isHead;
    int sock;
    int type;
    char binding[INET6_ADDRSTRLEN];
    char interface[INET6_ADDRSTRLEN];
    int if_index;
    union slp_sockaddr  addr;
    union 
    {
      struct lslp_listener l;
      struct lslp_connection c;
    };
  };
  
    
  /******** internal slp message definitions ********/

  /* URL entry definitions */
#define LSLP_URL_LIFE    1
#define LSLP_URL_LEN	 3
#define _LSLP_GETURLLIFE(h) _LSLP_GETSHORT((h), LSLP_URL_LIFE)
#define _LSLP_SETURLLIFE(h, i) _LSLP_SETSHORT((h), (i), LSLP_URL_LIFE)
#define _LSLP_GETURLLEN(h) _LSLP_GETSHORT((h), LSLP_URL_LEN)
#define _LSLP_SETURLLEN(h, i) _LSLP_SETSHORT((h), (i), LSLP_URL_LEN)

  /*       0                   1                   2                   3 */
  /*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |       Service Location header (function = SrvRqst = 1)        | */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |      length of <PRList>       |        <PRList> String        \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |   length of <service-type>    |    <service-type> String      \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |    length of <scope-list>     |     <scope-list> String       \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |  length of predicate string   |  Service Request <predicate>  \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |  length of <SLP SPI> string   |       <SLP SPI> String        \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

  /* ----- service request ------ */
  struct lslp_srv_req {
    uint16 prListLen;
    int8 *prList;
    uint16 srvcTypeLen;
    int8 *srvcType;
    lslpScopeList *scopeList;
    uint16 predicateLen;
    int8 *predicate;
    lslpSPIList *spiList;
    int8 *next_ext;
    int8 *ext_limit;
  };

  /* ----- service reply ------ */
  struct lslp_srv_rply {
    uint16 errCode;
    uint16 urlCount;
    uint16 urlLen;
    lslpURL *urlList;
    lslpAtomList *attr_list;
  }; 


  /*    SLP messages all begin with the following header: */

  /*       0                   1                   2                   3 */
  /*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |    Version    |  Function-ID  |            Length             | */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      | Length, contd.|O|F|R|       reserved          |Next Ext Offset| */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |  Next Extension Offset, contd.|              XID              | */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |      Language Tag Length      |         Language Tag          \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

  /* 4.3. URL Entries */

  /*       0                   1                   2                   3 */
  /*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |   Reserved    |          Lifetime             |   URL Length  | */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |URL len, contd.|            URL (variable length)              \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |# of URL auths |            Auth. blocks (if any)              \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */



  /* 8.2. Service Reply */

  /*       0                   1                   2                   3 */
  /*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |        Service Location header (function = SrvRply = 2)       | */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |        Error Code             |        URL Entry count        | */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |       <URL Entry 1>          ...       <URL Entry N>          \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

  struct lslp_srv_rply_out 
  {
    uint16 errCode;
    uint16 urlCount;
    uint16 urlLen;
    uint16 overflow;
    int8 *urlList;
  };
  

  /*    The format of the Attribute List Extension is as follows: */

  /*        0                   1                   2                   3 */
  /*        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
  /*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*       |      Extension ID = 0x0002    |     Next Extension Offset     | */
  /*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*       | Offset, contd.|      Service URL Length       |  Service URL  / */
  /*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*       |     Attribute List Length     |         Attribute List        / */
  /*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*       |# of AttrAuths |(if present) Attribute Authentication Blocks.../ */
  /*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */


  /*       0                   1                   2                   3 */
  /*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |         Service Location header (function = SrvReg = 3)       | */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |                          <URL-Entry>                          \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      | length of service type string |        <service-type>         \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |     length of <scope-list>    |         <scope-list>          \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |  length of attr-list string   |          <attr-list>          \ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
  /*      |# of AttrAuths |(if present) Attribute Authentication Blocks...\ */
  /*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */


  /* ----- service registration ------ */
  struct lslp_srv_reg {
    lslpURL *url; 				/* includes url auth block */
    uint16 srvTypeLen;
    int8 *srvType;
    lslpScopeList *scopeList;
    lslpAttrList *attrList;    
    lslpAuthBlock *attrAuthList;
  };

  /* ----- service acknowledgement ----- */
  struct lslp_srv_ack {
    uint16 errCode;
  };

  /* ----- da advertisement ----- */
  struct lslp_da_advert {
    uint16 errCode;
    uint32 statelessBootTimestamp;
    uint16 urlLen;
    int8 *url; /* not a url-entry - merely a url string */
    uint16 scopeListLen;
    int8 *scopeList;
    uint16 attrListLen;
    int8 *attrList;
    uint16 spiListLen;
    int8 *spiList;  /* comma separated values - see 6.4.1*/
    uint8 auths;
    lslpAuthBlock *authBlocks;
  };

  /* -----sa advertisement ----- */

  struct lslp_sa_advert {
    uint16 urlLen;
    int8 *url; /* not a url-entry - merely a url string */
    uint16 scopeListLen;
    int8 *scopeList;
    uint16 attrListLen;
    int8 *attrList;
    uint8 auths;
    lslpAuthBlock *authBlocks;
  };

  /* ----- srv type request  ----- */	

  struct lslp_srvtype_req {
    uint16 prListLen;
    int8 *prList;
    uint16 nameAuthLen;
    int8 *nameAuth;
    lslpScopeList *scopeList;
  };

  /* ----- srv type reply ----- */
  struct lslp_srvtype_rep {
    uint16 errCode;
    uint16 srvTypeListLen;
    int8 *srvTypeList;  /* see 4.1 for srvtype-string; this is a string-list */
  };

  /* ----- attr request ----- */
  struct lslp_attr_req {
    uint16 prListLen;
    int8 *prList;
    uint16 urlLen;
    int8 *url; /* not a url-entry - merely a url string */
    lslpScopeList *scopeList;
    uint16 tagListLen;
    int8 *tagList;
    lslpSPIList *spiList;
  };

  /* ----- attr reply ----- */
  struct lslp_attr_rep {
    uint16 errCode;
    uint16 attrListLen;
    int8 *attrList;
    uint8 auths;
    lslpAuthBlock *authBlocks;
  };

  /* ----- srv deregistration ----- */
  struct lslp_srv_dereg {
    uint16 scopeListLen;
    int8 *scopeList;
    lslpURL *urlList;
    uint16 tagListLen;
    int8 *tagList;
  };	

  /* general internal representation of all slp msgs */
  union lslp_msg_u{
    struct lslp_srv_req srvReq;
    struct lslp_srv_rply srvRply;
    struct lslp_srv_reg	srvReg;
    struct lslp_srv_ack srvAck;
    struct lslp_da_advert daAdvert;
    struct lslp_sa_advert saAdvert;
    struct lslp_srvtype_req srvTypeReq;
    struct lslp_srvtype_rep srvTypeRep;
    struct lslp_attr_req attrReq;
    struct lslp_attr_rep attrRep;
    struct lslp_srv_dereg srvDereg;
  };

  enum msg_types {
    srvReq = 1,
    srvRply,
    srvReg,
    srvDereg,
    srvAck,
    attrReq,
    attrRep,
    daAdvert,
    srvTypeReq,
    srvTypeRep,
    saAdvert
  };

  typedef enum msg_types msgTypes;
  
  typedef struct lslp_hdr{
    uint8 ver;
    uint8 msgid;
    uint32 len;
    uint16 flags;
    uint32 nextExt;
    uint16 xid;
    uint32 errCode;
    uint16 langLen;
    uint8 lang[19];	
    void *data;
  }lslpHdr;

  typedef struct lslp_msg {
    struct lslp_msg *next;
    struct lslp_msg *prev;
    BOOL isHead;
    msgTypes type;
    lslpHdr hdr;
    int8 dynamic;
    union lslp_msg_u msg;
  }lslpMsg;
 
  struct da_list
  {
    struct da_list *next;
    struct da_list *prev;
    BOOL isHead;
  
    int8 function;
    uint16 err;
    uint32 stateless_boot;
    uint32 last_forward;
    int8 *url;
    int8 *scope;
    int8 *attr;
    int8 *spi;
    int8 auth_blocks;
    int8 *auth;
    int8 remote[INET6_ADDRSTRLEN];
  } ;

  struct rply_list
  {
    struct rply_list *next;
    struct rply_list *prev;
    BOOL isHead;

    int8 function;
    uint16 err;
    uint16 lifetime;
    int8 *url;
    int8 auth_blocks;
    int8 *auth;
    int8 remote[INET6_ADDRSTRLEN];
  } ;

  struct  reg_list
  {
    struct reg_list *next;
    struct reg_list *prev;
    BOOL isHead;
    
    int8 *url;
    int8 *attributes;
    int8 *service_type;
    int8 *scopes;
    time_t lifetime;
  } ;

  struct  url_entry
  {
    struct url_entry *next;
    struct url_entry *prev;
    BOOL isHead;

    uint16 lifetime;
    int16 len;
    int8 *url;
    uint8 num_auths;
    int8 *auth_blocks;
  };

  struct sockaddr_list
  {
    struct sockaddr_list *next;
    struct sockaddr_list *prev;
    BOOL isHead;
    union slp_sockaddr addr;
    union slp_sockaddr bcast_addr;
    union slp_sockaddr hw_addr;
    union slp_sockaddr netmask;
    short flags;
    int mtu;
    int if_index;
    char name[LSLP_PATH_MAX + 1];
    char addr6[INET6_ADDRSTRLEN + 1];
  };
  
    

#define LSLP_DEFAULT_TCP_RETRIES 100
#define LSLP_DEFAULT_TCP_SIZE (1024 * 32) 
#define LSLP_MAX_TCP_SIZE 65535 * 4
#define LSLP_SCRATCH_SIZE 65535 * 4
#define LSLP_MAX_IP_ADDRS  16

  struct slp_client  {
    uint32 _pr_buf_len;
    uint32 _buf_len;
    int8 _version;
    int ip_version;
    int ipv6_tcp; // 1 = listen on tcp/ipv6, 0 = do not listen on tcp/ipv6
    int broadcast;
    union slp_sockaddr target_addr;
    unsigned long pad[4];
    union slp_sockaddr  local_addr;
    unsigned long pad2[4];
    uint16 _xid;
    uint16 _target_port;
    struct sockaddr_list local_addr_list;
    uint32 _msg_counts[12];
    lslpSPIList *_spi;
    lslpScopeList *_scopes;
    int8* _pr_buf;
    int8* _msg_buf;
    int8* _rcv_buf;
    int8* _scratch;
    int8* _err_buf;
    
    BOOL _use_das;
    int da_active_discovery;
    uint16 _da_target_port;
    uint32 _da_target_addr;
    
    /* add field and record separators for shell scripting */
    BOOL _use_separators;
    int8 _fs;
    int8 _rs;

    time_t _last_da_cycle;
    struct timeval _tv;
    int _retries;
    int _ttl;
    int  _convergence;
    void *_crypto_context;
#ifdef _WIN32
    int _winsock_count ;
    WSADATA _wsa_data;
#endif
    struct da_list das;
    lslpMsg replies;
    lslpSrvRegHead regs;
    struct slp_net connections;
    struct slp_net listeners;
    lslpMsg *(*get_response) (struct slp_client *, 
			      lslpMsg *);
    int (*find_das)(struct slp_client *, 
		    const int8 *, 
		    const int8 *);
    void (*discovery_cycle) (struct slp_client *, 
			     const int8 *, 
			     const int8 *, 
			     const int8 *);
    void (*converge_srv_req)(struct slp_client *,
			     const int8 *, 
			     const int8 *, 
			     const int8 *);
    void (*unicast_srv_req)(struct slp_client *, 
			    const int8 *, 
			    const int8 *, 
			    const int8 *, 
			    union slp_sockaddr *);
    void (*local_srv_req)(struct slp_client *,
			  const int8 *, 
			  const int8 *, 
			  const int8 *);
    void (*srv_req)( struct slp_client *, 
		     const int8 *, 
		     const int8 *, 
		     const int8 *);
    /** <<< Sat Jul 24 14:56:59 2004 mdd >>> add attr request **/

    void (*converge_attr_req)( struct slp_client *, 
			       const int8 *, 
			       const int8 *, 
			       const int8 * );
  
    void (*unicast_attr_req)( struct slp_client *,
			      const int8 *, 
			      const int8 *,
			      const int8 *,
			      union slp_sockaddr *);
  
    void (*local_attr_req)( struct slp_client *, 
			    const int8 *, 
			    const int8 *, 
			    const int8 * );

    void (*attr_req)( struct slp_client *, 
		      const int8 *, 
		      const int8 *,
		      const int8 *,
		      BOOL);
  
    void (*decode_attr_rply)( struct slp_client *, struct slp_net * );

    BOOL (*srv_reg)(struct slp_client *,
		    const int8 *,
		    const int8 *,
		    const int8 *,
		    const int8 *,
		    int16 );
    int (*srv_reg_all)(struct slp_client *,
		       const int8 *,
		       const int8 *,
		       const int8 *,
		       const int8 *,
		       int16 lifetime );
    int (*srv_reg_local) (struct slp_client *, 
			  const int8 *,
			  const int8 *, 
			  const int8 *, 
			  const int8 *, 
			  uint16 lifetime ); //jeb int16 to uint16
    void (*post_udp_listeners)(struct slp_client *client);
    /* TCP support */
    void (*post_tcp_listeners)(struct slp_client*);
    void (*remove_tcp_listener)(struct slp_client*, SOCKET);
    BOOL (*tcp_srv_reg)(struct slp_client* client, 
			struct slp_net *con, 
			const int8 *url,
			const int8 *attributes,
			const int8 *service_type,
			const int8 *scopes,
			int16 lifetime) ;
    void (*tcp_srv_req)(struct slp_client* client,
                        struct slp_net *con,
                        const int8 *service_type,
                        const int8 *scopes, 
                        const int8 *predicate  );
    int (*tcp_service_listeners)(struct slp_client* );
    int32 (*service_listener)( struct slp_client *, 
			       SOCKETD, lslpMsg * ) ;   //jeb
  
    void (*prepare_pr_buf)(struct slp_client *, 
			   const int8 *);
    int (*prepare_query)(struct slp_client *, 
			 unsigned short int,
			 const char *,
			 const char *, 
			 const char *);
    int (*prepare_attr_query)(struct slp_client *client, 
			      unsigned short xid,
			      const char  *url,
			      const char *scopes, 
			      const char *tags);
    
    void (*decode_msg)( struct slp_client *client, 
			struct slp_net *remote  );
    void (*decode_srvreq)(struct slp_client *, 
			  struct slp_net *);
    void (*decode_srvrply)(struct slp_client *,
			   struct slp_net *);
    void (*decode_daadvert)( struct slp_client *,
			     struct slp_net *);
    void (*decode_attrreq)(struct slp_client *, struct slp_net *);
    int (*send_rcv_udp)(struct slp_client *, struct sockaddr_list *, int);
    int32 (*service_listener_wait)(struct slp_client *, 
				   time_t, 
				   SOCKETD extra, 
				   BOOL, 
				   lslpMsg *) ;
    BOOL (*slp_previous_responder)(struct slp_client *, 
				   int8 *);

    const char *(*get_version)(void);
    const char *(*get_changeset)(void);
    
  };


  /*----------------------------- prototypes -----------------------------------*/

#define LSLP_DESTRUCTOR_DYNAMIC 1
#define LSLP_DESTRUCTOR_STATIC  0
  struct da_list *alloc_da_node(BOOL head);
  struct da_list *da_node_exists(struct da_list *head, const char *url);
  void free_da_list_members(struct da_list *da);
  void free_da_list_node(struct da_list *da);
  void free_da_list(struct da_list *list);
  struct rply_list *alloc_rply_list(BOOL head);
  struct rply_list *rpl_node_exists(struct rply_list *head, const void *key);
  void free_rply_list_members(struct rply_list *rply);
  void free_rply_list_node(struct rply_list *rply);
  void free_rply_list(struct rply_list *list);
  struct reg_list *alloc_reg_list(BOOL head);
  struct reg_list *reg_node_exists(struct reg_list *head, const void *key);
  void free_reg_list_members(struct reg_list *reg);
  void free_reg_list_node(struct reg_list *reg);
  void free_reg_list(struct reg_list *list);
  struct url_entry *alloc_url_entry(BOOL head);
  struct url_entry *url_node_exists(struct url_entry *head, const void *key);
  void free_url_entry_members(struct url_entry *url);
  void free_url_node(struct url_entry *node);
  void free_url_list(struct url_entry *list);
  char *slp_get_host_name( char *buf, int buf_size  );
#if defined( _WIN32 ) 
  int gethostbyname_r(const char *name, 
		      struct hostent *resultbuf, 
		      char *buf, 
		      size_t bufsize, 
		      struct hostent **result, 
		      int *errnop) ;
#endif

  char *slp_get_addr_string_from_url(const char *url, char *addr, int addr_len) ;
  char *slp_get_host_string_from_url(const char *url, char *host, int host_len) ;
  int slp_get_local_interfaces(struct slp_client *);
  
  void make_srv_ack(struct slp_client *client, union slp_sockaddr *remote, int8 response, int16 code );
  void prepare_pr_buf(struct slp_client *client, const int8 *address);
  int prepare_query( struct slp_client *client, 
		     unsigned short xid,
		     const char *service_type,
		     const char *scopes, 
		     const char *predicate) ;
  int prepare_attr_query(struct slp_client *client, 
			 unsigned short xid,
			 const char  *url,
			 const char *scopes, 
			 const char *tags);
  lslpMsg *get_response( struct slp_client *client , lslpMsg *head);
  int find_das(struct slp_client *client, 
	       const int8 *predicate, 
	       const int8 *scopes);
  void discovery_cycle ( struct slp_client *client, 
			 const int8 *type, 
			 const int8 *predicate, 
			 const int8 *scopes) ;

  void converge_srv_req(struct slp_client *client,
			const int8 *type, 
			const int8 *predicate, 
			const int8 *scopes);
  void unicast_srv_req( struct slp_client *client, 
			const int8 *type, 
			const int8 *predicate, 
			const int8 *scopes, 
			union slp_sockaddr *addr );
  void local_srv_req( struct slp_client *client,
		      const int8 *type, 
		      const int8 *predicate, 
		      const int8 *scopes );
  void srv_req( struct slp_client *client, 
		const int8 *type, 
		const int8 *predicate, 
		const int8 *scopes);

  /** <<< Sat Jul 24 14:56:59 2004 mdd >>> add attr request **/

  void converge_attr_req( struct slp_client *client, 
			  const int8 *url, 
			  const int8 *scopes, 
			  const int8 *tags);
  
  void unicast_attr_req( struct slp_client *client,
			 const int8 *url, 
			 const int8 *scopes,
			 const int8 *tags,
			 union slp_sockaddr *addr);
  
  void local_attr_req( struct slp_client *client, 
		       const int8 *url, 
		       const int8 *scopes, 
		       const int8 *tags );
  void attr_req( struct slp_client *client, 
		 const int8 *url, 
		 const int8 *scopes,
		 const int8 *tags,
		 BOOL retry);
  void decode_attr_rply( struct slp_client *client, struct slp_net *remote);

  void decode_srvreg(struct slp_client *client, struct slp_net *remote);
  
  void decode_msg( struct slp_client *client, 
		   struct slp_net *remote );
  void decode_srvrply( struct slp_client *client,
		       struct slp_net *remote );
  void decode_daadvert(struct slp_client *client, 
		       struct slp_net *remote);
  void  decode_attrreq(struct slp_client *, struct slp_net *);
  void decode_srvreq(struct slp_client *client, 
		     struct slp_net *remote );
  void decode_srvack(struct slp_client *client,
		     struct slp_net *remote_addr);

  /* TCP support */
  BOOL tcp_srv_reg(struct slp_client* client, 
		   struct slp_net *con, 
		   const int8 *url,
		   const int8 *attributes,
		   const int8 *service_type,
		   const int8 *scopes,
		   int16 lifetime) ;
  
  BOOL srv_reg(struct slp_client *client,
	       const int8 *url,
	       const int8 *attributes,
	       const int8 *service_type,
	       const int8 *scopes,
	       int16 lifetime) ;
  int32 service_listener_wait(struct slp_client *client, 
			      time_t wait, 
			      SOCKETD extra_sock,  //jeb 
			      BOOL one_only, 
			      lslpMsg *);      

  int32 service_listener(struct slp_client *client, 
			 SOCKETD extra_sock,
			 lslpMsg *);                //jeb

  int srv_reg_all( struct slp_client *client,
		   const int8 *url,
		   const int8 *attributes,
		   const int8 *service_type,
		   const int8 *scopes,
		   int16 lifetime);

  int srv_reg_local ( struct slp_client *client, 
		      const int8 *url,
		      const int8 *attributes, 
		      const int8 *service_type, 
		      const int8 *scopes, 
		      uint16 lifetime);   //jeb int16 to uint16
  void __srv_reg_local ( struct slp_client *client, 
			 const int8 *url,
			 const int8 *attributes, 
			 const int8 *service_type, 
			 const int8 *scopes, 
			 uint16 lifetime);   //jeb int16 to uint16

  BOOL slp_previous_responder(struct slp_client *client, 
			      int8 *pr_list);

  int udp_handle_listener(struct slp_client *client, struct slp_net *net);
  

  /* TCP support */
  void post_tcp_listeners(struct slp_client*);
  void remove_tcp_listener(struct slp_client* client, SOCKET );
  int tcp_service_listeners(struct slp_client* client);
  int tcp_handle_connection(struct slp_client *client, struct slp_net *net);
  int tcp_service_connections(struct slp_client* client);
  int tcp_handle_listener(struct slp_client *client, struct slp_net *net);
  int tcp_handle_connection(struct slp_client *client, struct slp_net *net);
  SLP_STORAGE_DECL struct slp_net *tcp_create_connection(struct slp_client* client, union slp_sockaddr *addr); //788
  int tcp_prepare_query(struct slp_client *client,
			struct slp_net *con, 
			unsigned short xid, 
			const char *service_type,
			const char *scopes, 
			const char *predicate  ) ;
  
  void tcp_srv_req(struct slp_client* client, 
		   struct slp_net *con,    // 1569
		   const int8 *service_type,
		   const int8 *scopes, 
		   const int8 *predicate  ) ;
  
  /* attribute and attribute parser */
  lslpAttrList *_lslpDecodeAttrString(int8 *s);
  lslpAttrList *lslpAllocAttr(int8 *name, int8 type, void *val, int16 len);
  lslpAttrList *lslpAllocAttrList(void);
  void lslpFreeAttr(lslpAttrList *attr);
  void lslpFreeAttrList(lslpAttrList *list, BOOL staticFlag);
  
  BOOL lslpStuffAttrList(int8 **buf, int32 *len, lslpAttrList *list, lslpAttrList *include);
  lslpAttrList *lslpUnstuffAttr(int8 **buf, int32 *len, int16 *err) ;


  /* url and url parser */
  lslpURL *lslpAllocURL(void);
  lslpURL *lslpAllocURLList(void);
  void lslpFreeURL(lslpURL *url);
  void lslpFreeURLList(lslpURL *list);
  BOOL  lslpStuffURL(int8 **buf, int32 *len, lslpURL *url) ;
  BOOL lslpStuffURLList(int8 **buf, int32 *len, lslpURL *list) ;
  lslpURL *lslpUnstuffURL(int8 **buf, int32 *len, int16 *err) ;


  lslpAtomList *lslpAllocAtom(void);
  lslpAtomList *lslpAllocAtomList(void);
  void lslpFreeAtom(lslpAtomList *l);
  void lslpFreeAtomList(lslpAtomList *l, int32 flag);
  lslpAtomizedURL *lslpAllocAtomizedURL(void);
  lslpAtomizedURL *lslpAllocAtomizedURLList(void);
  void lslpFreeAtomizedURL(lslpAtomizedURL *u);
  void lslpFreeAtomizedURLList(lslpAtomizedURL *l, int32 flag);
  void lslpInitAtomLists(void);
  void lslpInitURLList(void);

  lslpAtomizedURL *_lslpDecodeURLs(int8 *u[], int32 count);


  /* ldap filter and parser */

  lslpLDAPFilter *lslpAllocFilter(int ldap_operator);
  void lslpFreeFilter(lslpLDAPFilter *filter);
  void lslpFreeFilterList(lslpLDAPFilter *head, BOOL static_flag);
  void lslpInitFilterList(void );
  void lslpCleanUpFilterList(void);
  void lslpFreeFilterTree(lslpLDAPFilter *root);


  lslpLDAPFilter *_lslpDecodeLDAPFilter(int8 *filter) ;
  BOOL lslpEvaluateOperation(int compare_result, int operation);
  BOOL lslpEvaluateAttributes(const lslpAttrList *a, const lslpAttrList *b, int op);
  BOOL lslpEvaluateFilterTree(lslpLDAPFilter *filter, const lslpAttrList *attrs);

  /* scope lists */
  lslpScopeList *lslpAllocScope(void);
  lslpScopeList *lslpAllocScopeList(void);
  void lslpFreeScope(lslpScopeList *s);
  void lslpFreeScopeList(lslpScopeList *head);
  
#define lslpFreeSPI(a) lslpFreeScope((lslpScopeList *)(a))
#define lslpFreeSPIList(a) lslpFreeScopeList((lslpScopeList *)(a))

  
  BOOL lslpStuffScopeList(int8 **buf, int32 *len, lslpScopeList *list);
  lslpScopeList *lslpUnstuffScopeList(int8 **buf, int32 *len, int16 *err);
  lslpSPIList *lslpUnstuffSPIList(int8 **buf, int32 *len, int16 *err);
  BOOL lslpStuffSPIList(int8 **buf, int32 *len, lslpSPIList *list);
  

  lslpScopeList *lslpScopeStringToList(int8 *s, int32 len) ;
  

  int8 *lslp_foldString(int8 *s);
  BOOL extract_addr6_from_url( const char *url, char* addr, int addr_len);
  BOOL extract_addr4_from_url(const char *url, char *addr, int addr_len);
  BOOL extract_host_from_url(const char *url, char *host, int host_len);
  BOOL extract_addr_from_url(const char *url, char *addr, int addr_len);



  BOOL lslp_scope_intersection(lslpScopeList *a, lslpScopeList *b) ;
  /* caseless compare that folds whitespace */
  int lslp_string_compare(int8 *s1, int8 *s2);
  int lslp_isscope(int c) ;
  BOOL lslp_islegal_scope(int8 *s);
  
  /* service registrations */
  lslpSrvRegList *lslpAllocSrvReg(void);
  lslpSrvRegHead *lslpAllocSrvRegList(void);
  void lslpFreeSrvReg(lslpSrvRegList *s);
  void lslpFreeSrvRegList(lslpSrvRegHead *head);
  


  /* authorization blocks */

  lslpAuthBlock *lslpAllocAuthBlock(void);
  lslpAuthBlock *lslpAllocAuthList(void);
  void lslpFreeAuthBlock(lslpAuthBlock *auth);
  void lslpFreeAuthList(lslpAuthBlock *list);
  BOOL lslpStuffAuthList(int8 **buf, int32 *len, lslpAuthBlock *list);
  lslpAuthBlock *lslpUnstuffAuthList(int8 **buf, int32 *len, int16 *err);

  uint32 lslpCheckSum(int8 *s, int16 l);
  lslpHdr * lslpAllocHdr(void);
  void lslpFreeHdr(lslpHdr *hdr);
  void lslpDestroySrvReq(struct lslp_srv_req *r, int8 flag);
  void lslpDestroySrvRply(struct lslp_srv_rply *r, int8 flag) ;
  void lslpDestroySrvReg(struct lslp_srv_reg *r, int8 flag);
  void lslpDestroySrvAck(struct lslp_srv_ack *r, int8 flag);
  void lslpDestroyDAAdvert(struct lslp_da_advert *r, int8 flag);
  void lslpDestroySAAdvert(struct lslp_sa_advert *r, int8 flag);
  void lslpDestroySrvTypeReq(struct lslp_srvtype_req *r, int8 flag);
  void lslpDestroySrvTypeReply(struct lslp_srvtype_rep *r, int8 flag);
  void lslpDestroyAttrReq(struct lslp_attr_req *r, int8 flag);
  void lslpDestroyAttrReply(struct lslp_attr_rep *r, int8 flag);
  void lslpDestroySrvDeReg(struct lslp_srv_dereg *r, int8 flag);

  BOOL check_duplicate_resp(struct slp_client *client, lslpMsg *msg);
  struct lslp_srv_rply_out *_lslpProcessSrvReq(struct slp_client *client, 
					       struct lslp_srv_req *msg, 
					       int16 errCode, int buf_size,
					       struct slp_net *remote_addr);
  /* a is an attribute list, while b is a string representation of an ldap filter  */
  BOOL lslp_predicate_match(lslpAttrList *a, int8 *b);

  int8 * lslp_get_next_ext(int8 *hdr_buf);
  /* TCP support */
  SLP_STORAGE_DECL void lslpDestroyConnection(struct slp_net *);


  SLP_STORAGE_DECL const char *lslp_get_version(void);
  SLP_STORAGE_DECL const char *lslp_get_changeset(void);

  
  /****** ipv6 support additions ******/


#define IPVER_ANY     0
#define IPVER_4       4
#define IPVER_6       6
    

#define SVRLOC_GRP    "ff05:0:0:0:0:0:0:116"
#define SVRLOC_DA_GRP "ff05:0:0:0:0:0:0:123"
#define SLP_BASE      "ff05:0:0:0:0:0:1:1000"

  struct sockaddr_list *ip6_get_addr_list(struct sockaddr_list *head);
  struct sockaddr_list *ip_get_addr_list(struct sockaddr_list *head);

  void ip_set_client_addrs(struct slp_client *client, 
			   const char *target, 
			   const char *local,
			   unsigned short int port);
  void ip6_set_client_addrs(struct slp_client *client, 
			    const char *target, 
			    const char *local,
			    unsigned short int port);
  unsigned long ipv6_hash(const char *pc, unsigned int len);
  int ip4_join_leave_group(struct slp_client *, 
			   const char *, 
			   struct slp_net *, 
			   int join);
  int ip6_mcast_scope(const char *address);


  /* option IPV6_JOIN_GROUP or IPV6_LEAVE_GROUP */
  int ip6_join_leave_group(struct slp_client *, 
			   const char *, 
			   struct slp_net *, 
			   int option);
  void ip6_join_leave_group_all(const char *group, struct slp_client *client, int option);
  
  int ip4_join_leave_group_all(const char *group, struct slp_client *client, int join);
  
  void ip6_open_listen_socks(struct slp_client *client);
  struct slp_net *ip6_listener(struct slp_client *, struct sockaddr_list *);
  struct slp_net *ip4_listener(struct slp_client *, struct sockaddr_list *);
  void post_udp_listeners(struct slp_client *client);
  void remove_udp_listener(struct slp_client *client, int socket);
  void ip6_insert_reg(struct slp_client *client, struct lslp_srv_reg_instance *reg);
  void ip6_remove_reg(struct slp_client *client, struct lslp_srv_reg_instance *reg);
  int ip6_send_rcv_udp(struct slp_client *client, struct sockaddr_list *iface, int retry);
  void ip6_converge_req(struct slp_client *client,
			const char *one, 
			const char *two, 
			const char *three,
			int (*prepare)(struct slp_client *,
				       unsigned short int,
				       const char *, 
				       const char *, 
				       const char *));
  
  void ip6_unicast_req(struct slp_client *client,
		       const char *one, 
		       const char *two, 
		       const char *three,
		       int (*prepare)(struct slp_client *,
				      unsigned short int,
				      const char *, 
				      const char *, 
				      const char *));
  int ip6_reply(struct slp_client *client, union slp_sockaddr *remote, int len);
  int ip6_service_listeners(struct slp_client *client, struct timeval *wait);
  char *ip6_escape_url(const char *url);
  int ip6_is_wildcard(const char *buf);
  int ip6_escaped_pattern_match(const char *a, const char *b, int caseless);
  int ip6_addr_type(const char *addr);
  int ip6_is_loopback(const char *addr);
  char *ip6_normalize_addr(const char *, int);
  void ip6_listen_all_interfaces(struct slp_client *client);

  /***** Functions Exported by the library *****/
  SLP_STORAGE_DECL  BOOL  lslp_pattern_match(const int8 *s, const int8 *p, BOOL case_sensitive);
  SLP_STORAGE_DECL struct slp_client *create_slp_client(int ip_version, 
							const int8 *target_addr, 
							const int8 *local_interface, 
							uint16 target_port, 
							const int8 *spi, 
							const int8 *scopes,
							BOOL should_listen, 
							BOOL use_das,
							unsigned long flags);

#define CLIENT_FLAGS_NO_TCP_IPV6   0x00000001

  
  SLP_STORAGE_DECL  void destroy_slp_client(struct slp_client *client);
  SLP_STORAGE_DECL  int8 *encode_opaque(void *buffer, int32 length);
  SLP_STORAGE_DECL  void *decode_opaque(int8 *buffer);
  SLP_STORAGE_DECL  lslpMsg *alloc_slp_msg(BOOL head);
  SLP_STORAGE_DECL  void lslpDestroySLPMsg(lslpMsg *msg, int8 flag);
  SLP_STORAGE_DECL BOOL lslp_forward_reg(struct slp_client *client,
				       struct da_list *da,
				       const int8 *url,
				       const int8 *attributes,
				       const int8 *service_type,
				       const int8 *scopes,
				       int16 lifetime);
  SLP_STORAGE_DECL int lslp_forward_regs(struct slp_client *client, struct da_list *da);
  SLP_STORAGE_DECL  void lslp_print_srv_rply(lslpMsg *srvrply);
  SLP_STORAGE_DECL  void lslp_print_srv_rply_parse(lslpMsg *srvrply, int8 fs, int8 rs);
  

  SLP_STORAGE_DECL  void lslp_print_attr_rply(lslpMsg *attrrply);
  SLP_STORAGE_DECL  void lslp_print_attr_rply_parse(lslpMsg *attrrply, int8 fs, int8 rs);

  /** test functions - use these to test the correctness of
      slp strings. They will use the actual parsers to
      try and parse the strings. The return code indicates
      if the string is grammatically correct or not. 
  **/
  SLP_STORAGE_DECL BOOL test_service_type(int8 *type);
  SLP_STORAGE_DECL BOOL test_service_type_reg(int8 *type);
  SLP_STORAGE_DECL BOOL test_url(int8 *url);
  SLP_STORAGE_DECL BOOL test_attribute(int8 *attr);
  SLP_STORAGE_DECL BOOL test_scopes(int8 *scopes);
  SLP_STORAGE_DECL BOOL test_predicate(int8 *predicate);


  /*****************************************************************
   *  test_srv_reg
   *
   *  test the correctness of a service registration - 
   *  return codes: 
   *     0 == SUCCESS
   *     1 == type string failed parsing
   *     2 == url string failed parsing
   *     3 == attribute string failed parsing 
   *     4 == scope string failed parsing
   *
   *****************************************************************/
  SLP_STORAGE_DECL uint32 test_srv_reg(int8 *type, 
				       int8 *url,   
				       int8 *attr, 
				       int8 *scopes);
  
  /*****************************************************************
   * test_query 
   *
   * test the correctness of a service request
   * 
   * return codes:
   *    0 == SUCCESS
   *    1 == type string failed parsing
   *    2 == predicate string failed parsing
   *    3 == scope string failed parsing
   *****************************************************************/

  SLP_STORAGE_DECL uint32 test_query(int8 *type,
				     int8 *predicate, 
				     int8 *scopes);

#ifdef	__cplusplus
}
#endif

#endif /* SLP_CLIENT_INC */

/*
 * Local variables:
 *  c-file-style: "gnu"
 * End:
 */
