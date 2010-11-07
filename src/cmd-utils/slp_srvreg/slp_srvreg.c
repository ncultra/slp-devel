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



/*****************************************************************************
 *  Description: scripting shell for slp_client - generates an slp REGISTRATION
 *
 *****************************************************************************/


#define SLP_LIB_IMPORT 1
#include "../slp_client/slp_client.h"
#include <signal.h>
int8 *type = NULL, *url = NULL, *attrs = NULL, *addr = NULL, *scopes = NULL, *_interface = NULL, *spi = NULL;
uint16 life = 0x0fff, port=427;
BOOL should_listen = FALSE, dir_agent = FALSE, test = FALSE;
BOOL tcp = FALSE;
// Commenting the following since the LSLP_MTU being declared here and used as
// extern in slp_client.h does not work on Windows. So making this variable
// local to slp_client library and accessing it via method setLslpMtu.
//int32 LSLP_MTU=4096;
int ip_version = 0;
int broadcast = 0;
int no_ipv6_tcp = 0;
int da_active_discovery = 300;
int convergence = 3;


union slp_sockaddr address;
struct slp_client *client;

/* Mon May 16 17:24:30 2005 */ 
/* this is  a real hack. We need this program to be true daemon, 
 * it would be much better to have a general-purpose daemonize interface. 
 * this one is only for linux, although it should work in any unix. 
 */
#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int 
daemon_init(void)
{

/* W. Richard Stevens is the __MAN__ */
	SLP_TRACE("preparing to fork\n");
	pid_t pid;
	if((pid = fork()) < 0)
		return(-1);
	else if ( pid != 0 )
		exit (0);
	SLP_TRACE("forked, I am %d\n", pid);
	setsid();
	chdir("/");
	umask(0);
	return(0);
}

#endif 

void free_globals(void)
{
  if(type != NULL)
    free(type);
  if(url != NULL)
    free(url);
  if(attrs != NULL)
    free(attrs);
  if(addr != NULL)
    free(addr);
  if(scopes != NULL)
    free(scopes);
  if(_interface != NULL)
    free(_interface);
}


void termination_handler(int sig)
{
  printf("received signal -- terminating\n");
  
  free_globals();
  destroy_slp_client(client);
  
  exit(0);
  
}


void usage(void)
{
  printf("\nslp_srvreg -- transmit an SLP Service Registration and print the results.\n");
  printf("-------------------------------------------------------------------------\n");
  printf("slp_srvreg  --type=service-type-string\n");
  printf("            --url=url-string\n");
  printf("            --attributes=attribute-string\n");
  printf("           [--address=target-IP]\n");
  printf("           [--port=target-port]\n");
  printf("           [--scopes=scope-string]\n");
  printf("           [--lifetime=seconds]\n"); 
  printf("           [--interface=host-IP]\n");
  printf("           [--tcp]\n");
  printf("           [--daemon=true]\n");
  printf("           [--use_da=true]\n");
  printf("           [--mtu=buffer size]\n");
  printf("           [--test]\n");
  printf("           [--spi=security-parameters-index] (not used)\n");
  printf("           [--ip=version]\n");
  printf("           [--broadcast]\n");
  printf("           [--no-ipv6-tcp]\n");
  printf("           [--active-da-disc=seconds]\n");
  printf("           [--convergence=iterations]\n");
  printf("           [--version]\n");
  printf("\n");
  printf("All parameters must be a single string containing no spaces.\n");
  printf("Always use the format of <parameter>=<value>.\n");
  printf("Parameters enclosed in brackets are optional.\n"); // JSS - corrected to remove "not"
  
}


BOOL get_options(int argc, char *argv[])
{
  int i;
  int8 *bptr;
  
  for(i = 1; i < argc; i++){
    if((*argv[i] == '-') && (*(argv[i] + 1) == '-')) {
      if(TRUE == lslp_pattern_match(argv[i] + 2, "type=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	/* handle case where type is specified twice on the cmd line */
	if(type != NULL)
	  free(type);
	type = strdup(bptr);
	SLP_TRACE("get_options: type: %s\n", type); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "url=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(url != NULL)
	  free(url);
	url = strdup(bptr);
	SLP_TRACE("get_options: url: %s\n", url); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "attributes=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(attrs != NULL)
	  free(attrs);
	attrs = strdup(bptr);
	SLP_TRACE("get_options: attributes: %s\n", attrs); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "address=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(addr != NULL)
	  free(addr);
	addr = ip6_normalize_addr(bptr, ip_version);
	if (!addr)
	  addr = strdup(bptr);
	SLP_TRACE("get_options: address: %s\n", addr); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "scopes=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(scopes != NULL)
	  free(scopes);
	scopes = strdup(bptr);
	SLP_TRACE("get_options: scopes: %s\n", scopes); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "lifetime*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	life = strtoul(bptr, NULL, 10);
	SLP_TRACE("get_options: lifetime: %s\n", bptr); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "port=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	port = strtoul(bptr, NULL, 10);
	SLP_TRACE("get_options: port: %s\n", bptr); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "interface=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(_interface != NULL)
	  free(_interface);
	_interface = strdup(bptr);
	SLP_TRACE("get_options: interface: %s\n", _interface); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "daemon=true*", FALSE)) {
	should_listen = TRUE;
	SLP_TRACE("get_options: daemon: true\n"); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "mtu=*", FALSE)) {
	      bptr = argv[i] + 2;
	      while(*bptr != '=') bptr++;
	      bptr++;
	      setLslpMtu(strtol(bptr, NULL, 10) & 0x0000ffff);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "test*", FALSE)) {
	test = TRUE;
	SLP_TRACE("get_options: test: true\n"); // JSS - debug
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "use_da=true*", FALSE)) {
	dir_agent = TRUE;
	SLP_TRACE("get_options: use_da: true\n"); // JSS - debug
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "tcp*", FALSE)) {
	tcp = TRUE;
	SLP_TRACE("get_options: tcp: true\n");
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "ip=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	ip_version = strtoul(bptr, NULL, 10);
	SLP_TRACE("get_options: ip version: %d\n", ip_version);
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "broadcast*", FALSE)) {
	broadcast = TRUE;
	SLP_TRACE("get_options: broadcast turned ON\n");
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "no-ipv6-tcp*", FALSE)) {
	no_ipv6_tcp = TRUE;
	SLP_TRACE("get_options: Do NOT use tcp over ipv6\n");
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "active-da-disc=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	da_active_discovery = strtoul(bptr, NULL, 10);
	SLP_TRACE("get_options: active da discovery: %d\n", da_active_discovery);
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "convergence=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	convergence = strtoul(bptr, NULL, 10);
	SLP_TRACE("get_options: convergence %d\n", convergence);
      }else if (TRUE == lslp_pattern_match(argv[i] + 2, "version*", FALSE)) {
	printf("slp_srvreg version %s changeset %s.\nUsing client library version %s changeset %s\n", 
	       LSLP_TAG, LSLP_CHANGESET, lslp_get_version(), lslp_get_changeset());
	exit(1);
	
      }
    }
  }
  
  if( type == NULL || url == NULL || attrs == NULL )
    return FALSE;
  return TRUE;
}



int main(int argc, char **argv)
{

  int count = 0;
  time_t now, last;
  lslpMsg msg_list;
  struct slp_net *con;
  char addr6[INET6_ADDRSTRLEN];
  lslpMsg responses, *temp;
  unsigned long client_flags;
  
  
  _LSLP_INIT_HEAD(&msg_list);
  _LSLP_INIT_HEAD(&responses);
  setLslpMtu(4096); //default
  
  if(FALSE == get_options(argc, argv)) {
    usage();
  } else {
    if(scopes == NULL)
      scopes = strdup("DEFAULT");
    if(test == TRUE) {
      return test_srv_reg(type, url, attrs, scopes);
    }

    signal (SIGINT, termination_handler);
    SLP_TRACE("creating client");


    if (no_ipv6_tcp == TRUE)
      client_flags = CLIENT_FLAGS_NO_TCP_IPV6;

    if(NULL != (client = create_slp_client(ip_version, 
					   addr, 
					   _interface, 
					   port, 
					   "DSA", 
					   scopes, 
					   should_listen,
					   dir_agent,
					   client_flags))) {
      now = (last = time(NULL));
      client->broadcast = broadcast;
      client->da_active_discovery = da_active_discovery;
      client->_convergence = convergence;
      if (client->ip_version == 6 || client->ip_version == 0) {
	if (broadcast) {
	  SLP_TRACE("Broadcast not allowed by IPv6, disabling.\n");
	  broadcast = 0;
	}
	address.saddr.sa_family = AF_INET6;
	address.sin6.sin6_port = htons(port);
	if (!addr)
	  addr = ip6_normalize_addr("::", client->ip_version);
	inet_pton(AF_INET6, addr,  &address.sin6.sin6_addr);
	inet_ntop(AF_INET6, &address.sin6.sin6_addr, addr6, INET6_ADDRSTRLEN);
      }
      else if (client->ip_version == 4 || client->ip_version == 0) {
	if (broadcast && !tcp)
	  SLP_TRACE("Broadcasting over IPv4 interfaces.\n");
	address.sin.sin_port = htons(port);
	address.saddr.sa_family = AF_INET;
	if(!addr)
	  addr = ip6_normalize_addr("*", client->ip_version);
	address.sin.sin_addr.s_addr = inet_addr(addr);
	inet_ntop(AF_INET, &address.sin.sin_addr, addr6, INET6_ADDRSTRLEN);
      }
      
      if(tcp == TRUE) {
	SLP_TRACE("using TCP\n");
	if ( NULL == (con = tcp_create_connection(client, &address))) {
	  SLP_TRACE("error creating tcp connection\n");
	  return 1;
	}
	count = client->tcp_srv_reg(client, con, url, attrs, type, scopes, life);
	lslpDestroyConnection(con);
	
      } else { /* not tcp */
	if(ip6_is_loopback(addr)) {
	  SLP_TRACE("calling srv_reg_local\n");
	  client->srv_reg_local(client, url, attrs, type, scopes, life);
	}
	
	else if (addr != NULL) {
	  SLP_TRACE("calling srv_reg\n");
	  client->srv_reg(client, url, attrs, type, scopes, life);
	}
      }
      
      client->get_response(client, &responses);
      while (! _LSLP_IS_EMPTY(&responses)) {
	temp = responses.next;
	if(temp->type == srvAck) {
	  count++;
	}
	_LSLP_UNLINK(temp);
	lslpDestroySLPMsg(temp, LSLP_DESTRUCTOR_DYNAMIC);
      }
      

#if defined(__linux__) && ! defined(DEBUG_BUILD)
      if(should_listen == TRUE)
	daemon_init();
#endif 
// lslp-common-defs.h picks up our portability include header,	   JSS
// which is why we don't include it directly to access function	   JSS
// prototypes like "qslp_...".  Actual function defined in .so     JSS
// object.	   						   JSS
#ifdef _AIX							// JSS
      if (should_listen == TRUE) {				// JSS
        printf("slp_srvreg: create PID file\n");		// JSS
	qslp_CreatePidFile(pszSlpSaPidFile);			// JSS
        printf("slp_srvreg: looping on service_listener()\n");	// JSS
      }								// JSS
#endif								// JSS
      while(should_listen == TRUE) {
	_LSLP_SLEEP(10);
	client->service_listener(client, 0, &msg_list);
	client->tcp_service_listeners(client);

	now = time(NULL);
	if((now - last) > (life - 1)) {
	  if(tcp == TRUE) {
	    if ( NULL == (con = tcp_create_connection(client, &address))) {
	      SLP_TRACE("error creating tcp connection\n");
	    } else {
	      SLP_TRACE("slp_srvreg: call tcp_srv_reg()\n");		// JSS
	      client->tcp_srv_reg(client, con, url, attrs, type, scopes, life);
	      lslpDestroyConnection(con);
	    }
	  } else {
	    SLP_TRACE("slp_srvreg: call srv_reg_local()\n");		// JSS
	    client->srv_reg_local(client, url, attrs, type, scopes, life);
	  }
	  client->get_response(client, &responses);
	  while (! _LSLP_IS_EMPTY(&responses)) {
	    temp = responses.next;
	    if(temp->type == srvAck) {
	      SLP_TRACE("received SRVACK\n");
	    }
	    _LSLP_UNLINK(temp);
	    lslpDestroySLPMsg(temp, LSLP_DESTRUCTOR_DYNAMIC);
	  }
	  last = time(NULL);
	}
      }
      SLP_TRACE("slp_srvreg: call destroy_slp_client()\n");	// JSS
      destroy_slp_client(client);
    }
    printf("slp_srvreg: registered %s with %d SLP agents.\n", type, count); // JSS - changed func name
  }
  free_globals();
  
  if (count == 0){
	  return(1);
  } 
  
  return(0);
}



/*
 * Local variables:
 *  c-file-style: "gnu"
 * End:
 */
