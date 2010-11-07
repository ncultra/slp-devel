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

/*******************************************************************
 *  Description: slp definitions
 *
 *****************************************************************************/




#ifndef _LSLPDEFS_INCLUDE
#define _LSLPDEFS_INCLUDE
#ifdef	__cplusplus
extern "C" {
#endif


#ifndef __linux__
#pragma pack( push, lslp_defs )
#pragma pack(1)
#endif

#include "url.h"

  /********* circular list macros *********************/

  /*---------------------------------------------------------------------**
  ** structures used with these macros MUST have the following elements: **
  ** struct type-name {                                                  **
  ** 	struct type-name *next;                                            **
  ** 	struct type-name *prev;                                            **
  ** 	BOOL isHead;                                                       **
  ** 	}                                                                  **
  **---------------------------------------------------------------------*/

  /*  is node x the head of the list? */
  /* BOOL IS_HEAD(node *x); */
#define _LSLP_IS_HEAD(x) ((x)->isHead )

  /* where h is the head of the list */
  /* BOOL _LSLP_IS_EMPTY(head);          */
#define _LSLP_IS_EMPTY(h) \
	((((h)->next == (h)) && ((h)->prev == (h)) ) ? TRUE : FALSE)

  /* where n is the new node, insert it immediately after node x */
  /* x can be the head of the list                               */
  /* void _LSLP_INSERT(new, after);                                  */
#define _LSLP_INSERT(n, x)   	\
	{(n)->prev = (x); 		\
	(n)->next = (x)->next; 	\
	(x)->next->prev = (n); 	\
	(x)->next = (n); }		

#define _LSLP_INSERT_AFTER _LSLP_INSERT
#define _LSLP_INSERT_BEFORE(n, x)   \
	{(n)->next = (x);					\
	(n)->prev = (x)->prev;				\
	(x)->prev->next = (n);				\
	(x)->prev = (n); }

#define _LSLP_INSERT_WORKNODE_LAST(n, x)    \
        {gettimeofday(&((n)->timer));  \
	(n)->next = (x);	       \
	(n)->prev = (x)->prev;	       \
	(x)->prev->next = (n);	       \
	(x)->prev = (n); }

#define _LSLP_INSERT_WORKNODE_FIRST(n, x)    \
        {gettimeofday(&((n)->timer));  \
	(n)->prev = (x); 	       \
	(n)->next = (x)->next; 	       \
	(x)->next->prev = (n); 	       \
	(x)->next = (n); }		


  /* delete node x  - harmless if mib is empty */
  /* void _LSLP_DELETE_(x);                        */
#define _LSLP_UNLINK(x)				\
	{(x)->prev->next = (x)->next;	\
	(x)->next->prev = (x)->prev;}	

  /* given the head of the list h, determine if node x is the last node */
  /* BOOL _LSLP_IS_LAST(head, x);                                           */
#define _LSLP_IS_LAST(h, x) \
	(((x)->prev == (h) && (h)->prev == (x)) ? TRUE : FALSE)

  /* given the head of the list h, determine if node x is the first node */
  /* BOOL _LSLP_IS_FIRST(head, x);                                           */
#define _LSLP_IS_FIRST(h, x) \
	(((x)->prev == (h) && (h)->next == (x)) ? TRUE : FALSE)

  /* given the head of the list h, determine if node x is the only node */
  /* BOOL _LSLP_IS_ONLY(head, x);                                           */
#define _LSLP_IS_ONLY(h, x) \
	(((x)->next == (h) && (h)->prev == (x)) ? TRUE : FALSE)
  /* void _LSLP_LINK_HEAD(dest, src); */
#define _LSLP_LINK_HEAD(d, s) \
	{(d)->next = (s)->next;  \
	(d)->prev = (s)->prev;  \
	(s)->next->prev = (d);  \
	(s)->prev->next = (d) ; \
        (s)->prev = (s)->next = (s); }


  /************* bit-set macros *********************************/
  /* how many dwords do we need to allocate to hold b bits ? */
#define _LSLP_SIZEOF_BITARRAY(b) (((b) >> 5) + 1)
  /*  operating on an array of dwords */
#define _LSLP_IS_BIT_SET(b, a) (*((a) + ((b) >> 5)) & (1 << (((b)%32) - 1)))
#define _LSLP_SET_BIT(b, a) (*((a) + ((b) >> 5)) |= (1 << (((b)%32) - 1)))
#define _LSLP_CLEAR_BIT(b, a) (*((a) + ((b) >> 5)) ^= (1 << (((b)%32) - 1)))

  /* the usual */
#define _LSLP_MIN(a, b) ((a) < (b) ? (a) : (b))
#define _LSLP_MAX(a, b) ((a) > (b) ? (a) : (b))

  /********************** SLP V2 MACROS *************************/
  /**************************************************************/
#define LSLP_PROTO_VER 2
#define LSLP_PORT 427
#define _LSLP_LOCAL_BCAST inet_addr("255.255.255.255") 
#define _LSLP_MCAST       inet_addr("239.255.255.253")
  /* derive an address for a directed broadcast */
  /* a = uint32 addr, m = uint32 mask */
  /* #define _LSLP_DIR_BCAST(a, m) ((((a) &= (m)) &&  ((a) |= ~(m))) ? (a) : (a) ) */

  //#define LSLP_EN_US "en-USA"
  //#define LSLP_EN_US_LEN 6
  /* the absolute minimum hdr size */
#define LSLP_MIN_HDR  14

  /* slp v2 message types */
#define LSLP_SRVRQST 		 1
#define LSLP_SRVRPLY 		 2
#define LSLP_SRVREG 		 3
#define LSLP_SRVDEREG		 4
#define LSLP_SRVACK		 5
#define LSLP_ATTRREQ		 6
#define LSLP_ATTRRPLY		 7
#define LSLP_DAADVERT		 8
#define LSLP_SRVTYPERQST	 9
#define LSLP_SRVTYPERPLY        10
#define LSLP_SAADVERT		11
#define LSLP_MESHCTRL           12
#define LSLP_ANTI_ENTROPY       12
#define LSLP_MESHCTRL_NULL       0
#define LSLP_MESHCTRL_DATA       2
#define LSLP_MESHCTRL_PEER_IND   3
#define LSLP_MESHCTRL_DA_IND     4
#define LSLP_MESHCTRL_KEEPALIVE  5 
#define LSLP_MESHCTRL_EXT        0x0006
#define LSLP_MESH_FWD_EXT        0x0006
#define LSLP_MESH_FWD_REQUEST_FWD 1
#define LSLP_MESH_FWD_FWDED       2
#define LSLP_MESHCTRL_FORWARD    1

  /* slp error codes */
#define LSLP_LANGUAGE_NOT_SUPPORTED 	 1
#define LSLP_PARSE_ERROR 		 2
#define LSLP_INVALID_REGISTRATION	 3
#define LSLP_SCOPE_NOT_SUPPORTED 	 4
#define LSLP_AUTHENTICATION_UNKNOWN 	 5
#define LSLP_AUTHENTICATION_ABSENT       6
#define LSLP_AUTHENTICATION_FAILED       7
#define LSLP_VERSION_NOT_SUPPORTED       9
#define LSLP_INTERNAL_ERROR 	   	10
#define LSLP_DA_BUSY 			11
#define LSLP_OPTION_NOT_UNDERSTOOD      12
#define LSLP_INVALID_UPDATE 		13
#define LSLP_MSG_NOT_SUPPORTED 	        14
#define LSLP_REFRESH_REJECTED 		15

  /*SLP error codes */
#define LSLP_OK				0x00000000
#define LSLPERR_BASE 			0x00002000
#define LSLPERR_INVALID_SEM 	        LSLPERR_BASE + 	0x00000001
#define LSLP_NOT_INITIALIZED            LSLPERR_BASE + 	0x00000002	
#define LSLP_TIMEOUT			LSLPERR_BASE + 	0x00000003
#define LSLP_MALLOC_ERROR		LSLPERR_BASE + 	0x00000004
#define LSLP_FILE_ERROR                 LSLPERR_BASE +  0x00000005
#define LSLP_LDAP_BIND_ERROR            LSLPERR_BASE +  0x00000006
#define LSLP_LDAP_MODIFY_ERROR          LSLPERR_BASE +  0x00000007
#define LSLP_LDAP_NOT_INITIALIZED       LSLPERR_BASE +  0x00000008


  /* offsets into the SLPv2 header */
#define LSLP_VERSION 	 0
#define LSLP_FUNCTION 	 1
#define LSLP_LENGTH 	 2
#define LSLP_FLAGS 	 5
#define LSLP_NEXT_EX 	 7
#define LSLP_XID 	10
#define LSLP_LAN_LEN 	12
#define LSLP_LAN 	14


  /* macros to gain access to SLP header fields */
  /* h = (uint8 *)slpHeader */
  /* o = int32 offset */
  /* i = int32 value */
#ifdef WORDS_BIG_ENDIAN 
#define _LSLP_GETBYTE(h, o)  (0x00ff  & *((uint8 *) &((h)[(o)])))
#define _LSLP_GETSHORT(h, o) ((0xff00 & _LSLP_GETBYTE((h), (o))) + \
                               (0x00ff & _LSLP_GETBYTE((h), (o) + 1)))

#define _LSLP_SETBYTE(h, i, o) ((h)[(o)] = (uint8)(i))
#define _LSLP_SETSHORT(h, i, o) { _LSLP_SETBYTE((h), (0xff00 & (i)), (o)); \
                                  _LSLP_SETBYTE((h), (0x00ff & (i)), (o) + 1); }

#define _LSLP_GETLONG(h, o)   ((0xffff0000 & _LSLP_GETSHORT((h), (o))) + \
                               (0x0000ffff & _LSLP_GETSHORT((h), (o) + 2)))

#define _LSLP_SETLONG(h, i, o) { _LSLP_SETSHORT((h), (0xffff0000 & (i)), (o)); \
                                 _LSLP_SETSHORT((h), (0x0000ffff & (i)), (o) + 2); }

#define _LSLP_SETLONGLONG(h, i, o)		\
{						\
  _LSLP_SETLONG((uint8*)h, (((uint64)(i)) & 0xffffffff00000000), (o)) ;	\
  _LSLP_SETLONG((uint8*)h, (((uint64)(i)) & 0x00000000ffffffff), (o) + 4);	\
}

#define _LSLP_GETLONGLONG(h, o) \
  (uint64)((((uint64)_LSLP_GETLONG((uint8*)(h), (o))) & 0xffffffff00000000)  + \
	   (((uint64)_LSLP_GETLONG((uint8*)(h), (o) + 4)) & 0x00000000ffffffff) )

  /* length is a 3-byte value */

#define _LSLP_GET3BYTES(h, o) ((0xff0000 & (_LSLP_GETBYTE((h), (o)) << 16)) + \
                               (0x00ff00 & (_LSLP_GETBYTE((h), ((o) + 1)) << 8)) + \
                               (0x0000ff & (_LSLP_GETBYTE((h), ((o) + 2)))))

#define _LSLP_SET3BYTES(h, i, o) {_LSLP_SETBYTE((h), (0xff & ((uint8)((i) >> 16))), (o));  \
                                  _LSLP_SETBYTE((h), (0xff & ((uint8)((i) >> 8))), ((o) + 1));  \
                                  _LSLP_SETBYTE((h), (0xff & ((uint8)((i)))), ((o) + 2));}

#define _LSLP_GETLENGTH(h) _LSLP_GET3BYTES((h), LSLP_LENGTH)
#define _LSLP_SETLENGTH(h, i) _LSLP_SET3BYTES((h), (i), LSLP_LENGTH)

#define _LSLP_GETFIRSTEXT(h) _LSLP_GET3BYTES((h), LSLP_NEXT_EX)
#define _LSLP_SETFIRSTEXT(h, i) _LSLP_SET3BYTES((h), (i), LSLP_NEXT_EX)
  
#define _LSLP_GETNEXTEXT _LSLP_GET3BYTES
#define _LSLP_SETNEXTEXT _LSLP_SET3BYTES

#else /***** LITTLE  Endian *****/


#define _LSLP_GETBYTE(h, o)  (0x00ff  & *(uint8 *) &((h)[(o)]))
#define _LSLP_GETSHORT(h, o) ((0xff00 & _LSLP_GETBYTE((h), (o)) << 8) + \
				 (0x00ff & _LSLP_GETBYTE((h), (o) + 1)) ) 
#define _LSLP_SETBYTE(h, i, o) ((h)[(o)] = (uint8)i)
#define _LSLP_SETSHORT(h, i, o) { _LSLP_SETBYTE((h), (0xff & ((i) >> 8)), (o)); \
				     _LSLP_SETBYTE((h), (0xff & (i)), (o) + 1); }
#define _LSLP_GETLONG(h, o) ( (0xffff0000 & _LSLP_GETSHORT((h), (o)) << 16) + \
				  (0x0000ffff & _LSLP_GETSHORT((h), (o) + 2)) )
#define _LSLP_SETLONG(h, i, o) 	{ _LSLP_SETSHORT((h), (0xffff & ((i) >> 16)), (o) ); \
				  _LSLP_SETSHORT((h), (0xffff & (i)), (o) + 2); }


#define _LSLP_SETLONGLONG(h, i, o)		\
{						\
  _LSLP_SETLONG((uint8*)h, (((uint64)(i)) >> 32), (o)) ;	\
  _LSLP_SETLONG((uint8*)h, (((uint64)(i)) & 0xffffffff), (o) + 4);	\
}

#define _LSLP_GETLONGLONG(h, o) \
  (uint64)((((uint64)_LSLP_GETLONG((uint8*)(h), (o))) << 32)  +	\
   ((uint64)_LSLP_GETLONG((uint8*)(h), (o) + 4)))
  
  /* length is a 3-byte value */
#define _LSLP_GETLENGTH(h) ((0xff0000 & (*(uint8 *) &((h)[LSLP_LENGTH]) << 16)) + \
			(0x00ff00 & (*(uint8 *) &((h)[LSLP_LENGTH + 1]) << 8)) + \
			(0x0000ff & (*(uint8 *) &((h)[LSLP_LENGTH + 2]))))
#define _LSLP_SETLENGTH(h, i) {_LSLP_SETSHORT( (h), (((i) & 0xffff00) >> 8), LSLP_LENGTH ); \
			  _LSLP_SETBYTE((h), ((i) & 0x0000ff), LSLP_LENGTH + 2);}
  /* next option offset is a 3-byte value */
#define _LSLP_GETFIRSTEXT(h) ((0xff0000 & (*(uint8 *) &((h)[LSLP_NEXT_EX]) << 16)) + \
			(0x00ff00 & (*(uint8 *) &((h)[LSLP_NEXT_EX + 1]) << 8)) + \
				(0x0000ff & (*(uint8 *) &((h)[LSLP_NEXT_EX + 2]))))
#define _LSLP_SETFIRSTEXT(h, i) {_LSLP_SETSHORT((h), (((i) & 0xffff00) >> 8), LSLP_NEXT_EX ); \
			  _LSLP_SETBYTE((h), ((i) & 0x0000ff), LSLP_NEXT_EX + 2);}

#define _LSLP_GETNEXTEXT(h, o) ((0xff0000 & (*(uint8 *) &((h)[(o)]) << 16)) + \
			(0x00ff00 & (*(uint8 *) &((h)[(o) + 1]) << 8)) + \
				(0x0000ff & (*(uint8 *) &((h)[(o) + 2]))))
#define _LSLP_SETNEXTEXT(h, i, o) {_LSLP_SETSHORT((h), (((i) & 0xffff00) >> 8), (o) ); \
			  _LSLP_SETBYTE((h), ((i) & 0x0000ff), (o) + 2);}

#endif /* ENDIAN definitions */

  /* macros to get and set header fields */
#define _LSLP_GETVERSION(h) _LSLP_GETBYTE((h), LSLP_VERSION)
#define _LSLP_SETVERSION(h, i) _LSLP_SETBYTE((h), (i), LSLP_VERSION)
#define _LSLP_GETFUNCTION(h) _LSLP_GETBYTE((h), LSLP_FUNCTION)
#define _LSLP_SETFUNCTION(h, i) _LSLP_SETBYTE((h), (i), LSLP_FUNCTION)
#define _LSLP_GETFLAGS(h) _LSLP_GETBYTE((h), LSLP_FLAGS)
#define _LSLP_SETFLAGS(h, i) _LSLP_SETBYTE((h), (i), LSLP_FLAGS)
#define _LSLP_GETLANLEN(h) _LSLP_GETSHORT((h), LSLP_LAN_LEN)
#define _LSLP_SETLANLEN(h, i) _LSLP_SETSHORT((h), (i), LSLP_LAN_LEN)
  /* s = int8 *language-string */
#define _LSLP_SETLAN(h, s) {_LSLP_SETSHORT((h),(_LSLP_MIN( strlen((s)), 17)), LSLP_LAN_LEN); \
		    memcpy(&(h)[LSLP_LAN],(s),(_LSLP_MIN( strlen((s)) , 17)));}
#define _LSLP_GETXID(h) _LSLP_GETSHORT((h), LSLP_XID) 
#define _LSLP_SETXID(h, i) _LSLP_SETSHORT((h), (i), LSLP_XID)
#define _LSLP_HDRLEN(h) (14 + _LSLP_GETLANLEN((h))) 
  /* give (work *), how big is the msg header ? */
#define _LSLP_HDRLEN_WORK(work) (14 + (work)->hdr.langLen ) 
  /* given (work *), how big is the data field ? */
#define _LSLP_DATALEN_WORK(work) (((work)->hdr.len) - _LSLP_HDRLEN_WORK((work)))

#define LSLP_FLAGS_OVERFLOW 0x80
#define LSLP_FLAGS_FRESH    0x40
#define LSLP_FLAGS_MCAST    0x20

  /* SLPv2 message types */

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


  /******** internal slp message definitions ********/

  /* URL entry definitions */
#define LSLP_URL_LIFE    1
#define LSLP_URL_LEN	 3
#define _LSLP_GETURLLIFE(h) _LSLP_GETSHORT((h), LSLP_URL_LIFE)
#define _LSLP_SETURLLIFE(h, i) _LSLP_SETSHORT((h), (i), LSLP_URL_LIFE)
#define _LSLP_GETURLLEN(h) _LSLP_GETSHORT((h), LSLP_URL_LEN)
#define _LSLP_SETURLLEN(h, i) _LSLP_SETSHORT((h), (i), LSLP_URL_LEN)


  /* rfc 3528 mesh data structures */ 

  typedef struct 
  {
    uint64 timestamp;
    uint16 url_len;
    uint8* url;
  } accept_id_entry;
  
  typedef struct 
  {
    uint32 next_ext;
    uint32 fwd_id;
    uint64 version_timestamp;
    accept_id_entry accept_entry;
  } mesh_fwd_ext;
  

  typedef struct lslp_auth_block {
    struct lslp_auth_block *next;
    struct lslp_auth_block *prev;
    BOOL isHead;
    uint16 descriptor;
    uint16 len;
    time_t timestamp;
    uint16 spiLen;
    uint8 *spi;
    uint8 *block;
  }lslpAuthBlock;

  typedef struct lslp_url_entry {
    struct lslp_url_entry *next;
    struct lslp_url_entry *prev;
    BOOL isHead;
    time_t lifetime;
    uint16 len;
    uint8 *url;
    uint8 auths;
    lslpAuthBlock *authBlocks;
    lslpAtomizedURL *atomized;
  }lslpURL ;

  typedef struct lslp_ext {
    uint16 id;
    uint32 nextOffset;
    void *data;
  }lslpExt;

  enum attrTypes {
    head = -1,
    string = 1, 
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
    ldap_and = 259,    /* to match token values assigned in y_filter.h */ 
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
    int operator; 
  } filterHead;

  typedef struct ldap_filter_struct {
    struct ldap_filter_struct *next;
    struct ldap_filter_struct *prev;
    BOOL isHead;
    int operator; 
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
	uint8 *scope;
}lslpScopeList;

typedef lslpScopeList lslpSPIList;

  /* ----- service request ------ */
  struct lslp_srv_req {
    uint16 prListLen;
    uint8 *prList;
    uint16 srvcTypeLen;
    uint8 *srvcType;
    lslpScopeList *scopeList;
    uint16 predicateLen;
    uint8 *predicate;
    lslpSPIList *spiList;
    BOOL attr_ext_present;
  };

  /* ----- service reply ------ */
  struct lslp_srv_rply {
    uint16 errCode;
    uint16 urlCount;
    uint16 urlLen;
    int8 *urlList;
    BOOL attr_ext_present;
    int16 attr_buf_len;
    int8* attr_ext_buf;
    int attr_ext_offset;
    
  }; 

  /* ----- service registration ------ */
  struct lslp_srv_reg {
    lslpURL *url; 				/* includes url auth block */
    uint16 srvTypeLen;
    uint8 *srvType;
    lslpScopeList *scopeList;
    lslpAttrList *attrList;    
    lslpAuthBlock *attrAuthList;
    mesh_fwd_ext fwd_ext;
    
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
    uint8 *url; /* not a url-entry - merely a url string */
    uint16 scopeListLen;
    uint8 *scopeList;
    uint16 attrListLen;
    uint8 *attrList;
    uint16 spiListLen;
    uint8 *spiList;  /* comma separated values - see 6.4.1*/
    uint8 auths;
    lslpAuthBlock *authBlocks;
  };

  /* -----sa advertisement ----- */

  struct lslp_sa_advert {
    uint16 urlLen;
    uint8 *url; /* not a url-entry - merely a url string */
    uint16 scopeListLen;
    uint8 *scopeList;
    uint16 attrListLen;
    uint8 *attrList;
    uint8 auths;
    lslpAuthBlock *authBlocks;
  };

  /* ----- srv type request  ----- */	

  struct lslp_srvtype_req {
    uint16 prListLen;
    uint8 *prList;
    uint16 nameAuthLen;
    uint8 *nameAuth;
    lslpScopeList *scopeList;
  };

  /* ----- srv type reply ----- */
  struct lslp_srvtype_rep {
    uint16 errCode;
    uint16 srvTypeListLen;
    uint8 *srvTypeList;  /* see 4.1 for srvtype-string; this is a string-list */
  };

  /* ----- attr request ----- */
  struct lslp_attr_req {
    uint16 prListLen;
    uint8 *prList;
    uint16 urlLen;
    uint8 *url; /* not a url-entry - merely a url string */
    lslpScopeList *scopeList;
    uint16 tagListLen;
    uint8 *tagList;
    lslpSPIList *spiList;
  };

  /* ----- attr reply ----- */
  struct lslp_attr_rep {
    uint16 errCode;
    uint16 attrListLen;
    uint8 *attrList;
    uint8 auths;
    lslpAuthBlock *authBlocks;
  };

  /* ----- srv deregistration ----- */
  struct lslp_srv_dereg {
    uint16 scopeListLen;
    uint8 *scopeList;
    lslpURL *urlList;
    uint16 tagListLen;
    uint8 *tagList;
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
    srvReq,
    srvRply,
    srvReg,
    srvAck,
    daAdvert,
    saAdvert,
    srvTypeReq,
    srvTypeRep,
    attrReq,
    attrRep,
    srvDereg,
  }msgTypes;

  typedef struct lslp_msg {
    int8 type;
    int8 dynamic;
    union lslp_msg_u msg;
  }lslpMsg;

#ifdef	__cplusplus
}
#endif

#ifndef __linux__
#pragma pack( pop, lslp_defs )
#endif

#endif /* _LSLPDEFS_INCLUDE */
