/* make pattern matching work with a length parameter, like strncmp(a,b) */
/* <<< Wed Aug  7 20:37:13 2002 mdd >>> */

/* make all direct calls static (hidden), public calls to be exposed through client->() */
/* << Mon Sep 16 14:00:36 2002 mdd >> */
#include "slp_client.h"

static char *_LSLP_VERSION = LSLP_TAG;
static char *_LSLP_CHANGESET = LSLP_CHANGESET;

SLP_STORAGE_DECL const char *lslp_get_version(void)
{
  return _LSLP_VERSION;
}

SLP_STORAGE_DECL const char *lslp_get_changeset(void)
{
  return _LSLP_CHANGESET;
}


//extern int32 LSLP_MTU;
// Added setLslpMtu and getLslpMtu methods to access
// LSLP_MTU variable from outside this library.
int32 LSLP_MTU=0;
 
void setLslpMtu(int32 mtu)
{
	LSLP_MTU = mtu;
}
int32 getLslpMtu()
{
	return LSLP_MTU;
}
 

#ifdef _WIN32
int _winsock_count = 0;
WSADATA _wsa_data ;



#include <time.h>
#endif 

#ifdef _AIX		// JSS
#include <netdb.h>	// need struct hostent_data for gethostbyname_r()
#endif			// JSS

struct da_list *alloc_da_node(BOOL head)
{
  struct da_list *node = (struct da_list *)calloc(1, sizeof(struct da_list));
  if(node != NULL && head == TRUE){
    node->isHead = TRUE;
    node->next = node->prev = node;
  }
  return node;
}

struct da_list *da_node_exists(struct da_list *head, const char *url)
{
  if(head != NULL && _LSLP_IS_HEAD(head) && url != NULL) {
    struct da_list *temp = head->next;
    while( ! _LSLP_IS_HEAD(temp)){
      if(! strcmp(temp->url, url))
	return temp;
      temp = temp->next;
    }
  }
  return NULL;
}


void free_da_list_members(struct da_list *da)
{
  assert( ! _LSLP_IS_HEAD(da));
  if(da->url != NULL)
    free(da->url);
  if(da->scope != NULL)
    free(da->scope);
  if(da->attr != NULL)
    free(da->attr);
  if(da->spi != NULL)
    free(da->spi);
  if(da->auth != NULL)
    free(da->auth);
}

void free_da_list_node(struct da_list *da)
{
  assert( ! _LSLP_IS_HEAD(da));
  free_da_list_members(da);
  free(da);
}


/* DOES NOT free the list head ! */
void free_da_list(struct da_list *list)
{
  struct da_list *temp;
  assert(_LSLP_IS_HEAD(list));
  temp = list->next;
  while( ! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    free_da_list_node(temp);
    temp = list->next;
  }
}

struct rply_list *alloc_rply_list(BOOL head)
{
  struct rply_list *node = (struct rply_list *)calloc(1, sizeof(struct rply_list));
  if(node != NULL && head == TRUE){
    node->isHead = TRUE;
    node->next = node->prev = node;
  }
  return node;
}


struct rply_list *rpl_node_exists(struct rply_list *head, const void *key)
{
  if(head != NULL && _LSLP_IS_HEAD(head) && key != NULL){
    struct rply_list *temp = head->next;
    while( ! _LSLP_IS_HEAD(temp)){
      if( ! strcmp(temp->url, (int8 *)key))
	return temp;
      temp = temp->next;
    }
  }
  return NULL;
}

void free_rply_list_members(struct rply_list *rply)
{
  assert(! _LSLP_IS_HEAD(rply));
  if(rply->url != NULL)
    free(rply->url);
  if(rply->auth != NULL)
    free(rply->auth);
}

void free_rply_list_node(struct rply_list *rply)
{
  assert(! _LSLP_IS_HEAD(rply));
  free_rply_list_members(rply);
  free(rply);
}

/* DOES NOT free the list head ! */
void free_rply_list(struct rply_list *list)
{
  struct rply_list *temp;
  assert(_LSLP_IS_HEAD(list));
  temp = list->next;
  while( ! _LSLP_IS_HEAD(temp)){
    _LSLP_UNLINK(temp);
    free_rply_list_node(temp);
    temp = list->next;
  }
}

struct reg_list *alloc_reg_list(BOOL head)
{
  struct reg_list *node = (struct reg_list *)calloc(1, sizeof(struct reg_list));
  if( node != NULL && head == TRUE){
    node->isHead = TRUE;
    node->next = node->prev = node;
  }
  return node;
}

struct reg_list *reg_node_exists(struct reg_list *head, const void *key)
{
  if(head != NULL && _LSLP_IS_HEAD(head) && key != NULL){
    struct reg_list *temp = head->next;
    while( ! _LSLP_IS_HEAD(temp)){
      if( ! strcmp(temp->url, (int8 *)key))
	return temp;
      temp = temp->next;
    }
  }
  return NULL;
}

void free_reg_list_members(struct reg_list *reg)
{
  assert(! _LSLP_IS_HEAD(reg));
  if(reg->url != NULL)
    free(reg->url);
  if(reg->attributes != NULL)
    free(reg->attributes);
  if(reg->service_type != NULL)
    free(reg->service_type);
  if(reg->scopes != NULL)
    free(reg->scopes);
}

void free_reg_list_node(struct reg_list *reg)
{
  assert(! _LSLP_IS_HEAD(reg));
  free_reg_list_members(reg);
  free(reg);
}

/* DOES NOT free the list head ! */
void free_reg_list(struct reg_list *list)
{
  struct reg_list *temp;
  assert( _LSLP_IS_HEAD(list));
  temp = list->next;
  while(! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    free_reg_list_node(temp);
    temp = list->next;
  }
}

struct url_entry *alloc_url_entry(BOOL head)
{
  struct url_entry *node = (struct url_entry *)calloc(1, sizeof(struct url_entry));
  if(node != NULL && head == TRUE){
    node->isHead = TRUE;
    node->next = node->prev = node;
  }
  return node;
}

struct url_entry *url_node_exists(struct url_entry *head, const void *key)
{
  if(head != NULL && _LSLP_IS_HEAD(head) && key != NULL){
    struct url_entry *temp = head->next;
    while( ! _LSLP_IS_HEAD(temp)){
      if( ! strcmp(temp->url, (int8 *)key))
	return temp;
      temp = temp->next;
    }
  }
  return NULL;
}

void free_url_entry_members(struct url_entry *url)
{
  assert(! _LSLP_IS_HEAD(url));
  if(url->url != NULL)
    free(url->url);
  if(url->auth_blocks != NULL)
    free(url->auth_blocks);
}

void free_url_node(struct url_entry *node)
{
  assert(! _LSLP_IS_HEAD(node));
  free_url_entry_members(node);
  free(node);
}

/* DOES NOT free the list head ! */
void free_url_list(struct url_entry *list)
{
  struct url_entry *temp;
  assert( _LSLP_IS_HEAD(list));
  temp = list->next;
  while(! _LSLP_IS_HEAD(temp)) {
    _LSLP_UNLINK(temp);
    free_url_node(temp);
    temp = list->next;
  }
}


char * get_client_rcv_buf(struct slp_client* client, 
			  struct slp_net *connect)
{
  if (connect->type == NET_TYPE_CONNECT)
    return connect->c._rcv_buf;
  return client->_rcv_buf;
}

char *get_client_msg_buf(struct slp_client *client, struct slp_net *remote)
{
  char *buf = NULL;
  
  if (remote->type == NET_TYPE_CONNECT) {
    if(remote->c._msg_buf)
      free(remote->c._msg_buf);
    buf = (char *)calloc(LSLP_MAX_TCP_SIZE+1,  sizeof(char));
    if(buf == NULL) 
      abort();
    remote->c._msg_buf = buf;
    remote->c.msg_data_len = LSLP_MAX_TCP_SIZE;
  } else 
    buf = client->_msg_buf;
  
  return buf;
}

static BOOL tcp_msg_buffer_check(struct slp_net *con, int8** cursor, int size)
{
  int8* new_ptr;
  int msg_len = 0;
  
  if(! con || ! cursor || ! *cursor )
    abort();
  
  msg_len = *cursor - con->c._msg_buf;
  SLP_TRACE("msg_len is %i, size is %i, msg_data_len is %i\n", 
	    msg_len, size, con->c.msg_data_len);
  if(msg_len > LSLP_MAX_TCP_SIZE) {
    SLP_TRACE("message length greater than max tcp size, len %i, max %i\n",
	      msg_len, LSLP_MAX_TCP_SIZE);
    return FALSE;
  }
  
  while( con->c.msg_data_len - msg_len < size && con->c.msg_data_len < LSLP_MAX_TCP_SIZE ) {
    SLP_TRACE("realloc: msg_data_len %d; size %d; msg_len %d\n", 
	      con->c.msg_data_len, size, msg_len);
    
    new_ptr = (int8*)realloc(con->c._msg_buf, con->c.msg_data_len + 1024);
    if(new_ptr) {
      con->c._msg_buf = new_ptr;
      con->c.msg_data_len += 1024;
      *cursor = new_ptr + msg_len;
    } else 
      break;
  }
  if(con->c.msg_data_len < size) {
    SLP_TRACE("FALSE\n");
    return FALSE;
  }
  
  return TRUE;
}


static BOOL tcp_message_sanity_check(char* buf)
{
 
  if( _LSLP_GETVERSION(buf) != LSLP_PROTO_VER ) {
    SLP_TRACE("Received a message with wrong version: %d ", 
	      _LSLP_GETVERSION(buf));
    return FALSE ;
  }

  if ( _LSLP_GETFUNCTION(buf) < LSLP_SRVRQST || 
       _LSLP_GETFUNCTION(buf) > LSLP_SAADVERT) {
    SLP_TRACE("Received unexpected message type: %d ", 
	      _LSLP_GETFUNCTION(buf));
    return FALSE;
  }

  if( _LSLP_GETLENGTH(buf) < LSLP_MIN_HDR + 1 ) {
    SLP_TRACE("Received a message too small to be SLP: size %d ",  
	      _LSLP_GETLENGTH(buf));
    return FALSE;
  }

  if( _LSLP_GETLENGTH(buf) > LSLP_MAX_TCP_SIZE ) {
    SLP_TRACE("Received a message too large for my buffer: size %d ",  
	      _LSLP_GETLENGTH(buf));
    return FALSE;
  }
  
  return TRUE; 
}




#if defined( _WIN32 ) || defined( BSD )  
int gethostbyname_r(const char *name, 
		    struct hostent *resultbuf, 
		    char *buf, 
		    size_t bufsize, 
		    struct hostent **result, 
		    int *errnop) 
{
  name = name;
  resultbuf = resultbuf;
  buf = buf;
  bufsize = bufsize;
  
  if(NULL == (*result = gethostbyname(name))) {
#if defined( _WIN32 ) 
    *errnop = WSAGetLastError();
#else
    *errnop = errno;
#endif 
    return(-1);
  } 
  return(0);
}

#endif



/* client->local_addr_list must be initialized */
int slp_get_local_interfaces(struct slp_client *client)
{
  struct sockaddr_list *tmp;
  int i = 0;
  
  while (!_LSLP_IS_EMPTY(&client->local_addr_list)) {
    tmp = client->local_addr_list.next;
    _LSLP_UNLINK(tmp);
    free(tmp);
  }
  
  if (client->ip_version == 4 || client->ip_version == 0)
    ip_get_addr_list(&client->local_addr_list);
  if (client->ip_version == 6 || client->ip_version == 0)
    ip6_get_addr_list(&client->local_addr_list);

  tmp = client->local_addr_list.next;
  while (!_LSLP_IS_HEAD(tmp)) {
    i++;
    tmp = tmp->next;
  }
  return i;
} 

/* not called */
/*
void slp_join_leave_multicast_all(struct slp_client *client, 
				  const char *group, 
				  int join, 
				  int version)
{
  static char *ip4_group = "239.255.255.253";
  slp_get_local_interfaces(client);

  switch(version) {
  case IPVER_ANY:
    ip4_join_leave_group_all(ip4_group, client, join);
    if (group == NULL) {
      ip6_join_leave_group_all(SVRLOC_GRP, client, 
			       join ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP);
      ip6_join_leave_group_all(SVRLOC_DA_GRP, client, 
			       join ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP);
    } else 
      ip6_join_leave_group_all(group, client, 
			     join ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP);
    break;
    
  case IPVER_6:
    if (group == NULL) {
      ip6_join_leave_group_all(SVRLOC_GRP, client, 
			       join ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP);
      ip6_join_leave_group_all(SVRLOC_DA_GRP, client, 
			       join ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP);
    } else 
      ip6_join_leave_group_all(group, client, 
			     join ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP);
    break;
    
  case IPVER_4:
  default:
    if (group == NULL)
      ip4_join_leave_group_all(ip4_group, client, join);
    else
      ip4_join_leave_group_all(group, client, join);
    break;
  }
  return;
}
*/

/* not called */
/*
void slp_open_listen_socks(struct slp_client *client)
{
  struct slp_net * tmp = client->listeners.next;
  while ( ! _LSLP_IS_HEAD(tmp) ) {
    _LSLP_UNLINK( tmp );
    _LSLP_CLOSESOCKET(tmp->sock);
    free(tmp);
  } 
  post_udp_listeners(client);
  post_tcp_listeners(client);
}
*/
void prepare_pr_buf(struct slp_client *client, const int8 *address)
{
  if(address == NULL || client == NULL )
    return;;
  if(client->_pr_buf_len > LSLP_MTU) {
    SLP_TRACE("Error: pr buf length is %d, MTU is %d\n", client->_pr_buf_len, LSLP_MTU);
    return;
    /*  abort(); */
  }
  
  if(client->_pr_buf_len > 0 && client->_pr_buf_len < (LSLP_MTU - 2)) {
    if( NULL != strstr(client->_pr_buf, address)) {
      SLP_TRACE("prepare_pr_buf: address %s already on pr list\n", address);
      return;
    } else {
      SLP_TRACE("Adding %s to pr list\n", address);
    }
    client->_pr_buf[client->_pr_buf_len - 1] = ',';
  }
  do {
    client->_pr_buf[client->_pr_buf_len] = *address;
    address++;
    (client->_pr_buf_len)++;
  }while((*address != 0x00) && (client->_pr_buf_len < LSLP_MTU - 2)); 
  (client->_pr_buf_len)++;
  SLP_TRACE("pr list: %s\n", client->_pr_buf);
}

/* TCP support */
SLP_STORAGE_DECL struct slp_net *tcp_create_connection(struct slp_client *client, 
						       union slp_sockaddr *addr)
{
  int ccode;
  
  struct slp_net *con = (struct slp_net *)calloc(1, sizeof(struct slp_net));
  con->next = con->prev = con;  
  con->type = NET_TYPE_CONNECT;
  
  if (addr->saddr.sa_family == AF_INET)
    con->sock = _LSLP_SOCKET(AF_INET, SOCK_STREAM, 0);
  else
    con->sock = _LSLP_SOCKET(AF_INET6, SOCK_STREAM, 0);

  if (con->sock < 0) {
    free(con);
    return NULL;
  }
  
  ccode = 51000;
  setsockopt(con->sock, SOL_SOCKET, SO_RCVBUF, (int8 *)&ccode, sizeof(ccode));
  ccode = 51000;
  setsockopt(con->sock, SOL_SOCKET, SO_SNDBUF, (int8 *)&ccode, sizeof(ccode));
  ccode = 1;
  ccode = setsockopt(con->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&ccode, sizeof(ccode));
  bind(con->sock, &client->local_addr.saddr, sizeof(client->local_addr.saddr));
  
  ccode = connect(con->sock, &addr->saddr, sizeof(*addr));
  if (ccode == SOCKET_ERROR) {
    SLP_TRACE("connect failed, errno: %d\n", errno);
    lslpDestroyConnection(con);
    return NULL;    
  }
  
  con->c._msg_buf = (int8*)calloc(1, LSLP_MAX_TCP_SIZE);
  if(con->c._msg_buf)
    con->c.msg_data_len = LSLP_MAX_TCP_SIZE;
  else {
    lslpDestroyConnection(con);
    con = NULL;    
  }
  return con;
}



/****************************************************************
 *  connectedRead
 *  PARAMETERS: SOCKET fd - a socket that is connected.
 *				void *pv  - buffer to read into
 *				size_t n - number of bytes to read 
 *  DESCRIPTION: waits 1 second for a connected socket to have
 *				 data - timesout and returns zero if no data
 *  RETURNS: num bytes or <0 if error
 *
 ***************************************************************/
static int32 connectedRead(SOCKET fd, void *pv, size_t n)
{

  size_t nleft = n;
  int32    nread = 0, iErr;
  int8   *pc   = (int8 *) pv;
  fd_set fds;

  assert(fd >= 0);

  while (nleft > 0) {
    /* i can get rid of this select later */
    struct timeval tv = { 2, 0 };
    FD_ZERO(&fds);
    FD_SET(fd,&fds);
    iErr = _LSLP_SELECT(FD_SETSIZE ,&fds,NULL,NULL,&tv);
    if (iErr == SOCKET_ERROR ) {
      if(errno != EINTR)
	return(iErr);
    } 
    else if (iErr == 0)  /* time expired */
      break;
    else {
      if ((nread = recv(fd, pc, nleft, 0)) < 0) {  /* socket error */
	if (errno == EINTR)   /* blocking call was interrupted */
	  nread = 0; /* and call read() again */
	else 
	  return -1;	
	   
      } 
      else if (nread == 0) 
	break;   /* EOF  - connection closed */
      nleft -= nread;
      pc    += nread;
    }
  }
  return (n - nleft); /* return >= 0 */
}

/* TCP support */
/****************************************************************
 *  connectedWrite
 *
 *
 *  PARAMETERS: SOCKET fd - a connected socket
 *				void *pv - buffer to write
 *				size_t n - number of bytes to write

 *  DESCRIPTION:
 *
 *  RETURNS: 
 *
 ***************************************************************/
static int32 connectedWrite(SOCKET fd, void *pv, size_t n)
{

  size_t nleft = n;
  size_t ntowrite = n;
  int32 nwritten = 0;
  const int8 *pc = (int8 *) pv;

  assert(fd >= 0);
  
  while(nleft > 0) {
    ntowrite = nleft;
    if (ntowrite > 4096)
      ntowrite = 4096;
    
    if ((nwritten = send(fd, pc, ntowrite, 0)) < 0) {
      if (errno == EINTR) 
	nwritten = 0; /* call write() again */
      else {
	SLP_TRACE("connected write errno: %i\n", errno);
	return -1;                   /* error              */
      }
    }
    nleft -= nwritten;
    pc    += nwritten;
  }
  return(n);
}



void make_tcp_srv_ack(struct slp_net *con, int8 response, int16 code)
{
  int len;
  int8* bptr = NULL;
  
  if(con->c._msg_buf)
    free(con->c._msg_buf);

  con->c._msg_buf = (int8*)calloc(LSLP_MIN_HDR + 10 + 1, sizeof(int8));
  bptr = con->c._msg_buf;
  
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, response);

  _LSLP_SETFLAGS(bptr, 0);
  _LSLP_SETXID(bptr, _LSLP_GETXID(con->c._rcv_buf));
  _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
  bptr += _LSLP_HDRLEN(bptr) ;
  _LSLP_SETSHORT(bptr, code, 0 );
  bptr += 2;
  len = bptr - con->c._msg_buf;
  
  _LSLP_SETLENGTH(con->c._msg_buf, len);
  connectedWrite(con->sock, con->c._msg_buf, len);
  free(con->c._msg_buf);
  con->c._msg_buf = NULL;
}


/** attn need to role change to getflags line into nucleus **/
void make_srv_ack(struct slp_client *client, union slp_sockaddr *remote, int8 response, int16 code )
{
  int8 *bptr;
  if(TRUE == ( ((_LSLP_GETFLAGS( client->_rcv_buf )) & (LSLP_FLAGS_MCAST) ) ? FALSE : TRUE   ) ) {
    
    memset(client->_msg_buf, 0x00, LSLP_MTU);
    bptr = client->_msg_buf;
    _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
    _LSLP_SETFUNCTION(bptr, response);

    _LSLP_SETFLAGS(bptr, 0);
    _LSLP_SETXID(bptr, _LSLP_GETXID(client->_rcv_buf));
    _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
    bptr += _LSLP_HDRLEN(bptr) ;
    _LSLP_SETSHORT(bptr, code, 0 );
    bptr += 2;
    _LSLP_SETLENGTH(client->_msg_buf, bptr - client->_msg_buf);

    ip6_reply(client, remote, bptr - client->_msg_buf);

  } // successfully opened this socket
}



int tcp_prepare_query(struct slp_client *client, 
		      struct slp_net *con, 
		      unsigned short xid, 
		      const char *service_type,
		      const char *scopes, 
		      const char *predicate) 
{
  int32 len, total_len, buf_len, buf_needed;
  int8 *bptr, *bptrSave;

  /* estimate the buffer size needed for this query */
  if(service_type)
    buf_needed = strlen(service_type);
  if(scopes)
    buf_needed += strlen(scopes);
  else 
    buf_needed += strlen("DEFAULT");
  if(predicate)
    buf_needed += strlen(predicate);
  buf_needed += (LSLP_MIN_HDR * 4);
  
  /* make sure we have a buffer of sufficient size */
  if(! con->c._msg_buf ) {
    con->c._msg_buf = (int8*)calloc(1, buf_needed);
    con->c.msg_data_len = buf_needed;
  } else if(con->c._msg_buf && con->c.msg_data_len < buf_needed) {
    int8* tmp = (int8*)realloc(con->c._msg_buf, buf_needed);
    if(tmp == NULL)
      return FALSE;
  }
    
  /* now we have a good buffer */
  memset(con->c._msg_buf, 0x00, con->c.msg_data_len);
  bptr = con->c._msg_buf;
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_SRVRQST);
  /* we don't know the length yet */
  _LSLP_SETFLAGS(bptr, 0);
  _LSLP_SETXID(bptr, xid);
  _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
  bptr += ( total_len = _LSLP_HDRLEN(bptr) ) ;

  /* set the pr list length to zero */
  _LSLP_SETSHORT(bptr, 0, 0);
  total_len += 2;
  bptr += 2;
  if(service_type == NULL)
    len = DA_SRVTYPELEN;
  else
    len = strlen(service_type) ;

  /* set the service type string length */
  _LSLP_SETSHORT(bptr, len, 0);
  if(service_type != NULL)
    memcpy(bptr + 2, service_type, len);
  else
    memcpy(bptr + 2, DA_SRVTYPE, len);
	  	  
  total_len += (2 + len);
  bptr += (2 + len);
  /* set the scope len and scope type, advance the buffer */
	  
  if(scopes == NULL)
    len = DA_SCOPELEN;
  else
    len = strlen(scopes);

  _LSLP_SETSHORT(bptr, len, 0);
  if(scopes != NULL) 
    memcpy(bptr + 2, scopes, len);
  else
    memcpy(bptr + 2, DA_SCOPE, DA_SCOPELEN);

  total_len += ( 2 + len);
  bptr += (2 + len);
  /* stuff the predicate if there is one  */
  if(predicate == NULL)
    len = 0;
  else
    len = strlen(predicate) ;

  _LSLP_SETSHORT(bptr, len, 0);
  if(predicate != NULL)
    memcpy(bptr + 2, predicate, len);
	      
  total_len += (2 + len);
  bptrSave = (bptr += (2 + len));

  buf_len = con->c.msg_data_len - total_len;
  _LSLP_SETSHORT(bptr, 0x0000, 0);
  bptr += 2;
  total_len += 2;
  assert(total_len == bptr - con->c._msg_buf);
  /*  always add an attr extension to an srvrq if there is room */

  _LSLP_SETNEXTEXT(con->c._msg_buf, total_len);
  _LSLP_SETSHORT(bptr, 0x0002, 0);
  _LSLP_SET3BYTES(bptr, 0x00000000, 2);
  _LSLP_SETSHORT(bptr, 0x0000, 5);
  _LSLP_SETSHORT(bptr, 0x0000, 7);
  _LSLP_SETBYTE(bptr, 0x00, 9);
  total_len += 10;

  /* now go back and set the length for the entire message */
  _LSLP_SETLENGTH(con->c._msg_buf, total_len );
  return(TRUE);
}

int prepare_query(struct slp_client *client, 
		  unsigned short xid,
		  const char *service_type,
		  const char *scopes, 
		  const char *predicate  ) 
{
  int32 len, total_len, buf_len;
  int8 *bptr, *bptrSave;
  
  if(xid != client->_xid) {
    SLP_TRACE("new request, clearing pr buf\n");
    /* this is a new request */
    memset(client->_pr_buf, 0x00, LSLP_MTU);
    client->_pr_buf_len = 0;
    (client->_xid)++;
  } else {
    SLP_TRACE("should use pr buf: %s\n", client->_pr_buf);
  }
  SLP_TRACE("pr buf: %s\n", client->_pr_buf);
  SLP_TRACE("prepare_query, xid is %d\n", client->_xid);
  memset(client->_msg_buf, 0x00, LSLP_MTU);
  bptr = client->_msg_buf;
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_SRVRQST);
  /* we don't know the length yet */
  _LSLP_SETFLAGS(bptr, 0);
  _LSLP_SETXID(bptr, client->_xid);
  _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
  bptr += ( total_len = _LSLP_HDRLEN(bptr) ) ;
  if(client->_pr_buf_len + total_len < LSLP_MTU) {
    /* set the pr list length */
    _LSLP_SETSHORT(bptr, (len = client->_pr_buf_len), 0);
    if(len)
      memcpy(bptr + 2, client->_pr_buf, len);
    total_len += ( 2 + len );
    bptr += (2 + len);
    if(service_type == NULL)
      len = DA_SRVTYPELEN;
    else
      len = strlen(service_type) ;
    if(total_len + 2 + len < LSLP_MTU) {
      /* set the service type string length */
      _LSLP_SETSHORT(bptr, len, 0);
      if(service_type != NULL)
	memcpy(bptr + 2, service_type, len);
      else
	memcpy(bptr + 2, DA_SRVTYPE, len);
	  	  
      total_len += (2 + len);
      bptr += (2 + len);
      /* set the scope len and scope type, advance the buffer */
	  
      if(scopes == NULL)
	len = DA_SCOPELEN;
      else
	len = strlen(scopes);
      if( total_len + 2 + len < LSLP_MTU) {
	_LSLP_SETSHORT(bptr, len, 0);
	if(scopes != NULL) 
	  memcpy(bptr + 2, scopes, len);
	else
	  memcpy(bptr + 2, DA_SCOPE, DA_SCOPELEN);

	total_len += ( 2 + len);
	bptr += (2 + len);
	/* stuff the predicate if there is one  */
	if(predicate == NULL)
	  len = 0;
	else
	  len = strlen(predicate) ;
	if( total_len + 2 + len < LSLP_MTU) {
	  _LSLP_SETSHORT(bptr, len, 0);
	  if(predicate != NULL)
	    memcpy(bptr + 2, predicate, len);
	      
	  total_len += (2 + len);
	  bptrSave = (bptr += (2 + len));

	  /* <<< Thu Jun 10 08:51:03 2004 mdd >>>  force the spi string to be zero length*/

	  /* stuff the spi */
	  buf_len = LSLP_MTU - total_len;
	  _LSLP_SETSHORT(bptr, 0x0000, 0);
	  bptr += 2;
	  total_len += 2;
	  

	  /* 	  lslpStuffSPIList(&bptr, &buf_len, client->_spi); */
		
	  /* read back the length of the spi */
	  /* 	  total_len += (2 + _LSLP_GETSHORT(bptrSave, 0)); */

	  
	  /* <<< Thu Jun 10 08:51:03 2004 mdd >>>  force the spi string to be zero length*/

	  assert(total_len == bptr - client->_msg_buf);
	  /*  always add an attr extension to an srvrq if there is room */
	  if(total_len + 9 <= LSLP_MTU ) {
	    _LSLP_SETNEXTEXT(client->_msg_buf, total_len);
	    _LSLP_SETSHORT(bptr, 0x0002, 0);
	    _LSLP_SET3BYTES(bptr, 0x00000000, 2);
	    _LSLP_SETSHORT(bptr, 0x0000, 5);
	    _LSLP_SETSHORT(bptr, 0x0000, 7);
	    _LSLP_SETBYTE(bptr, 0x00, 9);
	    total_len += 10;
	  }
	  /* now go back and set the length for the entire message */
	  _LSLP_SETLENGTH(client->_msg_buf, total_len );
	  return(TRUE);

	} /*  room for predicate  */
      } /* room for the scope  */
    } /* room for the service type  */
  } /* room for the pr list  */
  return(FALSE);
}


// <<< Sat Jul 24 16:25:44 2004 mdd >>> attr request 


/*       0                   1                   2                   3 */
/*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |       Service Location header (function = AttrRqst = 6)       | */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |       length of PRList        |        <PRList> String        \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |         length of URL         |              URL              \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |    length of <scope-list>     |      <scope-list> string      \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |  length of <tag-list> string  |       <tag-list> string       \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |   length of <SLP SPI> string  |        <SLP SPI> string       \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

int prepare_attr_query( struct slp_client *client, 
			 unsigned short  xid,
			 const char *url,
			 const char *scopes, 
			 const char  *tags)
{
  int32 len, total_len;
  int8 *bptr;
  const int8 *scopeptr;
  static int8 default_scope[] = "DEFAULT";
  
  if(url == NULL)
    return FALSE;
  
  /* first off, check to see if this is a retry or a new request */ 
  if(xid != client->_xid) {
    /* this is a new request, not a retry */
    /* reset the previous responder buffer */ 
    memset(client->_pr_buf, 0x00, LSLP_MTU);
    client->_pr_buf_len = 0;
    client->_xid = xid;
  }
  
  /* reset the client's send buffer and initialize the buffer pointer */ 
  memset(client->_msg_buf, 0x00, LSLP_MTU);
  bptr = client->_msg_buf;
  
  /* initialize the SLP Header */
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_ATTRREQ);
  
  /* skip the length field until we know the actual length of the message */

  _LSLP_SETFLAGS(bptr, 0);
  _LSLP_SETXID(bptr, xid);
  _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
  
  /* adjust the buffer pointer forward and initialize our length counter */ 
  bptr += ( total_len = _LSLP_HDRLEN(bptr) );
  
  /* make sure the pr buffer will not cause an overflow and set the pr list length */ 
  if( client->_pr_buf_len + total_len < LSLP_MTU ) {
    _LSLP_SETSHORT(bptr, (len = client->_pr_buf_len), 0 );
    /* if there is a pr list, copy it into the msg buffer */ 
    if(len)
      memcpy(bptr + 2, client->_pr_buf, len);
    /* adjust our counter and buffer pointer */
    total_len += ( 2 + len );
    bptr += ( 2 + len );
    
    /* if there is room, set the url length and copy the url */ 
    len = strlen(url);
    if(total_len + 2 + len < LSLP_MTU ) {
      _LSLP_SETSHORT(bptr, len, 0 );
      if(len)
	memcpy(bptr + 2, url, len);
      /* adjust counter and buffer pointer */ 
      total_len += (2 + len);
      bptr += (2 + len);
      
      /* if there is room, set the scope list length and copy the scope list */ 
      if(scopes == NULL)
	scopeptr = default_scope;
      else
	scopeptr = scopes;
      
      len = strlen(scopeptr);
      if(total_len + 2 + len < LSLP_MTU) {
	_LSLP_SETSHORT(bptr, len, 0 );
	
	if(len)
	  memcpy(bptr + 2, scopeptr, len);
	
      	total_len += (2 + len);
	bptr += (2 + len);
	
	/* if there is room, set the tag list length and copy the tag list */
	if(tags != NULL)
	  len = strlen(tags);
	else
	  len = 0;
	
	if(total_len + 2 + len < LSLP_MTU) {
	  _LSLP_SETSHORT(bptr + 2, len, 0);
	  if(len)
	    memcpy(bptr + 2, tags, len);
	  total_len += ( 2 + len );
	  bptr += ( 2 + len );

	  /* leave the spi length at zero - spi will be deprecated in next version of SLP */ 
	  /* length needs to account for the spi string length */ 
	  total_len += 2;
	   
	  /* now go back and set the length for the entire message */
	  _LSLP_SETLENGTH(client->_msg_buf, total_len );
	  return(TRUE);

	} /* if tags list fits */ 
      } /* if scopes fit */ 
    } /* if the url fits */ 
  } /* if pr buffer fits */ 
  
  return FALSE;
}


void set_separators(struct slp_client *client, int8 fs, int8 rs)
{
  client->_fs = fs;
  client->_rs = rs;
  client->_use_separators = TRUE;
}



lslpMsg *get_response( struct slp_client *client, lslpMsg *head)
{
  assert(head != NULL && _LSLP_IS_HEAD(head));
  if( _LSLP_IS_EMPTY(&(client->replies)))
    return NULL;
  
  _LSLP_LINK_HEAD(head, &(client->replies));
  return(head);
}


int find_das(struct slp_client *client, 
	     const int8 *predicate, 
	     const int8 *scopes)
{
  converge_srv_req(client, "service:directory-agent", predicate, scopes);
  time(&(client->_last_da_cycle));
  if( ! _LSLP_IS_EMPTY(&(client->das))) {
    SLP_TRACE("found a Directory Agent using DAs for SRVREG");
    client->_use_das = TRUE;
  } else { client->_use_das = FALSE; }
  
  return((int) client->_use_das );
}


/* smart interface to slp discovery. uses das if they are present,  */
/* convergence otherwise. */
/* periodically forces an active da discovery cycle  */

void discovery_cycle(struct slp_client *client, 
		     const int8 *type, 
		     const int8 *predicate, 
		       const int8 *scopes) 
{
  // see if we have built a cache of directory agents 
  if( _LSLP_IS_EMPTY(&(client->das))) {
    // we don't know of any directory agents - see if we need to do active da discovery
    if( ((time(NULL)) - client->_last_da_cycle ) > (60 * 5) )
      find_das(client, NULL, scopes) ;
  }

  // if there are das, unicast a srvreq to each da

  if( ! _LSLP_IS_EMPTY(&(client->das))) {
    struct da_list *da = client->das.next;
    union slp_sockaddr addr;
    while( ! _LSLP_IS_HEAD(da)  ) {
      addr.sin.sin_port = htons(client->_target_port);
      addr.saddr.sa_family = AF_INET;
      addr.sin.sin_addr.s_addr = inet_addr(da->remote);
      unicast_srv_req(client, type, predicate, scopes, &addr);
      da = da->next;
    }
  } else {
    // do a convergence request because we don't have any das to use 
    converge_srv_req(client, type, predicate, scopes );
  }
  return; 
}

/* this request MUST be retried <_convergence> times on EACH interface  */
/* regardless of how many responses we have received  */
/* it can be VERY time consuming but is the most thorough  */
/* discovery method  */

void converge_srv_req(struct slp_client *client,
		      const int8 *type, 
		      const int8 *predicate, 
		      const int8 *scopes)
{
  ip6_converge_req(client, type, scopes, predicate, prepare_query);
}

// this request will be retried MAX <_retries> times 
// but will always end when the first response is received
// This request is best when using a directory agent
void unicast_srv_req(struct slp_client *client, 
		     const int8 *type, 
		     const int8 *predicate, 
		     const int8 *scopes, 
		     union slp_sockaddr *addr)
{
  union slp_sockaddr target_save;

  memcpy(&target_save, &client->target_addr, sizeof(target_save));
  memcpy(&client->target_addr, addr, sizeof(client->target_addr));
  
  ip6_unicast_req(client, type, scopes, predicate, prepare_query);

  memcpy(&client->target_addr, &target_save, sizeof(client->target_addr));
  return;
}

// this request is targeted to the loopback interface, 
// and has a tiny wait timer. It should be resolved quickly. 
// It will never be retried.
void local_srv_req( struct slp_client *client,
		    const int8 *type, 
		    const int8 *predicate, 
		    const int8 *scopes )
{

  union slp_sockaddr target_save;
  struct timeval tv_save;
  
  memcpy(&target_save, &client->target_addr, sizeof(target_save));
  tv_save.tv_sec = client->_tv.tv_sec;
  tv_save.tv_usec = client->_tv.tv_usec;
  client->_tv.tv_sec = 0;
  client->_tv.tv_usec = 10000;
  
  if (client->target_addr.saddr.sa_family == AF_INET) {
    client->target_addr.sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    client->target_addr.sin.sin_port = htons(client->_target_port);
  } else if (client->target_addr.saddr.sa_family == AF_INET6) {
    inet_pton(AF_INET6, "::1", &client->target_addr.sin6.sin6_addr);
    client->target_addr.sin6.sin6_port = htons(client->_target_port);
  }
  srv_req(client, type, scopes, predicate);

  memcpy(&client->target_addr, &target_save, sizeof(client->target_addr));
  client->_tv.tv_sec = tv_save.tv_sec;
  client->_tv.tv_usec = tv_save.tv_usec;

  return;
}

// workhorse request function
void srv_req( struct slp_client *client, 
	      const int8 *type, 
	      const int8 *predicate, 
	      const int8 *scopes)
{
  ip6_unicast_req(client, type, predicate, scopes, prepare_query);
}


/* TCP support */
void tcp_srv_req(struct slp_client* client,
		 struct slp_net *con, 
		 const int8 *service_type,
		 const int8 *scopes, 
		 const int8 *predicate  ) 
{
  int bytes_written;


  if(con->sock >= 0 && TRUE == tcp_prepare_query(client, con, client->_xid + 1, 
						 service_type, scopes, predicate)) {
    bytes_written = connectedWrite(con->sock, 
				   con->c._msg_buf, 
				   _LSLP_GETLENGTH(con->c._msg_buf));
    if(bytes_written == -1)
      goto sock_failure;
  }
  bytes_written = tcp_handle_connection(client, con); 
  return;

 sock_failure:
  SLP_TRACE("bad socket in tcp_srv_req\n");
  _LSLP_UNLINK(con);
  lslpDestroyConnection(con);
  return;
  
}


/* this request MUST be retried <_convergence> times on EACH interface  */
/* regardless of how many responses we have received  */
/* it can be VERY time consuming but is the most thorough  */
/* discovery method  */

void converge_attr_req( struct slp_client *client, 
			const int8 *url, 
			const int8 *scopes, 
			const int8 *tags)
{
  ip6_converge_req(client, url, scopes, tags, prepare_attr_query);
}

// this request will be retried MAX <_retries> times 
// but will always end when the first response is received
// This request is best when using a directory agent  

void unicast_attr_req( struct slp_client *client,
		       const int8 *url, 
		       const int8 *scopes,
		       const int8 *tags, 
		       union slp_sockaddr *addr)
{
  union slp_sockaddr target_save;
  
  memcpy(&target_save, &client->target_addr, sizeof(target_save));
  memcpy(&client->target_addr, addr, sizeof(*addr));
  
  ip6_unicast_req(client, url, scopes, tags, prepare_attr_query);

  memcpy(&client->target_addr, &target_save, sizeof(client->target_addr));
  return;

}

// targeted to the loopback interface  
void local_attr_req( struct slp_client *client, 
		     const int8 *url, 
		     const int8 *scopes, 
		     const int8 *tags )
{

  union slp_sockaddr target_save;
  struct timeval tv_save;
  
  memcpy(&target_save, &client->target_addr, sizeof(target_save));
  tv_save.tv_sec = client->_tv.tv_sec;
  tv_save.tv_usec = client->_tv.tv_usec;
  client->_tv.tv_sec = 0;
  client->_tv.tv_usec = 10000;
  
  if (client->target_addr.saddr.sa_family == AF_INET) {
    client->target_addr.sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    client->target_addr.sin.sin_port = htons(client->_target_port);
  } else if (client->target_addr.saddr.sa_family == AF_INET6) {
    inet_pton(AF_INET6, "::1", &client->target_addr.sin6.sin6_addr);
    client->target_addr.sin6.sin6_port = htons(client->_target_port);
  }

  attr_req(client, url, scopes, tags, FALSE) ;
  
  memcpy(&client->target_addr, &target_save, sizeof(client->target_addr));
  client->_tv.tv_sec = tv_save.tv_sec;
  client->_tv.tv_usec = tv_save.tv_usec;

  return;
   
}



// <<< Sat Jul 24 16:22:01 2004 mdd >>> attr request
// workhorse attr request function 
void attr_req( struct slp_client *client, 
	       const int8 *url, 
	       const int8 *scopes,
	       const int8 *tags,
	       BOOL retry)
{
  ip6_unicast_req(client, url, scopes, tags, prepare_attr_query);
}



/*       0                   1                   2                   3 */
/*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |       Service Location header (function = AttrRply = 7)       | */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |         Error Code            |      length of <attr-list>    | */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |                         <attr-list>                           \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |# of AttrAuths |  Attribute Authentication Block (if present)  \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

void decode_attr_rply( struct slp_client *client, struct slp_net *remote_addr)
{

  int8 *bptr;
  lslpMsg *reply;
  int32 total_len, purported_len;

  bptr = get_client_rcv_buf(client, remote_addr);
  
  purported_len = _LSLP_GETLENGTH(bptr);

  /* marshall the header data */
  reply = alloc_slp_msg(FALSE);
  if(reply == NULL) abort();
  reply->hdr.ver = _LSLP_GETVERSION(bptr);
  reply->type = reply->hdr.msgid = _LSLP_GETFUNCTION(bptr);
  reply->hdr.len = purported_len;
  reply->hdr.flags = _LSLP_GETFLAGS(bptr);
  reply->hdr.nextExt = _LSLP_GETNEXTEXT(bptr);
  reply->hdr.xid = _LSLP_GETXID(bptr);
  
    
  reply->hdr.langLen = _LSLP_GETLANLEN(bptr);
  memcpy(reply->hdr.lang, bptr + LSLP_LAN_LEN + 2, (_LSLP_MIN(reply->hdr.langLen, 19)));
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    
    /* process the attr rply */
    reply->hdr.errCode = reply->msg.attrRep.errCode = _LSLP_GETSHORT(bptr, 0);
    reply->msg.attrRep.attrListLen = _LSLP_GETSHORT(bptr, 2);
    if(reply->msg.attrRep.attrListLen + total_len < purported_len) {
      
      total_len += 4;
      bptr += 4;
      reply->msg.attrRep.attrList = (char *)calloc(1, reply->msg.attrRep.attrListLen + 1);
      if(reply->msg.attrRep.attrList == NULL) abort();
      memcpy(reply->msg.attrRep.attrList, bptr, reply->msg.attrRep.attrListLen);
      /* ignore auth blocks, they will be deprecated in next version of protocol */ 
      
      
    } /* attr list len checks out */ 
  } /* hdr len checks out */
  /* link the response to the client */
  _LSLP_INSERT(reply, &(client->replies));
}


/* failsafe against malformed messages. */
/* if it sees a message it does not like, it simply returns */ 
/* bad message is silently discarded. */ 


static BOOL  message_sanity_check(struct slp_client *client)
{
  if( _LSLP_GETLENGTH(client->_rcv_buf) > LSLP_MTU ) {
    SLP_TRACE("Received a message too large for my buffer: size %d ",  
	      _LSLP_GETLENGTH(client->_rcv_buf));
    return FALSE;
  }
  
  if( _LSLP_GETVERSION(client->_rcv_buf) != LSLP_PROTO_VER ) {
    SLP_TRACE("Received a message with wrong version: %d ", 
	      _LSLP_GETVERSION(client->_rcv_buf));
    return FALSE ;
  }
  
  if ( _LSLP_GETFUNCTION(client->_rcv_buf) < LSLP_SRVRQST || 
       _LSLP_GETFUNCTION(client->_rcv_buf) > LSLP_SAADVERT) {
    SLP_TRACE("Received unexpected message type: %d ", 
	      _LSLP_GETFUNCTION(client->_rcv_buf));
    return FALSE;
  }
  return TRUE;
}



void decode_msg( struct slp_client *client, 
		 struct slp_net *remote ) 
{
  int8 function = -1, response, err;
    char addr6[INET6_ADDRSTRLEN];
  
  if (remote->type == NET_TYPE_LISTENER || remote->type == NET_TYPE_CONNECT) {
    if(remote->c.flags & LSLP_CONNECTION_RCV_MSG) {
      err = tcp_message_sanity_check(remote->c._rcv_buf);
      if (!err)
	return;
      function = _LSLP_GETFUNCTION(remote->c._rcv_buf);
    } else
      return;
  } else if (remote->type == NET_TYPE_RCVFROM) {
    err = message_sanity_check(client);
    if (!err)
      return;
    function = _LSLP_GETFUNCTION(client->_rcv_buf);
    if (client->_xid == _LSLP_GETXID( client->_rcv_buf)) {
      if(function == LSLP_SRVRPLY || function == LSLP_ATTRRPLY || function == LSLP_SRVTYPERPLY) {
	if (remote->addr.saddr.sa_family == AF_INET)
	  inet_ntop(AF_INET, &remote->addr.sin.sin_addr.s_addr, addr6, INET_ADDRSTRLEN);
	else 
	  inet_ntop(AF_INET6, &remote->addr.sin6.sin6_addr, addr6, INET6_ADDRSTRLEN);
	prepare_pr_buf(client, addr6);
      }
    }
  } else
    return;
  
  switch(function) {
  case LSLP_DAADVERT:
    SLP_TRACE("Rcv DAADVERT");
    decode_daadvert(client, remote);
    return;		
  case LSLP_SRVRQST:
    SLP_TRACE("Rcv SRVRQST");
    decode_srvreq( client, remote );
    return;
  case LSLP_SRVRPLY:
    SLP_TRACE("Rcv SRVRPLY");
    decode_srvrply( client, remote );
    return;
  case LSLP_SRVACK:
    SLP_TRACE("Rcv SRVACK");
    decode_srvack(client, remote);
    return;
  case LSLP_ATTRREQ:
    SLP_TRACE("Rcv ATTRREQ");
    decode_attrreq( client, remote );
    return;
  case LSLP_ATTRRPLY:
    SLP_TRACE("Rcv ATTRRPLY");
    decode_attr_rply( client, remote);
    return;
    
  case LSLP_SRVTYPERQST:
    SLP_TRACE("Rcv SRVRYPERQST");
    response = LSLP_SRVTYPERPLY;
    break;
  case LSLP_SRVREG:
    SLP_TRACE("Rcv SRVREG");
    decode_srvreg(client, remote);
    return;
  case LSLP_SRVDEREG:
    SLP_TRACE("Rcv SRVDEREG");
  default:
    response = LSLP_SRVACK;
    break;
  }
  make_srv_ack(client, &remote->addr, response, LSLP_MSG_NOT_SUPPORTED);
  return;
}

void print_atomized_url(lslpAtomizedURL *url);

void decode_srvreg(struct slp_client *client, 
		   struct slp_net *remote)
{
  int8 *bptr, *url_string, *attr_string, *type_string, *scopes;
  uint16 lifetime ;
  int32 total_len, purported_len;
  BOOL mcast, is_tcp;
  int32 str_len;
  SLP_TRACE("decode_srvreg\n");
  
  
  bptr = get_client_rcv_buf(client, remote);

  if (remote->type == NET_TYPE_CONNECT)
    is_tcp = 1;
  else
    is_tcp = 0;
  

  mcast = ( ((_LSLP_GETFLAGS( bptr )) & (LSLP_FLAGS_MCAST) ) ? TRUE : FALSE   ) ;
  purported_len = _LSLP_GETLENGTH(bptr);
  
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(purported_len < LSLP_MTU && (total_len < purported_len)) {
    lslpURL *decoded_url;
    int32 diff;
    int16 err;
    diff = purported_len - total_len;
    
    /* decode the url entry */
    if(NULL != (decoded_url = lslpUnstuffURL(&bptr, &diff, &err))) {
      url_string = decoded_url->url;
      lifetime = decoded_url->lifetime;

      /* adjust pointers and size variables */
      /* bptr already adjusted by call to lslpUnstuffURL */
      total_len += ((purported_len - total_len) - diff);
      
      /* decode the service type string */
      if((total_len + 2 + (str_len = _LSLP_GETSHORT(bptr, 0))) < purported_len) {
	if(NULL != (type_string = (int8 *)malloc(str_len + 1))) {
	  memcpy(type_string, bptr + 2, str_len);
	  *(type_string + str_len) = 0x00;

	  /* adjust pointers and size variables */
	  bptr += (2 + str_len);
	  total_len += (2 + str_len);
	  
	  /* decode the scope list */
	  if((total_len + 2 + (str_len = _LSLP_GETSHORT(bptr, 0))) < purported_len) {
	    if(NULL != (scopes = (int8 *)malloc(str_len + 1))) {
	      memcpy(scopes, bptr + 2, str_len);
	      *(scopes + str_len) = 0x00;
	      
	      /* adjust pointers and size variables */
	      bptr += (2 + str_len);
	      total_len += (2 + str_len);
	      
	      /* decode the attr list */
	      if((total_len + 2 + (str_len = _LSLP_GETSHORT(bptr, 0))) < purported_len) {
		if(NULL != (attr_string = (int8 *)malloc(str_len + 1))) {
		  memcpy(attr_string, bptr + 2, str_len);
		  *(attr_string + str_len) = 0x00;
		  /* adjust pointers and size variables */
		  bptr += (2 + str_len);
		  total_len += (2 + str_len);
		  
		  __srv_reg_local(client, url_string, attr_string, type_string, scopes, lifetime);
		  if( is_tcp == TRUE)
		    make_tcp_srv_ack(remote, LSLP_SRVACK, 0);
		  else 
		    make_srv_ack(client, &remote->addr, LSLP_SRVACK, 0);
		  free(attr_string);
		  free(scopes);
		  free(type_string);
		  lslpFreeURL(decoded_url);
		  return;
		} /* malloced attr_string */
	      } else { /* attr string sanity check */
		SLP_TRACE("SRVREG message failed sanity check");
	      }
	      
	      free(scopes);
	    } /* malloced scopes string */
	  } else { /* scope list sanity check */
	    SLP_TRACE("SRVREG message failed sanity check");
	  }
	  
	  free(type_string);
	} /* malloced srv type string */
      } else { /* srv type sanity check */
	SLP_TRACE("SRVREG message failed sanity check");
      }
      
      lslpFreeURL(decoded_url);
      if( is_tcp == TRUE)
	make_tcp_srv_ack(remote, LSLP_SRVACK, LSLP_INTERNAL_ERROR);
      else 
	make_srv_ack(client, &remote->addr, LSLP_SRVACK, LSLP_INTERNAL_ERROR);
      return;
    } /* decoded the url entry */
  } /* initial length sanity check OK */
  if( is_tcp == TRUE)
    make_tcp_srv_ack(remote, LSLP_SRVACK, LSLP_PARSE_ERROR);
  else 
    make_srv_ack(client, &remote->addr, LSLP_SRVACK, LSLP_PARSE_ERROR);
  return;
}

void decode_srvack(struct slp_client *client,
		   struct slp_net *remote_addr)
{
  int8 *bptr, *bptr_orig;
  lslpMsg *reply;
  int32 total_len, purported_len;

  bptr = get_client_rcv_buf(client, remote_addr);
  bptr_orig = bptr;
  
  purported_len = _LSLP_GETLENGTH(bptr);

  /* marshall the header data */
  reply = alloc_slp_msg(FALSE);
  if(reply == NULL) abort();
  reply->hdr.ver = _LSLP_GETVERSION(bptr);
  reply->type = reply->hdr.msgid = _LSLP_GETFUNCTION(bptr);
  reply->hdr.len = purported_len;
  reply->hdr.flags = _LSLP_GETFLAGS(bptr);
  reply->hdr.nextExt = _LSLP_GETNEXTEXT(bptr);
  reply->hdr.xid = _LSLP_GETXID(bptr);
  
  reply->hdr.langLen = _LSLP_GETLANLEN(bptr);
  memcpy(reply->hdr.lang, bptr + LSLP_LAN_LEN + 2, (_LSLP_MIN(reply->hdr.langLen, 19)));
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    reply->msg.srvAck.errCode = _LSLP_GETSHORT(bptr, 0);
    reply->type = srvAck;
    _LSLP_INSERT(reply, &(client->replies));
  }
  return;
}

void decode_srvrply( struct slp_client *client,
		     struct slp_net *remote_addr )
{
  int8 *bptr, *bptr_orig,  *xtptr, *xtn_limit;
  lslpMsg *reply;
  int16 err;
  int32 count, buf_len;
  int32 total_len, purported_len;
  BOOL is_tcp;
  
  if (remote_addr->type == NET_TYPE_CONNECT) {
      is_tcp = 1;
      buf_len = remote_addr->c.rcv_data_len;
  } else {
    is_tcp = 0;
    buf_len = LSLP_MTU;
  }
  
  bptr = get_client_rcv_buf(client, remote_addr);
  bptr_orig = bptr;
  
  purported_len = _LSLP_GETLENGTH(bptr);
  if (purported_len > buf_len) {
    SLP_TRACE("service reply failed sanity check\n");
    return;
  }

  /* marshall the header data */
  reply = alloc_slp_msg(FALSE);
  if(reply == NULL) abort();
  reply->hdr.ver = _LSLP_GETVERSION(bptr);
  reply->type = reply->hdr.msgid = _LSLP_GETFUNCTION(bptr);
  reply->hdr.len = purported_len;
  reply->hdr.flags = _LSLP_GETFLAGS(bptr);
  reply->hdr.nextExt = _LSLP_GETNEXTEXT(bptr);
  reply->hdr.xid = _LSLP_GETXID(bptr);
  
  if((reply->hdr.nextExt != 0x00000000) && (reply->hdr.nextExt < reply->hdr.len )) {
    xtptr = bptr_orig + reply->hdr.nextExt;
    xtn_limit = xtptr + reply->hdr.len;
  }
  else { xtptr = NULL; } 
  
  reply->hdr.langLen = _LSLP_GETLANLEN(bptr);
  memcpy(reply->hdr.lang, bptr + LSLP_LAN_LEN + 2, (_LSLP_MIN(reply->hdr.langLen, 19)));
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    int32 next_ext = 0;
    
    /* process the srvrply */
    reply->hdr.errCode = reply->msg.srvRply.errCode = _LSLP_GETSHORT(bptr, 0);
    reply->msg.srvRply.urlCount = (count = _LSLP_GETSHORT(bptr, 2));
    bptr += 4;
    total_len += 4;
    if( count ){
      lslpURL *url;
      if( NULL == (reply->msg.srvRply.urlList = lslpAllocURLList()))
	abort();
      if(NULL != xtptr) {
	if( NULL == (reply->msg.srvRply.attr_list = lslpAllocAtomList())) 
	  abort();
      }
      
      buf_len -= total_len;
      
      /* get the urls */
      while(count-- && buf_len > 0 ) {
	url = lslpUnstuffURL(&bptr, &buf_len, &err);
	if( NULL != url ){
	  reply->msg.srvRply.urlLen = url->len;
	  _LSLP_INSERT(url, reply->msg.srvRply.urlList);
	} else {
	  SLP_TRACE("SRVRPY: error decoding URL");	
	}
	
      }

      /* get the attributes if they are present */

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

      while((NULL != xtptr) && (xtptr + 9 < xtn_limit )) {

	next_ext = _LSLP_GET3BYTES(xtptr, 2);
	if(0x0002 == _LSLP_GETSHORT(xtptr, 0)) {
	  lslpURL *rply_url = reply->msg.srvRply.urlList;
	  SLP_TRACE("SRVRPY: processing attribute extension");	  
	  //BOOL lslp_pattern_match(const int8 *string, const int8 *pattern, BOOL case_sensitive)
	  /* find the correct url to associate this attribute extension. */	  
	  if(rply_url != 0 && ! _LSLP_IS_HEAD(rply_url->next)){
	    int8 *url_buf;
	    int32 url_len = _LSLP_GETSHORT(xtptr, 5);
	    rply_url = rply_url->next;
	    url_buf = (int8 *)calloc(1, url_len + 1);
	    if(url_buf == 0 ) abort();
	    memcpy(url_buf, xtptr + 7, url_len);
	    SLP_TRACE("url is %d bytes, %s",url_len, url_buf);
	    while(! _LSLP_IS_HEAD(rply_url)) {
	      if(TRUE == ip6_escaped_pattern_match(url_buf, rply_url->url, FALSE)) {
		/* this is the correct url to associate with the next attribute */
		int32 attr_len;
		int32 attr_offset = 7 + _LSLP_GETSHORT(xtptr, 5);
		attr_len = _LSLP_GETSHORT(xtptr, attr_offset);
		attr_offset += 2;
		SLP_TRACE("attr len: %d; attr offset: %d\n", attr_len, attr_offset);
		
		if( (xtptr + attr_offset + attr_len) < xtn_limit) {
		  lslpAtomList *temp = lslpAllocAtom();
		  if(temp != NULL){
		    temp->str = (int8 *)malloc(attr_len + 1);
		    if(temp->str != NULL) {
		      
		      memcpy(temp->str, xtptr + attr_offset, attr_len);
		      temp->str[attr_len] = 0x00;
		      SLP_TRACE("attr string: %s\n", temp->str);

		      /* allocate an attr list head */
		      if(rply_url->attrs == NULL)
			rply_url->attrs = lslpAllocAtomList();
		      if(rply_url->attrs != NULL){
			_LSLP_INSERT(temp, rply_url->attrs);
			SLP_TRACE("inserted attr info in msg\n");
			
		      }
		    } else { lslpFreeAtom(temp); }
		  } /* alloced atom */
		} else { /* sanity check */
		  SLP_TRACE("SRVRPLY message failed sanity check in attr extension");
		}
		
	      } /* if we found the right url */
	      rply_url = rply_url->next;
	    } /* while traversing urls in the reply */
	    free(url_buf);
	  } else { /* if the reply contains urls */
	    SLP_TRACE("RPLY message did not contain any URLs");
	  }
	  
	} /* correct extension */
	if( next_ext == 0 ) 
	  break;
	xtptr = bptr_orig + next_ext;
      } /* traversing extensions */
    }
    /* link the response to the client */
    _LSLP_INSERT(reply, &(client->replies));
  }
  return;
}

void decode_daadvert(struct slp_client *client, struct slp_net *remote_addr)
{
  int8 *bptr;
  int32 str_len;
  int32 total_len, purported_len;
  int32 buf_len;
  BOOL is_tcp;
  
  if (remote_addr->type == NET_TYPE_CONNECT) {
      is_tcp = 1;
      buf_len = remote_addr->c.rcv_data_len;
  } else {
    is_tcp = 0;
    buf_len = LSLP_MTU;
  }
  
  bptr = get_client_rcv_buf(client, remote_addr);

  purported_len = _LSLP_GETLENGTH(bptr);
  if (purported_len > buf_len) {
    SLP_TRACE("DA Advertisement failed sanity check\n");
    return;
  }
  SLP_TRACE("Processing DA Advertisement\n");
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(total_len < purported_len) {
    struct da_list *adv = alloc_da_node(FALSE);
    if(adv == NULL) abort();
    adv->function = LSLP_DAADVERT;
    adv->err = _LSLP_GETSHORT(bptr, 0);
    adv->stateless_boot = _LSLP_GETLONG(bptr, 2);
    total_len += (8 + (str_len = _LSLP_GETSHORT(bptr, 6)));
    if(total_len < purported_len) {
      /* decode and capture the url  - note: this is a string, not a url-entry structure */
      if(NULL == (adv->url = (int8 *)malloc(str_len + 1)))
	abort();
      memcpy(adv->url, bptr + 8, str_len);
      *((adv->url) + str_len) = 0x00;
      /* advance the pointer past the url string */
      bptr += (str_len + 8);
      total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
      if(total_len < purported_len) {
	if(str_len > 0) {
	  if(NULL == (adv->scope = (int8 *)malloc(str_len + 1)))
	    abort();
	  memcpy(adv->scope, bptr + 2, str_len);
	  *((adv->scope) + str_len) = 0x00;
	}
	/* advance the pointer past the scope string  */
	bptr += (str_len + 2);
	total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	if(total_len < purported_len) {
	  if(str_len > 0) {
	    if(NULL == (adv->attr = (int8 *)malloc(str_len + 1)))
	      abort();
	    memcpy(adv->attr, bptr + 2, str_len);
	    *((adv->attr) + str_len) = 0x00;
	  }
	  /* advance the pointer past the attr string */
	  bptr += (str_len + 2);
	  total_len += (2 + (str_len = _LSLP_GETSHORT(bptr, 0)));
	  if(total_len < purported_len) {
	    struct da_list * exists;
	    
	    if(str_len > 0 ) {
	      if(NULL == (adv->spi = (int8 *)malloc(str_len + 1)))
		abort();
	      memcpy(adv->spi, bptr + 2, str_len);
	      *((adv->spi) + str_len) = 0x00;
	    } /*  if there is an spi  */
		
	    /* advance the pointer past the spi string  */
	    bptr += (str_len + 2);
	    adv->auth_blocks = _LSLP_GETBYTE(bptr, 0);
	    SLP_TRACE("Checking DA cache\n");
	    // if we already know about this da, remove the existing
	    // entry from our cache and insert this new entry
	    // maybe the stateless boot field changed or the da
	    // supports new scopes, etc.
 	    exists  = da_node_exists(&(client->das),  adv->url);
	    if(NULL != exists) {
	      SLP_TRACE("we know this DA\n");
	      if (exists->stateless_boot >= adv->stateless_boot) {
		/* this guy has rebooted since we last saw him */
		_LSLP_UNLINK(exists);
		free_da_list_node(exists);
	      } else 
		goto no_new_da;
	    } 

	    if (remote_addr->addr.saddr.sa_family == AF_INET)
	      inet_ntop(AF_INET, &remote_addr->addr.sin.sin_addr.s_addr, adv->remote, INET_ADDRSTRLEN);
	    else 
	      inet_ntop(AF_INET, &remote_addr->addr.sin6.sin6_addr, adv->remote, INET6_ADDRSTRLEN);
	    
	    lslp_forward_regs(client, adv);
	    SLP_TRACE("Adding remote DA %s to cache", adv->remote);
	    _LSLP_INSERT(adv, &(client->das));	    
	    return;
	  } /*  spi length field is consistent with hdr */
	} /* attr length field is consistent with hdr */
      } /*  scope length field is consistent with hdr */
    }
no_new_da:
    free_da_list_node(adv);
  }
  return;
}


void decode_srvreq(struct slp_client *client, struct slp_net *remote_addr )
{
  
  int8 *extptr, *next_extptr, *next_extptr_save;
  int32 ext_offset;
  int8 *bptr, *bptr_orig, *bptrSave, *msg_buf, *msg_buf_orig;
  int32 total_len, purported_len;
  BOOL mcast, is_tcp;
  struct lslp_srv_rply_out *rp_out = NULL;
  struct lslp_srv_req *rq = NULL;
  int32 str_len, buf_len;
  int16 err = LSLP_PARSE_ERROR ;

  bptr = get_client_rcv_buf(client, remote_addr);
  bptr_orig = bptr;

  if (remote_addr->type == NET_TYPE_CONNECT)
    is_tcp = 1;
  else
    is_tcp = 0;
  
  mcast = ((_LSLP_GETFLAGS(bptr) & LSLP_FLAGS_MCAST ) ? TRUE : FALSE) ;

  purported_len = _LSLP_GETLENGTH(bptr);
  
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  if(purported_len < LSLP_MTU && (total_len < purported_len)) {
    if( ! _LSLP_IS_EMPTY((lslpSrvRegList *)&(client->regs))) {
      // advance past the slp v2 header
      // get the previous responder list 
      str_len = _LSLP_GETSHORT(bptr, 0);
      if ((str_len + total_len + 2) < purported_len ) {
	if (FALSE == slp_previous_responder( client, (str_len ? bptr + 2 : NULL ))) {
	  rq = (struct lslp_srv_req *)calloc(1, sizeof(struct lslp_srv_req));
	  if( NULL != rq) {
	    bptr += 2 + str_len;
	    total_len += 2 + str_len;
	    // extract the service type string 
	    rq->srvcTypeLen = (str_len = _LSLP_GETSHORT(bptr, 0));
	    if(str_len && (str_len + total_len + 2 < purported_len )) {
	      rq->srvcType = (int8 *)malloc(str_len + 1);

	      
	      if(rq->srvcType == NULL) {
		free(rq);
		return ;
	      }
	      memcpy(rq->srvcType, bptr + 2, str_len);
	      *(rq->srvcType + str_len) = 0x00;
	      SLP_TRACE("service type is: %s\n", rq->srvcType);
	      bptr += 2 + str_len;
	      total_len += 2 + str_len;
	      
	      bptrSave = bptr;
	      buf_len = LSLP_MTU - total_len;
	      rq->scopeList = lslpUnstuffScopeList(&bptr, &buf_len, &err);
	      total_len += _LSLP_GETSHORT(bptrSave, 0);
	      if (TRUE == lslp_scope_intersection(client->_scopes, rq->scopeList) ) {
		/* continue marshalling data */
		
		if(total_len < LSLP_MTU - 2){
		  /* get the predicate */
		  rq->predicateLen = (str_len = _LSLP_GETSHORT(bptr, 0));
		  if( str_len && (str_len + total_len + 2) < LSLP_MTU ){
		    rq->predicate = (int8 *)malloc(str_len + 1);
		    if(rq->predicate != NULL){
		      memcpy(rq->predicate, bptr + 2, str_len);
		      *(rq->predicate + str_len) = 0x00;
		    }
		  } /* predicate */
		  
		  bptr += str_len + 2;
		  total_len += str_len + 2;
		  /* get the spi list */
		  buf_len = LSLP_MTU - total_len ;
		  rq->spiList = lslpUnstuffSPIList(&bptr, &buf_len, &err);

		  /* check for an extension */
		  rq->next_ext = lslp_get_next_ext(bptr_orig);
		  if(rq->next_ext) {
		    /* derive the offset limit for the next extension and do a sanity check */
		    rq->ext_limit = (int8 *)(purported_len + bptr_orig);
		    if(rq->ext_limit < rq->next_ext) {
		      rq->next_ext = NULL;
		    }
		  }
		  /* initialize the scratch buffer before processing the srvrq */
		  /* that way we can know if there are extensions if the first 5 bytes changed */
		  memset(client->_scratch, 0x00, LSLP_SCRATCH_SIZE);
		  
		  _LSLP_SETSHORT(client->_scratch, 0x0000, 0);
		  _LSLP_SET3BYTES(client->_scratch, 0x00000000, 2);
		  if (is_tcp)
		    rp_out = _lslpProcessSrvReq(client, rq, 0, LSLP_SCRATCH_SIZE - 18,
						remote_addr);
		  else
		    rp_out = _lslpProcessSrvReq(client, rq, 0, LSLP_MTU - 18,
						remote_addr);
		  SLP_TRACE("srvrply errcode: %i, count %i, length %i, overflow %i, mtu %i\n",
			    rp_out->errCode, rp_out->urlCount, rp_out->urlLen, 
			    rp_out->overflow, LSLP_MTU);
		  
		} else { /* sanity check on remaining buffer */
		  SLP_TRACE("SRVRQ sanity check failed");
		}
		
	      } else { /* my scopes intersect */
		SLP_TRACE("SRVRQ scopes do not intersect");
	      }
	      
	    } else { /* service type sanity check */
	      SLP_TRACE("SRVRQ sanity check failed");
	    }
	    
	  } /* allocated req struct */
	} else { 
	  SLP_TRACE("Service request - I am on the PR list");
	  goto srv_rply_out;
	}
      } else { /* pr list sanity check */
	SLP_TRACE("SRVRQ sanity check failed");      
      }
    } else { /* we have regs */
	          
      SLP_TRACE("Received SRVREQ but I have no local registrations");
    }

    if(mcast == FALSE || (rp_out != NULL && rp_out->urlCount > 0 )) {
      SLP_TRACE("srvrply mcast: %d\n", mcast);
      
      msg_buf = msg_buf_orig = get_client_msg_buf(client, remote_addr);

      // we need to respond to this message
      _LSLP_SETVERSION(msg_buf, LSLP_PROTO_VER);
      _LSLP_SETFUNCTION(msg_buf, LSLP_SRVRPLY);
      // skip the length for now
      _LSLP_SETFLAGS(msg_buf, 0);
      
      _LSLP_SETNEXTEXT(msg_buf, 0);
      _LSLP_SETXID( msg_buf, ( _LSLP_GETXID(bptr_orig)));
      _LSLP_SETLAN(msg_buf, LSLP_EN_US, LSLP_EN_US_LEN);
      total_len = _LSLP_HDRLEN(msg_buf);
      msg_buf  += total_len;
      if(rp_out != NULL) {
	if((is_tcp == FALSE && rp_out->urlLen < (LSLP_MTU - total_len)) || 
	   (is_tcp == TRUE && 
	    TRUE == tcp_msg_buffer_check(remote_addr, &msg_buf, rp_out->urlLen + total_len)) ) {
	  SLP_TRACE("copying urls into message buffer\n");
	  
	  memcpy(msg_buf, rp_out->urlList, rp_out->urlLen);
	  msg_buf += rp_out->urlLen;
	  total_len += rp_out->urlLen;
	  if (rp_out->overflow) {
	    SLP_TRACE("reply overflows mtu\n");
	    _LSLP_SETFLAGS(msg_buf_orig, LSLP_FLAGS_OVERFLOW);
	  }
	} else {
	  _LSLP_SETSHORT( msg_buf, 0, 0 );
	  _LSLP_SETFLAGS(msg_buf_orig, LSLP_FLAGS_OVERFLOW);
	  total_len += 2;
	  bptr += 2;
	  /* clear the extension ptr, we don't have room for it. */
	  rq->next_ext = NULL;
	}
      } else {
	/* set the error code */
	_LSLP_SETSHORT(msg_buf, 0, 0);
	/* set the url count */
	_LSLP_SETSHORT(msg_buf, 0, 2);
	msg_buf += 4;
	total_len += 4;
      }

      extptr = client->_scratch;
      next_extptr = msg_buf_orig + LSLP_NEXT_EX;
      next_extptr_save = next_extptr;
      
      ext_offset = msg_buf - msg_buf_orig ;
      SLP_TRACE("next ext offset: %d\n", ext_offset);
      
      if ( 0x0002 == (_LSLP_GETSHORT(extptr, 0))) {
	/* set the next extension field in the header */
	_LSLP_SET3BYTES(next_extptr, ext_offset, 0);
	while( 0x0002 == (_LSLP_GETSHORT(extptr, 0))) {
	  /* check see if it fits */
	  int32 ext_len = 5;

	  SLP_TRACE("through ext while loop,extptr is %p\n", extptr);
	  	  
	  /* accumulate url len */
	  ext_len += (2 + _LSLP_GETSHORT(extptr, ext_len));
	  /* accumulate attr len */
	  ext_len += (2 + _LSLP_GETSHORT(extptr, ext_len));
	  /* add the auths byte */
	  ext_len++;
	  if((is_tcp == FALSE && (ext_len + total_len) < LSLP_MTU) || 
	     (is_tcp == TRUE && 
	      TRUE == tcp_msg_buffer_check(remote_addr, &msg_buf, ext_len + total_len))) {
	    /* set the next extension pointer to be the third byte of the first extension */
	    next_extptr = msg_buf_orig + ext_offset + 2;
	    next_extptr_save = next_extptr;
	    /* copy the extension */
	    memcpy(msg_buf, extptr, ext_len);
	    msg_buf += ext_len;
	    total_len += ext_len;

	    
	    /* set the offset of the next extension */
	    _LSLP_SET3BYTES(next_extptr, ext_offset + ext_len, 0);
	    extptr += ext_len;

	    /* adjust the pointers */
	    ext_offset += ext_len;
	    next_extptr = msg_buf + ext_offset + 2;
	    
	    /* minimum length of attr extension is 10 bytes - see if we should continue */
	    if((is_tcp == FALSE && total_len + 10 >= LSLP_MTU) || 
	       (is_tcp == TRUE && 
		FALSE == tcp_msg_buffer_check(remote_addr, &msg_buf, total_len + 10))) {
	      SLP_TRACE("SRVREQ: out of buffer space, sending a truncated response\n");
	      /* no room, set next ext field in this extension to zero */
	      _LSLP_SET3BYTES(next_extptr_save, 0x00000000, 0);
	      /* if there is another extension, set the truncated flag */
	      if ( 0x0002 == (_LSLP_GETSHORT(extptr, 0))) {
		_LSLP_SETFLAGS(msg_buf, LSLP_FLAGS_OVERFLOW);
	      }
	      break;
	    } /* no more room */
	  }  else  {
	    _LSLP_SET3BYTES(next_extptr_save, 0x00000000, 0); 
	    break;
	  } 
	} /* while there are extensions */
	/* terminate the last extension */
	_LSLP_SET3BYTES(next_extptr_save, 0x00000000, 0); 
      } /* if there is at least one extension */
	
      // ok, now we can set the length
      _LSLP_SETLENGTH(msg_buf_orig, total_len );
      SLP_TRACE("set srvrq len to: %i\n", total_len);
      
      _LSLP_SETSHORT(client->_scratch, 0x0000, 0);
      _LSLP_SET3BYTES(client->_scratch, 0x00000000, 2);
      // msg_buf is stuffed with the service reply. now we need 
      // to allocate a socket and send it back to the requesting node 

      if(is_tcp) {
	int bytes;
	
	/* write the message to the socket */
	SLP_TRACE("writing reply message...\n");
	
	bytes = connectedWrite(remote_addr->sock, remote_addr->c._msg_buf, total_len);
	SLP_TRACE("wrote %i bytes of %i\n", bytes, total_len);
	
	free(remote_addr->c._msg_buf);
	remote_addr->c._msg_buf = NULL;
      } else {
	if(mcast == TRUE ) 
	  _LSLP_SLEEP(rand() % 10 );
	SLP_TRACE("flags are %i\n", _LSLP_GETFLAGS(msg_buf_orig));
	ip6_reply(client, &remote_addr->addr, total_len);
      }
    } // we must respond to this request 
srv_rply_out:
    /* free resources */
    if(rq != NULL){
      if(rq->prList != NULL)
	free(rq->prList);
      if(rq->srvcType != NULL)
	free(rq->srvcType);
      if(rq->scopeList != NULL)
	lslpFreeScopeList(rq->scopeList);
      if(rq->predicate != NULL)
	free(rq->predicate);
      if(rq->spiList != NULL)
	lslpFreeSPIList(rq->spiList);
      free(rq);
    }
    if(rp_out != NULL){
      if(rp_out->urlList != NULL)
	free(rp_out->urlList);
      free(rp_out);
    }
  } /* header sanity check */
}


BOOL tcp_srv_reg(struct slp_client* client, 
		 struct slp_net *con, 
		 const int8 *url,
		 const int8 *attributes,
		 const int8 *service_type,
		 const int8 *scopes,
		 int16 lifetime) 
{
  int32 len, str_len, buf_len, buf_needed;
  int8* bptr;
  int bytes_written;
  lslpURL* url_entry = NULL;
  BOOL attr_is_good = FALSE;
  lslpAttrList* temp_attr = NULL;
  if(client == NULL || con == NULL || url == NULL || service_type == NULL)
    return FALSE;
  
  if (con->sock < 0)
    return FALSE;

/* estimate the buffer size needed */
  buf_needed = strlen(url);
  buf_needed += strlen(service_type);
  if(attributes)
    buf_needed += strlen(attributes);
  if(scopes)
    buf_needed += strlen(scopes);
  else
    buf_needed += strlen("DEFAULT");
  buf_needed += (LSLP_MIN_HDR * 4);
  
  /* make sure we have a buffer of sufficient size */
  if(! con->c._msg_buf ) {
    con->c._msg_buf = (int8*)calloc(1, buf_needed);
    con->c.msg_data_len = buf_needed;
  } else if(con->c._msg_buf && con->c.msg_data_len < buf_needed) {
    int8* tmp = (int8*)realloc(con->c._msg_buf, buf_needed);
    if(tmp == NULL)
      return FALSE;
  }
  memset(con->c._msg_buf, 0x00, con->c.msg_data_len);
  bptr = con->c._msg_buf;
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_SRVREG);
  /* we don't know the length yet */
  _LSLP_SETXID(bptr, client->_xid + 1024);
  
  _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
  bptr += (len =  _LSLP_HDRLEN(bptr) ) ;
	
  /* construct a url-entry  */

  url_entry = (lslpURL *)calloc(1, sizeof(lslpURL));
  if(url_entry == NULL)
    return FALSE;
  url_entry->lifetime = lifetime + time(NULL);
  url_entry->len = strlen(url);
  url_entry->url = strdup(url);
  url_entry->auths = 0;
  buf_len = con->c.msg_data_len - len;
  SLP_TRACE("len: %d; MTU: %d; buf_len: %d\n", len, con->c.msg_data_len, buf_len);
  
  if( TRUE == lslpStuffURL(&bptr, &buf_len, url_entry )) {
    lslpFreeURL(url_entry);
    url_entry = NULL;
    
    len = con->c.msg_data_len - buf_len ;
    /* stuff the service type string */

    /* stuff the service type  */
    str_len = strlen(service_type) ;
    _LSLP_SETSHORT(bptr, str_len, 0);
    memcpy(bptr + 2, service_type, str_len);
    bptr += (2 + str_len);
    len += (2 + str_len);
    /* stuff the scope list if there is one */
    if(scopes == NULL)
      str_len = 0;
    else 
      str_len = strlen(scopes);	  

    _LSLP_SETSHORT(bptr, str_len, 0);
    if(str_len) 
      memcpy(bptr + 2, scopes, str_len);
    len += (2 + str_len);
    bptr += (2 + str_len);
    /* stuff the attribute string if there is one */
    if(attributes == NULL) {
      attr_is_good = TRUE;
      str_len = 0;
    } else {
      // <<< Tue Sep 13 16:50:17 2005 mdd >>>
      // fail reg if attribute string is bad

      if(NULL != (temp_attr = _lslpDecodeAttrString((int8*)attributes))) {
	attr_is_good = TRUE;
	lslpFreeAttrList(temp_attr, TRUE);
      } else {
	attr_is_good = TRUE;
	SLP_TRACE("Error stuffing parsing string - syntax incorrect");
      }
      str_len = strlen(attributes);
    }
	
    if( attr_is_good == TRUE ) {
      _LSLP_SETSHORT(bptr, str_len, 0);
      if(str_len)
	memcpy(bptr + 2, attributes, str_len);
      
      len += ( 2 + str_len);
      bptr += (2 + str_len);
      
      /* <<< Thu Jun 10 09:03:11 2004 mdd >>> force no authentication blocks */ 
      
      /* no attribute auths for now */
      _LSLP_SETBYTE(bptr, 0x00, 0);
      len += 1;
      
      /* set the length field in the header */
      _LSLP_SETLENGTH(con->c._msg_buf, len);
      bytes_written = connectedWrite(con->sock, con->c._msg_buf, 
				     _LSLP_GETLENGTH(con->c._msg_buf));
      if(bytes_written == -1)
	goto socket_failure;

      bytes_written = tcp_handle_connection(client, con);
      if (bytes_written > 0) {

	return TRUE;
      }
      
      return FALSE;
      
    } else {/* attribute string is good  */
      SLP_TRACE("Error, bad  attribute string");
    }
  }  else {
    SLP_TRACE("Error stuffing url string");
    /* url fits into buffer  */
  }
  
  if(url_entry != NULL)
    lslpFreeURL(url_entry);
  return(FALSE);

 socket_failure:
  _LSLP_UNLINK(con);
  lslpDestroyConnection(con);  
  return FALSE;
  
}


BOOL  srv_reg(struct slp_client *client,
	      const int8 *url,
	      const int8 *attributes,
	      const int8 *service_type,
	      const int8 *scopes,
	      int16 lifetime) 
{
  uint32 len;
  int32 str_len, buf_len;
  int8 *bptr;
  lslpURL *url_entry = NULL;
  BOOL attr_is_good = FALSE;
  lslpAttrList *temp_attr = NULL;
  
  /* this is always a new request */
  memset( client->_pr_buf, 0x00, LSLP_MTU);
  client->_pr_buf_len = 0;
  (client->_xid)++ ;

  memset(client->_msg_buf, 0x00, LSLP_MTU);
  bptr = client->_msg_buf;
  _LSLP_SETVERSION(bptr, LSLP_PROTO_VER);
  _LSLP_SETFUNCTION(bptr, LSLP_SRVREG);
  /* we don't know the length yet */
  _LSLP_SETXID(bptr, client->_xid);
  _LSLP_SETLAN(bptr, LSLP_EN_US, LSLP_EN_US_LEN);
  bptr += (len =  _LSLP_HDRLEN(bptr) ) ;
	
  /* construct a url-entry  */

  url_entry = (lslpURL *)calloc(1, sizeof(lslpURL));
  if(url_entry == NULL)
    return FALSE;
  url_entry->lifetime = lifetime + time(NULL);
  url_entry->len = strlen(url);
  url_entry->url = strdup(url);
  url_entry->auths = 0;
  buf_len = LSLP_MTU - len;
  SLP_TRACE("len: %d; MTU: %d; buf_len: %d\n", len, LSLP_MTU, buf_len);
  
  if( TRUE == lslpStuffURL(&bptr, &buf_len, url_entry )) {
    len = LSLP_MTU - buf_len ;
    /* stuff the service type string */

    /* stuff the service type  */
    str_len = strlen(service_type) ;
    if(len + 2 + str_len < LSLP_MTU) {
      int retries;
      _LSLP_SETSHORT(bptr, str_len, 0);
      memcpy(bptr + 2, service_type, str_len);
      bptr += (2 + str_len);
      len += (2 + str_len);
      /* stuff the scope list if there is one */
      if(scopes == NULL)
	str_len = 0;
      else 
	str_len = strlen(scopes);	  
      if(len + 2 + str_len < LSLP_MTU) {
	_LSLP_SETSHORT(bptr, str_len, 0);
	if(str_len) 
	  memcpy(bptr + 2, scopes, str_len);
	len += (2 + str_len);
	bptr += (2 + str_len);
	/* stuff the attribute string if there is one */
	if(attributes == NULL) {
	  attr_is_good = TRUE;
	  str_len = 0;
	} else {
	  // <<< Tue Sep 13 16:50:17 2005 mdd >>>
	  // fail reg if attribute string is bad

	  if(NULL != (temp_attr = _lslpDecodeAttrString((int8*)attributes))) {
	    attr_is_good = TRUE;
	    lslpFreeAttrList(temp_attr, TRUE);
	  }
		
	  str_len = strlen(attributes);
	}
	
	if(len + 2 + str_len < LSLP_MTU && attr_is_good == TRUE ) {
	  _LSLP_SETSHORT(bptr, str_len, 0);
	  if(str_len)
	    memcpy(bptr + 2, attributes, str_len);
		  
	  len += ( 2 + str_len);
	  bptr += (2 + str_len);

	  /* <<< Thu Jun 10 09:03:11 2004 mdd >>> force no authentication blocks */ 

	  /* no attribute auths for now */
	  if(len + 1 < LSLP_MTU) {
	    _LSLP_SETBYTE(bptr, 0x00, 0);
	    len += 1;
	  }

	  /* <<< Thu Jun 10 09:03:11 2004 mdd >>> force no authentication blocks */ 

	  /* set the length field in the header */
	  _LSLP_SETLENGTH( client->_msg_buf, len );
	  
	  retries = client->_retries;
	  while( --retries ) {
	    if(TRUE == ip6_send_rcv_udp(client, NULL, TRUE)) {
	      if(LSLP_SRVACK == _LSLP_GETFUNCTION( client->_rcv_buf )) {
		if(0x0000 == _LSLP_GETSHORT( client->_rcv_buf, (_LSLP_HDRLEN( client->_rcv_buf )))) {
		  memset(client->_msg_buf, 0x00, LSLP_MTU);
		  if(url_entry != NULL)
		    lslpFreeURL(url_entry);
		  return(TRUE); 
		}
	      }
	    } // received a response 
	  } // retrying the unicast 
	} else {/* attribute string fits into buffer */
	  SLP_TRACE("Error stuffing attribute string");
	}
      } else { /* scope string fits into buffer  */
	SLP_TRACE("Error stuffing scope string");
      }
    } else { /* service type fits into buffer  */
      SLP_TRACE("Error stuffing type string");
    }
  }  else {
    SLP_TRACE("Error stuffing url string");
    /* url fits into buffer  */
  }
  
  memset( client->_msg_buf, 0x00, LSLP_MTU);
  if(url_entry != NULL)
    lslpFreeURL(url_entry);
  return(FALSE);
}

int udp_handle_listener(struct slp_client *client, struct slp_net *net)
{
  struct slp_net remote;
  int err;
  unsigned int size;
  
  remote.sock = net->sock;
  remote.type = NET_TYPE_RCVFROM;
  memcpy(&remote.addr, &net->addr, sizeof(remote.addr));

  size = sizeof(remote);
  err = recvfrom(net->sock, 
		 client->_rcv_buf,
		 LSLP_MTU,
		 0,
		 &remote.addr.saddr, &size);
  
  SLP_TRACE("read %d bytes from sock %d\n", err, net->sock);
  if (0 < err)
    decode_msg(client, &remote);
  return err;
}


/* next two are to read server sockets to handle incoming requests */
/* responses are read by ip6_service_listener */
int32 service_listener(struct slp_client *client, SOCKETD extra_sock, lslpMsg *list)
{
  return service_listener_wait(client, 0, extra_sock, TRUE, list);
}

int32 service_listener_wait(struct slp_client *client, 
			    time_t wait, 
			    SOCKETD extra_sock, 
			    BOOL one_only, 
			    lslpMsg *list)
{
  int err;
  time_t now, start = time(NULL);
  struct timeval tv = {0, 10000};
  do {
    err = ip6_service_listeners(client, &tv);
    if (err < 0 || (err > 0 && one_only))
      break;
    _LSLP_SLEEP(100);
  } while (time(&now) && now - wait <= start);
  get_response(client, list);

  // see if we need to do active da discovery
  if(((time(NULL)) - client->_last_da_cycle ) > client->da_active_discovery) {
    SLP_TRACE("Performing Active DA Discovery\n");
    find_das(client, NULL, "DEFAULT") ;
  }
  return err;
}

int tcp_handle_listener(struct slp_client *client, struct slp_net *net)
{
  
  unsigned int bytes_read, ret = FALSE;
  int msg_len;
  struct slp_net *con;

  if (net->sock < 0)
    return ret;

  con = (struct slp_net *)calloc(1, sizeof(struct slp_net));
  if(con == NULL) 
    return 0;
  
  _LSLP_INIT_HEAD(con);
  con->type = NET_TYPE_CONNECT;
  
  con->c._rcv_buf = (char*)calloc(LSLP_DEFAULT_TCP_SIZE + 1, sizeof(char));
  if (con->c._rcv_buf == NULL ) 
    return 0;

  con->c.rcv_data_len = LSLP_DEFAULT_TCP_SIZE;

  do {
    bytes_read = sizeof(union slp_sockaddr);
    con->sock = accept(net->sock, (struct sockaddr *)&con->addr, &bytes_read);
  } while (con->sock == INVALID_SOCKET && (errno == EINTR));

  if ( con->sock == INVALID_SOCKET ) {
    SLP_TRACE("error accepting connection: errno %d\n", errno);
    goto err_out;
  }

  bytes_read = 51000;
  setsockopt(con->sock, SOL_SOCKET, SO_RCVBUF, (const char*)&bytes_read, sizeof(bytes_read));
  bytes_read = 51000;
  setsockopt(con->sock, SOL_SOCKET, SO_SNDBUF, (const char*)&bytes_read, sizeof(bytes_read));

  bytes_read = connectedRead(con->sock, con->c._rcv_buf, LSLP_MIN_HDR);
  if(bytes_read != LSLP_MIN_HDR) {
    SLP_TRACE("error connected read: %i, errno %i\n", bytes_read, errno);
    goto err_out;
  }
  if (FALSE == tcp_message_sanity_check(con->c._rcv_buf))
    goto err_out;
     
  msg_len = _LSLP_GETLENGTH(con->c._rcv_buf);
  SLP_TRACE("tcp msg len: %i\n", msg_len);
  
  if (msg_len > LSLP_DEFAULT_TCP_SIZE ) {
    int8* tmp;
    while(con->c.rcv_data_len  < msg_len && con->c.rcv_data_len < LSLP_MAX_TCP_SIZE) {
      SLP_TRACE("realloc tcp connection rcv buffer for msg of len %i\n", msg_len);
      
      tmp = (int8*)realloc(con->c._rcv_buf, con->c.rcv_data_len + 1024);
      if(tmp) {
	con->c._rcv_buf = tmp;
	con->c.rcv_data_len += 1024;
      } else 
	goto err_out;
    }
  }
      
  bytes_read = connectedRead(con->sock, 
			     con->c._rcv_buf + LSLP_MIN_HDR, 
			     msg_len - LSLP_MIN_HDR);

  if(bytes_read != msg_len - LSLP_MIN_HDR ) 
    goto err_out;

  con->c.flags = LSLP_CONNECTION_RCV_MSG;

  SLP_TRACE("Created a new TCP connection\n");
  decode_msg(client, con);
  ret = TRUE;

 err_out:
  lslpDestroyConnection(con);
  return ret;
}


/* service listeners that have been posted for this client */
int tcp_service_listeners(struct slp_client* client)
{
  fd_set readfds;
  int sel;
  struct timeval tv = {0,0};
  struct slp_net *listen = client->listeners.next;
  while ( ! _LSLP_IS_HEAD(listen) ) {
    if ( listen->sock < 0 || listen->l.type != SOCK_STREAM)
      goto skip;
    
    /* test the socket to see if it can be read */
    FD_ZERO(&readfds) ;
    FD_SET(listen->sock, &readfds); 
    do {
      (sel = _LSLP_SELECT(FD_SETSIZE, &readfds, NULL, NULL, &tv ));
    } while((sel == SOCKET_ERROR ) && ( errno == EINTR));

    if (sel < 0 ) {  
      /* remove and free this listener */
      _LSLP_UNLINK(listen);
      _LSLP_CLOSESOCKET(listen->sock);
      free(listen);
      return 0;
    } 
    else if (sel > 0)
      tcp_handle_listener(client, listen);
skip:
    listen = listen->next;
    if ( ! _LSLP_IS_HEAD( listen ) ) 
      _LSLP_SLEEP(10);
  }
  return 0 ;
}


int tcp_handle_connection(struct slp_client *client, struct slp_net *net)
{


  /* read the slp header to determine the required buffer size */
  int bytes_read;
  int msg_len; 
  char buf[LSLP_MIN_HDR + 1], *tmp;
  memset(buf, 0x00, LSLP_MIN_HDR + 1);
  bytes_read = connectedRead(net->sock, buf, LSLP_MIN_HDR);
  if(bytes_read != LSLP_MIN_HDR) 
    goto err_out;
      
  msg_len = _LSLP_GETLENGTH(buf);
  if (msg_len > LSLP_MAX_TCP_SIZE )
    goto err_out;
    
  tmp = (char*)realloc(net->c._rcv_buf, msg_len+1);
  if (tmp  == NULL )
    goto err_out;
  net->c._rcv_buf = tmp;
  net->c.rcv_data_len = msg_len + 1;
      
  /* copy over the slp header */
  memcpy(net->c._rcv_buf, buf, LSLP_MIN_HDR);

  bytes_read = 51000;
  setsockopt(net->sock, SOL_SOCKET, SO_RCVBUF, (const char*)&bytes_read, sizeof(bytes_read));
  bytes_read = 51000;
  setsockopt(net->sock, SOL_SOCKET, SO_SNDBUF, (const char*)&bytes_read, sizeof(bytes_read));
  bytes_read = connectedRead(net->sock, net->c._rcv_buf + LSLP_MIN_HDR, msg_len - LSLP_MIN_HDR);
  if(bytes_read != msg_len - LSLP_MIN_HDR ) {
    SLP_TRACE("bytes read: %d; expected %d\n", bytes_read, msg_len - LSLP_MIN_HDR);
    goto err_out;
  }

  LSLP_CONNECTION_SET_RCV(net);
  decode_msg(client, net);
  return bytes_read;

 err_out:
  _LSLP_UNLINK(net);
  lslpDestroyConnection(net);
  return 0;
}

/* TCP support */
#ifdef __linux__
#ifndef TCP_NODELAY
#define TCP_NODELAY 1
#endif
#endif


void post_tcp_listeners(struct slp_client* client)
{
  int err = 1; 
  struct slp_net *lis;

  if ((client->ip_version == 6 || client->ip_version == 0) && client->ipv6_tcp == 1) {
    SLP_TRACE("posting tcp/ipv6 listener: client->ipv6_tcp = %i\n", client->ipv6_tcp);
    
    lis = (struct slp_net *)calloc(1, sizeof(struct slp_net));
    if (lis == NULL)
      return;
    lis->type = NET_TYPE_LISTENER;  
    lis->sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (lis->sock < 0) {
      free(lis);
      goto try_ipv4;
    }
    lis->l.type = SOCK_STREAM;
    lis->addr.saddr.sa_family = AF_INET6;
    lis->addr.sin6.sin6_port = htons(client->_target_port);
    inet_pton(AF_INET6, "::", &lis->addr.sin6.sin6_addr);
    setsockopt(lis->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(int));
    setsockopt(lis->sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&err, sizeof(err));
    /* set huge receive buffers */
    err = 51000;
    setsockopt(lis->sock, SOL_SOCKET, SO_RCVBUF, (char *)&err, sizeof(int)) ;
    err = 51000;
    setsockopt(lis->sock, SOL_SOCKET, SO_SNDBUF, (char *)&err, sizeof(int)) ;
    setsockopt(lis->sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&err, sizeof(int)) ;
    if ((err = bind(lis->sock, &lis->addr.saddr, sizeof(lis->addr))) < 0 || 
	(err = listen(lis->sock, SOMAXCONN)) < 0) {
      _LSLP_CLOSESOCKET(lis->sock);
      free(lis);
    } else {
      strcpy(lis->binding, "::");
      strcpy(lis->interface, "IF_ANY");
      _LSLP_INSERT(lis, &(client->listeners));
    }
  }
  
 try_ipv4:
  if(client->ip_version == 4 || client->ip_version == 0) {
    SLP_TRACE("posting tcp/ipv4 listener\n");
    lis = (struct slp_net *)calloc(1, sizeof(struct slp_net));
    if (lis == NULL)
      return;
    lis->type = NET_TYPE_LISTENER;  
    lis->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (lis->sock < 0) {
      free(lis);
      return;
    }

    lis->l.type = SOCK_STREAM;
    lis->addr.saddr.sa_family = AF_INET;

    lis->addr.sin.sin_port = htons(client->_target_port);
    lis->addr.sin.sin_addr.s_addr = INADDR_ANY;
    setsockopt(lis->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(int));
    /* set huge receive buffers */
    err = 51000;
    setsockopt(lis->sock, SOL_SOCKET, SO_RCVBUF, (char *)&err, sizeof(int)) ;
    err = 51000;
    setsockopt(lis->sock, SOL_SOCKET, SO_SNDBUF, (char *)&err, sizeof(int)) ;
    setsockopt(lis->sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&err, sizeof(int)) ;  
    if ((err =  bind(lis->sock, &lis->addr.saddr, sizeof(lis->addr))) < 0 ||
	(err = listen(lis->sock, SOMAXCONN)) < 0 ) {
      _LSLP_CLOSESOCKET(lis->sock);
      free(lis);
    } else {
      strcpy(lis->binding, "0.0.0.0");
      strcpy(lis->interface, "IF_ANY");
      _LSLP_INSERT(lis, &(client->listeners));
    }
  }
}

/* TCP support */
void remove_tcp_listener(struct slp_client* client, SOCKET sock)
{
  struct slp_net *tmp = client->listeners.next;
  while ( ! _LSLP_IS_HEAD(tmp) ) {
    if ( tmp->sock == sock ) {
      SLP_TRACE("removing listening sock %d\n", tmp->sock);
      
      _LSLP_UNLINK( tmp );
      _LSLP_CLOSESOCKET ( tmp->sock );
      free(tmp);
      return;
    }
    tmp = tmp->next;
  }
  return;
}

void post_udp_listeners(struct slp_client *client)
{
  ip6_listen_all_interfaces(client);
  if (client->ip_version == 6 || client->ip_version == 0) {
    ip6_join_leave_group_all(SVRLOC_GRP, client, IPV6_JOIN_GROUP);
    ip6_join_leave_group_all(SVRLOC_DA_GRP, client, IPV6_JOIN_GROUP);
  }
  if (client->ip_version == 4 || client->ip_version == 0) 
    ip4_join_leave_group_all("239.255.255.253", client, 1);
}

void remove_udp_listener(struct slp_client *client, int socket)
{
  remove_tcp_listener(client, socket);
}


/* TODO allow for ipv6 target_addr */
int srv_reg_all( struct slp_client *client,
		 const int8 *url,
		 const int8 *attributes,
		 const int8 *service_type,
		 const int8 *scopes,
		 int16 lifetime)
{
  union slp_sockaddr target_save;
  int convergence_save;
  
  // keep track of how many times we register
  int registrations = 0;
  assert(client != NULL && url != NULL && attributes != NULL && service_type != NULL && scopes != NULL);
  if(client == NULL || url == NULL || attributes == NULL || service_type == NULL || scopes == NULL )
    return(0);

  // save target and convergence parameters 

  if (client->_use_das == TRUE) {
    memcpy(&target_save, &client->target_addr, sizeof(target_save));
    convergence_save = client->_convergence;
    client->_convergence = 0;
    client->target_addr.saddr.sa_family = AF_INET;
    client->target_addr.sin.sin_port = htons(client->_target_port);
    find_das(client, NULL, scopes) ;
    if( ! _LSLP_IS_EMPTY(&(client->das)) ) {
      struct da_list *da = client->das.next;
      while( da && ! _LSLP_IS_HEAD(da) ) {
	SLP_TRACE("Found a directory agent @ %s ", da->remote);
	client->target_addr.sin.sin_addr.s_addr = inet_addr(da->remote);
	if( TRUE == srv_reg(client, url, attributes, service_type, scopes, lifetime)) {
	  registrations++;
	  SLP_TRACE("Registered with directory agent @ %s ", da->remote);
	}
	da = da->next;;
      }
    }
  // restore parameters 
    client->_convergence = convergence_save;
    memcpy(&client->target_addr, &target_save, sizeof(client->target_addr));
  }
/* do a unicast srvreg if we have the target set */
  if(client->target_addr.sin.sin_addr.s_addr != _LSLP_MCAST) {
    if (TRUE == srv_reg( client, url, attributes, service_type, scopes, lifetime)) {
      registrations++;
      SLP_TRACE("Registered with service agent");
    }
	  
  } 
  
  return(registrations);
}

int  srv_reg_local(struct slp_client *client, 
		   const int8 *url,
		   const int8 *attributes,
		   const int8 *service_type,
		   const int8 *scopes,
		   uint16 lifetime)
     
{
  int count = 0;
    
  union slp_sockaddr target_save;
  int convergence_save;

  assert(client != NULL && url != NULL && attributes != NULL && service_type != NULL && scopes != NULL);
  if(client == NULL || url == NULL || attributes == NULL || service_type == NULL || scopes == NULL )
    return 0;

  memcpy(&target_save, &client->target_addr, sizeof(target_save));
  convergence_save = client->_convergence;
  client->_convergence = 0;
  
  if (client->target_addr.saddr.sa_family == AF_INET) {
    client->target_addr.sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    client->target_addr.sin.sin_port = htons(client->_target_port);
  } else if (client->target_addr.saddr.sa_family == AF_INET6) {
    inet_pton(AF_INET6, "::1", &client->target_addr.sin6.sin6_addr);
    client->target_addr.sin6.sin6_port = htons(client->_target_port);
  }
  
  if(TRUE == srv_reg(client, url, attributes, service_type, scopes, lifetime)) {
    SLP_TRACE("registered with local service agent");
    count = 1;
  }
  
  client->_convergence = convergence_save;
  memcpy(&client->target_addr, &target_save, sizeof(client->target_addr));
  return count;
}



void __srv_reg_local ( struct slp_client *client, 
		       const int8 *url,
		       const int8 *attributes, 
		       const int8 *service_type, 
		       const int8 *scopes, 
		       uint16 lifetime)   //jeb int16 to uint16  
{

  int8 *url_copy;
  lslpSrvRegList *reg;
  lslpAttrList *new_attrs;
  struct da_list *das;
  BOOL err;
  
  assert(client != NULL && url != NULL && attributes != NULL && \
	 service_type != NULL && scopes != NULL);
  
  url_copy = strdup(url);
  if(url_copy == NULL)
    abort();

  // <<< Tue Sep 13 16:49:10 2005 mdd >>>
  // fail registrations that have a bad attribute string 
  if( NULL == (new_attrs = _lslpDecodeAttrString((int8 *)attributes)))
    return;
  
  /* update the attributes if an existing registration */
  reg   = client->regs.next;
  while(! _LSLP_IS_HEAD(reg)) { 
    if ( 0 == lslp_string_compare(url_copy, reg->url->url)){
      SLP_TRACE("Refreshing %s with new attributes", url_copy);
	    
      free(url_copy);
      reg->directoryTime = lifetime + time(NULL);
      reg->url->lifetime = lifetime + time(NULL); 
      if(reg->attrList != NULL)
	lslpFreeAttrList(reg->attrList, LSLP_DESTRUCTOR_DYNAMIC); 
      reg->attrList = new_attrs;
      return; 
    }
    reg = reg->next; 
  }
  if (new_attrs)
    lslpFreeAttrList(new_attrs, LSLP_DESTRUCTOR_DYNAMIC);
  
  reg = (lslpSrvRegList *)calloc(1, sizeof(lslpSrvRegList));
  if(reg != NULL){
    int32 len;
    int8 *scope_copy = strdup(scopes);
    if(scope_copy == NULL) { /* <<< Thu Oct 26 13:59:43 2006 mdd >>> */
      free(url_copy);
      return;
    }
    
    if(NULL == (reg->url = lslpAllocURL()))
      abort();
    reg->directoryTime = lifetime + time(NULL);
    //    reg->url->atomized = _lslpDecodeURLs(&url_copy, 1);
    reg->url->atomized = NULL;
    
    reg->url->url = url_copy;
    reg->url->lifetime = lifetime + time(NULL);
    reg->url->auths = 0;
    reg->url->len = strlen(url_copy);
    reg->srvType = strdup(service_type);
    len = strlen(scopes) + 1;
    reg->scopeList  = lslpScopeStringToList(scope_copy, len);
    len = strlen(attributes);
    if(scope_copy)
      free(scope_copy);
    reg->attrList  = _lslpDecodeAttrString((int8 *)attributes);
    ip6_insert_reg(client, reg);
    SLP_TRACE("Registered %s locally ", url);
  }

  das = client->das.next;
  while (!_LSLP_IS_HEAD(das)) {
    err = lslp_forward_reg(client, das, url, attributes, service_type,
			   scopes, lifetime);
    if (err == FALSE) {
      struct da_list *tmp;
      
      SLP_TRACE("error forwarding reg %s to DA %s\n",
		url, das->remote);
      tmp = das;
      das = das->next;
      _LSLP_UNLINK(tmp);
      free_da_list_node(tmp);
      continue;
    }
    das = das->next;
  }
  return;
}

BOOL slp_previous_responder(struct slp_client *client, int8 *pr_list)
{

  int8 *a, *s = NULL;
  struct sockaddr_list *l;
  
  if(pr_list == NULL || 0 == strlen(pr_list)) {
    SLP_TRACE("no pr list\n");
    return(FALSE);
  }
  SLP_TRACE("pr list is: %s\n", pr_list);
  
  a = _LSLP_STRTOK(pr_list, ",", &s);
  while(NULL != a ) {
    SLP_TRACE("checking pr list: %s\n", a);
    l = client->local_addr_list.next;
    while (!_LSLP_IS_HEAD(l)) {
      SLP_TRACE("pr list: %s, me: %s\n", a, l->addr6);
      if (! strcmp(l->addr6, a)) {
	SLP_TRACE("I am on the pr list: %s\n", l->addr6);
	return TRUE;
      }
      
      l = l->next;
    }
    a = _LSLP_STRTOK(NULL, ",", &s);
  }
  return(FALSE);
}

SLP_STORAGE_DECL struct slp_client *create_slp_client(int ip_version,
						      const int8 *target_addr, 
						      const int8 *local_interface, 
						      uint16 target_port, 
						      const int8 *spi, 
						      const int8 *scopes,
						      BOOL should_listen, 
						      BOOL use_das,
						      unsigned long flags)
{
  int32 len;
  struct slp_client *client;
  int8 *scope_copy;
  
  if(spi == NULL || scopes == NULL )
    return NULL;
  
  client = (struct slp_client *)calloc(1, sizeof(struct slp_client)); //jeb
  if(client == NULL)
    return NULL;
  
#ifdef _WIN32
  WindowsStartNetwork();
#endif

  /* initialize the random number generator for randomizing the 
     timing of multicast responses */
  srand(time(NULL));

  client->ip_version = ip_version;
  if (flags & CLIENT_FLAGS_NO_TCP_IPV6) {
    SLP_TRACE("slp client will not post a tcp listener for ipv6\n");
    client->ipv6_tcp = 0;
  } else
    client->ipv6_tcp = 1;
  

  /* LSLP_MTU is the buffer size. The maximum IP datagram size is 65535. 
     The maximum slp datagram size is 65535 - IP header - UDP header. The 
     IP header is variable, but usually 20 bytes. The UDP header is 12 bytes.
     To be safe we assume that the maximum slp message can be: 
     
     65535 - 40 - 12
  */

  if(LSLP_MTU > 65483) {
    LSLP_MTU = 65483;
    SLP_TRACE("adjusted max buffer size to account for IP and UDP headers\n");
  }
  client->get_response = get_response;
  client->find_das = find_das;
  client->discovery_cycle = discovery_cycle;
  client->converge_srv_req = converge_srv_req;
  client->unicast_srv_req = unicast_srv_req;
  client->local_srv_req = local_srv_req;
  client->srv_req = srv_req;
  
  client->converge_attr_req = converge_attr_req;
  client->unicast_attr_req = unicast_attr_req;
  client->local_attr_req = local_attr_req;
  client->attr_req = attr_req;
  client->decode_attr_rply = decode_attr_rply;
  
  client->srv_reg = srv_reg;
  client->srv_reg_all = srv_reg_all;
  client->srv_reg_local = srv_reg_local;
  client->tcp_srv_reg = tcp_srv_reg;
  
  client->post_tcp_listeners = post_tcp_listeners;
  client->post_udp_listeners = post_udp_listeners;
  client->remove_tcp_listener = remove_tcp_listener;
  client->tcp_service_listeners = tcp_service_listeners;
  client->service_listener = service_listener;
  client->prepare_pr_buf = prepare_pr_buf;
  client->prepare_query = prepare_query;
  client->prepare_attr_query = prepare_attr_query;
  client->decode_msg = decode_msg;
  client->decode_srvreq = decode_srvreq;
  client->decode_srvrply = decode_srvrply;
  client->decode_attrreq = decode_attrreq;
  client->tcp_srv_req = tcp_srv_req;
  
  client->decode_daadvert = decode_daadvert;
  client->send_rcv_udp = ip6_send_rcv_udp;
  client->service_listener_wait = service_listener_wait;
  client->slp_previous_responder = slp_previous_responder;

  client->get_version = lslp_get_version;
  client->get_changeset = lslp_get_changeset;

  SLP_TRACE("Allocating buffers, MTU is set to %d\n", LSLP_MTU);
  client->_pr_buf = (int8* )calloc(LSLP_MTU, sizeof(int8));
  client->_msg_buf = (int8* )calloc(LSLP_MTU, sizeof(int8));
  client->_rcv_buf = (int8* )calloc(LSLP_MTU, sizeof(int8));
  client->_scratch = (int8* )calloc(LSLP_SCRATCH_SIZE, sizeof(int8));
  client->_err_buf = (int8* )calloc(255, sizeof(int8));

  client->_buf_len = LSLP_MTU;
  client->_version = 1;
  client->_xid = 1;
  if(!target_port)
    target_port = 427;
  client->_target_port = target_port;
  SLP_TRACE("client port set to %d\n", client->_target_port);
  SLP_TRACE("client ip version: %d\n", client->ip_version);
  
  _LSLP_INIT_HEAD(&client->local_addr_list);
  
  /* only set ip6 default addresses if ip_version is 6 OR 
     target_addr is an ip6 addr.
   */

  if (client->ip_version == 6 || ip6_addr_type(target_addr) == 6)
    ip6_set_client_addrs(client, target_addr, local_interface, target_port);
  else
    ip_set_client_addrs(client, target_addr, local_interface, target_port);
  
  SLP_TRACE("client port (2) set to %d\n", client->_target_port);
  scope_copy = strdup(spi);
  if(scope_copy == NULL){
    free(client);
    return NULL;
  }
  
  len = strlen(scope_copy) + 1;
  client->_spi = lslpScopeStringToList(scope_copy, len);
  free(scope_copy);
  scope_copy = strdup(scopes);
  if(scope_copy == NULL){
    free(client); 
    return NULL;
  }
  len = strlen(scope_copy) + 1;
  client->_scopes = lslpScopeStringToList(scope_copy, len);
  free(scope_copy);
  client->_tv.tv_sec = 0;
  client->_tv.tv_usec = 0;

  client->_use_das = use_das;
  client->da_active_discovery = 300; /* five minutes */
  client->_retries = 3;
  client->_ttl = 255;
  client->_convergence = 5;

  client->das.isHead = TRUE;
  client->das.next = &client->das;
  client->das.prev = &client->das;
  
  client->replies.isHead = TRUE;
  client->replies.next = &client->replies;
  client->replies.prev = &client->replies;
  
  client->regs.isHead = TRUE;
  client->regs.next = (lslpSrvRegList *)&client->regs;
  client->regs.prev = (lslpSrvRegList *)&client->regs;

  client->listeners.isHead = TRUE;
  client->listeners.next = &client->listeners;
  client->listeners.prev = &client->listeners;
  
  client->connections.isHead = TRUE;
  client->connections.next = &client->connections;
  client->connections.prev = &client->connections;

  slp_get_local_interfaces(client);
  

  /* see if we can use a local DA. If not, open up the listen socket */
  if(should_listen == TRUE ) {
    client->post_tcp_listeners(client);
    client->post_udp_listeners(client);
  }
  
  if(client->_use_das == TRUE) {
    local_srv_req(client, NULL, NULL, "DEFAULT"); 
    if( ! _LSLP_IS_EMPTY(&(client->das))) {
      client->_use_das = TRUE;
      client->_da_target_port = htons(client->_target_port);
      client->_da_target_addr = inet_addr("127.0.0.1");
    }
  }

  /* field and record separators */
  client->_use_separators = FALSE;
  client->_fs = '\0';
  client->_rs = '\0';
  SLP_TRACE("client->srv_reg_all set to %p\n", client->srv_reg_all);  
  return client;
}

SLP_STORAGE_DECL void destroy_slp_client(struct slp_client *client)
{
  lslpSrvRegList *reg = NULL;
  struct slp_net *n;
  
  SLP_TRACE("entering destroy_slp_client");
  if(client == NULL)
    return;
	
  lslpFreeScopeList((lslpScopeList *)(client->_spi));
  lslpFreeScopeList(client->_scopes);
  _LSLP_FREE_DEINIT(client->_crypto_context);
	
  /* unlink and free each service registration */
  reg = client->regs.next;
  while (! _LSLP_IS_HEAD(reg)) {
    ip6_remove_reg(client, reg);
    SLP_TRACE("%s unlinked", reg->srvType);
    lslpFreeSrvReg(reg);
    reg = client->regs.next;
  }
  
  n = client->listeners.next;
  while (! _LSLP_IS_HEAD(n) ) {
    _LSLP_UNLINK(n);
    if (n->sock >= 0)
      _LSLP_CLOSESOCKET(n->sock);
    free(n);
    n = client->listeners.next;
  }
	
  n = client->connections.next;
  while(! _LSLP_IS_HEAD(n) ) {
    _LSLP_UNLINK(n);
    lslpDestroyConnection(n);
    n = client->connections.next;
  }
  /* destroy interface list */	
  while (!_LSLP_IS_EMPTY(&client->local_addr_list)) {
    struct sockaddr_list *tmp = client->local_addr_list.next;
    _LSLP_UNLINK(tmp);
    FREE(tmp);
  }
  free(client->_pr_buf);
  free(client->_msg_buf);
  free(client->_rcv_buf);
  free(client->_scratch);
  free(client->_err_buf);
  free(client);
  return;
}

/* returns a length-preceded escaped string compliant with an slp opaque value */
/* length is encoded as two-byte value at the beginning of the string */
/* size of returned buffer will be ((3 * length) + 3 + 2) */
SLP_STORAGE_DECL int8 *encode_opaque(void *buffer, int32 length)
{
  static int8 transcode[16] = 
    { 
      '0', '1', '2', '3', '4', '5', '6', '7', 
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
  
  int8 *bptr, *srcptr, *buf;
  int32 encoded_length;
  
  if(buffer == NULL || length == 0)
    return(NULL);
  
  encoded_length = (length * 3) + 5;
  /* see if the encoded length will overflow the max size of an slp string */
  if( 0xffff0000 & encoded_length )
    return NULL;
  
  buf = malloc(encoded_length);
  if(buf == NULL)
    return NULL;
  bptr = buf;
  
  /* encode the length */
  _LSLP_SETSHORT(bptr, (uint16)encoded_length, 0);
  bptr += 2;
  srcptr = (int8 *)buffer;
  *bptr = 0x5c; *(bptr + 1) = 0x66; *(bptr + 2) = 0x66;
  bptr += 3;
  
  while(length){
    *bptr = 0x5c;
    *(bptr + 1) = transcode[((*srcptr) & 0xf0) >> 4];
    *(bptr + 2) = transcode[(*srcptr) & 0x0f];
    bptr += 3;
    srcptr++;
    length--;
  }
  return buf;
}

/* returns an opaque buffer. size of opaque buffer will be */
/* ((size of escaped opaque string - 2) / 3) - 1  */
SLP_STORAGE_DECL void *decode_opaque(int8 *buffer)
{
  
  int32 encoded_length, alloc_length;
  int8 *bptr, *srcptr, *retptr;

  if(buffer == NULL)
    return NULL;

  srcptr = buffer;
  retptr = bptr = NULL;
  /* get the length */
  encoded_length = _LSLP_GETSHORT(srcptr, 0);
  if(0 == encoded_length || 0 > encoded_length)
    return NULL;
  alloc_length = (encoded_length / 3) - 1;
  
  srcptr += 2;
  encoded_length -= 2; 
  
  /* check the header */
  if( *srcptr == 0x5c ){
    if( (*(srcptr + 1) == 0x46) || (*(srcptr + 1) == 0x66 ) ) {
      if( (*(srcptr + 2) == 0x46) || (*(srcptr + 2) == 0x66 ) ) {
	retptr = (bptr = malloc(alloc_length));
	if(bptr == NULL)
	  return NULL;
	/* adjust the encoded length to reflect that we consumed the header */
	encoded_length -= 3;
	srcptr += 3;
	
	while(encoded_length && alloc_length){
	  int8 accumulator = 0;
	  if(*srcptr == 0x5c){
	    /* encode the high nibble */
	    if( *(srcptr + 1) < 0x3a && *(srcptr + 1) > 0x2f){
	      /* asci decimal char */
	      accumulator = (*(srcptr + 1) - 0x30) * 0x10;
	    }
	    else if( *(srcptr + 1) < 0x47 && *(srcptr + 1) > 0x40){
	      accumulator = (*(srcptr + 1) - 0x37) * 0x10;
	    }
	    else if( *(srcptr + 1) < 0x67 && *(srcptr + 1) > 0x60){
	      accumulator = (*(srcptr + 1) - 0x57) * 0x10;
	    }
	    /* encode the low nibble */
	    if( *(srcptr + 2) < 0x3a && *(srcptr + 2) > 0x2f){
	      /* asci decimal char */
	      accumulator += *(srcptr + 2) - 0x30;
	    } 
	    else if( *(srcptr + 2) < 0x47 && *(srcptr + 2) > 0x40){
	      accumulator += *(srcptr + 2) - 0x37;
	    }
	    else if( *(srcptr + 2) < 0x67 && *(srcptr + 2) > 0x60){
	      accumulator += *(srcptr + 2) - 0x57;
	    }
	    /* store the byte */
	    *bptr = accumulator;
	  }
	  else
	    break;

	  /* update pointers and counters */
	  srcptr += 3;
	  bptr++;
	  encoded_length -= 3;
	  alloc_length--;
	}
	if( alloc_length || encoded_length) {
	  free(retptr);
	  retptr = NULL;
	}
      }
    }
  }
  return (void *)retptr;
}



/* right now we don't support the attr request unless it specifies a complete url. */

void decode_attrreq(struct slp_client *client, struct slp_net *remote_addr)
{
  int8 *bptr, *bptr_orig, *msg_buf, *msg_buf_orig;
  BOOL is_tcp;
  lslpURL *url;
  lslpScopeList *scopes;
  lslpAttrList *attr_tags, *attr_return;
  lslpSrvRegList *regs;
  int16 parse_err;
  int32 total_len, purported_len;  
  int32 str_len, buf_len, err = 0;

  bptr = get_client_rcv_buf(client, remote_addr);
  bptr_orig = bptr;
  
  if (remote_addr->type == NET_TYPE_CONNECT)
    is_tcp = 1;
  else
    is_tcp = 0;
  
  
  /* read the length from the slp header */
  bptr_orig = bptr;
  
  purported_len = _LSLP_GETLENGTH(bptr);
  if(purported_len > LSLP_MTU )
    return;
  
  bptr += (total_len = _LSLP_HDRLEN(bptr));
  
  /* use total_len as a running count of the msg length, ensure it is */
  /* consistent with the purported length to protect against overflow */
  if(total_len < purported_len){
    /* get the length of the PR list and look at it */
    str_len = _LSLP_GETSHORT(bptr, 0);
    if( (str_len + total_len + 2) < purported_len) {
      if(is_tcp == FALSE || FALSE == slp_previous_responder(client, (str_len ? bptr + 2 : NULL))) {
	bptr += str_len + 2;
	total_len += str_len + 2;

	/* decode the url */
	str_len = _LSLP_GETSHORT(bptr, 0);
	bptr += 2;
	
	if( (str_len + total_len + 2) < purported_len ) {
	  if(NULL != (url = lslpAllocURL())) {
	    if(NULL != (url->url = (int8 *)calloc(str_len + 1, sizeof(int8)))) {
	      memcpy(url->url, bptr, str_len);
	      *(bptr + str_len) = 0x00;
	      url->atomized = _lslpDecodeURLs( (int8 **)&(url->url), 1);
	      bptr += str_len + 2;
	      total_len += str_len + 2;
	      buf_len = LSLP_MTU - total_len;
	      str_len = _LSLP_GETSHORT(bptr, 0);
	      scopes = lslpUnstuffScopeList(&bptr, &buf_len, &parse_err);
	      total_len += str_len + 2;
	      /* see if we even support the scopes */
	      if(lslp_scope_intersection(client->_scopes, scopes)) {
		/* decode the attribute tags */
		int8 *bptrSave;
		str_len = _LSLP_GETSHORT(bptr, 0 );
		attr_tags = lslpUnstuffAttr(&bptr, &buf_len, &parse_err);
		total_len += str_len + 2;
		/* see if we have url match */

		regs = client->regs.next;
		attr_return = NULL;
		 
		while (! _LSLP_IS_HEAD(regs) ) {
		  if (TRUE == ip6_escaped_pattern_match(regs->url->url, url->url, FALSE)) {
		    /* we have a hit on the url, see if the scopes intersect */
		    if( TRUE == lslp_scope_intersection(regs->scopeList, scopes)) {
		      attr_return = regs->attrList;
		      err = 0;
		      break;
		    }
		  }
		  regs = regs->next;
		}
		 
		msg_buf = msg_buf_orig = get_client_msg_buf(client, remote_addr);
		
		/* stuff the attribute reply */
		_LSLP_SETVERSION(msg_buf, LSLP_PROTO_VER);
		_LSLP_SETFUNCTION(msg_buf, LSLP_ATTRRPLY);
		// skip the length for now
		_LSLP_SETFLAGS(msg_buf, 0);
		_LSLP_SETNEXTEXT(msg_buf, 0);
		_LSLP_SETXID(msg_buf, ( _LSLP_GETXID(bptr_orig)));
		_LSLP_SETLAN(msg_buf, LSLP_EN_US, LSLP_EN_US_LEN);
		total_len = _LSLP_HDRLEN(msg_buf);

		 
		bptr = msg_buf + total_len;
		_LSLP_SETSHORT(bptr, err, 0);
		bptr += 2;
		total_len += 2;
		if(is_tcp)
		  buf_len = remote_addr->c.msg_data_len - total_len + 1;
		else 
		  buf_len = LSLP_MTU - (total_len + 1);
		bptrSave = bptr;
		 
		/* stuff the attr list */

		if( (FALSE == lslpStuffAttrList(&bptr, &buf_len, attr_return, attr_tags)) &&  
		    ((is_tcp == TRUE && 
		      FALSE == tcp_msg_buffer_check(remote_addr, &bptrSave, 
						    remote_addr->c.msg_data_len + 1024)) ||
		     (is_tcp == FALSE)) ) {
		  /* overflow, set the flag, stuff a zero attr list */
		  _LSLP_SETFLAGS(msg_buf,  LSLP_FLAGS_OVERFLOW);
		  buf_len = LSLP_MTU - total_len;
		  bptr = bptrSave;
		  lslpStuffAttrList(&bptr, &buf_len, NULL, NULL);
		  SLP_TRACE("Sending a truncated attribute reply");
		   
		}
		/* for now don't support auth blocks */

		_LSLP_SETBYTE(bptr, 0, 0);
		 
		/* to calculate the length, re-read the attr list length */
		total_len +=(2 + _LSLP_GETSHORT(bptrSave, 0));
		/* add the length of the auth block (zero plus the number of auths) */
		total_len += 1;
		 
		// ok, now we can set the length
		_LSLP_SETLENGTH(msg_buf, total_len );
		
		/* 		msg_buf is stuffed with the attr reply. now we need  */
		/* 		to allocate a socket and send it back to the requesting node  */

		/* only send the response if there is an attribute or if this is a unicast */ 
		if(is_tcp == TRUE ) {
		  /* write the message to the socket */
		  connectedWrite(remote_addr->sock, remote_addr->c._msg_buf, total_len);
		  free(remote_addr->c._msg_buf);
		  remote_addr->c._msg_buf = NULL;
		} else {
		  if(attr_tags != NULL || ! (_LSLP_GETFLAGS(client->_rcv_buf) & LSLP_FLAGS_MCAST)) {
		    ip6_reply(client, &remote_addr->addr, total_len);
		  } 
		}
		if(attr_tags)
		  lslpFreeAttrList(attr_tags, TRUE);
	      } else { /* scopes intersect */
		/* send a tcp response */
		SLP_TRACE("Attribute request - scopes do not intersect");
		if(is_tcp == TRUE) {
		  memset(remote_addr->c._msg_buf, 0x00, remote_addr->c.msg_data_len);
		  msg_buf = remote_addr->c._msg_buf;
		   
		  _LSLP_SETVERSION(msg_buf, LSLP_PROTO_VER);
		  _LSLP_SETFUNCTION(msg_buf, LSLP_ATTRRPLY);
		  // skip the length for now
		  _LSLP_SETFLAGS(msg_buf, 0);
		  _LSLP_SETNEXTEXT(msg_buf, 0);
		  _LSLP_SETXID(msg_buf, ( _LSLP_GETXID(bptr_orig)));
		  _LSLP_SETLAN(msg_buf, LSLP_EN_US, LSLP_EN_US_LEN);
		  total_len = _LSLP_HDRLEN(msg_buf);
		  bptr = msg_buf + total_len;
		  _LSLP_SETSHORT(bptr, err, 0);
		   
		  /* write the message to the socket */
		  connectedWrite(remote_addr->sock, remote_addr->c._msg_buf, total_len);
		  free(remote_addr->c._msg_buf);
		  remote_addr->c._msg_buf = NULL;
		}
	      }
	      if(scopes)
		lslpFreeScopeList(scopes);
	       
	    } /* url string allocated */
	    if(url)
	      lslpFreeURL(url);
	  } /* url allocated */
	}
      } else { /* not on the pr list */
	SLP_TRACE("Attribute request - I am on the PR list");
      }
      
    }
  }
}


/* folds extra whitespace out of the string - in place */
/* destroys parameter ! */
int8 *lslp_foldString(int8 *s)
{
  int8 *src, *dst;
  assert(s != 0);
  src = s;
  while(isspace(*src))
    src++;
  if(*src == 0x00) {
    /* return a string of exactly one space  */
    *s = 0x20;
    *(s + 1) = 0x00;
    return(s);
  }
  dst = s;
  while(*src != 0x00) {
    *dst = *src;
    dst++;
    src++;
    if(isspace(*src)) {
      *dst = *src;
      dst++; 
      src++;
      while(isspace(*src))
	src++;
    }
  }
  *dst = 0x00;
  if(isspace(*(dst - 1)))
    *(dst - 1) = 0x00;
  return(s);
}



BOOL lslpStuffAttrList(int8 **buf, int32 *len, lslpAttrList *list, lslpAttrList *include)
{
		
  lslpAttrList *attrs, *included;
  int32 attrLen = 0, lenSave;
  int8 *bptr;
  BOOL ccode = FALSE, hit = TRUE;
  assert(buf != NULL);
  assert(len != NULL && *len > 3);
  /*   assert(list != NULL); */
  if (buf == NULL || len == NULL || *len < 3 )
    return(FALSE);
  /* always return TRUE on an empty list so we can continue to build the */
  /* msg buffer - an empty list is not an error! */
  if (list == NULL || _LSLP_IS_EMPTY(list)) {
    _LSLP_SETSHORT((*buf), 0, 0);  
    (*buf) += 2;
    (*len) -= 2;
    return(TRUE);
  }
  /* attr list */
  lenSave = *len;
  attrs = list->next;
  bptr = *buf;
  
  /* <<< Fri May 14 17:13:22 2004 mdd >>> 
     on some platforms memset must be called with a 4-byte aligned address as the buffer */ 
  /*  memset(bptr, 0x00, *len); */
  (*buf) += 2;	 /* reserve space for the attrlist length short */
  (*len) -= 2;
  while (! _LSLP_IS_HEAD(attrs) && attrLen + 1 < *len) {
    assert(attrs->type != head);
    if(include != NULL && _LSLP_IS_HEAD(include) && (! _LSLP_IS_EMPTY(include))) {
      included = include->next;
      hit = FALSE;
      
      while(! _LSLP_IS_HEAD(included)){
	if( ! lslp_string_compare(attrs->name, included->name)){
	  hit = TRUE;
	  break;
	}
	included = included->next;
      }
    }
    
    if(hit == FALSE){
      attrs = attrs->next;
      continue;
    }
    
    if (attrLen + (int32)strlen(attrs->name) + 3 < *len)
      {
	/* << Wed Jun  9 14:07:54 2004 mdd >> properly encode multi-valued attributes */
	if( _LSLP_IS_HEAD(attrs->prev) || lslp_string_compare(attrs->prev->name, attrs->name)) {
	  

	  if (attrs->type != tag) {
	    **buf = '(';  
	    (*buf)++; 
	    attrLen++;
	    (*len)--;
	  }
	  
	  strcpy(*buf, attrs->name);
	  (*buf) += strlen(attrs->name); 
	  attrLen += strlen(attrs->name);
	  (*len) -= strlen(attrs->name);

	
	  if (attrs->type != tag)
	    {
	      **buf = '='; 
	      (*buf)++; 
	      attrLen++;
	      (*len)--;
	    }
	} /* if not a multi-val */ 
	switch (attrs->type)
	  {
	  case tag:
	    ccode = TRUE;
	    
	    break;
	  case string:
	    if (attrLen + (int32)strlen(attrs->val.stringVal) + 2 < *len)
	      {
		strcpy(*buf, (attrs->val.stringVal));
		(*buf) += strlen(attrs->val.stringVal); 
		attrLen +=  strlen(attrs->val.stringVal);
		(*len) -= strlen(attrs->val.stringVal);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case integer:
	    if (attrLen + 33 + 2 < *len)
	      {
		_itoa( attrs->val.intVal, *buf, 10 );
		attrLen += strlen(*buf);
		(*buf) += strlen(*buf);
		(*len) -= strlen(*buf);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case bool_type:
	    if (attrLen + 6 + 2 < *len)
	      {
		if (attrs->val.boolVal)
		  strcpy(*buf, "TRUE");
		else
		  strcpy(*buf, "FALSE");
		attrLen += strlen(*buf);
		(*buf) += strlen(*buf);
		(*len) -= strlen(*buf);
		ccode = TRUE;
	      }
	    else
	      ccode = FALSE;
	    break;
	  case opaque:
	    {
	      int32 opLen;
	      opLen = (_LSLP_GETSHORT(((int8 *)attrs->val.opaqueVal), 0));
	      if (attrLen + opLen + 3 < *len)
		{
		  memcpy(*buf, (((int8 *)attrs->val.opaqueVal) + 2), opLen);
		  (*buf) += opLen;
		  attrLen += opLen;
		  (*len) -= opLen;
		  ccode = TRUE;
		}
	      else
		ccode = FALSE;
	      break;
	    }
	  default:
	    ccode = FALSE;
	    break;
	  }
	if (ccode == TRUE && attrLen + 2 < *len && attrs->type != tag)
	  {
	    /* << Wed Jun  9 14:07:54 2004 mdd >> properly encode multi-valued attributes */
	    if( _LSLP_IS_HEAD(attrs->next) || lslp_string_compare(attrs->next->name, attrs->name  )) {
	      **buf = ')'; 
	      (*buf)++; 
	      attrLen++;
	      (*len)--;
	    }
	  }
	if (ccode == TRUE && ! _LSLP_IS_HEAD(attrs->next) && attrLen + 1 < *len)
	  {
	    **buf = ','; 
	    (*buf)++; 
	    attrLen++;
	    (*len)--;
	  }
      }  /* if room for the attr name */
    else
      {
	ccode = FALSE;
	break;
      }
    attrs = attrs->next;
  } /* while we are traversing the attr list */

  /* check for a trailing comma, which may end up in the buffer if we are */
  /* selecting attributes based upon a tag list */
  if(*buf && *(*buf - 1) == ',') {
    *(*buf - 1) = 0x00;
    attrLen -= 1;
    
  }
  

  /* set the length short */
  if (ccode == TRUE)
    {
      _LSLP_SETSHORT(bptr, attrLen, 0);
    }
  else
    {
      (*buf) = bptr; 
      (*len) = lenSave;
      memset(*buf, 0x00, *len);
    }
  return(ccode);
}	


lslpAttrList *lslpUnstuffAttr(int8 **buf, int32 *len, int16 *err)  
{
  int32 tempLen;
  lslpAttrList *temp = NULL;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL);
  assert(err != NULL);
  *err = 0;
  tempLen = _LSLP_GETSHORT(*buf, 0);
  if(tempLen > 0) {
    (*buf) += sizeof(int16);
    (*len) -= sizeof(int16);
    if(*len >= tempLen) {
      if(NULL != (temp = _lslpDecodeAttrString(*buf))) {
	(*buf) += tempLen;
	(*len) -= tempLen;
      } else {*err = LSLP_PARSE_ERROR; }
    } else {*err = LSLP_INTERNAL_ERROR; }
  }
  return(temp);
}



lslpURL *lslpAllocURL(void)
{
  lslpURL *url = (lslpURL *)calloc(1, sizeof(lslpURL));

  return(url);
}	

lslpURL *lslpAllocURLList(void)
{
  lslpURL *head;
  if (NULL != (head = lslpAllocURL()))
    {
      head->next = head->prev = head;
      head->isHead = TRUE;
      return(head);
    }
  return(NULL);	
}	

/* url MUST be unlinked from list */
void lslpFreeURL(lslpURL *url)
{
  assert(url != NULL);
  if (url->url != NULL)
    free(url->url);
  if ( url->authBlocks != NULL)
    lslpFreeAuthList(url->authBlocks);
  if(url->atomized != NULL)
    lslpFreeAtomizedURLList(url->atomized, TRUE);
  if(url->attrs)
    lslpFreeAtomList(url->attrs, LSLP_DESTRUCTOR_DYNAMIC);
  free(url);
  return;
}	

void lslpFreeURLList(lslpURL *list)
{								
  lslpURL *temp;

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  while(! (_LSLP_IS_EMPTY(list)))
    {
      temp = list->next;
      _LSLP_UNLINK(temp);
      lslpFreeURL(temp);
    }
  lslpFreeURL(list);
  return;
}	


/* for protected scopes, we need to change stuff URL so that it */
/* only stuffs url auth blocks that match spis in the srv req */


/* this routine may return FALSE AND alter the buffer and length */
BOOL  lslpStuffURL(int8 **buf, int32 *len, lslpURL *url) 
{
  assert((buf != NULL) && (*buf != NULL));
  assert((len != NULL) && (*len > 8));
  assert((url != NULL) && (! _LSLP_IS_HEAD(url)));
  if(_LSLP_IS_HEAD(url))
    return(FALSE);
  memset(*buf, 0x00, *len);
  /* advanced past the reserved portion */
  (*buf) += sizeof(int8);
  _LSLP_SETSHORT(*buf, url->lifetime - time(NULL), 0);
  (*buf) += sizeof(int16);
  url->len = strlen(url->url);
  /* url->url is a null terminated string, but we only stuff the non-null bytes */
  _LSLP_SETSHORT(*buf, url->len, 0);

  (*buf) += sizeof(int16);
  (*len) -= 5;
  if(*len < url->len - 1)
    return(FALSE);
  memcpy(*buf, url->url, url->len);
  (*buf) += url->len;
  (*len) -= url->len;
  return(lslpStuffAuthList(buf, len, url->authBlocks));
}

/* this routine may  return FALSE AND alter the buffer and length */
BOOL lslpStuffURLList(int8 **buf, int32 *len, lslpURL *list) 
{
  BOOL ccode = TRUE;
  assert((buf != NULL) && (*buf != NULL));
  assert((len != NULL) && (*len > 8));
  assert((list != NULL) && (_LSLP_IS_HEAD(list)));
  if(! _LSLP_IS_HEAD(list))
    return(FALSE);
  while((ccode == TRUE) && (! _LSLP_IS_HEAD(list->next))) {
    list = list->next;
    ccode = lslpStuffURL(buf, len, list);
  }
  return(ccode);
}

lslpURL *lslpUnstuffURL(int8 **buf, int32 *len, int16 *err) 
{

  int8 *tempurl;
  lslpURL *temp;

  SLP_TRACE("lslpUnstuffURL\n");
  
  if(buf == NULL || *buf == NULL)
    return NULL;
  if(len == NULL || *len <= 8 )
    return NULL;
  if(err == NULL)
    return NULL;
  
  *err = 0;
  if(NULL != (temp = lslpAllocURL())) {
    temp->lifetime = _LSLP_GETSHORT((*buf), 1);
    temp->len = _LSLP_GETSHORT((*buf), 3);
    (*buf) += 5;
    (*len) -= 5;
    if(*len >= temp->len) { 
      if (NULL != (temp->url = (int8 *)calloc(1, temp->len + 1))) {
	memcpy(temp->url, *buf, temp->len);
	*((temp->url) + temp->len) = 0x00;
	tempurl = temp->url;
	temp->atomized = _lslpDecodeURLs((int8 **)&tempurl, 1);
	SLP_TRACE("atomized url: %p\n", temp->atomized);
	(*buf) += temp->len;
	(*len) -= temp->len;
	if(temp->atomized != NULL)
	  temp->authBlocks = lslpUnstuffAuthList(buf, len, err);
      } else {*err = LSLP_INTERNAL_ERROR;}
    } else {*err = LSLP_PARSE_ERROR; }
  } else {*err = LSLP_INTERNAL_ERROR;}
  if(*err != 0 && temp != NULL) {
    lslpFreeURL(temp);
    temp = NULL;
  }
  return(temp);
}


void lslpFreeFilterTree(lslpLDAPFilter *root)
{
  assert(root != NULL);
  if( ! _LSLP_IS_EMPTY( &(root->children) ) ) {
    lslpFreeFilterTree((lslpLDAPFilter *)root->children.next);
  }
  if( ! (_LSLP_IS_HEAD(root->next)) && (! _LSLP_IS_EMPTY(root->next)) )
    lslpFreeFilterTree(root->next);
  
  if(root->attrs.next != NULL) {
    while(! (_LSLP_IS_EMPTY(&(root->attrs)))) {
      lslpAttrList *attrs = root->attrs.next;
      _LSLP_UNLINK(attrs);
      lslpFreeAttr(attrs);
    }
  }
}


BOOL lslpEvaluateOperation(int compare_result, int operation)
{
  switch(operation) {
  case expr_eq:
    if(compare_result == 0)	/*  a == b */
      return(TRUE);
    break;
  case expr_gt:
    if(compare_result >= 0)	/*  a >= b  */
      return(TRUE);
    break;

  case expr_lt:			/* a <= b  */
    if(compare_result <= 0)
      return(TRUE);
    break; 
  case expr_present:
  case expr_approx:
  default:
    return(TRUE);
    break;
  }
  return(FALSE);
}

/* evaluates attr values, not names */
BOOL lslpEvaluateAttributes(const lslpAttrList *filter, const lslpAttrList *registered, int op)
{
  assert(filter != NULL && registered != NULL && (! _LSLP_IS_HEAD(filter)) && (! _LSLP_IS_HEAD(registered))) ;
  /* first ensure they are the same type  */
  switch(filter->type) {
  case opaque:
  case string:
    if(registered->type != string && registered->type != opaque)
      return(FALSE);
    
    if( registered->type == opaque || filter->type == opaque) {
      return(lslpEvaluateOperation(memcmp(registered->val.stringVal, 
					  filter->val.stringVal, 
					  _LSLP_MIN(registered->attr_len, 
						    strlen(filter->val.stringVal))), op));
    } else {
      if( TRUE == lslp_pattern_match(registered->val.stringVal, 
				     filter->val.stringVal, 
				     FALSE)) {
	return( lslpEvaluateOperation(0, op)) ;
      } else {
	return (lslpEvaluateOperation(1, op));
      }
    }
    break;
  case integer:
    return( lslpEvaluateOperation( filter->val.intVal - registered->val.intVal, op));
  case tag:			/* equivalent to a presence test  */
    return(TRUE);
  case bool_type:
    if((filter->val.boolVal != 0) && (registered->val.boolVal != 0))
      return(TRUE);
    if((filter->val.boolVal == 0) && (registered->val.boolVal == 0))
      return(TRUE);
    break;
  default: 
    break;
  }
  return(FALSE);
}

/* filter is a filter tree, attrs is ptr to an attr listhead */

BOOL lslpEvaluateFilterTree(lslpLDAPFilter *filter, const lslpAttrList *attrs)
{
  assert(filter != NULL);
  assert(attrs != NULL);
  assert(! _LSLP_IS_HEAD(filter));
  if(filter == NULL || (_LSLP_IS_HEAD(filter)) || attrs == NULL)
    return FALSE;
  
  if(! _LSLP_IS_HEAD(filter->children.next) ) {
    lslpEvaluateFilterTree((lslpLDAPFilter *)filter->children.next, attrs);
  }
  if( ! (_LSLP_IS_HEAD(filter->next)) && (! _LSLP_IS_EMPTY(filter->next)) ) {
    lslpEvaluateFilterTree(filter->next, attrs);
  }
  if(filter->ldap_operator == ldap_and || filter->ldap_operator == ldap_or || filter->ldap_operator == ldap_not) {
    /* evaluate ldap logical operators by evaluating filter->children as a list of filters */
    lslpLDAPFilter *child_list = (lslpLDAPFilter *)filter->children.next;
    /* initialize  the filter's logical value to TRUE */
    if(filter->ldap_operator == ldap_or)
      filter->logical_value = FALSE;
    else
      filter->logical_value = TRUE;
    while(! _LSLP_IS_HEAD(child_list)) {
      if(child_list->logical_value == TRUE)  {
	if(filter->ldap_operator == ldap_or) {
	  filter->logical_value = TRUE;
	  break;
	}
	if(filter->ldap_operator == ldap_not) {
	  filter->logical_value = FALSE;
	  break;
	}
	/* for an & operator keep going  */
      } else {
	/* child is FALSE */
	if(filter->ldap_operator == ldap_and) {
	  filter->logical_value = FALSE;
	  break;
	}
      }
      child_list = child_list->next;
    }
  } else  {
    /* find the first matching attribute and set the logical value */
    filter->logical_value = FALSE;
    if(! _LSLP_IS_HEAD(filter->attrs.next) ) {

      /* need to continue to search until the attr list end to support multi-valued attributes */

      attrs = attrs->next;
      while(! _LSLP_IS_HEAD(attrs)) {
	/* advance to a matching attribute name if it exists */
	while( (! _LSLP_IS_HEAD(attrs )) && 
	       ( FALSE  == lslp_pattern_match(filter->attrs.next->name, attrs->name, FALSE)) ) {
	  attrs = attrs->next ; 
	}
	/* either we have traversed the list or found the first matching attribute */
	if( ! _LSLP_IS_HEAD(attrs) ) {
	  /* we found the first matching attribute, now do the comparison */
	  if (filter->ldap_operator == expr_present || filter->ldap_operator == expr_approx) 
	    filter->logical_value = TRUE;
	  else
	    filter->logical_value = lslpEvaluateAttributes(filter->attrs.next, attrs, filter->ldap_operator );
	}
	else { break; } /* did not find a matching attribute */
	
	/* if we found a match, exit the comparison loop */
	if(filter->logical_value == TRUE)
	  break;
	/* no match, continue searching attributes. */
	attrs = attrs->next;
      }
    }
  }
  return(filter->logical_value);
}

lslpScopeList *lslpScopeStringToList(int8 *s, int32 len) 
{
  int8 *p, *saveP, *t;
  lslpScopeList *h, *temp;
  if(s == NULL)
    return(lslpAllocScopeList());
  if(NULL != (h  = lslpAllocScopeList())) {
    saveP = (p = (int8 *)malloc(len + 1));
    if( p == NULL)
      return NULL;
    memcpy(p, s, len);
    *(p + len) = 0x00;
    
    while(NULL != (t = strchr(p, ','))) {
      *t = 0x00;
      t++;
      p = lslp_foldString(p);
      if((lslp_islegal_scope(p)) && (NULL != (temp = lslpAllocScope()))) {
	if(NULL != (temp->scope = strdup(p))) {
	  _LSLP_INSERT(temp, h);
	}
      }
      p = t;
    }
    p = lslp_foldString(p);
    if((lslp_islegal_scope(p)) && (NULL != (temp = lslpAllocScope()))) {
      if(NULL != (temp->scope = strdup(p))) {
	_LSLP_INSERT(temp, h);
      } 
    }
    free(saveP);
  }
  return(h);
}

BOOL lslpStuffScopeList(int8 **buf, int32 *len, lslpScopeList *list)
{
  lslpScopeList *scopes;
  int32 scopeLen = 0, lenSave;
  int8 *bptr;
  BOOL ccode = FALSE;
  assert(buf != NULL);
  assert(len != NULL && *len > 3);
  assert(list != NULL);
  if (buf == NULL || len == NULL || *len < 3 || list == NULL)
    return(FALSE);
  /* always return TRUE on an empty list so we can continue to build the */
  /* msg buffer - an empty list is not an error! */
  if (_LSLP_IS_EMPTY(list))
    return(TRUE);
  /* scope list */
  lenSave = *len;
  scopes = list->next;
  bptr = *buf;
  memset(*buf, 0x00, *len);
  (*buf) += 2;	 /* reserve space for the scopelist length short */
  (*len) -= 2;
  while (! _LSLP_IS_HEAD(scopes) && scopeLen + 1 < *len)
    { 
      if (scopeLen + (int32)strlen(scopes->scope) < *len)
	{
	  ccode = TRUE;
	  strcpy(*buf, scopes->scope);
	  (*buf) += strlen(scopes->scope); 
	  scopeLen += strlen(scopes->scope) ;
	  if (! _LSLP_IS_HEAD(scopes->next) )
	    {
	      **buf = ','; 
	      (*buf)++; 
	      scopeLen++;
	    }
	}  /* if room for the attr name */
      else
	{
	  ccode = FALSE;
	  break;
	}
      scopes = scopes->next;
    } /* while we are traversing the attr list */
  /* set the length short */
  if (ccode == TRUE)
    {
      //      **buf = 0x00;
      //      (*buf)++;
      //      scopeLen++;
      //      << Thu May 30 16:18:57 2002 mdd >>
      (*len) -= scopeLen;
      _LSLP_SETSHORT(bptr, scopeLen, 0);
    }
  else
    {
      *len = lenSave;
      *buf = bptr;
      memset(*buf, 0x00, *len);
    }
  return(ccode);
	
}	


lslpScopeList *lslpUnstuffScopeList(int8 **buf, int32 *len, int16 *err)
{
  int32 tempLen;
  lslpScopeList *temp = NULL;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL);
  assert(err != NULL);
  *err = 0;
  tempLen = _LSLP_GETSHORT(*buf, 0);
  (*buf += 2);
  (*len -= 2);  
  if(tempLen != 0) {
    if (tempLen <= *len ) {
      if(NULL != (temp = lslpScopeStringToList(*buf, tempLen))) {
	(*buf) += tempLen;
	(*len) -= tempLen;
      } else{*err = LSLP_INTERNAL_ERROR;}
    } else {*err = LSLP_PARSE_ERROR;}
  }
  return(temp);
}


lslpSPIList *lslpUnstuffSPIList(int8 **buf, int32 *len, int16 *err)
{
  return((lslpSPIList *)lslpUnstuffScopeList(buf, len, err));
}


BOOL lslpStuffSPIList(int8 **buf, int32 *len, lslpSPIList *list)
{
  return(lslpStuffScopeList(buf, len, (lslpScopeList *)list));
}


BOOL lslp_scope_intersection(lslpScopeList *a, lslpScopeList *b) 
{
  if((a == NULL) || (b == NULL))
    return(TRUE);

  assert(_LSLP_IS_HEAD(a));
  assert(_LSLP_IS_HEAD(b));
  if((_LSLP_IS_EMPTY(a)) || (_LSLP_IS_EMPTY(b))) {
    return(FALSE);
  }
  while(! (_LSLP_IS_HEAD(a->next))) {
    a = a->next;
    while(!(_LSLP_IS_HEAD(b->next))) {
      b = b->next;
      assert((a->scope != NULL) && (b->scope != NULL));
      if(! strcasecmp(a->scope, b->scope)) {
	return(TRUE);
      }
    }
    /* reset b */
    b = b->next;
  }
  return(FALSE);
}

/* caseless compare that folds whitespace */
int lslp_string_compare(int8 *s1, int8 *s2)
{
  assert(s1 != NULL);
  assert(s2 != NULL);
  lslp_foldString(s1);
  lslp_foldString(s2);
  if( TRUE == ip6_escaped_pattern_match(s1, s2, FALSE))
    return 0;
  return -1;
}

/* return 1 if char is legal for scope value, 0 otherwise */
int lslp_isscope(int c) 
{  
  int i;
  static int8 scope_reserved[] = { '(', ')', ',', 0x5c, '!', '<', '=', '>', '~', ';', '*', '+', 0x7f };
  
  if(! _LSLP_ISASCII(c))
    return 0;
  
  if(c < 0x20)
    return(0);
  for(i = 0; i < 13; i++) {
    if(c == scope_reserved[i])
      return(0);
  }
  return(1);
}

BOOL lslp_islegal_scope(int8 *s)
{
  int8 *temp;
  assert(s != NULL);
  if(! strlen(s)) {
    return(FALSE);
  }
  temp = s;
  while(*temp != 0x00) {
    if(! lslp_isscope(*temp)) {
      return(FALSE);
    }
    temp++;
  }
  return(TRUE);
}

BOOL lslp_srvtype_match(const int8 *s, int8 *r)
{
  int8 *sp;
  assert(s != NULL);
  assert(r != NULL);
  /* check to see if search is for an abstract service type */
  sp = (int8 *)r;
  while(*sp != 0x00) { sp++; }
  /* if this is a search for an abstract service type, use a wildcard */
  /* to collect all concrete registrations under this type */
  if(*(sp - 1) == ':') {*(sp - 1) = '*'; }
  /* check for a dangling semicolon and kill it  */
  else if ( *(sp - 1) == ';') { *(sp - 1) = 0x00 ; }

  return(lslp_pattern_match(s, r, FALSE));
}


lslpScopeList *lslpAllocScope(void)
{
  return((lslpScopeList *)calloc(1, sizeof(lslpScopeList)));
}

lslpScopeList *lslpAllocScopeList(void)
{
  lslpScopeList *head;
  if (NULL != (head = lslpAllocScope()))
    {
      head->next = head->prev = head;
      head->isHead = TRUE;	
    }
  return(head);
}	

void lslpFreeScope(lslpScopeList *s)
{
  assert(s != NULL);
  if (s->scope != NULL)
    free(s->scope);
  free(s);
}	

void lslpFreeScopeList(lslpScopeList *head)
{
  lslpScopeList *temp;
  assert(head != NULL);
  assert(_LSLP_IS_HEAD(head));
  while (! (_LSLP_IS_EMPTY(head)))
    {
      temp = head->next;
      _LSLP_UNLINK(temp);
      lslpFreeScope(temp);
    }
  lslpFreeScope(head);
  return;
}	



lslpSrvRegList *lslpAllocSrvReg(void)
{
  lslpSrvRegList *l;
  if (NULL != (l = (lslpSrvRegList *)calloc(1, sizeof(lslpSrvRegList)))) {
    if(NULL != (l->url = lslpAllocURL())) {
      if ( NULL != (l->scopeList = lslpAllocScopeList())) {
	if ( NULL != (l->attrList = lslpAllocAttrList())) {
	  if (NULL != (l->authList = lslpAllocAuthList())) {
	    return(l);
	  }
	  lslpFreeAttrList(l->attrList, TRUE);     
	}
	lslpFreeScopeList(l->scopeList);
      }
      lslpFreeURL(l->url); 
    }
    free(l);
  }
  return(NULL);
}	

lslpSrvRegHead *lslpAllocSrvRegList(void)
{
  lslpSrvRegHead *head;
  if (NULL != (head = (lslpSrvRegHead *)calloc(1, sizeof(lslpSrvRegHead)))) {
    head->next = head->prev = (struct lslp_srv_reg_instance *)head;
    head->isHead = TRUE;
    return(head);
  }
  return(NULL);
}	

void lslpFreeSrvReg(lslpSrvRegList *s)
{

  SLP_TRACE("free srv reg for %s\n", s->url->url);

  assert(s != NULL);
  if(s->url != NULL)
    lslpFreeURL(s->url);
  if (s->srvType != NULL)
    free(s->srvType);
  if (s->scopeList != NULL)
    lslpFreeScopeList(s->scopeList);
  if (s->attrList != NULL)
    lslpFreeAttrList(s->attrList, TRUE);
  if(s->authList != NULL)
    lslpFreeAuthList(s->authList);
  free(s);
}	

void lslpFreeSrvRegList(lslpSrvRegHead *head)
{
  lslpSrvRegList *temp;
  assert(head != NULL);
  assert(_LSLP_IS_HEAD(head));
  while (!  (_LSLP_IS_EMPTY((struct lslp_srv_reg_instance *)head)))
    {
      temp = head->next;
      _LSLP_UNLINK(temp);
      lslpFreeSrvReg(temp);
    }
  free(head);
}	


lslpAuthBlock *lslpAllocAuthBlock(void)
{
  return((lslpAuthBlock *)calloc(1, sizeof(lslpAuthBlock)));
}	

lslpAuthBlock *lslpAllocAuthList(void)
{
  lslpAuthBlock *head;
  if (NULL != (head = lslpAllocAuthBlock()))
    {
      head->next = head->prev = head;
      head->isHead = TRUE;
      return(head);
    }
  return(NULL);
}

	
/* NOTE: be CERTAIN block is not linked to a list !!! */
void lslpFreeAuthBlock(lslpAuthBlock *auth)
{
  if (auth->spi != NULL)
    free(auth->spi);
  if (auth->block != NULL)
    free(auth->block); 
  free(auth);
  return;		
}	

void lslpFreeAuthList(lslpAuthBlock *list)
{								
  lslpAuthBlock *temp;

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  while(! (_LSLP_IS_EMPTY(list)))
    {
      temp = list->next;
      _LSLP_UNLINK(temp);
      lslpFreeAuthBlock(temp);
    }
  lslpFreeAuthBlock(list);
  return;
}	

/* this routine will return FALSE AND alter the buffer and length */
BOOL lslpStuffAuthList(int8 **buf, int32 *len, lslpAuthBlock *list)
{
  int8 *bptr;
  int16 numAuths = 0;
  lslpAuthBlock *auths;
  BOOL ccode = TRUE;
  assert(buf != NULL);
  assert(len != NULL);
  /*   assert(list != NULL); */
  if (buf == NULL || *buf == NULL || len == NULL )
    return(FALSE);
  /* always return true on an empty list so we can continue to build the */
  /* msg buffer - an empty list is not an error! */
  bptr = *buf;
  _LSLP_SETBYTE(bptr, numAuths, 0);

  (*buf)+= 1;
  (*len) -= 1;
  if(list == NULL || _LSLP_IS_EMPTY(list))
    return(TRUE);
  /* auth list */
  auths  = list->next;
  memset(*buf, 0x00, *len);
  while (! _LSLP_IS_HEAD(auths) && auths->len <= *len)
    {
      /* store the auth bsd */ 
      _LSLP_SETSHORT(*buf, auths->descriptor, 0);
      /* store the the bsd size */
      _LSLP_SETSHORT(*buf, auths->len, 2);
      /* store the timestamp */
      _LSLP_SETLONG(*buf, auths->timestamp, 4);
      /* store spi string length */
      _LSLP_SETSHORT(*buf, auths->spiLen, 8);
      assert(strlen(auths->spi) + 1 == auths->spiLen );
      /* store the spi string */
      strcpy((*buf) + 10, auths->spi);
      /* store the authentication block */
      if (auths->block != NULL && (auths->len - (auths->spiLen + 10) ) > 0 )
	memcpy(((*buf) + 10 + auths->spiLen), auths->block, (auths->len - (auths->spiLen + 10)));
      (*buf) += auths->len; 
      (*len) -= auths->len;
      numAuths++;
      auths = auths->next;
    } /* while we are traversing the attr list */
  if (! _LSLP_IS_HEAD(auths))
    {
      /* we terminated the loop before copying all the auth blocks */
      ccode = FALSE;
    }
  else
    {
      _LSLP_SETBYTE(bptr, numAuths, 0);
    }
  return(ccode);
}	

lslpAuthBlock *lslpUnstuffAuthList(int8 **buf, int32 *len, int16 *err)
{
  int32 tempLen;
  lslpAuthBlock *list = NULL, *temp;
  assert(buf != NULL && *buf != NULL);
  assert(len != NULL);
  assert(err != NULL);
  *err = 0;
  tempLen = _LSLP_GETBYTE(*buf, 0);        /* get the number of auth-blocks */
  (*buf) += 1;                                /* advance to the auth blocks */
  (*len) -= 1;
  if((tempLen == 0) || (NULL != (list = lslpAllocAuthList()))) {
    if(list != NULL) {
      while((tempLen > 0) && (*len > 10) && (*err == 0)) {
	if(NULL != (temp = lslpAllocAuthBlock())) {
	  temp->descriptor = _LSLP_GETSHORT(*buf, 0); /* get the block structure descriptor */
	  (*buf) += 2;                                  /* advance to the block length */
	  temp->len = _LSLP_GETSHORT(*buf, 0);        /* get the block length */
	  (*buf) += 2;                                  /* advance to the timestamp */
	  temp->timestamp = _LSLP_GETLONG(*buf, 0);   /* get the timestamp */
	  (*buf) += 4;                                  /* advance to the spi length */
	  temp->spiLen = _LSLP_GETSHORT(*buf, 0);     /* get the spi length */
	  (*buf) += 2;                                  /* advance to the spi */ 
	  *len -= 10;
	  if(*len >= (temp->spiLen)) {
	    if(NULL != (temp->spi = (char *)calloc(temp->spiLen + 1, sizeof(uint8)))) {
	      memcpy(temp->spi, *buf, temp->spiLen);  /* copy the spi */
	      (*buf) += temp->spiLen;                   /* advance to the next block */
	      (*len) -= temp->spiLen;
	      if(*len >= (temp->len - (10 + temp->spiLen))) {
		if (NULL != (temp->block = 
			     (char *)calloc((temp->len - (10 + temp->spiLen)) + 1, 
					     sizeof(uint8)))) {
		  memcpy(temp->block, *buf, (temp->len - (10 + temp->spiLen)) );
		  _LSLP_INSERT(temp, list);                /* insert the auth block into the list */ 
		  (*buf) += (temp->len - (10 + temp->spiLen));
		  (*len) -= (temp->len - (10 + temp->spiLen));
		  temp = NULL;
		} else {*err = LSLP_INTERNAL_ERROR; } /* if we alloced the auth block buffer */
	      } else { *err = LSLP_PARSE_ERROR;}
	    } else { *err = LSLP_INTERNAL_ERROR ;}/* if we alloced the spi buffer */
	  } else { *err = LSLP_PARSE_ERROR;} 
	}else { *err = LSLP_INTERNAL_ERROR; } /* if we alloced the auth block */
	tempLen--; /* decrement the number of auth blocks */
      } /* while there is room and there are auth blocks to process */
    } else {
      if(tempLen == 0)
	*err = 0;
      else
	*err = LSLP_INTERNAL_ERROR ; 
    }
  }
  if(*err != 0 && list != NULL) {
    lslpFreeAuthList(list);
    list = NULL;
  }
  return(list);
}


uint32 lslpCheckSum(int8 *s, int16 l)
{
  uint32 c;
  BOOL pad = 0;
  uint16 a = 0, b = 0, *p;
  assert(s != NULL);
  if (l == 1)
    return(0);
  assert(l > 1);
  if (l & 0x01)
    pad = TRUE;
  p = (uint16 *)s;
  while (l > 1)
    {
      a += *p++;
      b += a;
      l -= 2;
    }
  /* "pad" the string with a zero word */
  if (pad == TRUE)
    b += a;
  /* return the value as a dword with containing two shorts in */
  /* network order -- ab */
  _LSLP_SETSHORT((uint8 *)&c, a, 0 );
  _LSLP_SETSHORT((uint8 *)&c, a, 2 );
  return(c);
}	


lslpHdr * lslpAllocHdr(void)
{
  return((lslpHdr *)calloc(1, sizeof(lslpHdr)));
}	

void lslpFreeHdr(lslpHdr *hdr)
{
  assert(hdr != NULL);
  if (hdr->data != NULL)
    free(hdr->data);
  free(hdr);
}	

void lslpDestroySrvReq(struct lslp_srv_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->srvcType != NULL)
    free(r->srvcType);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (r->predicate != NULL)
    free(r->predicate);
  if(r->spiList != NULL)
    lslpFreeSPIList(r->spiList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvRply(struct lslp_srv_rply *r, int8 flag) 
{
  assert(r != NULL);
  if (r->urlList != NULL)
    lslpFreeURLList(r->urlList);
  if(r->attr_list != NULL)
    lslpFreeAtomList(r->attr_list, LSLP_DESTRUCTOR_DYNAMIC);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvReg(struct lslp_srv_reg *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    lslpFreeURLList(r->url);
  if (r->srvType != NULL)
    free(r->srvType);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (r->attrList != NULL)
    lslpFreeAttrList(r->attrList, TRUE);
  if (r->attrAuthList != NULL)
    lslpFreeAuthList(r->attrAuthList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvAck(struct lslp_srv_ack *r, int8 flag)
{
  assert(r != NULL);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroyDAAdvert(struct lslp_da_advert *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->spiList != NULL)
    free(r->spiList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySAAdvert(struct lslp_sa_advert *r, int8 flag)
{
  assert(r != NULL);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
}	

void lslpDestroySrvTypeReq(struct lslp_srvtype_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->nameAuth != NULL)
    free(r->nameAuth);
  if (r->scopeList != NULL)
    lslpFreeScopeList(r->scopeList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;
}	

void lslpDestroySrvTypeReply(struct lslp_srvtype_rep *r, int8 flag)
{
  assert(r != NULL);
  if (r->srvTypeList != NULL)
    free(r->srvTypeList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	

void lslpDestroyAttrReq(struct lslp_attr_req *r, int8 flag)
{
  assert(r != NULL);
  if (r->prList != NULL)
    free(r->prList);
  if (r->url != NULL)
    free(r->url);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->tagList != NULL)
    free(r->tagList);
  if (r->spiList != NULL)
    lslpFreeSPIList(r->spiList);			
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	

void lslpDestroyAttrReply(struct lslp_attr_rep *r, int8 flag)
{
  assert(r != NULL);
  if (r->attrList != NULL)
    free(r->attrList);
  if (r->authBlocks != NULL)
    lslpFreeAuthList(r->authBlocks);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
	
}	

void lslpDestroySrvDeReg(struct lslp_srv_dereg *r, int8 flag)
{
  assert(r != NULL);
  if (r->scopeList != NULL)
    free(r->scopeList);
  if (r->urlList != NULL)
    lslpFreeURLList(r->urlList);
  if (r->tagList != NULL)
    free(r->tagList);
  if (flag == LSLP_DESTRUCTOR_DYNAMIC)
    free(r);
  return;	
}	


SLP_STORAGE_DECL lslpMsg *alloc_slp_msg(BOOL head)
{
  lslpMsg *ret = (lslpMsg *)calloc(1, sizeof(lslpMsg));
  if(ret != NULL ) {
    ret->dynamic = LSLP_DESTRUCTOR_DYNAMIC;
    if( head == TRUE ){  
      ret->next = ret->prev = ret;
      ret->isHead = TRUE;
    }
  }
  return ret;
}


SLP_STORAGE_DECL void lslpDestroySLPMsg(lslpMsg *msg, int8 flag)
{
  assert(msg != NULL);
  switch (msg->type)
    {
    case srvReq:
      lslpDestroySrvReq(&(msg->msg.srvReq), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvRply:
      lslpDestroySrvRply(&(msg->msg.srvRply), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvReg:
      lslpDestroySrvReg(&(msg->msg.srvReg), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvAck:
      lslpDestroySrvAck(&(msg->msg.srvAck), LSLP_DESTRUCTOR_STATIC);
      break;
    case daAdvert:
      lslpDestroyDAAdvert(&(msg->msg.daAdvert), LSLP_DESTRUCTOR_STATIC);
      break;
    case saAdvert:
      lslpDestroySAAdvert(&(msg->msg.saAdvert), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvTypeReq:
      lslpDestroySrvTypeReq(&(msg->msg.srvTypeReq), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvTypeRep:
      lslpDestroySrvTypeReply(&(msg->msg.srvTypeRep), LSLP_DESTRUCTOR_STATIC);
      break;
    case attrReq:
      lslpDestroyAttrReq(&(msg->msg.attrReq), LSLP_DESTRUCTOR_STATIC);
      break;
    case attrRep:
      lslpDestroyAttrReply(&(msg->msg.attrRep), LSLP_DESTRUCTOR_STATIC);
      break;
    case srvDereg:
      lslpDestroySrvDeReg(&(msg->msg.srvDereg), LSLP_DESTRUCTOR_STATIC);
      break;
    default:
      break;
    }
  if (msg->dynamic == LSLP_DESTRUCTOR_DYNAMIC)
    free(msg);
  return;
}	

/* TCP support */
SLP_STORAGE_DECL void lslpDestroyConnection(struct slp_net *con)
{
  if(con == NULL)
    return;
  _LSLP_UNLINK(con);
  if (con->sock >= 0)
    _LSLP_CLOSESOCKET(con->sock);
  if(con->c._msg_buf != NULL)
    free(con->c._msg_buf);
  if(con->c._rcv_buf != NULL)
    free(con->c._rcv_buf);
  if(con->c._scratch != NULL)
    free(con->c._scratch);
  if(con->c._err_buf != NULL)
    free(con->c._err_buf);
  free(con);
}



/* a is an attribute list, while b is a string representation of an ldap filter  */
BOOL lslp_predicate_match(lslpAttrList *a, int8 *b)
{
  BOOL ccode;
  lslpLDAPFilter *ftree;
  if (b == NULL || ! strlen(b)) {
    SLP_TRACE("no predicate specified - automatic match\n");
    return TRUE;
  }
  
  if(a == NULL) {
    SLP_TRACE("lslp_predicate_match: attr list is null\n");
    return FALSE;
  }

  SLP_TRACE("predicate: %s; attribute: %s\n", (char *)b, (char *)a);
  assert(_LSLP_IS_HEAD(a));
  if(NULL != (ftree = _lslpDecodeLDAPFilter(b))) {
    ccode = lslpEvaluateFilterTree(ftree, a);
    lslpFreeFilterTree(ftree);
    return(ccode);
  }
  return(FALSE);
}


/* examining the URL if it contains a numerical IPv6 address. 
   if yes, extracting the address and saving it into the array pointed to by addr. 
   addr_len is the space size of addr */
BOOL extract_addr6_from_url( const char *url, char* addr, int addr_len)
{
  char * start, *end;
  struct in6_addr sin6_addr;

  start = strstr(url, "//[");
  if (start == NULL )
    return (FALSE);
   
  start += 3;
  end = strrchr(start, (int)']');
  if ( end == NULL )
    return (FALSE);

  if ( (end -start) > addr_len ) 
    return (FALSE);
   
   
  memcpy(addr, start, end - start );
  addr[end - start ] = '\0';
   
  SLP_TRACE("the IP6 address in url: %s\n",addr);
   
  if ( inet_pton (AF_INET6 , addr , &sin6_addr) <= 0 )
    return (FALSE);

  return (TRUE);
   
}


BOOL extract_addr4_from_url(const char *url, char *addr, int addr_len)
{

  char *start, *end;
  struct in_addr sin_addr;
  
  start = strstr(url, "://");
  if (start == NULL)
    return FALSE;
  
  start += 3;
  end = strrchr(start, '/');
  if (end == NULL) {
    end = start;
    while (*end != 0x00)
      end++;
  }
  if (end - start < 6)
    return FALSE;
  
  if ( (end - start) > addr_len ) 
    return (FALSE);
  
  memcpy(addr, start, end - start );
  addr[end - start ] = '\0';
  
  if (inet_pton(AF_INET, addr, &sin_addr) <= 0)
    return FALSE;
  return TRUE;
  
}


BOOL extract_host_from_url(const char *url, char *host, int host_len)
{
  char *start, *end;

  start = strstr(url, "://");
  if (start == NULL)
    return FALSE;
  
  start += 3;
  end = strrchr(start, '/');
  if (end == NULL) {
    end = start;
    while (*end != 0x00)
      end++;
  }
  
  if ( (end - start) > host_len ) 
    return FALSE;
  
  memcpy(host, start, end - start );
  host[end - start ] = '\0';

  return TRUE;
  
}

BOOL extract_addr_from_url(const char *url, char *addr, int addr_len)
{
  struct hostent hostbuf, *hp;
  size_t hstbuflen;
  char host[LSLP_MAX_PATH + 1];
  char *tmphstbuf;
  int res;
  int herr;
  
  if (FALSE == extract_addr4_from_url(url, addr, addr_len)) {
    if (FALSE == extract_addr6_from_url(url, addr, addr_len)) {
      if (FALSE == extract_host_from_url(url, host, LSLP_MAX_PATH))
	return FALSE;
      
      hstbuflen = 1024;
      /* Allocate buffer, remember to free it to avoid memory leakage.  */
      tmphstbuf = malloc (hstbuflen);
      while ((res = gethostbyname_r (host, &hostbuf, tmphstbuf, hstbuflen,
				     &hp, &herr)) == ERANGE) {
	/* Enlarge the buffer.  */
	hstbuflen *= 2;
	tmphstbuf = realloc (tmphstbuf, hstbuflen);
      }
      /*  Check for errors.  */
      if (res || hp == NULL)
	return FALSE;
      
      strncpy(addr, hp->h_addr, addr_len - 1);
      addr[addr_len] = 0x00;
      free(tmphstbuf);
    }
  }
  return TRUE;
}

BOOL lslp_url_scope_intersection(lslpURL * url,struct slp_net *remote_addr)           
{
  char remote_addr6[INET6_ADDRSTRLEN + 1];
  char url_addr6[INET6_ADDRSTRLEN + 1];
  if ( (remote_addr) && 
       (remote_addr->addr.saddr.sa_family == AF_INET6) ){     
    inet_ntop(AF_INET6, &remote_addr->addr.sin6.sin6_addr,remote_addr6, INET6_ADDRSTRLEN);
    if ( TRUE == extract_addr6_from_url(url->url, url_addr6,INET6_ADDRSTRLEN ) )
      {
	/*           
		     From RFC 3111,
		     SLPv2 SAs and DAs MUST determine which scope a service: URL address
		     is in.  This may be possible by examining the URL if it contains a
		     numerical IPv6 address.  If the URL contains a host name, the SA or
		     DA MUST resolve that name to a set of addresses.
           
		     A SLPv2 SA or DA MUST NOT respond to a SrvRqst with a service: URL
		     for a service with an address scope less than the request's source
		     address scope.  The rules are given below.
           
		     Request Source Address Scope
		     +------------+------------+---------+
		     | Link-Local | Site-Local | Global  |
		     +-------------+------------+------------+---------+
		     Service  | Link-Local  |  Respond   |    Drop    |   Drop  |
		     Address  +-------------+------------+------------+---------+
		     Scope    | Site-Local  |  Respond   |   Respond  |   Drop  |
		     +-------------+------------+------------+---------+
		     | Global      |  Respond   |   Respond  | Respond |
		     +-------------+------------+------------+---------+
		     Out-of-Scope Rules
           
		     This prevents UAs from being able discover service: URLs for services
		     which cannot be accessed.
	*/
	if ( ip6_mcast_scope((const char *)remote_addr) > ip6_mcast_scope((const char *)url_addr6) )
	  {
            SLP_TRACE("won't sent url %s to the remote address %s\n", url->url,remote_addr6);
            return (FALSE);
	  }
      }
  }
  return (TRUE);
}




struct lslp_srv_rply_out *_lslpProcessSrvReq(struct slp_client *client, 
					     struct lslp_srv_req *msg, 
					     int16 errCode,
					     int buf_size,
					     struct slp_net *remote_addr)
{
  int8 *buf;
  int32 bufLen, avail;
  lslpSrvRegList *reg;
  int32 ext_offset;
  int8 *extptr, *next_extptr;
  BOOL pile_up_attrs = FALSE;
  struct lslp_srv_rply_out *temp_rply = NULL;
  
  SLP_TRACE("entering _lslp_ProcessSrvReq\n");
  
  temp_rply = 
    (struct lslp_srv_rply_out *)calloc(1, sizeof(struct lslp_srv_rply_out));
  assert(msg != NULL);
  if(msg != NULL && temp_rply != NULL) {
    if((temp_rply->errCode = errCode) == 0) {
      buf = (int8 *)calloc(buf_size, sizeof(char));
      if(buf != NULL) {
	bufLen = buf_size;
	temp_rply->urlList = buf;
	buf += 4;

	avail = bufLen - 4;

	/* check for the attr list extension */
	/* remember to subtract 5 bytes from the limit for each extension we traverse */


	/*    The format of the Attribute List Extension is as follows: */

	/*       0                   1                   2                   3 */
	/*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 */
	/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
	/*      |      Extension ID = 0x0002    |     Next Extension Offset     | */
	/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
	/*      | Offset, contd.|      Service URL Length       |  Service URL  / */
	/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
	/*      |     Attribute List Length     |         Attribute List        / */
	/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
	/*      |# of AttrAuths |(if present) Attribute Authentication Blocks.../ */
	/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

	/*    The Extension ID is 0x0002. */




	if(msg->next_ext != NULL) {
	  /* we are looking for extension ID 0x0002 */
	  extptr = msg->next_ext;
	  while(extptr && extptr < msg->ext_limit) {
	    int8 * extptr_save = extptr;
	    if( 0x0002 == _LSLP_GETSHORT(extptr, 0)) {
	      /* make sure it doesn't overflow */
	      if (extptr + 9 > msg->ext_limit) {
		break;
	      }
	      /* got our extension */
	      /* do a sanity check offset five should be 0x0000, */
	      /* offset 7 should be 0x0000*/
	      if( (0x0000 == _LSLP_GETSHORT(extptr, 5)) && 
		  (0x0000 == _LSLP_GETSHORT(extptr, 7)) ) {
		/* ok, accumulate attribute data */
		SLP_TRACE("will accumulate attribute data\n");
		pile_up_attrs = TRUE;
	      }
	      break;
	    }
	    /* see if there are any more extensions */
	    extptr += _LSLP_GET3BYTES(extptr, 2);
	    if(extptr == extptr_save)
	      break;
	  } /* while traversing extensions */ 
	} /* if there is an extension */
	
	reg = client->regs.next;
	extptr = client->_scratch;
	next_extptr = extptr + 2;
	ext_offset = 0;
	
	if (_LSLP_IS_HEAD(reg)) {
	  SLP_TRACE("no registrations right now\n");
	}
	
	while(! _LSLP_IS_HEAD(reg)) {
	  if(time(NULL) > reg->directoryTime) { 
	    /* the lifetime for this registration has expired */
	    /* unlink and free it */
	    lslpSrvRegList *temp_reg = reg;
	    SLP_TRACE("registration timed out, removing\n");
	    	    
	    reg = reg->next;
	    ip6_remove_reg(client, temp_reg);
	    lslpFreeSrvReg(temp_reg);
	    continue;
	  }
	  if(TRUE == lslp_srvtype_match(reg->srvType, msg->srvcType)) {
	    if ( (remote_addr->addr.saddr.sa_family == AF_INET) ||
		 (TRUE == lslp_url_scope_intersection(reg->url, remote_addr))){
	      /* we need to check for the special case - of a srvreq with service type directory-agent */
	      /* - it is allowed to omit scopes */
	      if((msg->scopeList == NULL) ||  
		 (_LSLP_IS_EMPTY(msg->scopeList) && (! strncasecmp(msg->srvcType, DA_SRVTYPE, DA_SRVTYPELEN ))) || 
		 (TRUE == lslp_scope_intersection(reg->scopeList, msg->scopeList))) {

		SLP_TRACE("scopes intersect \n");
	      
		if(TRUE == lslp_predicate_match(reg->attrList, msg->predicate)) {
		  SLP_TRACE("predicates match %s\n", msg->predicate);
		
		  /* we have a match */
		  /* for protected scopes, we need to change stuff URL so that it */
		  /* only stuffs url auth blocks that match spis in the srv req */

		  /* check remaining buffer size */
		  if (reg->url->len + 5 > avail) {
		    SLP_TRACE("response buffer is overflowing. url count is %i, avail is %i, buf size is %i, overflow is %i\n", 
			      temp_rply->urlCount, avail, buf_size, temp_rply->overflow);
		    temp_rply->overflow = 1;
		    goto finished_stuffing_urls;
		  }
		  if(TRUE == lslpStuffURL(&buf, &avail, reg->url)) {
		    SLP_TRACE("stuffed URL\n");
		    temp_rply->urlCount++; 
		    if(TRUE == pile_up_attrs && ((reg->url->len + ext_offset + 2) < LSLP_SCRATCH_SIZE)) {
		      SLP_TRACE("piling up attributes\n");
		    
		      /* use the scratch buffer as temporary storage for the attribute extensions */
		      /* go back and set the length of the next extension in prev extension */
		      _LSLP_SET3BYTES(next_extptr, ext_offset, 0);
		      next_extptr = extptr + 2;
		      /* set the extension id */
		      _LSLP_SETSHORT(extptr, 0x0002, 0);
		      /* init the next ext to zero */
		      _LSLP_SET3BYTES(next_extptr, 0x00000000, 0);
		      /* length of url string */
		      _LSLP_SETSHORT(extptr, reg->url->len, 5);
		      memcpy(extptr + 7, reg->url->url, reg->url->len);
		      ext_offset += (7 + reg->url->len);
		      extptr += 7 + reg->url->len;
		    
		      /* attr list length */
		      if( reg->attrList && ext_offset + 2 + reg->attrList->attr_string_len < LSLP_SCRATCH_SIZE) {
			_LSLP_SETSHORT(extptr, reg->attrList->attr_string_len, 0);
			memcpy(extptr + 2, reg->attrList->attr_string, reg->attrList->attr_string_len);
			ext_offset += (2 + reg->attrList->attr_string_len);
			extptr += (2 + reg->attrList->attr_string_len);
			if( ext_offset + 1 < LSLP_SCRATCH_SIZE) {
			  /* set the number of attr auths to zero */
			  _LSLP_SETBYTE(extptr, 0, 0);
			  extptr += 1;
			  ext_offset += 1;
			} else { 
			  SLP_TRACE("error path\n");
 			
			  temp_rply->overflow = 1; pile_up_attrs = FALSE; _LSLP_SETSHORT(client->_scratch, 0x0000, 0);
			}
		      } else { 
			SLP_TRACE("error path\n");
			temp_rply->overflow = 1; pile_up_attrs = FALSE; _LSLP_SETSHORT(client->_scratch, 0x0000, 0);
		      }
		    } else { 
		      SLP_TRACE("error path url len %i,ext_offset %i, scratch %i\n",
				reg->url->len, ext_offset + 2, LSLP_SCRATCH_SIZE);
		      temp_rply->overflow = 1; pile_up_attrs = FALSE; _LSLP_SETSHORT(client->_scratch, 0x0000, 0);
		    }
		  } else {
		    SLP_TRACE("error path\n");
		    temp_rply->errCode = LSLP_INTERNAL_ERROR;
		    return(temp_rply);
		  }
		} /* if we have a match */
	      } /* if we found a scope intersection */
	    }/*if the multicast scope intersection match*/
	  } /* if the types match */
	  reg = reg->next;
	} /* while we are traversing the reg list for this da */
	/* stuff the error code and url count */
finished_stuffing_urls:
	_LSLP_SETSHORT(temp_rply->urlList, temp_rply->errCode, 0);
	_LSLP_SETSHORT(temp_rply->urlList, temp_rply->urlCount, 2);
	/* resize to the actual size needed */
	temp_rply->urlLen = (buf - temp_rply->urlList) ;
      } /* if we alloced our buffer */
    } /* if we need to look for matches */
    else {
      if(NULL != (temp_rply->urlList = (int8 *)calloc(8, sizeof(int8)))) {  //jeb
	_LSLP_SETSHORT(temp_rply->urlList, temp_rply->errCode, 0);
      }
    } /* error reply */
    if(temp_rply->urlList == NULL) {
      free(temp_rply);
      temp_rply = NULL;
    }
  } /* if we alloced our reply structure */
  return(temp_rply);
}


#define lslp_to_lower(c)  (((c) > 0x40 && (c) < 0x5b) ? ((c) + 0x20) : (c))

#define MAX_RECURSION  10
SLP_STORAGE_DECL  BOOL lslp_pattern_match(const int8 *string, const int8 *pattern, BOOL case_sensitive)
{
  
  int8 s, p;
  static int recursion_level;
  if(recursion_level == MAX_RECURSION) {
    return FALSE;
  }
  recursion_level++;
  
  assert(string != NULL && pattern != NULL);

  while(1) {
    p = *pattern;
    if( p == '\0' ) {
      recursion_level--;
      if (*string == '\0')
	return TRUE;
      else
	return FALSE;
    }

    if((*string == '\0') && (p != '*')) {
      recursion_level--;
      return FALSE;
    }

    if(p == '*') {
      while(*(++pattern) == '*') { ; }
      p = *pattern;
      if( p == '\0') {
	recursion_level--;
	return TRUE;
      }
      while(1) {
	if((p != '[') && (p != '?') && (p != '\\')) {
	  /* advance the string until there is a match */
	  while(*string) {
	    s = *string;
	    if(case_sensitive == FALSE) {
	      p = lslp_to_lower(p);
	      s = lslp_to_lower(s);
	    }
	    if(s == p)
	      break;
	    string++;
	  }
	}
	if(TRUE == lslp_pattern_match(string, pattern, case_sensitive)) {
	  recursion_level--;
	  return TRUE;
	}
	if(*string == '\0') {
	  recursion_level--;
	  return FALSE;
	}
	string++;
      }
    }
    if(p == '?') {
      pattern++;
      string++;
      continue;
    }
    
    if(p == '[') {
      int8 start, end;
      pattern++;
      s = *(string++);
      if(case_sensitive == FALSE) {
	s = lslp_to_lower(s);
      }
      while(1) {
	if(*pattern == ']' || *pattern == '\0') {
	  recursion_level--;
	  return FALSE ;
	}
	start = *(pattern++);
	if(case_sensitive == FALSE){
	  start = lslp_to_lower(start);
	}
	if(*pattern == '-') {
	  pattern++;
	  if(*pattern == '\0') {
	    recursion_level--;
	    return FALSE;
	  }
	  end = *(pattern++);
	  if(case_sensitive == FALSE) {
	    end = lslp_to_lower(end);
	  }
	  if(((start <= s) && (s <= end )) || ((end <= s) && (s <= start))) {
	    break;
	  }
	} else if (start == s) {
	  
	  break;
	}
      }
      while(*pattern != ']') {
	if(*pattern == '\0') {
	  pattern--;
	  break;
	}
	pattern++;
      }
      pattern++;
      continue;
    }

    if (p == '\\') {
      pattern++;
      string++;
      
      if (*pattern == '\0') {
	recursion_level--;
	return FALSE;
      }
    }
    s = *string;
    p = *pattern;
    string++;
    pattern++;
    
    if(case_sensitive == FALSE) {
      p = lslp_to_lower(p);
      s = lslp_to_lower(s);
    }
    if(s == p) {
      continue;
    }
    recursion_level--;
    return FALSE;
  }
}


int8 * lslp_get_next_ext(int8 *hdr_buf)
{
  int32 len;
  assert(hdr_buf != NULL);
  if( 0 != _LSLP_GETNEXTEXT(hdr_buf)) {
    len = _LSLP_GETLENGTH(hdr_buf);
    if(len > _LSLP_GETNEXTEXT(hdr_buf)) {
      return( hdr_buf + _LSLP_GETNEXTEXT(hdr_buf));
    }
  }
  return NULL;
}

SLP_STORAGE_DECL BOOL lslp_forward_reg(struct slp_client *client,
				       struct da_list *da,
				       const int8 *url,
				       const int8 *attributes,
				       const int8 *service_type,
				       const int8 *scopes,
				       int16 lifetime)
  
{

  struct slp_net *con;
  union slp_sockaddr da_addr;  
  BOOL err;
  
  /* find the address type of the da */
  if (6 == ip6_addr_type(da->remote)) {
    da_addr.saddr.sa_family = AF_INET6;
    inet_pton(AF_INET6, da->remote, &da_addr.sin6.sin6_addr);
    da_addr.sin6.sin6_port = htons(client->_target_port);
  } else {
    da_addr.saddr.sa_family = AF_INET;
    inet_pton(AF_INET, da->remote, &da_addr.sin.sin_addr);
    da_addr.sin.sin_port = htons(client->_target_port);
  }
  
  con = tcp_create_connection(client, &da_addr);
  if (con == NULL) {
    SLP_TRACE("Error unable to connect to remote DA at %s\n", da->remote);
    return 0;
  }

  SLP_TRACE("forward_reg: xid is %d\n", client->_xid);
  
  err = tcp_srv_reg(client, 
		    con, 
		    url, 
		    attributes,
		    service_type, 
		    scopes,
		    lifetime);
  lslpDestroyConnection(con);
  SLP_TRACE("forward_reg: xid is %d\n", client->_xid);
  return err;
  
}

SLP_STORAGE_DECL int lslp_forward_regs(struct slp_client *client, struct da_list *da)
{
  lslpSrvRegList *reg;
  
  int count, lifetime, len, attr_len, used_scratch = 0;
  lslpAttrList *attr_list;
  char *attrs = NULL;
  char *scope;
  char scope_list[LSLP_MAX_PATH + 1];  

  SLP_TRACE("Forwarding regs to new DA\n");
  /* check for a quick exit */
  if (da->stateless_boot == da->last_forward) {
    SLP_TRACE("We've already forwarded regs to this DA\n");  
    return 0;
  }
  
  reg = client->regs.next;
  
  while (!_LSLP_IS_HEAD(reg)) {
    BOOL err;
    SLP_TRACE("starting with reg %s\n", reg->url->url);
    
    lifetime = reg->directoryTime - time(NULL);
    if (lifetime > 0) {
      SLP_TRACE("Reg lifetime is %d\n", lifetime);
      len = LSLP_MAX_PATH;
      scope = scope_list;
      if (TRUE == lslpStuffScopeList(&scope, &len, reg->scopeList)) {
	SLP_TRACE("Stuffed scope list\n");
	scope = &scope_list[2];
	SLP_TRACE("Successfully stuffed scope list: %s\n", scope);

	memset(client->_scratch, 0x00, LSLP_MTU);
	attrs = client->_scratch;
	attr_len = LSLP_MTU;
	used_scratch = 1;
	
	if (FALSE == lslpStuffAttrList(&attrs, &attr_len, reg->attrList, NULL)) {
	  attrs = NULL;
	  SLP_TRACE("error parsing attributes in forward regs to da\n");
	  reg = reg->next;
	  continue;
	  
	}
	
	SLP_TRACE("Forwarding reg %s to DA %s\n", reg->url->url, da->remote);
	
	err = lslp_forward_reg(client, 
			       da,
			       reg->url->url, 
			       client->_scratch + 2,
			       reg->srvType,
			       scope,
			       lifetime);
	
	if (err == TRUE) {
	  da->last_forward = da->stateless_boot;
	  count++;
	  SLP_TRACE("Success forwarding reg %s to DA %s\n", reg->url->url, da->remote);
	}
	
	else
	  break;
      }
    }
    reg = reg->next;
  }
  if (used_scratch)
    memset(client->_scratch, 0x00, LSLP_MTU);
  return count;
}


/** uses a newline as the field separator, two consecutive newlines as the record separator **/
SLP_STORAGE_DECL void lslp_print_srv_rply(lslpMsg *srvrply)
{

  lslpURL *url_list;
  if(srvrply != NULL && srvrply->type == srvRply) {
    /* output errCode, urlCount, urlLen */
    printf("%d\n%d\n%d\n%d\n", srvrply->msg.srvRply.errCode, 
	   srvrply->msg.srvRply.urlCount, 
	   srvrply->msg.srvRply.urlLen,
	   srvrply->hdr.flags);
    if((NULL != srvrply->msg.srvRply.urlList) && 
       (! _LSLP_IS_EMPTY( srvrply->msg.srvRply.urlList))) {
      
      url_list = srvrply->msg.srvRply.urlList->next;
      while( ! _LSLP_IS_HEAD(url_list)){
	
	if(NULL != url_list->url)
	  printf("URL: %s\n", url_list->url);
	else
	  printf("URL: \n");
	
	/* see if there are attributes to print */
	if(NULL != url_list->attrs && ! _LSLP_IS_HEAD(url_list->attrs->next)){
	  lslpAtomList *attrs = url_list->attrs->next;
	  while(! _LSLP_IS_HEAD(attrs)){
	    printf("ATTR: %s\n", attrs->str);
	    attrs = attrs->next;
	  } /* while traversing attr list */
	} /* if attr list */
	url_list = url_list->next;
	
      } /* while traversing url list */
    } /* if there are urls to print */
    /* print the record separator */
    printf("\n\n");
  }
}

/* outputs information for machine consumption */
SLP_STORAGE_DECL void lslp_print_srv_rply_parse(lslpMsg *srvrply, int8 fs, int8 rs)
{
  BOOL dont_print_extra_rs = FALSE;
  
  lslpURL *url_list;
  if(srvrply != NULL && srvrply->type == srvRply) {
    /* output errCode, urlCount, urlLen */
    printf("%d%c%d%c%d%c%d%c", 
	   srvrply->msg.srvRply.errCode, 
	   fs,
	   srvrply->msg.srvRply.urlCount, 
	   fs,
	   srvrply->msg.srvRply.urlLen,
	   fs,
	   srvrply->hdr.flags,
	   fs);
    if((NULL != srvrply->msg.srvRply.urlList) && 
       (! _LSLP_IS_EMPTY( srvrply->msg.srvRply.urlList))) {
      
      url_list = srvrply->msg.srvRply.urlList->next;
      while( ! _LSLP_IS_HEAD(url_list)) {
	
	if(NULL != url_list->url)
	  printf("%s%c", url_list->url, fs);
	else 
	  printf("%c", fs);
	
	/* see if there are attributes to print */
	if(NULL != url_list->attrs && ! _LSLP_IS_HEAD(url_list->attrs->next)) {
	  lslpAtomList *attrs = url_list->attrs->next;
	  while(! _LSLP_IS_HEAD(attrs) && attrs->str && strlen(attrs->str)){
	    printf("%s", attrs->str);
	    attrs = attrs->next;
	    if(! _LSLP_IS_HEAD(attrs) && attrs->str && strlen(attrs->str)) {
	      printf("%c", fs);
	    }
	  } /* while traversing attr list */
	} else {  /* if no attr list, print the record separator  */
	  printf("%c", rs);
	  dont_print_extra_rs = TRUE;
	  
	}
	
	url_list = url_list->next;
	/* if there is another url, print a record separator */
	/* also print the first four fields so the consuming program's */ 
	/* parser doesn't get upset */
	if( ! _LSLP_IS_HEAD(url_list) && dont_print_extra_rs == FALSE) {
	  
	  printf("%c", rs);
	  printf("%d%c%d%c%d%c", 
		 srvrply->msg.srvRply.errCode, 
		 fs,
		 srvrply->msg.srvRply.urlCount, 
		 fs,
		 srvrply->msg.srvRply.urlLen,
		 fs);
	}
	
      } /* while traversing url list */

    } /* if there are urls to print */
    /* print the record separator */
    printf("%c", rs);
    
  }
}

/** uses a newline as the field separator, two consecutive newlines as the record separator **/
SLP_STORAGE_DECL   void lslp_print_attr_rply(lslpMsg *attrrply)
{
  if(attrrply != NULL && attrrply->type == attrRep) {
    
    /* output the err, attr len, attr string */ 
    printf("error code: %d\nattribute length: %d\n%s\n\n",
	   attrrply->msg.attrRep.errCode, 
	   attrrply->msg.attrRep.attrListLen, 
	   attrrply->msg.attrRep.attrList);
  } else { printf("no correctly formatted responses\n\n"); }   
  
}


SLP_STORAGE_DECL   void lslp_print_attr_rply_parse(lslpMsg *attrrply, int8 fs, int8 rs)
{
  if(attrrply != NULL && attrrply->type == attrRep) {
    
    /* output the err, attr len, attr string */ 
    printf("%d%c%d%c%s%c",
	   attrrply->msg.attrRep.errCode, fs,
	   attrrply->msg.attrRep.attrListLen,fs, 
	   attrrply->msg.attrRep.attrList, rs);
  }   
}



/* string must not be null and must start with "service:" */
SLP_STORAGE_DECL  BOOL test_service_type(int8 *type)
{
  int8 * temp;
  BOOL ccode;
  temp = strdup(type);
  if(type == NULL || ! strlen(type))
    return FALSE;
  if(temp == NULL) abort();
  ccode = lslp_pattern_match( "service:*", temp, FALSE);
  free(temp);
  return ccode;
}


SLP_STORAGE_DECL  BOOL test_service_type_reg(int8 *type)
{
  int8 * temp;
  BOOL ccode;
   
  if(type == NULL || ! strlen(type))
    return FALSE;
  temp = strdup(type);
  if(temp == NULL) abort();
  ccode = lslp_pattern_match(temp, "service:*", FALSE);
  free(temp);
  return ccode;
}


SLP_STORAGE_DECL  BOOL test_url(int8 *url)
{

  int8 *temp;
  lslpAtomizedURL *aurl;
  
  if(url == NULL)
    return FALSE;
  temp = strdup(url);
  if(temp == NULL) abort();
  aurl = _lslpDecodeURLs(&temp, 1);
  free(temp);
  if(aurl != NULL){
    lslpFreeAtomizedURLList(aurl, TRUE);
    return TRUE;
  }
  return FALSE;
}

SLP_STORAGE_DECL  BOOL test_attribute(int8 *attr)
{
  int8 *temp;
  lslpAttrList *attr_list;
  
  if(attr == NULL)
    return FALSE;
  if(! strlen(attr))
    return TRUE;
  
  temp = strdup(attr);
  if(temp == NULL) abort();
  
  attr_list = _lslpDecodeAttrString(temp);
  free(temp);
  
  if(attr_list != NULL){
    lslpFreeAttrList(attr_list, TRUE);
    return TRUE;
  }
  return FALSE;

}

SLP_STORAGE_DECL  BOOL test_scopes(int8 *scopes)
{
  BOOL ccode;
  int8 *temp;
  
  if(scopes == 0 ){
    return FALSE;
  }
  
  if(! strlen(scopes))
    return TRUE;
    
  temp = strdup(scopes);
  if(temp == 0 ) abort();
  ccode = lslp_islegal_scope(temp);
  free(temp);

  
  return ccode;
}

SLP_STORAGE_DECL  BOOL test_predicate(int8 *predicate)
{
  int8 * temp;
  lslpLDAPFilter *filter;
  
  if(predicate == 0)
    return FALSE;
  if(! strlen(predicate))
    return TRUE;
  
  temp = strdup(predicate);
  if(temp == 0) abort();
  filter = _lslpDecodeLDAPFilter(temp);
  free(temp);
  if(filter != NULL) {
    lslpFreeFilter(filter);
    return TRUE;
  }
  return FALSE;
}


uint32 test_srv_reg(int8 *type, 
		    int8 *url,   
		    int8 *attr, 
		    int8 *scopes)
{

  /* it is legal for the scope string and attribute list to be empty */
  if(type == NULL || 0 == strlen(type) || FALSE == test_service_type_reg(type))
    return 1;
  if(url == NULL || 0 == strlen(url) || FALSE == test_url(url) )
    return 2;
  if(attr != NULL && FALSE == test_attribute(attr))
    return 3;
  if(scopes != NULL && FALSE == test_scopes(scopes))
    return 4;
  return 0;
}

uint32 test_query(int8 *type,
		  int8 *predicate, 
		  int8 *scopes)
{
  /* it is legal for predicate and scopes to be empty */
  
  if(type == NULL || 0 == strlen(type) || FALSE == test_service_type(type))
    return 1;

  if(predicate != NULL && FALSE == test_predicate(predicate))
    return 2;
    
  if(scopes != NULL && FALSE == test_scopes(scopes))
    return 3;
 
  return 0;
}



#ifdef INCLUDE_TEST_CODE     

/* , (serial-number=78-HNRX5)" */
/* , (fru=model 8657\\-a)" */
/*                 "name=chassis_4_b205g134), (uuid=\\ff\0d\0a\25\56\0d\0a\25\56\0d\0a\25\56)" \ */
/*                 "(ip-address=9.37.98.90)" */



#define MM_URL "service:management-hardware.IBM:management-module://chassis_one:4456/%20;%20hello"
#define MM_URL2 "service:management-hardware.IBM:management-module://chassis_two:4456/%20;%20hello"
#define MM_URL3 "service:management-hardware.IBM:management-module://chassis_three:4456/%20;%20hello"
#define MM_ATTR "management-module, (mangement-module_test =     )"
#define MM_ATTR2 "(enclosure-serial-number=\\ff\\04\\05), management-module"
#define MM_ATTR3 "(enclosure-serial-number=\\ff\\04\\05), management-module, (nothing=one)"
#define MM_SRV_TYPE "service:management-hardware.IBM:management-module" 
#define MM_SCOPES "DEFAULT"
#define MM_LIFETIME 0xff
#define MM_PRED "(management-module =*)"
#define MM_PRED2 "management-module, (enclosure-serial-number=\\ff\\04\\05)"


int32 opaque_buf[] = {  
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
  0x09, 0x0a, 0xfe0b, 0xcdac, 0x0d, 0x0e, 0x10
};
int16 opaque_len = 15 * sizeof(int32);

#define NUM_STRINGS_TO_MATCH 8
int8 *patterns[] = { 
  "hello, world", 
  "hEllo, wOrld", 
  "Custer is a h?ll of a d**n*ed fo*\\; -- Phil Sheridan",
  "See Mug, See",
  "management-hardware.[ih][ba][ml]",
  "The telephone is [hlr]eap [bds]ig medicine -- Crazy Horse",
  "Young kids today are [A-z]ool", 
  "Young kids today are [A-z]ool" 
};

int8 *strings[] = {
  "hello, world",
  "hello, world",
  "custer is a hill of a dinged fog; -- Phil Sheridan",
  "see mug, see",
  "management-Hardware.HAL",
  "the telephone is leap dig medicine -- Crazy Horse",
  "young kids today are cool", 
  "young kids today are fool"
};


int main(int argc, char **argv)
{
  BOOL ccode;

  lslpMsg *replies, rply;
  int i ;
  time_t now, last;
  
  
  struct slp_client *client = create_slp_client(NULL, NULL, 427, "DSA", "DEFAULT, TEST SCOPE", TRUE, TRUE);
  
  if(client != NULL){
    int8 *encoded, *test_buf;
    void *decoded;
    int16 encoded_len;
    printf("Testing opaque encode/decode\n");
    
    encoded = encode_opaque(opaque_buf, opaque_len);
    decoded = decode_opaque(encoded);
    if( memcmp(decoded, opaque_buf, opaque_len) )
      printf("opaque decoder produced mismatch\n");
    
    encoded_len = _LSLP_GETSHORT(encoded, 0 );
    test_buf = (int8 *)calloc(encoded_len, sizeof(int8));
    if(test_buf != NULL) {
      memcpy(test_buf, encoded + 2, encoded_len - 2);
      printf("%s\n", test_buf);
      free(test_buf);
    }
    
    free(encoded);
    free(decoded);
    
    for (i = 0; i < NUM_STRINGS_TO_MATCH ; i++){
      if(TRUE == lslp_pattern_match(strings[i], patterns[i], FALSE))
	printf("%s MATCHES %s (no case)\n", strings[i], patterns[i]);
      else 
	printf("%s DOES NOT MATCH %s (no case)\n", strings[i], patterns[i]);
    }
  
    for (i = 0; i < NUM_STRINGS_TO_MATCH ; i++){
      if(TRUE == lslp_pattern_match(strings[i], patterns[i], TRUE))
	printf("%s MATCHES %s \n", strings[i], patterns[i]);
      else 
	printf("%s DOES NOT MATCH %s \n", strings[i], patterns[i]);
    }
    client->srv_reg_local(client, 
			  "service:service-agent:\\9.37.98.90", 
			  "(service-type = )", "service-agent", 
			  MM_SCOPES, MM_LIFETIME);
    
    ccode = client->srv_reg_all(client, MM_URL, MM_ATTR, MM_SRV_TYPE, MM_SCOPES, MM_LIFETIME);
    printf("registered with %d agents.\n", ccode );
    ccode = client->srv_reg_all(client, MM_URL2, MM_ATTR2, MM_SRV_TYPE, MM_SCOPES, MM_LIFETIME);
    printf("registered with %d agents.\n", ccode );
    ccode = client->srv_reg_all(client, MM_URL3, MM_ATTR3, MM_SRV_TYPE, MM_SCOPES, MM_LIFETIME);
    printf("registered with %d agents.\n", ccode );
    client->converge_srv_req(client, MM_SRV_TYPE, MM_PRED, MM_SCOPES);

    client->converge_srv_req(client, "service-agent", "(service-type =*)", MM_SCOPES);
    
    replies = &rply;
    
    client->service_listener(client, 0, replies);
    if(_LSLP_IS_HEAD(replies->next))
      printf("Recieved 0 Service Replies\n");
    else {
      replies = replies->next;
      while( ! _LSLP_IS_HEAD(replies)) {
	if( replies->type == srvRply ) {
	  printf("SRVRPLY: err_code %d url_count %d, url_len %d \n", 
		 replies->msg.srvRply.errCode, replies->msg.srvRply.urlCount, 
		 replies->msg.srvRply.urlLen);
	  if( NULL != replies->msg.srvRply.urlList ) {
	    if( ! _LSLP_IS_EMPTY(replies->msg.srvRply.urlList)) {
	      lslpURL *url_list = replies->msg.srvRply.urlList->next;
	      while( ! _LSLP_IS_HEAD(url_list)) {
		printf("\t%s\n", url_list->url);
		if((NULL != replies->msg.srvRply.attr_list) && 
		   (! _LSLP_IS_EMPTY(replies->msg.srvRply.attr_list))) {
		  lslpAtomList *temp = replies->msg.srvRply.attr_list->next;
		  if(! _LSLP_IS_HEAD(temp)) {
		    if( temp->str != NULL) {
		      printf("ATTRS: %s\n", temp->str);
		      _LSLP_UNLINK(temp);
		      lslpFreeAtom(temp);
		    }
		  }
		}
		url_list = url_list->next;
	      }
	    } else { printf("no URLs\n"); } /* url list not empty */
	  } else { printf("no URLs\n"); } /* url list  not NULL */
	} /* srvrply */
	replies = replies->next;
      } /* traversing replies */
    }
  }
  
  now = (last = time(NULL));

  while(1){
    lslpMsg msg;
    _LSLP_SLEEP(100);
    client->service_listener(client, 0, &msg);
    now = time(NULL);
    if( now - last > ( MM_LIFETIME - 1) ) {
      client->srv_reg_local(client, MM_URL2, MM_ATTR2, MM_SRV_TYPE, MM_SCOPES, MM_LIFETIME);
      last = time(NULL);
    }
  }
  return 1;
}


#endif //test code

/*
 * Local variables:
 *  c-file-style: "gnu"
 * End:
 */



