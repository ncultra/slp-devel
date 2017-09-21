
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



/*******************************************************************
 *  Description: portable kernel routines for SLP v2   
 *
 *****************************************************************************/




 #include "lslp-common-defs.h"

 /* DA or SA advertising configuration */

/* listeners, OS transport configuration */
LSLP_CONFIG_ADV lslpConfigAdv = {&lslpConfigAdv, &lslpConfigAdv, TRUE };
LSLP_CONFIG_ADV lslpConfigDisc = {&lslpConfigDisc, &lslpConfigDisc, TRUE  } ;
LSLP_CONFIG_ADV lslpConfigListen = { &lslpConfigListen, &lslpConfigListen, TRUE };
LSLP_CONFIG_ADV lslpConfigDontListen = { &lslpConfigDontListen, &lslpConfigDontListen, TRUE };
LSLP_CONFIG_ADV lslpConfigTransport= { &lslpConfigTransport, &lslpConfigTransport, TRUE };

 /* protocol timing defaults, in seconds */
int16 lslp_config_opport = LSLP_PORT; /* slp operational port */
int32 lslp_config_mc_max = 3; /* max time to wait for a complete multicast query response */
int32 lslp_config_start_wait = 3; /* wait to perform da discovery upon reboot */
int32 lslp_config_retry = 1; /* wait interval before retransmission of multicast rqs */
int32 lslp_config_retry_max = 3; /* give up on unicast request retransmission */
int32 lslp_config_da_beat = 5000; /* da advert heartbeat */
int32 lslp_da_find = 60 * 15;	/* 15 minutes  minimum to wait to repeate active da discovery */
int32 lslp_reg_passive = 1; /* wait to register services on passive da discovery */

int32 lslp_reg_active  = 1;  /* wait to register service on active da discovery */

int32 lslp_config_close_conn =  300 ; /* five minutes */
int32 lslp_config_mtu = 1432; /* 802 media MTU - IP header - UDP header */
int32 lslp_config_ttl = 255;  /* ttl for multicast transmissions */
int16 lslp_config_apiport = 4800; /* stream interface port */
int32 lslp_config_is_da = 1; /* come up as a directory agent */
int32 lslp_config_is_sa = 0; /* come up as a service agent */
int32 lslp_config_max_msg = 8192; /* max msg size we will accept */
int32 lslp_config_max_url = 2048; /* max url we can parse - not set-able */
int32 lslp_converge_retries = 3; /* how many retries to reach convergence */
int32 lslp_converge_wait = 70; /* ms - how long to wait for each round of convergence */
int32 lslp_use_syslog = 0;
int32 lslp_use_stderr = 0;
int32 lslp_log_level =  1;

uint32 lslp_mcast_int = INADDR_ANY;

 /* proper da url is 23 bytes */
int8 *lslp_da_url = "service:directory-agent://";
 /* proper sa url is 21 bytes */
int8 *lslp_sa_url = "service:service-agent://";
int8 *lslp_da_spi = "none";
int8 *lslp_da_attr = "(vendor = unknown), (version = .01), (mesh-enhanced)";

int8 *lslp_host_part = "";
int8 *lslp_log_file ;
int8 *lslp_web_file; /* where should the slp service write its htmls? */
int8 *lslp_fifo ; /* pipe incoming registrations to this file */

/* settings for multi-homed hosts */
int32 lslp_config_force_da_bind = 1; /* force the DA to bind to its own interface */
                                     /* when sending daadverts. */
int8 *lslp_da_addr;

LSLP_SYSLOG_T lslp_syslog_handle = (LSLP_SYSLOG_T)0;
BOOL syslog_is_open = FALSE;
BOOL is_nt_service = FALSE;

int8 *LSLP_EN_US;
int8 *LSLP_LANGUAGE;
int32 LSLP_EN_US_LEN;
int32 LSLP_LANGUAGE_LEN;

int8 *lslp_scope_string;
lslpScopeList *lslp_scope_list;

/* should the da respond to broadcasts or multicasts originating */
/* on one of its own interfaces ? doing so is useful for testing */
BOOL lslp_respond_self_bcast = TRUE ;

uint32 _lslp_config_int(void *parm, int32 *i);
uint32 _lslp_config_daadvert(void *parm);
uint32 _lslp_config_api_sock(void *parm);
uint32 _lslp_config_op_sock(void *parm);
uint32 _lslp_config_default(void *parm);
uint32 _lslp_config_mc_max(void *parm);
uint32 _lslp_config_start_wait(void *parm);
uint32 _lslp_config_retry(void *parm);
uint32 _lslp_config_retry_max(void *parm);
uint32 _lslp_config_da_beat(void *parm);
uint32 _lslp_da_find(void *parm);
uint32 _lslp_reg_passive(void *parm);
uint32 _lslp_reg_active(void *parm);
uint32 _lslp_config_close_conn(void *parm);
uint32 _lslp_config_mtu(void *parm);
uint32 _lslp_config_ttl(void *parm);
uint32 _lslp_config_is_da(void *parm);
uint32 _lslp_config_is_sa(void *parm);
uint32 _lslp_config_max_msg(void *parm);
uint32 _lslp_converge_retries(void *parm);
uint32 _lslp_converge_wait(void *parm);
uint32 _lslp_log_file(void *parm);
uint32 _lslp_web_file(void *parm);
uint32 _lslp_use_syslog(void *parm);
uint32 _lslp_use_stderr(void *parm);
uint32 _lslp_log_level(void *);
uint32 _lslp_da_addr(void *parm) ;
uint32 _lslp_listeners(void *parm);
uint32 _lslp_multicast(void *parm);
uint32 _lslp_fifo(void *parm) ;
uint32 _lslp_config_language(void *parm);
uint32 _lslp_config_scopes(void *parm);
uint32 _lslp_config_bcast_respond(void *parm);

 LSLP_CONFIG_PARSE lslp_config_parse[] = {
   {"da_advert", _lslp_config_daadvert},
   {"da_disc", _lslp_config_daadvert},
   {"api_port", _lslp_config_api_sock},
   {"operational_port", _lslp_config_op_sock},
   {"mc_max", _lslp_config_mc_max},
   {"start_wait", _lslp_config_start_wait},
   {"retry_interval", _lslp_config_retry},
   {"retry_max", _lslp_config_retry_max},
   {"da_beat", _lslp_config_da_beat},
   {"da_find", _lslp_da_find},
   {"reg_passive", _lslp_reg_passive},
   {"reg_active", _lslp_reg_active},
   {"close_conn", _lslp_config_close_conn},
   {"slp_mtu", _lslp_config_mtu},
   {"slp_ttl", _lslp_config_ttl},
   {"slp_da", _lslp_config_is_da},
   {"slp_sa", _lslp_config_is_sa},
   {"max_msg", _lslp_config_max_msg},
   {"converge_retries", _lslp_converge_retries},
   {"converge_wait", _lslp_converge_wait},
   {"log_file", _lslp_log_file },
   {"use_syslog", _lslp_use_syslog },
   {"log_stderr", _lslp_use_stderr },
   {"log_level", _lslp_log_level },
   {"www_file", _lslp_web_file},
   {"da_addr", _lslp_da_addr},
   {"listen", _lslp_listeners},
   {"listen_exclude", _lslp_listeners },
   {"multicast", _lslp_multicast },
   {"fifo", _lslp_fifo },
   {"language", _lslp_config_language},
   {"scopes", _lslp_config_scopes},
   {"own_bcast", _lslp_config_bcast_respond},
   {NULL, NULL}
 };


 LSLP_MSGS lslp_Msgs[] = {
 /*  000 */ { " ", 0, 0},	
 /*  001 */ { "%.2hx%.2hx%.2hx%.2hx %.2hx%.2hx%.2hx%.2hx %.2hx%.2hx%.2hx%.2hx %.2hx%.2hx%.2hx%.2hx %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c \n", 32, 0xffffffff},
 /*  002 */ { "typ %i sta %#08lx idx %i apis %i tim %i,%i \nver %i mid %i len %i flag %#08lx nxtext %i xid %i\n", 12, 0x00000fff},
 /*  003 */ { "udpsock %i tcpsock %i myport %i myaddr %s rmtport %i rmtaddr %s udplen %i tcplen %i \n", 8, 0x000000eb},
 /*  004 */ { "SLP Kernel Started.\n", 0, 0},
 /*  005 */ { "Error StartingSLP Kernel.\n", 0, 0},
 /*  006 */ { "StoppingSLP Kernel.\n", 0, 0},
 /*  007 */ { "Kernel found %i local interfaces....\n", 1, 0x00000001},
 /*  008 */ { "Timeout waiting to read connected socket. %i\n", 1, 0x00000001},
 /*  009 */ { "Read %ld bytes from %s socket %i.\n", 3, 0x00000005},
 /*  010 */ { "Error (system) reading connected socket %i.\n", 1, 0x00000001},
 /*  011 */ { "Wrote %ld bytes to %s socket %i.\n", 3, 0x00000005},
 /*  012 */ { "Error (system) writing tcp socket %i.\n", 1, 0x00000001},
 /*  013 */ { "Error preparing udp socket for sending multicast & broadcast.\n",0, 0},
 /*  014 */ { "Error getting udp socket for sending datagram.\n", 0, 0},
 /*  015 */ { "Error binding datagram socket for sending.\n", 0, 0},
 /*  016 */ { "Dispatching a valid SLPv2 message received over the net.\n ", 0, 0},
 /*  017 */ { "Error waiting on session array semaphore.\n", 0, 0},
 /*  018 */ { "Session %i returned (session array contains empty entries).\n", 1, 0x00000001},
 /*  019 */ { "Orphaned session %i reset after %i seconds.\n", 2, 0x00000003},
 /*  020 */ { "Session %i timer reused for new message exchange.\n", 1, 0x00000001},
 /*  021 */ { "Session %i reused (least recently used session).\n", 1, 0x00000001},
 /*  022 */ { "Session returned to available pool.\n", 0, 0},
 /*  023 */ { "Error waiting on work list %i semaphore.\n", 1, 0x00000001},
 /*  024 */ { "Work node inserted into list %i.\n", 1, 0x00000001},
 /*  025 */ { "Work node unlinked from list %i.\n", 1, 0x00000001},
 /*  026 */ { "First work node unlinked from list %i.\n", 1, 0x00000001},
 /*  027 */ { "Error (system, ioctlsocket) on socket %i.\n", 1, 0x00000001},
 /*  028 */ { "Error (system, select) on non-blocking stream socket %i.\n", 1, 0x00000001},
 /*  029 */ { "Connection made on tcp socket %i.\n", 1, 0x00000001},
 /*  030 */ { "Error (api, kernel), sending Internal Error response on socket %i.\n", 1, 0x00000001},
 /*  031 */ { "Error (kernel, network), sending Internal Error response.\n", 0, 0},
 /*  032 */ { "Error, received SLPv2 message with wrong version number (%i).\n", 1, 0x00000001},
 /*  033 */ { "Error, receive SLPv2 message with invalid message ID (%i).\n", 1, 0x00000001},
 /*  034 */ { "Error (network, accept) on tcp socket %i.\n",  1, 0x00000001},
 /*  035 */ { "Error - unable to allocate memory.\n", 0, 0},
 /*  036 */ { "Connection accepted on tcp socket %i.\n", 1, 0x00000001},
 /*  037 */ { "Read less than the minimum SLPv2 header size on %s socket %i.\n", 2, 0x00000001},
 /*  038 */ { "Read less than the expected amount (%i bytes, expected %i) on %s socket %i.\n", 4, 0x0000000d},
 /*  039 */ { "Unable to allocate session for udp socket - placing work on waiting queue.\n", 0, 0},
 /*  040 */ { "Error - losing work node due to multiple errors.\n", 0, 0},
 /*  041 */ { "%s Thread initializing.\n", 1, 0},
 /*  042 */ { "%s Thread initialized and running.\n", 1, 0},
 /*  043 */ { "Dispatching incoming SLPv2 message id %i.\n", 1, 0x00000001},
 /*  044 */ { "Connection on tcp socket %i closed by other party.\n", 1, 0x00000001},
 /*  045 */ { "Re-reading tcp socket %i.\n", 1, 0x00000001},
 /*  046 */ { "Shutting down tcp socket %i due to timeout.\n", 1, 0x00000001},
 /*  047 */ { "Reprocessing worknode to finish multi-message read operation.\n", 0, 0},
 /*  048 */ { "Closing lingering reply after timeout interval expired.\n", 0, 0},
 /*  049 */ { "Error - %s request timed out after %i seconds.\n", 2, 0x00000001},
 /*  050 */ { "Retransmitting udp request after waiting %i seconds.\n",  1, 0x00000001},
 /*  051 */ { "%s Thread terminating.\n", 1, 0},
 /*  052 */ { "Error (system, select) on array of blocking sockets.\n", 0, 0},
 /*  053 */ { "Error reading target information from %s API socket %i.\n", 2, 0x00000001},
 /*  054 */ { "Target of incoming API message is local host.\n", 0, 0},
 /*  055 */ { "Outgoing multicast or broadcast datagram truncated - was %i bytes, is %i bytes.\n", 2, 0x00000003},
 /*  056 */ { "Recieved damaged or malicious SLP message; msg len was %i bytes, but hdr said msg len was %i bytes.\n", 2, 0x00000003},
 /*  057 */ { "Recieved TCP message that is too big; msg len was %i bytes, TCP msg limit is %i bytes.\n", 2, 0x00000003}, 
 /*  058 */ { "Received a malicious or corrupt message from the network; header length field is %i bytes.\n", 1, 0x00000001},
 /*  059 */ { "Invalid state transition in mesh control; state is %i; next state should be %i.\n", 2, 0x00000003},
 /*  060 */ { "Invalid state transition in mesh control; attempt to transition to or from an invalid state.\n", 0, 0},
 /*  061 */ { "Mesh Ctrl: new node. rmtAddr %s; state %i; next state %i; %s\n", 4, 0x00000006},
 /*  062 */ { "Mesh Ctrl: state transition;  rmtAddr %s; state %i; next state %i; %s \n", 4, 0x00000006},
 /*  063 */ { "Mesh Ctrl: state transition completed;  rmtAddr %s; state %i; next state %i; %s \n", 4, 0x00000006},
 /*  064 */ { "Mesh Ctrl: transmission error;  rmtAddr %s; state %i; next state %i; %s \n", 4, 0x00000006},
 /*  065 */ { "Mesh Ctrl: peer reboot - restarting state machine;  rmtAddr %s; state %i; next state %i; %s \n", 4, 0x00000006},
 /*  066 */ { "Mesh Ctrl: connection refused - other peer not listening;  rmtAddr %s; state %i; next state %i; %s \n", 4, 0x00000006},
 /*  067 */ { "Mesh Ctrl: peer timed out;  rmtAddr %s; state %i; next state %i; %s \n", 4, 0x00000006},
 /*  068 */ { "NT Service Ctrl: StartServiceControlDispatcher error %d.\n", 1, 0x00000001},
 /*  069 */ { "NT Service Ctrl: RegisterServiceCtrlHandler error  %d.\n", 1, 0x00000001},
 /*  070 */ { "NT Service Ctrl: SLP Initialization Routine error  %d.\n", 1, 0x00000001},
 /*  071 */ { "NT Service Ctrl: SetServiceStatus error %d.\n", 1, 0x00000001},
 /*  072 */ { "SLP Not running on a Windows NT Computer. %s.\n", 1, 0x00000000},
 /*  073 */ { "SLP created as a service on this computer.\n", 0, 0x00000000},
 /*  074 */ { "Error - %s: %d.\n", 2, 0x00000001},
 /*  075 */ { "SLP Service deleted from this computer.\n", 0, 0x00000000},
 /*  076 */ { "Error - api client reset socket before I could read it! .\n", 0, 0},
 /*  077 */ { "Error opening %s socket for listening. \n", 1, 0x00000000}, 
 /*  078 */ { "Error opening slp configuration file - exiting. \n", 0, 0x00000000}, 
           {" ", 0, 0}  
};


/* format is da_advert, type, interval, address, iface, mask */
/* where type is < unicast | broadcast | multicast > */
/* interval is the number of seconds between advertisements */
/* address is the address to transmit the broadcast to */
/* iface is the local ip address to which the advertisement is bound */
/* mask is the network mask to construct broadcast addresses (optional) */

void _lslpDestroyConfigAdvertList(LSLP_CONFIG_ADV *l, int32 flag) 
{
  LSLP_CONFIG_ADV *temp;
  assert(_LSLP_IS_HEAD(l));
  temp = l->next;
  while(! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    free(temp);
    temp = l->next;
  }
  if(flag)
    free(l);
}

uint32 _lslp_config_daadvert(void *parm)
{
  BOOL advert;
  LSLP_CONFIG_ADV *new_config;
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
  if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(! strcasecmp(bptr, "da_advert"))
      advert = TRUE;
    else
      advert = FALSE;
    if(NULL != (new_config = (LSLP_CONFIG_ADV *)calloc(1, sizeof(LSLP_CONFIG_ADV)))) {
      new_config->iface = INADDR_ANY; 
      new_config->mask = inet_addr("255.255.255.255");
      if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
	if(! strcasecmp(bptr, "broadcast")) {
	  new_config->flag = LSLP_STATUS_BCAST;
	} else if (! strcasecmp(bptr, "multicast")) {
	  new_config->flag = LSLP_STATUS_MCAST;
	}
	if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
	  new_config->frequency = (time_t) strtoul(bptr, NULL, 0);
	  if(advert == FALSE) {
	    /* frequency MUST be greater than mc_max * retry */
	    if(new_config->frequency <= (lslp_config_mc_max * lslp_config_retry)) 
	      new_config->frequency = (lslp_config_mc_max * lslp_config_retry) + lslp_config_retry;
	  }
	  if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
	    new_config->addr = inet_addr(bptr);
	    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
	      new_config->iface = inet_addr(bptr); 
	      if(new_config->flag == LSLP_STATUS_BCAST) {
		if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
		  new_config->mask  = inet_addr(bptr);
		  new_config->addr &= new_config->mask;
		  new_config->addr |= ~(new_config->mask);
		} /* mask */
	      } /* if bcast */
	    } /* iface */ 	      
	    if(advert == TRUE) {
	      _LSLP_INSERT(new_config, &lslpConfigAdv);
	    } else {
	      _LSLP_INSERT(new_config, &lslpConfigDisc);
	    }
	    return(LSLP_OK);
	  } /* address */
	} /* frequency  */
      } /* transmission type */
      free(new_config);
    } /* allocated our structure */
  } /* first token */
  return(LSLP_PARSE_ERROR);
}


/* format is log_file, <path> \n */
/* where <path> is the directory and name of the log */
uint32 _lslp_log_file(void *parm)
{
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
  if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      lslp_log_file  =  strdup(bptr);
      return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);
}

/* format is da_addr, <addr> */
uint32 _lslp_da_addr(void *parm) 
{
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
   if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      lslp_da_addr  =  strdup(bptr);
      return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);
}

/* format is multicast, <addr> */

uint32 _lslp_multicast(void *parm)
{
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
   if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if (NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
	lslp_mcast_int = inet_addr(bptr);
	return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);
}


/* format is listeners, <addr>[, <addr> ] */
uint32 _lslp_listeners(void *parm)
{
  LSLP_CONFIG_ADV *listener;
  int8 *saveptr = NULL, *bptr;
  BOOL exclude = FALSE;
  const int8 delimiters[] =" ," ;
   if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
     if(strstr(bptr, "exclude") || strstr(bptr, "EXCLUDE"))
	 exclude = TRUE;
    while (NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      if(NULL != (listener = (LSLP_CONFIG_ADV *)calloc(1, sizeof(LSLP_CONFIG_ADV)))) {
	listener->iface = inet_addr(bptr);
	if(exclude == TRUE) {
	  _LSLP_INSERT(listener, &lslpConfigDontListen);
	} else {
	  _LSLP_INSERT(listener, &lslpConfigListen);
	}
      }
    }
    return(LSLP_OK);
  }
  return(LSLP_PARSE_ERROR);
}


/* format is www_file, <path> \n */
uint32 _lslp_web_file(void *parm) 
{
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
   if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      lslp_log_file  =  strdup(bptr);
      return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);
}

/* format is fifo, <path> \n */

uint32 _lslp_fifo(void *parm) 
{
 
  int8 *saveptr = NULL, *bptr;

  const int8 delimiters[] =" ," ;
  printf("parsing the fifo path name\n\n\n");
   if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      lslp_fifo  =  strdup(bptr);
      return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);
}

uint32 _lslp_config_short(void *parm, int16 *i) ;
uint32 _lslp_config_api_sock(void *parm)
{
 return(_lslp_config_short(parm, &lslp_config_apiport));
}

uint32 _lslp_config_op_sock(void *parm)
{
 return(_lslp_config_short(parm, &lslp_config_opport));
}

uint32 _lslp_config_int(void *parm, int32 *i)
{
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
  
  assert(parm != NULL && i != NULL);
  if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      *i =  strtol(bptr, NULL, 0);
      return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);

}
int8 *lslp_foldString(int8 *s);
uint32 _lslp_config_boolean(void *parm, BOOL *i) 
{
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
 
  assert(parm != NULL && i != NULL);
  if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      lslp_foldString(bptr);
      *i = ( strcasecmp(bptr, "true") ? FALSE : TRUE );
      return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);
}

uint32 _lslp_config_short(void *parm, int16 *i)
{
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
 
  assert(parm != NULL && i != NULL);
  if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      
      *i =  (int16)((0x0000ffff) & strtol(bptr, NULL, 0));
      return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);
}


uint32 _lslp_config_mc_max(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_mc_max));
}
uint32 _lslp_config_start_wait(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_start_wait));
}

uint32 _lslp_config_retry(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_retry));
}
uint32 _lslp_config_retry_max(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_retry_max));
}
uint32 _lslp_config_da_beat(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_da_beat));
}
uint32 _lslp_da_find(void *parm)
{
  return(_lslp_config_int(parm, &lslp_da_find));
}
uint32 _lslp_reg_passive(void *parm)
{
  return(_lslp_config_int(parm, &lslp_reg_passive));
}
uint32 _lslp_reg_active(void *parm)
{
  return(_lslp_config_int(parm, &lslp_reg_active));
}
uint32 _lslp_config_close_conn(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_close_conn));
}
uint32 _lslp_config_mtu(void *parm)
{
  return(1432);
}
uint32 _lslp_config_ttl(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_ttl));
}
uint32 _lslp_config_is_da(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_is_da));
}
uint32 _lslp_config_is_sa(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_is_sa));
}
uint32 _lslp_config_max_msg(void *parm)
{
  return(_lslp_config_int(parm, &lslp_config_max_msg));
}
uint32 _lslp_converge_retries(void *parm)
{
  return(_lslp_config_int(parm, &lslp_converge_retries));
}
uint32 _lslp_converge_wait(void *parm)
{
  return(_lslp_config_int(parm, &lslp_converge_wait));
}

uint32 _lslp_config_default(void *parm)
{
  return(LSLP_OK);
}

uint32 _lslp_use_syslog(void *parm)
{
  return(_lslp_config_int(parm, &lslp_use_syslog));
}

uint32 _lslp_use_stderr(void *parm)
{
  return(_lslp_config_int(parm, &lslp_use_stderr)) ;
}

uint32 _lslp_log_level(void *parm)
{
  return(_lslp_config_int(parm, &lslp_log_level)) ;
}

/* format is language, <language-string> \n */


uint32 _lslp_config_language(void *parm)
{
  int8 *saveptr = NULL, *bptr;
  const int8 delimiters[] =" ," ;
  if(NULL != (bptr = _LSLP_STRTOK((int8 *)parm, delimiters, &saveptr))) {
    if(NULL != (bptr = _LSLP_STRTOK(NULL, delimiters, &saveptr))) {
      LSLP_EN_US = (LSLP_LANGUAGE  = strdup(bptr));
      LSLP_EN_US = (LSLP_LANGUAGE = lslp_foldString(LSLP_LANGUAGE));
      LSLP_EN_US_LEN = (LSLP_LANGUAGE_LEN = strlen(LSLP_LANGUAGE));
      return(LSLP_OK);
    }
  }
  return(LSLP_PARSE_ERROR);
}

uint32 _lslp_config_scopes(void *parm)
{
  int8 *bptr = (int8 *)parm;
  if(bptr){
    while(*bptr != 0x00 && *bptr != ',')
      bptr++;
    if(*bptr == 0x00){
      lslp_scope_string = strdup("DEFAULT");
    } else {
      lslp_scope_string = strdup(bptr + 1);
    }
    if(lslp_scope_string)
      return(LSLP_OK);
  }
  return(LSLP_PARSE_ERROR);
}

uint32 _lslp_config_bcast_respond(void *parm)
{
  printf("\n\nrespond to own broadcasts configured\n");
  
  return _lslp_config_boolean(parm, &lslp_respond_self_bcast) ;
}

uint32 _lslpParseConfigFile(int8 *f)
{
  static int8 buf[256];
  static int8 buf2[256];
  int8 *bptr, *token;
  LSLP_CONFIG_PARSE *temp;
  FILE *cf;
  int8 *saveptr = NULL;
  const int8 delimiters[] =" ," ;
 
  if(! strlen(f)) {
    if(NULL == (cf = fopen("slp.conf", "r")))
      return(-1);
  } else if(NULL == (cf = fopen(f, "r"))) {
    return(-1);
  }
  memset(&buf[0], 0x00, 256);
  while(NULL != (bptr = fgets(buf, 255, cf))) {
    memcpy(buf2, bptr, 256);
    if(NULL != (token  = _LSLP_STRTOK(bptr, delimiters, &saveptr))) {
      temp = &lslp_config_parse[0];
      while(temp->token != NULL) {
	if(! strcasecmp(temp->token, token)) {
	  if( LSLP_PARSE_ERROR == temp->p((void *)buf2))
	    return(-1);
	  break;
	}
	temp++;
      } /* while we are searching for keyword tokens */
    } /* if we read the first token of the line */
  } /* while we are reading lines */
  fclose(cf);
  return(LSLP_OK);
}



