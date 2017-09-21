
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



#include "lslp-common-defs.h"
#ifdef _WIN32_WINNT
#include <winsock2.h>
#else
#include <netinet/tcp.h>
#endif

/* defined in lslp-kernel.c */
extern lslpStream streamInHead;
extern lslpDirAgent lslpDAs, lslpRemoteDAs;


/* similar to code in lslp-kernel.c, except that it prepares a connecting socket */
BOOL mslp_prepare_sock(SOCKET sock)
{
  if(TRUE == prepareSock(sock)) {
    int opt = 1;
    
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(int));
    opt = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(int));
    return TRUE;
  }
  return FALSE ;
}


uint32 mslp_connect_peers(lslpDirAgent *da_list, lslpStream *stream_list)
{
  
  uint32 ccode, connections = 0;
  lslpStream temp  = { &temp, &temp, TRUE };
  
  assert(da_list != NULL && _LSLP_IS_HEAD(da_list));
  assert(stream_list != NULL && _LSLP_IS_HEAD(stream_list));
  
  _LSLP_WAIT_SEM(da_list->writeSem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    lslpDirAgent *this_agent = da_list->next;
    while(! _LSLP_IS_HEAD(this_agent)) {
      if( TRUE == _LSLP_CONNECT_TO_PEER(this_agent->status)) {
	/* create a stream data structure */
	lslpStream *in_stream ;
	int sock_flags;
	
	
	if(NULL != (in_stream = (lslpStream *)calloc(1, sizeof(lslpStream)))) {
	  in_stream->status = (LSLP_STATUS_PEER_SOCKET | LSLP_STATUS_KEEP_SOCKET | 
			       LSLP_CONNECT_CONNECTING );
	  in_stream->close_interval = lslp_config_close_conn;
	  
	  in_stream->peerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	  mslp_prepare_sock(in_stream->peerSock);
	  
	  
	  /* set the peerAddr structure */
	  in_stream->peerAddr.sin_family = AF_INET;
	  in_stream->peerAddr.sin_port = htons(lslp_config_opport);
	  in_stream->peerAddr.sin_addr.s_addr = this_agent->addr.sin_addr.s_addr;
	  
	  /* set sock options */
	  
	  
#ifdef _WIN32_WINNT
	  {
	    int cmd = 1;
	    

	    sock_flags = ioctlsocket(in_stream->peerSock, 
				     FIONBIO, 
				     &cmd);
	  }
#else
	  /* set socket to non-blocking mode, then start the connect */
	  sock_flags = fcntl(in_stream->peerSock, F_GETFL, 0);
	  fcntl(in_stream->peerSock, F_SETFL, sock_flags | O_NONBLOCK);
#endif	  

	  ccode = connect(in_stream->peerSock, 
			  (struct sockaddr *)&(in_stream->peerAddr), 
			  sizeof(struct sockaddr));

	  /* connection is started, check back later for status */
	  _LSLP_INSERT(in_stream, &temp);
	  /* change the status of this peer to connecting, so we don't try this again */
	  this_agent->status |= LSLP_CONNECT_CONNECTING;
	  
	} /* if we allocated the stream struct */
      } /* if we need to connect to this peer */
      
      this_agent = this_agent->next;
    }
    _LSLP_SIGNAL_SEM(da_list->writeSem);
  }

  /* now that we are done with the remote da list, take the new stream structures */
  /* and link them to the list of streams that we have to manage */
  if( ! (_LSLP_IS_EMPTY(&temp))) {
    
    _LSLP_WAIT_SEM(stream_list->sem, LSLP_Q_WAIT, &ccode);
    assert(ccode == LSLP_WAIT_OK);
    if(ccode == LSLP_WAIT_OK) {
      _LSLP_LINK_HEAD(stream_list, &temp);
      _LSLP_SIGNAL_SEM(stream_list->sem);
    }
  }
}



/******************************************************************
 * Determine how to treat an incoming da advertisement, specifically
 * regarding the mesh status and potential peer relationship 
 * 
 * returns: LSLP_DA_NOT_PEER not a tcp connection, no mesh implications 
 *          LSLP_DA_POTENTIAL_PEER  already a peer connection - 
 *            the other side is dumping its peer
 *            list. flag the da as a potential peer. 
 *          LSLP_DA_PEER_CONNECTED - the other side is initiating 
 *            a peer connection. flag the da as a peer, and 
 *            store the connection socket in the da structure
 ******************************************************************/  



uint32 mslp_existing_da_init(lslpWork *work, uint8 *scopes)
{
  lslpStream *stream;
  uint32 ccode = LSLP_DA_NOT_PEER;
  
  /* first check for a tcp connection. */
  if( ! (work->status & LSLP_STATUS_TCP ) )
    return ccode;
  
  /* next, find the stream structure associated with this socket */
  
  stream = _lslpStreamLockFindSock(&streamInHead, work->ses_idx);
  if(stream != NULL) {
    /* if this is a peer stream, we don't need to do anything*/ 
    if(stream->status & ( LSLP_DA_INITIATED_PEER_CONN || 
			  LSLP_DA_PEER_CONNECTED )) {
      /* just set the peer socket bits */ 
      stream->status &= ( LSLP_STATUS_PEER_SOCKET | LSLP_DA_KEEP_SOCKET );
      ccode = LSLP_DA_POTENTIAL_PEER;
    } else {
      /* new peer. Set the bits and also set the da_fwd_daadv and da_needs_regs bits */ 
      stream->status |= 
	( LSLP_DA_INITIATED_PEER_CONN | LSLP_DA_PEER_CONNECTED | 
	  LSLP_STATUS_PEER_SOCKET | LSLP_STATUS_KEEP_SOCKET | 
	  LSLP_DA_NEEDS_REGS | LSLP_DA_FWD_DAADV ) ;

      /* attach the scope information to the stream */
      stream->scopeList = lslpScopeStringToList(scopes, strlen(scopes));
      ccode = LSLP_DA_PEER_CONNECTED;
    }

    /* don't leave the list locked */ 
    _LSLP_SIGNAL_SEM(streamInHead.sem);
  }
  return ccode;
}

uint64 mslp_init_64bit_timestamp(void)
{
  struct timeval current;
  uint64 stamp = 0;
  
  gettimeofday(&current, NULL);
  stamp = current.tv_sec;
  stamp *= 1000000;
  stamp += current.tv_usec;
  return stamp;
}


void mslp_init_accept_id_entry(accept_id_entry *entry, uint8 *url)
{
  assert(entry != NULL);
  assert(url != NULL);
  if(entry && url) {
    entry->timestamp = mslp_init_64bit_timestamp();
    entry->url_len = strlen(url);
    entry->url = calloc(entry->url_len + 1, sizeof(uint8));
    memcpy(entry->url, url, entry->url_len);
  }
}


BOOL mslpStuffAcceptIDEntry(uint8 **buf, int16 *len, accept_id_entry *entry)
{
  assert(buf && (*buf));
  assert(len && (*len > 10 ));
  assert(entry);

  if(buf == NULL || *buf == NULL)
    return FALSE;
  if(len == NULL || (*len) <= 10)
    return FALSE;
  if(entry == NULL)
    return FALSE;
  
  memset(*buf, 0x00, *len);
  _LSLP_SETLONGLONG(*buf, entry->timestamp, 0);
  (*buf) += 8;
  (*len) -= 8;
  if((*len) >= (2  + entry->url_len )) {
    _LSLP_SETSHORT(*buf, entry->url_len, 0);
    if(entry->url_len)
      memcpy((*buf) + 2, entry->url, entry->url_len);
    (*buf) += (2 + entry->url_len);
    (*len) -= (2 + entry->url_len);
    return TRUE;
  }
  return FALSE;
}


BOOL mslpStuffFwdExt(uint8 **buf, int16 *len, mesh_fwd_ext *ext)
{
  assert(buf && (*buf));
  assert(len && (*len >= 22 ));
  assert(ext);
  
  if(buf == NULL || *buf == NULL)
    return FALSE;
  if(len == NULL || *len < 22)
    return FALSE;
  if(ext == NULL)
    return FALSE;
  
  memset(*buf, 0x00, *len);
  _LSLP_SETSHORT(*buf, 0x0006, 0);
  (*buf) += 2;
  (*len) -= 2;
  
  _LSLP_SETNEXTEXT(*buf, ext->next_ext, 0);
  _LSLP_SETBYTE(*buf, ext->fwd_id, 3);
  (*buf) += 4;
  (*len) -= 4;
  _LSLP_SETLONGLONG(*buf, ext->version_timestamp, 0);
  (*buf) += 8;
  (*len) -= 8;

  return mslpStuffAcceptIDEntry(buf, len, &(ext->accept_entry));
  
}

mesh_fwd_ext *mslpUnstuffFwdExt(uint8 **buf, int16 *len)
{
  mesh_fwd_ext *ext = NULL;
  
  assert(buf && (*buf));
  assert(len && (*len >= 24));
  if(buf == NULL || *buf == NULL)
    return FALSE;
  if(len == NULL || *len < 24)
    return FALSE;

  if( LSLP_MESHCTRL_EXT != _LSLP_GETSHORT(*buf, 0) ) {
    
    return NULL;
    
  }
  
  ext = (mesh_fwd_ext *)calloc(sizeof(mesh_fwd_ext), sizeof(uint8));
  if(ext) {
    
    ext->next_ext = _LSLP_GETNEXTEXT(*buf, 2);
    ext->fwd_id = _LSLP_GETBYTE(*buf, 5);
    
    (*buf) += 6;
    (*len) -= 6;
    
    // (uint64)ext->version_timestamp = _LSLP_GETLONGLONG(*buf, 0);
    ext->version_timestamp = _LSLP_GETLONGLONG(*buf, 0); // JSS
    (*buf) += 8;
    (*len) -= 8;
    
    ext->accept_entry.timestamp = _LSLP_GETLONGLONG(*buf, 0);
    (*buf) += 8;
    (*len) -= 8;

    ext->accept_entry.url_len = _LSLP_GETSHORT(*buf, 0);
    if( ext->accept_entry.url_len ) {
      if( *len >= ext->accept_entry.url_len + 2) {
	ext->accept_entry.url = calloc(ext->accept_entry.url_len, sizeof(uint8));
	if(ext->accept_entry.url ) {
	  memcpy(ext->accept_entry.url, (*buf) + 2, ext->accept_entry.url_len);
	}
      }
    }
  }
  return ext;
  
}


mesh_fwd_ext *mslpExtractFwdExt(lslpWork *w)
{
  uint8 *ext;
  int16 len;
  
  int32 offset = LSLP_NEXT_EX;
  ext = lslpGetNextExtension(w, &offset);
  while(NULL != ext) {
    if(LSLP_MESHCTRL_EXT == _LSLP_GETSHORT(ext, 0) ) {
      len = w->na.data_len;
      len -= offset ;
      return mslpUnstuffFwdExt(&ext, &len ) ;
    }
    ext = lslpGetNextExtension(w, &offset);
  }
  return NULL;
}


#ifdef MSLP_TEST_64_BIT_OPS

int main(int argc, char **argv) 
{

  uint64 a, b, c, d;
  BOOL ccode;
  uint8 buf[512];
  uint8 *bptr;
  
  mesh_fwd_ext fwd_ext, *test_ext;
  uint8 test_url[] = "service:nothing://mdday.raleigh.ibm.com/another";
  int16 len = 512;
  
  
  a = 0x3db5a96e329e4LL;
  b = 0x3db5a96e00000LL;
  printf("value of b %qx\n", b);
  
  _LSLP_SETLONGLONG(&c, b, 0);

  d = _LSLP_GETLONGLONG((uint8*)&c, 0); 
  

  printf("value of d %qx\n", d);
  d += a;
  printf("value of d + a: %qx\n", d);
  
  fwd_ext.next_ext=0xff;
  fwd_ext.fwd_id = 2;
  fwd_ext.version_timestamp = mslp_init_64bit_timestamp();
  printf("value of version_timestamp is %#qx\n", (uint64)fwd_ext.version_timestamp);
  
  fwd_ext.accept_entry.timestamp = mslp_init_64bit_timestamp();
  fwd_ext.accept_entry.url_len = strlen(test_url);
  fwd_ext.accept_entry.url = strdup(test_url);
  bptr = buf;
  
  ccode = mslpStuffFwdExt(&bptr, &len, &fwd_ext);
  bptr = buf;
  len = 512;
  
  test_ext = mslpUnstuffFwdExt(&bptr, &len);
  printf("value of version_timestamp is %#qx\n", (uint64)test_ext->version_timestamp);
  

  return ccode;
  
}

#endif /* test 64-bit ops */

