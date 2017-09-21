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


#ifndef _LSLP_CONFIG_INCLUDE_
#define _LSLP_CONFIG_INCLUDE_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lslp_config_parse {
  int8 *token;
  uint32 (*p)(void *);
} LSLP_CONFIG_PARSE;


typedef struct lslp_config_adv {
  struct lslp_config_adv *next;
  struct lslp_config_adv *prev;
  BOOL isHead;
  time_t frequency;  /* in seconds */
  time_t last;
  uint32 flag;       /* unicast, multicast, or broadcast */
  uint32 addr;       /* send/rcv address */ 
  uint32 iface;      /* bind to this interface (for multi-homed hosts) */
  uint32 mask;
} LSLP_CONFIG_ADV;

/* make certain interface configuration flags get defined for all platforms */

/* Standard interface flags - from if.h. */
#ifndef IFF_UP
#define	IFF_UP		0x1		/* interface is up		*/
#endif
#ifndef IFF_BROADCAST
#define	IFF_BROADCAST	0x2		/* broadcast address valid	*/
#endif
#ifndef IFF_LOOPBACK
#define	IFF_LOOPBACK	0x8		/* is a loopback net		*/
#endif
#ifndef IFF_MULTICAST
#define IFF_MULTICAST	0x1000		/* Supports multicast		*/
#endif
#ifndef IFF_DYNAMIC
#define IFF_DYNAMIC	0x8000		/* dialup device with changing addresses*/
#endif

typedef struct lslp_msgs{
  int8 *msg;
  uint32 inserts;
  uint32 mask;
} LSLP_MSGS;

extern int16 lslp_config_opport;
extern int32 lslp_config_mc_max;
extern int32 lslp_config_start_wait;
extern int32 lslp_config_retry;
extern int32 lslp_config_retry_max;
extern int32 lslp_config_da_beat;
extern int32 lslp_da_find;

extern int32 lslp_reg_passive;
extern int32 lslp_reg_active;

extern int32 lslp_config_close_conn;
extern int32 lslp_config_mtu;
extern int32 lslp_config_ttl;
extern int16 lslp_config_apiport;
extern int32 lslp_config_is_da;
extern int32 lslp_config_is_sa;
extern int32 lslp_config_max_msg;
extern int32 lslp_config_max_url;
extern LSLP_MSGS lslp_Msgs[];
extern int32 lslp_converge_retries;
extern int32 lslp_converge_wait; /* milliseconds */
extern int32 lslp_use_syslog ;
extern int32 lslp_use_stderr ;
extern int32 lslp_log_level ;

extern uint32 lslp_mcast_int;

/* settings for multi-homed hosts */
extern int32 lslp_config_force_da_bind ; /* force the DA to bind to its own interface */
                                         /* when sending daadverts. */
extern int8 *lslp_da_addr;

extern LSLP_SYSLOG_T lslp_syslog_handle;
extern BOOL syslog_is_open ;

extern int8 *LSLP_EN_US;
extern int8 *LSLP_LANGUAGE;
extern int32 LSLP_EN_US_LEN;
extern int32 LSLP_LANGUAGE_LEN;
extern int8 *lslp_scope_string;

/* proper da url is 23 bytes */
extern int8 *lslp_da_url;
/* proper sa url is 21 bytes */
extern int8 *lslp_sa_url;
extern int8 *lslp_da_spi;
extern int8 *lslp_da_attr;
extern int8 *lslp_host_part;
extern int8 *lslp_log_file;
extern int8 *lslp_fifo ; /* pipe incoming registrations to this file */
void _lslpDestroyConfigAdvertList(LSLP_CONFIG_ADV *l, int32 flag) ;
uint32 _lslpParseConfigFile(int8 *f) ;

  extern BOOL lslp_respond_self_bcast;
  

#ifdef __cplusplus
}
#endif

#endif /*_LSLP_CONFIG_INCLUDE_ */

