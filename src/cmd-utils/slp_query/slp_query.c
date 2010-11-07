
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
 *  Description: scripting shell for slp_client - generates an slp QUERY 
 *
 *****************************************************************************/





/**** define the following token to import labels from the slp_client library *****/
#define SLP_LIB_IMPORT 1
#include "../slp_client/slp_client.h"


static int8 *type;
static int8 *addr = NULL;
static int8 *scopes;
static int8 *_interface;
static int8 *predicate;
static  int8 *spi;
static int16 port = 427, converge;
static BOOL dir_agent = FALSE;
static BOOL test = FALSE;
static BOOL parsable= FALSE;
static BOOL tcp = FALSE;
static int8 fs='\t', rs='\n';
static union slp_sockaddr address;  
static struct slp_net *con = NULL;
int ip_version = 4;
int broadcast = FALSE;

// Commenting the following since the LSLP_MTU being declared here and used as
// extern in slp_client.h does not work on Windows. So making this variable
// local to slp_client library and accessing it via method setLslpMtu.
//int32 LSLP_MTU=4096;

void free_globals(void)
{
  if(type != NULL)
    free(type);
  if(addr != NULL)
    free(addr);
  if(scopes != NULL)
    free(scopes);
  if(_interface != NULL)
    free(_interface);
  if(predicate != NULL)
    free(predicate);
  if(spi != NULL)
    free(spi);
}


void usage(void)
{
  printf("\nslp_query -- transmit an SLP Service Request and print the results.\n");
  printf("-------------------------------------------------------------------------\n");
  printf("slp_query --type=service-type-string\n");
  printf("         [--predicate=predicate-string]\n"); 
  printf("         [--scopes=scope-string]\n");
  printf("         [--address=target-IP]\n");
  printf("         [--port=target-port]\n");
  printf("         [--interface=host-IP]\n");
  printf("         [--use_da=true]\n");
  printf("         [--mtu=buffer size]\n");
  printf("         [--tcp]\n");
  printf("         [--converge=convergence-cycles]\n");
  printf("         [--spi=security-parameters-index] (not used)\n");
  printf("         [--test] (tests parameters)\n");
  printf("         [--fs=field-separator]\n");
  printf("         [--rs=record-separator]\n");
  printf("         [--ip=version]\n");
  printf("         [--broadcast]\n");
  printf("         [--version]\n");
  printf("\n");
  printf("All parameters must be a single string containing no spaces.\n");
  printf("Always use the format of <parameter>=<value>.\n");
  printf("Parameters enclosed in brackets are optional.\n");
    
   
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
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "predicate=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(predicate != NULL)
	  free(predicate);
	predicate = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "scopes*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(scopes != NULL)
	  free(scopes);
	scopes = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "address*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(addr != NULL)
	  free(addr);
	addr = ip6_normalize_addr(bptr, ip_version);
	if (!addr)
	  addr = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "port*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	port = strtoul(bptr, NULL, 10);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "interface*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(_interface != NULL)
	  free(_interface);
	_interface = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "use_da=true*", FALSE)) {
	dir_agent = TRUE;
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "mtu=*", FALSE)) {
	      bptr = argv[i] + 2;
	      while(*bptr != '=') bptr++;
	      bptr++;
	      //LSLP_MTU = (strtol(bptr, NULL, 10) & 0x0000ffff);
	      setLslpMtu((strtol(bptr, NULL, 10) & 0x0000ffff));
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "test*", FALSE)) {
	test = TRUE;
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "converge=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	converge = strtoul(bptr, NULL, 10);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "fs=*", FALSE)){
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	fs = *bptr;
	parsable=TRUE;
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "rs=*", FALSE)){
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	rs = *bptr;
	parsable=TRUE;
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
      }else if (TRUE == lslp_pattern_match(argv[i] + 2, "version*", FALSE)) {
	printf("slp_query version %s changeset %s.\nUsing client library version %s changeset %s\n", 
	       LSLP_TAG, LSLP_CHANGESET, lslp_get_version(), lslp_get_changeset());
	exit(1);
	
      }
    }
  }
  
  if( type == NULL )
    return FALSE;
  return TRUE;
}


int main(int argc, char **argv)
{
  struct slp_client *client;
  lslpMsg responses, *temp;
  char addr6[INET6_ADDRSTRLEN];
  
  int addr_type;
  setLslpMtu(4096); //default
  _LSLP_INIT_HEAD(&responses);
  
  if(FALSE == get_options(argc, argv)) {
    usage();
  } else {
    if(scopes == NULL)
      scopes = strdup("DEFAULT");
    if(test == TRUE){
      return test_query(type, predicate, scopes);
    }
    SLP_TRACE("creating slp client");

      if (addr) {
	addr_type = ip6_addr_type(addr);
	if (addr_type == 6 && ip_version != 6)
	  ip_version = 0;
 	if (addr_type == 4 && ip_version != 4)
	  ip_version = 0;
      }

    if(NULL != (client = create_slp_client(ip_version, 
					   addr, 
					   _interface, 
					   port, 
					   "DSA", 
					   scopes, 
					   FALSE, 
					   dir_agent, 0))) {
      
      
      SLP_TRACE("client is at %p, ip version is %i\n", client, client->ip_version);
      client->broadcast = broadcast;

      if (!addr)
	addr = ip6_normalize_addr("*", client->ip_version);      
      if (client->ip_version == 6 || (addr && addr_type == 6)) {
	address.saddr.sa_family = AF_INET6;
	address.sin6.sin6_port = htons(port);
	inet_pton(AF_INET6, addr,  &address.sin6.sin6_addr);
	inet_ntop(AF_INET6, &address.sin6.sin6_addr, addr6, INET6_ADDRSTRLEN);
	if (client->broadcast && client->ip_version == 6) {
	  client->broadcast = FALSE;
	  SLP_TRACE("Broadcasting is not allowed by IPv6, disabling.\n");
	}
      } else if (client->ip_version == 4 || client->ip_version == 0) {
	ip_version = 4;
	address.sin.sin_port = htons(port);
	address.saddr.sa_family = AF_INET;
	address.sin.sin_addr.s_addr = inet_addr(addr);
	inet_ntop(AF_INET, &address.sin.sin_addr, addr6, INET6_ADDRSTRLEN);
      }
      
      if(tcp == TRUE) {
	SLP_TRACE("using TCP\n");
	if ( NULL == (con = tcp_create_connection(client, &address))) {
	  SLP_TRACE("error creating tcp connection\n");
	  return 1;
	}
	client->tcp_srv_req(client, con, type, scopes, predicate);

      } else { /* tcp */
	if( ip6_is_loopback(addr))  {
	  SLP_TRACE("issuing a local srv req\n");
	  client->local_srv_req(client, type, predicate, scopes);
	} else {
	  client->_convergence = converge ? converge : 0;
	  if (client->_convergence == 0) {
	    SLP_TRACE("issuing a unicast srv req\n");  
	    client->unicast_srv_req(client, type, predicate, scopes, &address);
	  } else {
	    SLP_TRACE("issuing a converged srv req\n");
	    client->converge_srv_req(client, type, predicate, scopes);
	  } 
	}
      } /* udp */
      
      client->get_response(client, &responses);
      while( ! _LSLP_IS_EMPTY(&responses)) {
	temp = responses.next;
	if(temp->type == srvRply) {
	  if(parsable == TRUE )
	    lslp_print_srv_rply_parse(temp, fs, rs);
	  else
	    lslp_print_srv_rply(temp);
	}
	_LSLP_UNLINK(temp);
	lslpDestroySLPMsg(temp, LSLP_DESTRUCTOR_DYNAMIC);
      }
      destroy_slp_client(client); /* will destroy the connection */
    }
  }
  
  free_globals();
  return 1;

}


/*
 * Local variables:
 *  c-file-style: "gnu"
 * End:
 */
