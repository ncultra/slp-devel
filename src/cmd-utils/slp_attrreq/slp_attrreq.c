#define SLP_LIB_IMPORT 1
#include "../slp_client/slp_client.h"

static int8 *url = NULL, *scopes = NULL, *tags = NULL, *addr = NULL, *_interface = NULL;
static int16 port = 427, converge ;
static BOOL dir_agent = FALSE;
static BOOL test = FALSE;
static BOOL parsable = FALSE;
static int8 fs='\t', rs='\n';
int ip_version = 4;
int broadcast = FALSE;
union slp_sockaddr address;

// Commenting the following since the LSLP_MTU being declared here and used as
// extern in slp_client.h does not work on Windows. So making this variable
// local to slp_client library and accessing it via method setLslpMtu.
//int32 LSLP_MTU=4096;

void free_globals(void)
{
  if(url != NULL) free(url);
  if(scopes != NULL) free(scopes);
  if(tags != NULL) free(tags);
  if(addr != NULL) free(addr);
  if(_interface != NULL) free(_interface);
  
}


void usage(void)
{
  printf("\nslp_attrreq -- transmit an SLP Attribute Request and print the results.\n");
  printf("-------------------------------------------------------------------------\n");
  printf("slp_attrreq --url=url-string\n");
  printf("         [--tags=tags-string]\n"); 
  printf("         [--scopes=scope-string]\n");
  printf("         [--address=target-IP]\n");
  printf("         [--port=target-port]\n");
  printf("         [--interface=host-IP]\n");
  printf("         [--use_da=true]\n");
  printf("         [--mtu=buffer size]\n");
  printf("         [--test] (tests parameters)\n");
  printf("         [--converge=convergence-cycles]\n");
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
      if(TRUE == lslp_pattern_match(argv[i] + 2, "url=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	/* handle case where type is specified twice on the cmd line */
	if(url != NULL)
	  free(url);
	url = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "tags=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(tags != NULL)
	  free(tags);
	tags = strdup(bptr);
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
	port = strtol(bptr, NULL, 10);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "interface*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	if(_interface != NULL)
	  free(_interface);
	_interface = strdup(bptr);
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "test*", FALSE)) {
	test = TRUE;
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "use_da=true*", FALSE)) {
	dir_agent = TRUE;
      } else if(TRUE == lslp_pattern_match(argv[i] + 2, "mtu=*", FALSE)) {
	      bptr = argv[i] + 2;
	      while(*bptr != '=') bptr++;
	      bptr++;
	      //LSLP_MTU = (strtol(bptr, NULL, 10) & 0x0000ffff);
	      setLslpMtu((strtol(bptr, NULL, 10) & 0x0000ffff));
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
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "ip=*", FALSE)) {
	bptr = argv[i] + 2;
	while(*bptr != '=') bptr++;
	bptr++;
	ip_version = strtoul(bptr, NULL, 10);
	SLP_TRACE("get_options: ip version: %d\n", ip_version);
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "broadcast*", FALSE)) {
	broadcast = TRUE;
	SLP_TRACE("get_options: broadcast turned ON\n");
      } else if (TRUE == lslp_pattern_match(argv[i] + 2, "version*", FALSE)) {
	printf("slp_attreq version %s changeset %s.\nUsing client library version %s changeset %s\n", 
	       LSLP_TAG, LSLP_CHANGESET, lslp_get_version(), lslp_get_changeset());
	exit(1);
	
      } 

    }
  }
  
  if( url == NULL )
    return FALSE;
  return TRUE;
}

int main(int argc, char **argv)
{

  struct slp_client *client;
  lslpMsg responses, *temp;
  char addr6[INET6_ADDRSTRLEN];;

  setLslpMtu(4096); //default
  _LSLP_INIT_HEAD(&responses);
  
  if(FALSE == get_options(argc, argv)) {
    usage();
    return 0;
    
  } else {
    if(scopes == NULL)
      scopes = strdup("DEFAULT");
    
    if(NULL != (client = create_slp_client(ip_version, addr, _interface, port,
					   "DSA", scopes, FALSE, dir_agent, 0))) {

      if (client->ip_version == 6 ) {
	if (!addr)
	  addr = ip6_normalize_addr("::", client->ip_version);
	inet_pton(AF_INET6, addr,  &address.sin6.sin6_addr);
	if (broadcast)
	  SLP_TRACE("Broadcasting is not allowed by IPv6, disabling.\n");
	
	address.saddr.sa_family = AF_INET6;
	address.sin6.sin6_port = htons(port);
	inet_ntop(AF_INET6, &address.sin6.sin6_addr, addr6, INET6_ADDRSTRLEN);
      } else {
	client->ip_version = 4;
	if (broadcast) {
	  client->broadcast = TRUE;
	  SLP_TRACE("Broadcasting enabled\n");
	}
	
	if (!addr)
	  addr = ip6_normalize_addr("localhost", client->ip_version);
	address.sin.sin_port = htons(port);
	address.saddr.sa_family = AF_INET;
	address.sin.sin_addr.s_addr = inet_addr(addr);
	inet_ntop(AF_INET, &address.sin.sin_addr, addr6, INET6_ADDRSTRLEN);
	
      }
      
      if (ip6_is_loopback(addr))
	client->local_attr_req(client, url, scopes, tags);
      else if (converge) {
	client->_convergence = converge;
	client->converge_attr_req(client, url, scopes, tags);
      } else
	client->unicast_attr_req(client, url, scopes, tags, &address);
      
       /* end of request  */
      
      client->get_response(client, &responses);
      while( ! _LSLP_IS_EMPTY(&responses) ) {
	temp = responses.next;
	
	if(temp->type == attrRep) {
	  if(parsable == TRUE && temp->msg.attrRep.attrListLen > 0 ) {
	    lslp_print_attr_rply_parse(temp, fs, rs);
	  } else {
	    if( temp->msg.attrRep.attrListLen > 0) {
		    printf("Attr. Reply for %s\n" , url);
	      lslp_print_attr_rply(temp); 
	    }
	  }
	}/* if we got an attr rply */ 
	_LSLP_UNLINK(temp);
	lslpDestroySLPMsg(temp, LSLP_DESTRUCTOR_DYNAMIC);	

      } /* while traversing response list */ 
      destroy_slp_client(client);
      
    } /* client successfully created */ 
    
  }
  free_globals();
  return 1 ;
 }

/*
 * Local variables:
 *  c-file-style: "gnu"
 * End:
 */
