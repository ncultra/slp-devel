
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
 *  Description: slp message handlers
 *
 *****************************************************************************/




#include "lslp-msg.h"

extern LSLP_SEM_T lslpPrintSem;
extern int8 *lslp_logPath;
extern int32 lslp_num_remote_das;
extern lslpDirAgentHead lslpDAs;
extern lslpDirAgentHead lslpSAs;
extern lslpDirAgentHead lslpRemoteDAs;
extern lslpDirAgentHead lslpRemoteSAs;
extern LSLP_SEM_T xid_sem;
extern LSLP_CONFIG_ADV lslpConfigTransport; /* transport configuration */
extern LSLP_CONFIG_ADV lslpConfigListen;
extern lslpWork workHeads[LSLP_QS];
extern lslpStream streamInHead;
extern lslpStream connectHead ;
extern int pipe_fd;
extern BOOL pipe_open;

struct timeval peerWait = {0, 0}; /* semi-random peer wait interval*/
BOOL mesh_data_sync = FALSE; 

void (*_mesh_trans[MESH_MAX_STATES + 1][MESH_MAX_STATES + 1])(lslpStream *, void *) = {
  { 
    _mesh_start_start,                /* start, start */
    _mesh_start_connect,              /* start, connect */
    _mesh_start_accept,               /* start, accept */
    _mesh_invalid,                    /* start, txpeer */
    _mesh_invalid,                    /* start, rxpeer */
    _mesh_invalid,                    /* start, txda */
    _mesh_invalid,                    /* start, rxda */
    _mesh_invalid,                    /* start, sync */
    _mesh_error
  } ,                      /* start, error */

  {
    _mesh_invalid,                    /* connect, start */
    _mesh_invalid,                    /* connect, connect */
    _mesh_invalid,                    /* connect, accept */
    _mesh_connect_txpeer,             /* connect, txpeer */
    _mesh_connect_rxpeer,              /* connect, rxpeer - race condition */
    _mesh_invalid,                    /* connect, txda */
    _mesh_invalid,                    /* connect, rxda */
    _mesh_invalid,                    /* connect, sync */
    _mesh_error
  },                      /* connect, error */

  {
    _mesh_invalid,                    /* accept, start */
    _mesh_invalid,                    /* accept, connect */
    _mesh_invalid,                    /* accept, accept */
    _mesh_invalid,                    /* accept, txpeer */
    _mesh_accept_rxpeer,              /* accept, rxpeer */
    _mesh_invalid,                    /* accept, txda */
    _mesh_invalid,                    /* accept, rxda */
    _mesh_invalid,                    /* accept, sync */
    _mesh_error
  },                      /* accept, error */

  {  
    _mesh_invalid,                    /* txpeer, start */
    _mesh_invalid,                    /* txpeer, connect */
    _mesh_invalid,                    /* txpeer, accept */
    _mesh_invalid,                    /* txpeer, txpeer */
    _mesh_txpeer_rxpeer,              /* txpeer, rxpeer - race condition */
    _mesh_txpeer_txda,                /* txpeer, txda */
    _mesh_invalid,                    /* txpeer, rxda */
    _mesh_invalid,                    /* txpeer, sync */
    _mesh_error
  },                      /* txpeer, error */
    
  {
    _mesh_invalid,                    /* rxpeer, start */
    _mesh_invalid,                    /* rxpeer, connect */
    _mesh_invalid,                    /* rxpeer, accept */
    _mesh_invalid,                    /* rxpeer, txpeer */
    _mesh_invalid,                    /* rxpeer, rxpeer */
    _mesh_invalid,                    /* rxpeer, txda */
    _mesh_rxpeer_rxda,                /* rxpeer, rxda */
    _mesh_invalid,                    /* rxpeer, sync */
    _mesh_error
  },                      /* rxpeer, error */

  {
    _mesh_invalid,                    /* txda, start */
    _mesh_invalid,                    /* txda, connect */
    _mesh_invalid,                    /* txda, accept */
    _mesh_invalid,                    /* txda, txpeer */
    _mesh_txda_rxpeer,                /* txda, rxpeer - race condition */
    _mesh_invalid,                    /* txda, txda */
    _mesh_txda_rxda,                  /* txda, rxda */
    _mesh_invalid,                    /* txda, sync */
    _mesh_error
  },                      /* txda, error */

  {
    _mesh_invalid,                    /* rxda, start */
    _mesh_invalid,                    /* rxda, connect */
    _mesh_invalid,                    /* rxda, accept */
    _mesh_invalid,                    /* rxda, txpeer */
    _mesh_invalid,                    /* rxda, rxpeer */
    _mesh_rxda_txda,                  /* rxda, txda */
    _mesh_invalid,                    /* rxda, rxda */
    _mesh_invalid,                    /* rxda, sync */
    _mesh_error
  },                      /* rxda, error */

  {
    _mesh_sync_start,                 /* sync, start */
    _mesh_invalid,                    /* sync, connect */
    _mesh_invalid,                    /* sync, accept */
    _mesh_invalid,                    /* sync, txpeer */
    _mesh_invalid,                    /* sync, rxpeer */
    _mesh_invalid,                    /* sync, txda */
    _mesh_invalid,                    /* sync, rxda */
    _mesh_invalid,                    /* sync, sync */
    _mesh_error
  },                      /* sync, error */

  {
    _mesh_invalid,                    /* error, start */
    _mesh_invalid,                    /* error, connect */
    _mesh_invalid,                    /* error, accept */
    _mesh_invalid,                    /* error, txpeer */
    _mesh_invalid,                    /* error, rxpeer */
    _mesh_invalid,                    /* error, txda */
    _mesh_invalid,                    /* error, rxda */
    _mesh_invalid,                    /* error, sync */
    _mesh_error
  }                      /* error, error */
};


#ifdef __linux
extern volatile sig_atomic_t dieNow;
#else
extern uint32 dieNow ;
#endif
extern uint32 waitInterval ;


/* this routine does not allow collecting of multiple responses by linking */
/* them to the original request. That is a UA function that I don't anticipate */
/* handling in this kernel. The only convergence requests I anticipate are */
/* active DA discoveries, which we can handle on a per-response basis. */
void  lslpCleanPendingQ(lslpWork *rply)
{
  lslpWork *rq ;
  lslpWork destroyList = {&destroyList, &destroyList, TRUE};
  assert(rply != NULL) ;
  if(LSLP_WAIT_OK == _lockList(LSLP_Q_PENDING)) {
    rq = workHeads[LSLP_Q_PENDING].next;
    while( ! _LSLP_IS_HEAD(rq)) {
      if(rq->hdr.xid == rply->hdr.xid) {
	/* check to see if this is a convergence request */
	if ((rq->status & LSLP_STATUS_CONVERGE) || 
	    (rq->status & LSLP_STATUS_MCAST) || 
	    (rq->status & LSLP_STATUS_BCAST)) {
	  assert(!(rq->status & LSLP_STATUS_TCP));
	  /* ok - this is a convergence request, now check to */
	  /* see if we need to rewrite the pr list */
	  if((rq->hdr.msgid == LSLP_SRVRQST) || 
	     (rq->hdr.msgid == LSLP_ATTRREQ) || 
	     (rq->hdr.msgid == LSLP_SRVTYPERQST)) {
	    /* we need to rewrite the pr list */
	    /* what happens now is that this request remains on the pending q */
	    /* upon timeout, the pending q will resend this request including */
	    /* the pr list that we are updating here. ultimately this request */
	    /* will expire and the pending q will destroy it. */
	    int8 *aptr, *dptr;
	    int16 room, needed, prlen;
	    room = LSLP_LSL_MTU - rq->na.data_len;
	    needed = (1 + strlen((aptr = inet_ntoa(rply->na.rmtAddr.sin_addr))));
	    if(needed <= room) {
	      dptr = (int8 *)rq->hdr.data;
	      dptr += (_LSLP_HDRLEN_WORK(rq));
	      /* update the pr list len */
	      prlen = _LSLP_GETSHORT(dptr, 0);
	      prlen += needed;
	      _LSLP_SETSHORT(dptr, prlen, 0);
	      /* move the existing msg buf by the size of the new addr */
	      dptr += sizeof(int16);
	      memmove(dptr + needed , dptr, 
		      (_LSLP_GETLENGTH((uint8 *)rq->hdr.data) - _LSLP_HDRLEN_WORK(rq) - 2));
	      /* insert the new addr */
	      memcpy(dptr, aptr, needed - 1);
	      if(prlen - needed) 
		*(dptr + needed  - 1) = ',';
	      else
		*(dptr + needed - 1) = 0x00;
	      /* update the msg hdr and work hdr */
	      rq->na.data_len += needed;
	      _LSLP_SETLENGTH(((uint8 *)rq->hdr.data), rq->na.data_len);
	      rq->hdr.len = rq->na.data_len;
	    } /* if there is room to expand the pr list */
	  } /* if the rq has a pr list */
	} else { 
	  /* this is not a convergence request */
	  /* unlink and free rq */
	  lslpWork *temp;
	  temp = rq;
	  rq = rq->next;
	  _LSLP_UNLINK(temp);
	  _LSLP_INSERT(temp, &destroyList)
	    continue;
	}
      } /* if we found a match */
      rq = rq->next;
    } /* while we are traversing the pending q */
    _unlockList(LSLP_Q_PENDING);
  } /* if we locked the pending q */
  rq = destroyList.next;
  while(! _LSLP_IS_HEAD(rq)) {
    _LSLP_UNLINK(rq);
    rq->txerr(rq);
    rq = destroyList.next;
  }
  return ;
}


void lslpHandleRemoteSrvRply(lslpWork *work) 
{
  int8 *bptr;
  lslpWork *rply;
  int16 numURLs = 0;

  assert(work != NULL);
  /* traverse once to get the url count */
  rply = (lslpWork *)work->rply.next; 
  while(! _LSLP_IS_HEAD(rply)) {
    numURLs++;
    rply = rply->next;
  }
  /* write the number of urls */
  if( 0 <  (work->hdr.errCode = connectedWrite(work->apiSock, &numURLs, sizeof(int16)))) {
  
  /* traverse again to write the url entries */
    if(numURLs) {
      rply = (lslpWork *)work->rply.next; 
      while(numURLs && ! _LSLP_IS_HEAD(rply)) {
	bptr = rply->hdr.data;
	bptr += (( _LSLP_HDRLEN_WORK(rply)) + (2 * sizeof(int16)));
	/* we know these responses will always be udp because they came in as the */
	/* result of a multicast or broadcast request */
	if( 0 < (work->hdr.errCode =connectedWrite(work->apiSock, bptr, 
				(rply->na.data_len - 
				 (( _LSLP_HDRLEN_WORK(rply)) + (2 * sizeof(int16))))))) {
	  break;
	}
	rply = rply->next;
	numURLs--;
      }
    }
  }
  work->txerr(work);
}

void  lslpHandleSrvRply(lslpWork *work) 
{
  lslpCleanPendingQ(work);
  work->txerr(work);
  
  return;
}

/* service request tunnelled to us by the api port */
void lslpHandleRemoteSrvReq(lslpWork *work)
{
  lslpWork *rmtmsg;
  int8 *bptr;
  assert(work != NULL);
  if(NULL != (rmtmsg = allocWorkNode())) {
    rmtmsg->type = LSLP_Q_SLP_OUT;
    rmtmsg->apiSock = work->apiSock;
    rmtmsg->status = (LSLP_STATUS_API | LSLP_STATUS_RQ);
    /* marshall data from the input buffer */
    bptr = work->hdr.data;
    memcpy(&(rmtmsg->na.rmtAddr), bptr, sizeof(SOCKADDR_IN));
    bptr += sizeof(SOCKADDR_IN);
    rmtmsg->hdr.flags = _LSLP_GETLONG(bptr, 0);
    if(rmtmsg->hdr.flags &  LSLP_FLAGS_MCAST) {
      rmtmsg->status |= (LSLP_STATUS_CONVERGE | LSLP_STATUS_MCAST | LSLP_STATUS_UDP);
      rmtmsg->na.rmtAddr.sin_port = htons(lslp_config_opport);
      rmtmsg->na.rmtAddr.sin_addr.s_addr = _LSLP_MCAST;
    } /* if this is a multicast srvreq */
    rmtmsg->hdr.langLen = _LSLP_GETSHORT(bptr, 4);
    bptr += 6;
    memcpy(rmtmsg->hdr.lang, bptr, rmtmsg->hdr.langLen);
    bptr += rmtmsg->hdr.langLen;
    rmtmsg->hdr.ver = LSLP_PROTO_VER;
    rmtmsg->hdr.msgid = LSLP_SRVRQST;
    rmtmsg->hdr.xid = _lslp_get_xid();
    rmtmsg->hdr.data = &(rmtmsg->na.static_data[0]);
    /* bptr points to slp msg data. what is the size of slp message data ? */
    rmtmsg->na.data_len = work->na.data_len;
    rmtmsg->na.data_len -= (bptr - ((int8 *)work->hdr.data) );
    if(rmtmsg->na.data_len > (lslp_config_mtu - (_LSLP_HDRLEN_WORK(rmtmsg)))) {
      /* error */
      if(! ((work->status & LSLP_STATUS_BCAST)  ||
	    (work->status & LSLP_STATUS_MCAST) ||
	    (work->status & LSLP_STATUS_CONVERGE))) {
	tcpInternalErr(work->apiSock, rmtmsg->hdr.xid, LSLP_PARSE_ERROR);
      }
      _LSLP_CLOSESOCKET(work->apiSock);
      rmtmsg->txerr(rmtmsg);
      return;
    }
    bptr = formSLPMsg(&(rmtmsg->hdr), bptr, rmtmsg->na.data_len, FALSE);
    if(LSLP_WAIT_OK == _insertWorkNode(rmtmsg)) {
      return;
    }
       rmtmsg->txerr(rmtmsg);
  } /* if we allocated our remote msg work node */
  if(! ((work->status & LSLP_STATUS_BCAST)  ||
	(work->status & LSLP_STATUS_MCAST) ||
	(work->status & LSLP_STATUS_CONVERGE))) {
    tcpInternalErr(work->apiSock, 0, LSLP_INTERNAL_ERROR);
  }
    return;
}


void lslpHandleSimpleSrvReg(lslpWork *work)
{
  int8 ack[LSLP_API_HDR_SIZE];
  int8 *bptr; 
  int16 tempLen, errCode;
  lslpSrvRegList *sreg;
  assert(work != NULL);
  if(NULL != (sreg = lslpAllocSrvReg())) {
    errCode = LSLP_OK;
    bptr = work->hdr.data;
    sreg->url->lifetime = _LSLP_GETSHORT(bptr, 0);

    bptr += sizeof(int16);
    if(0 < (tempLen = strlen(bptr))) {
     sreg->srvType = strdup(bptr);
      bptr += (tempLen + 1);
      if(0 < (tempLen = strlen(bptr))) {
	sreg->url->url = strdup(bptr);
	sreg->url->atomized = _lslpDecodeURLs(&bptr, 1);
	sreg->url->len = strlen(sreg->url->url) + 1;
	bptr += (tempLen + 1);
	if(0 < (tempLen = strlen(bptr))) { 
	  lslpAttrList *temp = _lslpDecodeAttrString(bptr);
	  if(NULL != temp) {
	    lslpFreeAttrList(sreg->attrList, TRUE);
	    sreg->attrList = temp;
	  } else {errCode = LSLP_PARSE_ERROR;}
	} /* if there is an attribute string */
      } else {errCode = LSLP_PARSE_ERROR;} /* if there is a url length */
    } else {errCode = LSLP_PARSE_ERROR;} /* if there is a srv type string */
    if(errCode == LSLP_OK) {
      int8 scopes[9];
      lslpScopeList *temp;
      memcpy(&scopes[0], "DEFAULT\0", 8);
      tempLen = 9;
      bptr = &scopes[0];
      temp = lslpScopeStringToList(bptr, 8);
      if(NULL != temp) {
	lslpWork *fwd;
	time_t fwd_time;
	lslpFreeScopeList(sreg->scopeList);
	sreg->scopeList = temp;
	/* mesh-enhanced: forward this srvreg to our mesh peers */
	/* maybe I should check that we have peers to fwd to before doing the work ? */
	sreg->url->lifetime += time(&fwd_time);
	if( NULL != (fwd = lslpCreateSrvReg(sreg, &(work->na.rmtAddr) ))) {
	  _lslpMeshFwdSrvReg(fwd) ;
	  fwd->txerr(fwd);
	}
	sreg->url->lifetime -= fwd_time;
	if(lslp_config_is_da) {
	  /* this call either frees or links the reg for us */
	  errCode = _lslpCheckScopeAndLink(sreg, (lslpDirAgent *)&lslpDAs);
	} else {
	  errCode = _lslpCheckScopeAndLink(sreg, (lslpDirAgent *)&lslpSAs);
	}
	if(errCode == LSLP_OK)
	  sreg = NULL;
      } /* if we should try to link */
    }  else { ; } /* if we should try to fabricate the scope list */
  } else{errCode = LSLP_INTERNAL_ERROR;} /* if we allocated our srvreg */
  /* write the ack back to the api client */
  _LSLP_SETLONG(&ack[0], LSLP_API_HDR_VERSION, LSLP_API_OFFSET_VER);
  memcpy(&(ack[LSLP_API_OFFSET_SIG]), LSLP_API_SIGNATURE, LSLP_API_SIGNATURE_SIZE);
  _LSLP_SETSHORT(&ack[0], LSLP_API_HDR_SIZE, LSLP_API_OFFSET_LEN);
  _LSLP_SETSHORT(&ack[0], errCode, LSLP_API_OFFSET_ERR);
  _LSLP_SETSHORT(&ack[0], LSLP_API_ACK, LSLP_API_OFFSET_ID);
  connectedWrite(work->apiSock, (void *)&ack[0], LSLP_API_HDR_SIZE);
  _LSLP_CLOSESOCKET(work->apiSock);
  if(sreg != NULL)
    lslpFreeSrvReg(sreg);
}

void lslpHandleSimpleSrvReq(lslpWork *work) 
{
  int16 tempLen;
  int8 ah[LSLP_API_HDR_SIZE];
  int8 *bptr;
  struct lslp_srv_rply err;
  struct lslp_srv_req *sre = NULL;
  struct lslp_srv_rply *srp = NULL;
  int8 twobytes[] = {0x00, 0x00};
  assert(work != NULL);
  memset(&err, 0x00, sizeof(struct lslp_srv_rply));
  if(work->na.data_len  > 10 ) {
     if (NULL != (sre = (struct lslp_srv_req *)calloc(1, sizeof(struct lslp_srv_req)))) {
       if(NULL != (sre->srvcType = strdup(work->hdr.data))) {
	 sre->srvcTypeLen = strlen(sre->srvcType);
	 if(NULL == (srp = _lslpProcessSrvReq(sre, 0))) {
	   err.errCode = LSLP_INTERNAL_ERROR;
	 } /* if we couldn't get an srp */
       } else { err.errCode = LSLP_INTERNAL_ERROR;}  /* if we duped the service type string */
       lslpDestroySrvReq(sre, LSLP_DESTRUCTOR_DYNAMIC);
     } /* if we allocated our request struct */
  } /* if we are at least the bare minimum size */
  tempLen = LSLP_API_HDR_SIZE;
  _LSLP_SETLONG(&ah[0], LSLP_API_HDR_VERSION, LSLP_API_OFFSET_VER);
  memcpy(&ah[LSLP_API_OFFSET_SIG], LSLP_API_SIGNATURE, LSLP_API_SIGNATURE_SIZE);
  if(srp != NULL) {
    if(srp->urlCount && (srp->urlList != NULL)) {
      tempLen += 4 + 2 + srp->urlLen;
    }
  } 
  _LSLP_SETSHORT(&ah[0], tempLen, LSLP_API_OFFSET_LEN);
  _LSLP_SETSHORT(&ah[0], err.errCode, LSLP_API_OFFSET_ERR);
  _LSLP_SETSHORT(&ah[0], LSLP_SIMPLE_SRVRPLY, LSLP_API_OFFSET_ID);
  connectedWrite(work->apiSock, (void *)&ah[0], LSLP_API_HDR_SIZE); 
  tempLen -= LSLP_API_HDR_SIZE;
  if(srp != NULL) {
    if(srp->urlCount && srp->urlList != NULL) {
      int16 count, errCode;
      lslpURL *tempURL;
      /* write the error code and number of urls */
      connectedWrite(work->apiSock, (void *)srp->urlList, sizeof(int32));
      tempLen -= sizeof(int32);
      bptr = srp->urlList + sizeof(int32);
      count = srp->urlCount;
      while(count--) {
	tempURL = lslpUnstuffURL(&bptr, &(srp->urlLen), &errCode);
	if(tempURL != NULL && tempURL->url != NULL) {
	  connectedWrite(work->apiSock, tempURL->url, tempURL->len);
	  tempLen -= tempURL->len;
	  lslpFreeURL(tempURL);
	  //	  bptr += sizeof(int32);
	} else{ count = 0; }
      }
      connectedWrite(work->apiSock, (void *)&twobytes[0], 2);
      tempLen -= 2;
    }
    lslpDestroySrvRply(srp, LSLP_DESTRUCTOR_DYNAMIC);
  }  else { /* if we have our reply */
    err.errCode = htons(err.errCode);
    connectedWrite(work->apiSock, (void *)&err, sizeof(struct lslp_srv_rply));
    tempLen -= sizeof(struct lslp_srv_rply);
  } /* we didn't get our reply */

  /* this is really stupid but not easy to fix without pre-allocating a big buffer */
  while(tempLen--) {
    connectedWrite(work->apiSock, (void *)&twobytes[0], 1);
  }
  _LSLP_CLOSESOCKET(work->apiSock);
  return;
}

void lslpHandleSrvReq(lslpWork *work) 
{
  int8 *bptr;
  int16 len, errCode;
  struct lslp_srv_req *sre;
  struct lslp_srv_rply *srp;
  assert(work != NULL && work->hdr.data != NULL);
  bptr = work->hdr.data;
  len = (work->hdr.len - (_LSLP_HDRLEN_WORK(work)));
  bptr += _LSLP_HDRLEN_WORK(work);
  if(len > 10)  {
    sre = (struct lslp_srv_req *)calloc(1, sizeof(struct lslp_srv_req));
    if(sre != NULL) { 
      sre->prListLen = _LSLP_GETSHORT(bptr, 0);

      /* don't allocate the pr list if it is not present in the request << Wed May  5 10:38:22 2004 mdd >> */
      if(((len - 2) >= sre->prListLen) && (sre->prList = (int8 *)calloc(sre->prListLen + 1, sizeof(int8)))) {
	memcpy(sre->prList, bptr + 2, sre->prListLen);
	/* if we are on the previous responder list, discard the message */
	if(TRUE == lslpPreviousResponder(sre->prListLen, sre->prList)) {
	  lslpDestroySrvReq(sre, LSLP_DESTRUCTOR_DYNAMIC);
	  work->txerr(work);
	  return;
	}
	

	{
	  /* now that we know we must process this request, check for the attrlist extension */ 
	  int8* extension;
	  int32 offset = LSLP_NEXT_EX;
	  extension = lslpGetNextExtension(work, &offset);
	  while(NULL != extension) {
	    if(0x0002 == _LSLP_GETSHORT(extension, 0)) {
	      sre->attr_ext_present = TRUE;
	    }
	    extension = lslpGetNextExtension(work, &offset);
	  }
	}
	
	len -= (2 + sre->prListLen);
	bptr += (2 + sre->prListLen);
	sre->srvcTypeLen = _LSLP_GETSHORT(bptr, 0);
	if(((len - 2) >= sre->srvcTypeLen) && 
	   (NULL != (sre->srvcType = (int8 *)calloc(sre->srvcTypeLen + 1, sizeof(int8))))) {
	  memcpy(sre->srvcType, bptr + 2, sre->srvcTypeLen);
	  len -= (2 + sre->srvcTypeLen);
	  bptr += (2 + sre->srvcTypeLen);
	  sre->scopeList = lslpUnstuffScopeList(&bptr, &len, &errCode);
	  if(! errCode) {
	    /* we will need to change the next four lines of code when we support predicate matching */
	    sre->predicateLen = _LSLP_GETSHORT(bptr, 0);
	    if(sre->predicateLen > 0) {
	      if (((len - 2) >= sre->predicateLen) && 
		  (sre->predicate = (int8 *)calloc(sre->predicateLen + 1, sizeof(int8)))) {
		memcpy(sre->predicate, bptr + 2, sre->predicateLen);
	      }
	    }
	    len -= (2 + sre->predicateLen);
	    bptr += (2 + sre->predicateLen);
	    sre->spiList = lslpUnstuffSPIList(&bptr, &len, &errCode);
	    /* check for the special srvreq that is asking for directory-agents */
	    if(! strncasecmp(sre->srvcType, lslp_da_url, 23 )) {
	      lslpWork *list = lslpHandleSpecialSrvReq(sre, LSLP_TYPE_DA, work);
	      if(list != NULL) {
		lslpWork *temp = list->next;
		while(! _LSLP_IS_HEAD(temp)) {
		  _LSLP_UNLINK(temp) ;
		  temp->txerr = stream_txerr;
		  if(LSLP_WAIT_OK != _insertWorkNode(temp)) {
		    errCode = LSLP_INTERNAL_ERROR;
		    break;
		  }
		  temp = list->next;
		}
		temp = list->next;
		while(! _LSLP_IS_HEAD(temp)) {
		  _LSLP_UNLINK(temp);
		  temp->txerr(temp);
		  temp = list->next;
		}
		list->txerr(list);
		if(errCode == LSLP_OK) {
		  lslpDestroySrvReq(sre, LSLP_DESTRUCTOR_DYNAMIC);
		  work->txerr(work);
		  return;
		}
	      } else { /* if we got a list or replies */
		lslpDestroySrvReq(sre, LSLP_DESTRUCTOR_DYNAMIC);
		work->txerr(work);
		return;
	      }
	    } /* special da srvreq case */
	    /* also check for the special srvreq that is asking for service-agents */
	  } /* if the scope list was OK */
	}else {errCode = LSLP_INTERNAL_ERROR;} /* if we alloced the srvc type string */
      } else {errCode = LSLP_INTERNAL_ERROR;} /* if we alloced the pr list */
      
      /* all the data is marshalled, now search our local registrations */
      if(NULL == (srp = _lslpProcessSrvReq(sre, errCode))) {
	errCode = LSLP_INTERNAL_ERROR;
      }  else {
	/* if this is a multicast srvreq and there are zero urls in the reply, */
	/* drop the reply. */ 
	if((work->hdr.flags & LSLP_FLAGS_MCAST) && (srp->urlCount == 0 )) {
	  lslpDestroySrvReq(sre, LSLP_DESTRUCTOR_DYNAMIC);
	  lslpDestroySrvRply(srp, LSLP_DESTRUCTOR_DYNAMIC);
	  work->txerr(work);
	  return;
	}
	work->hdr.msgid =  LSLP_SRVRPLY;
	work->hdr.errCode = errCode;

	if(NULL != lslpMakeRply(work, (void *)srp->urlList, srp->urlLen, srp->attr_ext_offset)) {
	  /*work->hdr.data has a fully initialized service reply message */
	  /* link the node */
	  work->txerr = stream_txerr;
	  if(LSLP_WAIT_OK != _insertWorkNode(work)) {
	    errCode = LSLP_INTERNAL_ERROR;
	  }

	} else {errCode = LSLP_INTERNAL_ERROR; }/* if we got our reply structure initialized */      
	lslpDestroySrvRply(srp, LSLP_DESTRUCTOR_DYNAMIC);
      }
      lslpDestroySrvReq(sre, LSLP_DESTRUCTOR_DYNAMIC);
    } else {errCode = LSLP_INTERNAL_ERROR;}  /* if we alloced the msg */
  } else {errCode = LSLP_PARSE_ERROR;} /* if the buffer is not too short */
  if(errCode) {
    if(! ((work->status & LSLP_STATUS_BCAST)  ||
	  (work->status & LSLP_STATUS_MCAST) ||
	  (work->status & LSLP_STATUS_CONVERGE))) {
      if(work->status & LSLP_STATUS_TCP)
	tcpInternalErr(work->ses_idx, work->hdr.xid, (uint8)errCode);
      else 
	udpInternalErr(&(work->na.rmtAddr), 
			&(work->na.myAddr), 
			work->hdr.xid, (uint8)errCode);
    }
    work->txerr(work);
  }
  return;
}

/* for handling a DA request, which is a special form of service request  */
/* don't lock the local DA list - it won't change while we are running */
/* and we are not going to add or remove registrations */
lslpWork *lslpHandleSpecialSrvReq(struct lslp_srv_req *r, 
				   int type, 
				   lslpWork *work) 
{

  lslpWork *temp, *head = NULL;
  assert(r != NULL);
  assert((type == LSLP_TYPE_DA) || (type == LSLP_TYPE_SA));
  if(type == LSLP_TYPE_DA) {
    lslpDirAgent *da;
    da = lslpDAs.next;
    while (! _LSLP_IS_HEAD(da)) {
      /* check scope, spi */
      if( (r->scopeList == NULL) || (_LSLP_IS_EMPTY(r->scopeList)) || (TRUE == lslp_scope_intersection(r->scopeList, da->scopeList))) {
	if(TRUE == lslp_predicate_match(da->attrList, r->predicate)) {
	  /* when we implement authentication, we need to be selective */
	  /* about the auth blocks we return - discriminate on */
	  /* r->spiList */
	  lslpAtomizedURL *tau = da->url;
	  if(NULL != (head = allocWorkNode())) {
	    head->isHead = TRUE;
	    head->next = head->prev = head;
	    while(tau != NULL && (! _LSLP_IS_HEAD(tau->next))) {
	      if (NULL != (temp = lslpCreateDAAdvert(da, tau->next->url, work, NULL))) {
		_LSLP_INSERT(temp, head);
	      }
	      tau = tau->next;
	    } /* while traversing da urls */
	  } /* if we got our list head */
	} /* if predicates match */
      } /* if scopes match */
      da = da->next;
    } /* while we are traversing the da list */
  } /* if we are processing a srvreq for das */
  if(head != NULL) {
    if(_LSLP_IS_EMPTY(head)) {
      head->txerr(head);
      head = NULL;
    }
  }
  return(head);
}

/* buffer must be fully stuffed for this to work */
int8 *lslpMakeRply(lslpWork *work, void *buf, int16 bufsize, uint32 next_ext)
{
  int32 hdrLen = 14 + work->hdr.langLen;
  
  if(NULL == lslpReInitWorkBuf(work, bufsize + hdrLen )) {
    if(! ((work->status & LSLP_STATUS_BCAST)  ||
	  (work->status & LSLP_STATUS_MCAST) ||
	  (work->status & LSLP_STATUS_CONVERGE))) {
      if(work->status & LSLP_STATUS_TCP)
	tcpInternalErr(work->ses_idx, work->hdr.xid, LSLP_INTERNAL_ERROR);
      else 
	udpInternalErr(&(work->na.rmtAddr), 
			&(work->na.myAddr), 
			work->hdr.xid, LSLP_INTERNAL_ERROR);
    }
  }
  work->type = LSLP_Q_SLP_OUT;
  if(work->status & LSLP_STATUS_TRUNCATED) 
    work->hdr.flags =  LSLP_FLAGS_OVERFLOW ;
  else 
    work->hdr.flags = 0;
  /* always clear nextext */
  /* << Thu Apr 29 15:33:03 2004 mdd >> change this to allow the attribute extension */
  work->hdr.nextExt = next_ext;
  work->status &= (~LSLP_STATUS_RQ);
  work->status |= LSLP_STATUS_RP;
  return(formSLPMsg(&(work->hdr), buf, bufsize, FALSE));
}


int8* lslpReInitWorkBuf(lslpWork* work, int16 newSize)
{
  /* check to see if we need to clean up the data pointer */
  if(work->hdr.data && work->hdr.data != &(work->na.static_data[0]) && 
      work->hdr.data != work->na.dyn_data) 
    {
      free(work->hdr.data);
      work->hdr.data = NULL;
    }
     
     /* check the new size and see if we can use the static buffer */
     if(newSize <= lslp_config_mtu) {
       /* use the static buffer */
       work->hdr.data = &(work->na.static_data[0]);
       memset(work->hdr.data, 0x00, lslp_config_mtu);
       work->na.data_len = newSize;
       if(work->na.dyn_data) {
	 free(work->na.dyn_data);
	 work->na.dyn_data = NULL;
       }
       return work->hdr.data;
       
     } else {
       /* new size is greater than the mtu - see if we can reuse the 
	  dynamic buffer */
       if(work->na.dyn_data) {
	 work->hdr.data = work->na.dyn_data = (uint8*)realloc(work->na.dyn_data, newSize + 1);
	 work->na.data_len = newSize;
       } else {
	 work->hdr.data = work->na.dyn_data = (uint8*)calloc(newSize + 1, sizeof(uint8));
	 work->na.data_len = newSize;
       }
     }

     /* now check for a malloc failure and reset the pointer and size if necessary */
     if(NULL == work->na.dyn_data) {
       work->hdr.data = &(work->na.static_data[0]);
       work->na.data_len = lslp_config_mtu;
       work->status |= LSLP_STATUS_TRUNCATED;
     }
     return work->hdr.data;
}

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

/* if the attr extension is present, change the initial allocation to 4k */ 
struct lslp_srv_rply *_lslpProcessSrvReq(struct lslp_srv_req *msg, int16 errCode)
{
  uint32 ccode, bufSave;
  int8 *buf, *attr_buf, *attr_buf_ptr, *next_attr_ptr;
  int16 availSave, bufLen, avail, attr_buf_size, attr_buf_remaining;
  
  lslpSrvRegList *reg;
  int alloc_chunk = 1024;
  struct lslp_srv_rply *temp_rply = (struct lslp_srv_rply *)calloc(1, sizeof(struct lslp_srv_rply));
  assert(msg != NULL);
  if(msg != NULL && temp_rply != NULL) {
    if((temp_rply->errCode = errCode) == 0) {
      lslpDirAgent *a, *head;
      if(msg->attr_ext_present == TRUE) {
	alloc_chunk = 4096;
	attr_buf = attr_buf_ptr = next_attr_ptr = (int8*)calloc(alloc_chunk, sizeof(int8));
	if(NULL == attr_buf) {
	  next_attr_ptr = NULL;
	  msg->attr_ext_present = FALSE;
	  alloc_chunk = 1024;
	  attr_buf_remaining = 0;
	} else {
	  attr_buf_remaining = attr_buf_size = alloc_chunk;
	}
	
      } else {
	attr_buf = attr_buf_ptr = next_attr_ptr = NULL;
	attr_buf_remaining = 0;
	alloc_chunk = 1024;
      }
      
      buf = (int8 *)calloc(alloc_chunk, sizeof(int8));
      if(buf != NULL) {
	bufLen = alloc_chunk;
	temp_rply->urlList = buf;
	buf += 4;
	avail = alloc_chunk - 4;
	if(lslp_config_is_da) {
	  head = (lslpDirAgent *)&lslpDAs;
	} else {
	  head = (lslpDirAgent *)&lslpSAs;
	}
	_LSLP_WAIT_SEM(head->writeSem, LSLP_Q_WAIT, &ccode); 
	assert(ccode == LSLP_WAIT_OK);
	if(ccode  == LSLP_WAIT_OK) {
	  /* see if we have any latent srv regs on the overload list */
	  a = head->next;	  
	  reg = a->srvRegList->next;
	  while(! _LSLP_IS_HEAD(reg)) {
	    if((0 != reg->url->lifetime) && (reg->url->lifetime < time(NULL))) { 
	      /* the lifetime for this registration has expired */
	      /* unlink and free it */
	      lslpSrvRegList *temp_reg = reg;
	      reg = reg->next;
	      _LSLP_UNLINK(temp_reg);
	      lslpFreeSrvReg(temp_reg);
	      continue; /* traversing reg loop */
	    }
	    if(TRUE == lslp_srvtype_match(reg->srvType, msg->srvcType)) {
	      /* we need to check for the special case - of a srvreq with service type directory-agent */
	      /* - it is allowed to omit scopes */
	      if((msg->scopeList == NULL) ||  
		 (_LSLP_IS_EMPTY(msg->scopeList) && (! strncasecmp(msg->srvcType, lslp_da_url, 23 ))) || 
		 (TRUE == lslp_scope_intersection(reg->scopeList, msg->scopeList))) {


		if(TRUE == lslp_predicate_match(reg->attrList, msg->predicate)) {
		  int8 tryCount = 16; /* allows a buffer increase of up to 16Kb, 20Kb with attr ext */
		  bufSave = (buf - temp_rply->urlList);
		  availSave = avail;
		  /* we have a match */
		  /* for protected scopes, we need to change stuff URL so that it */
		  /* only stuffs url auth blocks that match spis in the srv req */
		  while((avail < (8 + reg->url->len)) || (FALSE == lslpStuffURL(&buf, &avail, reg->url))) {
		    if((--tryCount == 0) || 
		       (NULL == (temp_rply->urlList = realloc(temp_rply->urlList, (bufLen += alloc_chunk))))) { 
		      break;
		    }
		    /* reset our variables - they were changed */
		    availSave += alloc_chunk;
		    avail = availSave;
		    buf = temp_rply->urlList + bufSave;
		  }
		  /* if we succeeded, bufLen, tempBuf, and avail contain correct values */
		  if((tryCount != 0 ) && (temp_rply->urlList != NULL)) {
		    temp_rply->urlCount++;

		    /* we were successful in stuffing the rply buffer with the url entry */
		    /* now we need to check to see if we need to stuff this url into the */
		    /* attribute extension buffer. */ 
		    /* 
		       reg->url->len contains url string len
		       reg->url->url contains url string
		       reg->attrList contains attr list head
		       attrList->attr_string_len contains attribute string length
		       attrList->attr_string contains the attribute string
		    */
		    if(msg->attr_ext_present == TRUE && 
		       attr_buf_ptr != NULL && 
		       reg->url->url != NULL && 
		       reg->attrList != NULL) {
		      if(attr_buf_remaining > 10 + reg->url->len) {
			int attr_buf_position;
			int attr_realloc_count;
			
			/* see if we need to set the next attr ptr */
			/* note - we will need to re-adjust all of the next extension offsets */
			/* to be relative to the beginning of the message. */
			/* right now they are relative to the beginning of the attribute */
			/* extension buffer */ 
			if(next_attr_ptr && next_attr_ptr != attr_buf) {
			  _LSLP_SETNEXTEXT(next_attr_ptr, (attr_buf_ptr - attr_buf), 0);
			}
			
			/* store the attr extension ID and advance the buffer pointer */
			/* beyond the next extension offset */
			next_attr_ptr = attr_buf_ptr + 2;
			_LSLP_SETSHORT(attr_buf_ptr, 0x0002, 0);
			_LSLP_SETNEXTEXT(attr_buf_ptr, 0x00000000, 2);
			
			/* advance past the extension id and the next extension offset */
			attr_buf_ptr += 5;
			attr_buf_remaining -= 5;
			
			/* store the url length and the url */
			_LSLP_SETSHORT(attr_buf_ptr, reg->url->len, 0);
			memcpy(attr_buf_ptr + 2, reg->url->url, reg->url->len);
			attr_buf_ptr += 2 + reg->url->len;
			attr_buf_remaining -= 2 + reg->url->len;
			attr_realloc_count = 1;
			attr_buf_position = attr_buf_ptr - attr_buf;
 			while ((attr_realloc_count > 0) &&  
 			       (attr_buf != NULL) && 
 			       ((attr_buf_remaining < 3) || 
  				(FALSE == lslpStuffAttrList(&attr_buf_ptr,  
							    &attr_buf_remaining,  
							    reg->attrList,  
							    NULL)))) { 
 			  attr_realloc_count--; 
 			  if(NULL != (attr_buf = (int8*)realloc(attr_buf, attr_buf_size + alloc_chunk))) { 
 			    attr_buf_size += alloc_chunk; 
 			    attr_buf_remaining += alloc_chunk; 
 			    attr_buf_ptr = attr_buf + attr_buf_position; 
 			  } 
 			  if(attr_buf == NULL ) { 
 			    free(attr_buf); 
 			    attr_buf = attr_buf_ptr = next_attr_ptr = NULL; 
 			    attr_buf_remaining = attr_buf_size = 0; 
 			    msg->attr_ext_present = FALSE; 
 			    continue; 
			    
 			  }
			} /* while realloc loop */
			/* store the number of attr auths (zero) */
 			*attr_buf_ptr = 0x00; 
 			attr_buf_ptr++; 
 			attr_buf_remaining--; 
		      } /* room for url */
		    } /* attr_ext_present == TRUE */
		  } else {
		    temp_rply->errCode = LSLP_INTERNAL_ERROR;
		    _LSLP_SIGNAL_SEM(head->writeSem);
		    return(temp_rply);
		  }
		} /* if we have a match */
	      } /* if we found a scope intersection */
	    } /* if the types match */
	    reg = reg->next;
	  } /* while we are traversing the reg list for this da */
	  _LSLP_SIGNAL_SEM(head->writeSem);
	} else {temp_rply->errCode = LSLP_INTERNAL_ERROR;}  /* if we locked the local DA list */
	 
	/* stuff the error code and url count */
	_LSLP_SETSHORT(temp_rply->urlList, temp_rply->errCode, 0);
	_LSLP_SETSHORT(temp_rply->urlList, temp_rply->urlCount, 2);
	/* resize to the actual size needed */
	/* initialize the url buffer length */
	temp_rply->urlLen = bufLen - avail;
	

	if(temp_rply->errCode == LSLP_OK && 
	   msg->attr_ext_present == TRUE && 
	   attr_buf != NULL) {
	  /* resize the attr buf to the actual size needed */
	  temp_rply->attr_buf_len = attr_buf_ptr - attr_buf;
	  temp_rply->attr_ext_buf = attr_buf; 
	} else {
	  temp_rply->attr_buf_len = 0;
	  temp_rply->attr_ext_buf = NULL;
	}
	

	if(msg->attr_ext_present && temp_rply->attr_ext_buf && temp_rply->attr_buf_len ) {
	  if( avail < temp_rply->attr_buf_len ) {
	    int new_size = temp_rply->urlLen + temp_rply->attr_buf_len + 1;
	    temp_rply->urlList = realloc(temp_rply->urlList, new_size);
	    if(temp_rply->urlList == NULL) {
	      free(temp_rply);
	      return NULL;
	    }
	  }
	  memcpy(temp_rply->urlList + (bufLen - avail),temp_rply->attr_ext_buf, temp_rply->attr_buf_len);
	  temp_rply->urlLen += temp_rply->attr_buf_len;
	  
	  free(temp_rply->attr_ext_buf);
	  temp_rply->attr_ext_buf = NULL;
	  temp_rply->attr_buf_len = 0;
	  
	  /* do a sanity check and store the offset to the first extension */
	  assert( 0x0002 == _LSLP_GETSHORT(temp_rply->urlList + (bufLen - avail) , 0) );
	  /* ext offset is relative to beginning of buffer, need to normalize later */ 
	  temp_rply->attr_ext_offset = bufLen - avail;
	}
      } /* if we alloced our buffer */
    } /* if we need to look for matches */
    else {
      if(NULL != (temp_rply->urlList = calloc(8, sizeof(int8)))) {
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


/* returns true if any one of our host addresses is on the previous responder list */
/* MODIFIES input string */
BOOL lslpPreviousResponder(int16 len, int8 *list )
{
  LSLP_CONFIG_ADV *t;

 /* under windows "s" is flagged as an unreferenced local variable */
 /* but leave it alone because non-windows glibc needs it for multithread strtok()  */
  int8 *a, *s;
  uint32 addr;
  if(list == NULL)
    return(FALSE);
  if(len == 0)
    return(FALSE);

  a = _LSLP_STRTOK(list, ",", &s);
  while(NULL != a ) {
    if(INADDR_NONE != (addr = inet_addr(a))) {
      t = lslpConfigTransport.next;
      while(! _LSLP_IS_HEAD(t)) {
	if(t->iface == addr)
	  return(TRUE);
	t = t->next;
      }
    }
    a = _LSLP_STRTOK(NULL, ",", &s);
  }
  return(FALSE);
}

void lslpHandleDAAdvert(lslpWork *work) 
{

  assert(work != NULL);
  _lslpCreateDirAgentFromAdvert(work) ;
  lslpCleanPendingQ(work);
  work->txerr(work);
  
  return; 
}



/* in order to only keep one list locked at a time, and to avoid a deadlock */
/* we will first traverse the meshIn list and copy those peers to which we */
/* should forward local services. After that, we will traverse the local */
/* registration list and forward the appropriate registrations. */
void _lslpRegisterLocalServices(lslpStream *list)
     
{
  int32 ccode;
  lslpStream *rmtDA;
  lslpDirAgent  *localList, *localHead;
  lslpWork *msg;
  lslpSrvRegList *reg;
  SOCKADDR_IN rmtAddr;
  static int8 scopeListBuf[512];
  lslpStream fwdList = {&fwdList, &fwdList, TRUE};
  lslpWork msgList = {&msgList, &msgList, TRUE};
  lslpWork destroyList = {&destroyList, &destroyList, TRUE};

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  
  _LSLP_WAIT_SEM(list->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    rmtDA = list;
    while(! _LSLP_IS_HEAD(rmtDA->next)) {
      rmtDA = rmtDA->next;
      if( rmtDA->status & LSLP_DA_NEEDS_REGS ) {
	lslpStream *temp = lslpAllocStream(&rmtDA->peerAddr);
	if(NULL != temp) {
	  int16 errCode;
	  int8 *bptr = &(scopeListBuf[0]);
	  int16 len = 511;
	  if(rmtDA->scopeList != NULL) {
	    lslpStuffScopeList(&bptr, &len, rmtDA->scopeList);
	    temp->scopeList = lslpUnstuffScopeList(&bptr, &len, &errCode);
	  }
	  temp->status = rmtDA->status;
	  temp->status |= LSLP_DA_KEEP_SOCKET;
	  temp->peerSock = rmtDA->peerSock;
	  _LSLP_INSERT(temp, &fwdList) ;
	  temp = NULL;
	  rmtDA->status &= (~LSLP_DA_NEEDS_REGS);
	} /* if we allocated the peer copy  */
      } /* if the peer needs to get our regs */
    } /* while we are traversing the peer list */ 
    _LSLP_SIGNAL_SEM(list->sem);  
  } /* if we locked the peer list */


  if(lslp_config_is_da) {
    localHead = (lslpDirAgent *)&(lslpDAs);
  } else {
    localHead = (lslpDirAgent *)&(lslpSAs);
  }
  rmtDA = &fwdList;
  while(! _LSLP_IS_HEAD(rmtDA->next)) {
    rmtDA = rmtDA->next;
    /* lock the local agent */
    _LSLP_WAIT_SEM(localHead->writeSem, LSLP_Q_WAIT, &ccode);
    assert(ccode == LSLP_WAIT_OK);
    if(ccode == LSLP_WAIT_OK) {
      localList = localHead;
      while(! _LSLP_IS_HEAD(localList->next)) {
	localList = localList->next;
	reg = (lslpSrvRegList *)localList->srvRegList;
	while(! _LSLP_IS_HEAD(reg->next)) {
	  reg = reg->next;
	  if(lslp_scope_intersection(reg->scopeList, rmtDA->scopeList )) {

	    memcpy(&rmtAddr, &(rmtDA->peerAddr), sizeof(SOCKADDR_IN));
	    if(NULL != (msg = lslpCreateSrvReg(reg, &rmtAddr))) {
	      if(rmtDA->status & LSLP_DA_PEER_CONNECTED) {
		/* mesh-enhanced */
		/* if this guy is a peer, dump the reg to him over the peer connection */
		msg->ses_idx = rmtDA->peerSock;
		msg->status |= (LSLP_STATUS_TCP | LSLP_STATUS_RQ |
				LSLP_STATUS_DONT_LINGER | 
				LSLP_STATUS_KEEP_SOCKET | 
				LSLP_STATUS_PEER_SOCKET ) ;
		msg->txerr = stream_txerr ; 
	      } else {
		/* otherwise, use UDP  */
		msg->ses_idx = _getSession(&(msg->na));	/* potential deadlock here !!! */
	      }
	      if(-1 != msg->ses_idx) {
		msg->type = LSLP_Q_SLP_OUT;
		_LSLP_INSERT(msg, &msgList);
	      } else {
		_LSLP_INSERT(msg, &destroyList); 
	      }
	    } /* if we created the reg msg */
	  } /* if there is a scope intersection */
	} /* inner (reg) loop */
	/* clear the needs regs bit */
      } /* middle (sa) loop */
      _LSLP_SIGNAL_SEM(localHead->writeSem);
    } /* if we locked the local DA */ 
  } /* while we are traversing the fwd list */
  lslpDestroyStreamList(&fwdList, 0);
  msg = msgList.next;
  while(! _LSLP_IS_HEAD(msg)) {
    _LSLP_UNLINK(msg);
    if(LSLP_WAIT_OK != _insertWorkNode(msg)) {
      _LSLP_INSERT(msg, &destroyList);
    }
    /* pause here so we don't flood the new mesh node with too much data */
    _LSLP_SLEEP(waitInterval);
    msg = msgList.next;
  }
  msg = destroyList.next;
  while(! _LSLP_IS_HEAD(msg)) {
    _LSLP_UNLINK(msg);
    msg->txerr(msg);
    msg = destroyList.next;
  }
  return;
}

/* clone a request, except replace its remote address and local address */
/* with new ones passed as parameters. Also replace the xid */
/* DOES NOT CLONE WORK NODE REPLY LIST */
/* DOES NOT CLONE work->scratch */
/* DOES NOT re-initialize work->ses_idx */
lslpWork *lslpCloneRequest(lslpWork *w, SOCKADDR_IN *rmt, SOCKADDR_IN *my) 
{
  lslpWork *clone;

  assert(w != NULL);
  assert(rmt != NULL);
  assert(my != NULL);
  assert((w->status & LSLP_STATUS_RQ) && (! (w->status & LSLP_STATUS_RP)));

  if((w == NULL) || (rmt == NULL) || (my == NULL) ||     
     (w->status & LSLP_STATUS_RP) || (!(w->status & LSLP_STATUS_RQ)))
    return(NULL);

  if(NULL != (clone = allocWorkNode())) {
    clone->version = w->version;
    clone->type = w->type;
    clone->status = w->status;
    clone->ses_idx = w->ses_idx;
    clone->apiSock = w->apiSock;
    clone->timer = w->timer;
    memcpy(&(clone->hdr), &(w->hdr), sizeof(lslpHdr));
    /* get a new xid so we can handle the response correctly */
    clone->hdr.xid = _lslp_get_xid();
    memcpy(&(clone->na), &(w->na), sizeof(struct nameAddr));
    /* now investigate the original to see how we need to buffer the msg data */
    if((w->hdr.data != NULL) && (w->hdr.data == &(w->na.static_data[0]))) {
      /* the message data is a static udp buffer */
      memcpy(&(clone->na.static_data[0]), 
	     &(w->na.static_data[0]), 
	     w->na.data_len);
      clone->hdr.data = &(clone->na.static_data[0]);
    } else if ((w->hdr.data != NULL) && ( w->hdr.data == w->na.dyn_data )) {
      /* the msg data is an allocated tcp buffer */
      if(NULL != (clone->hdr.data = 
		  clone->na.dyn_data  = 
		  (uint8 *)malloc( w->na.data_len))) {
	memcpy(clone->hdr.data, w->hdr.data, w->na.data_len);
      } 
    } else if((w->hdr.data != NULL) && (w->status & LSLP_STATUS_TCP)) {
      /* a dynamic tcp buffer that is not initialized in the na struct */
      if(NULL != (clone->hdr.data = malloc(w->na.data_len)))
	memcpy(clone->hdr.data, w->hdr.data, w->na.data_len);
    } else if ((w->hdr.data != NULL) && (w->status & LSLP_STATUS_TCP)) {
      /* a dynamically allocated udp buffer (shouldn't happen, but just in case...*/
      if(NULL != (clone->hdr.data = malloc(w->na.data_len)))
	memcpy(clone->hdr.data, w->hdr.data, w->na.data_len);
    }
    /* insert the new addresses */
    memcpy(&(clone->na.rmtAddr), rmt, sizeof(SOCKADDR_IN));
    memcpy(&(clone->na.myAddr), my, sizeof(SOCKADDR_IN));

    if(clone->hdr.data == NULL) {
      freeWorkNode(clone);;
      clone = NULL;
    }
  }  /* if we alloced our work node */
  return(clone);
}


void _lslpMeshFwdSrvReg(lslpWork *work)
{
  lslpScopeList *scopeList;
  lslpURL *url;
  int8 *bptr; 
  int16 len, err;
  int32 msgLen;
  lslpWork *clone;
  lslpWork msgList = {&msgList, &msgList, TRUE};
  lslpWork destroyList = {&destroyList, &destroyList, TRUE};

  /* this is time consuming but we need to extract the scope list */
  /* it will end up reducing network traffic because we will only forward */
  /* srv reg msgs that have intersecting scopes with the remote da */

  bptr = work->hdr.data;
  msgLen = _LSLP_GETLENGTH(bptr);
  bptr += _LSLP_HDRLEN_WORK(work);
  if(NULL != (url = lslpUnstuffURL(&bptr, (int16 *)&msgLen, &err))) {
    lslpFreeURL(url);
    /* jump past the service type string */
    len = _LSLP_GETSHORT(bptr, 0);
    bptr += (2 + len);
    if(NULL != (scopeList = lslpUnstuffScopeList(&bptr, (int16 *)&msgLen, &err))) {
      lslpStream *rmtDA;
      uint32 ccode;
      _LSLP_WAIT_SEM(streamInHead.sem, LSLP_Q_WAIT, &ccode);
      assert(ccode == LSLP_WAIT_OK);
      if(ccode == LSLP_WAIT_OK) {
	rmtDA = streamInHead.next;
	while(! _LSLP_IS_HEAD(rmtDA)) {
	  if(TRUE == lslp_scope_intersection(scopeList, rmtDA->scopeList)) {
	    if(NULL != (clone = lslpCloneRequest(work, 
						  &(rmtDA->peerAddr), 
						  &(work->na.myAddr)))) {
	      if(rmtDA->status & LSLP_DA_PEER_CONNECTED) {
		clone->ses_idx = rmtDA->peerSock;
		clone->status |= (LSLP_STATUS_TCP | 
				  LSLP_STATUS_KEEP_SOCKET | 
				  LSLP_STATUS_DONT_LINGER | 
				  LSLP_STATUS_PEER_SOCKET ) ;
		clone->txerr = stream_txerr;
	      } else {
		clone->ses_idx = _getSession(&(clone->na));
		clone->status &= (~LSLP_STATUS_TCP);
	      }
	      clone->status &= (~(LSLP_STATUS_RCVD | LSLP_STATUS_TX));
	      if(clone->ses_idx != -1) {
		clone->type = LSLP_Q_SLP_OUT;
		_LSLP_INSERT(clone, &msgList);
	      }  else {  _LSLP_INSERT(clone, &destroyList); }  /* if we got a socket or a session */
	    } /* if we cloned the request */
	  } /* if there is a scope intersection */
	  rmtDA = rmtDA->next;
	}
	_LSLP_SIGNAL_SEM(streamInHead.sem);
      }
      lslpFreeScopeList(scopeList);
    } /* if we got the scope list */
  }

  /* take the cloned messages and put them on the send list */
  clone = msgList.next;
  while(! _LSLP_IS_HEAD(clone)) {
    _LSLP_UNLINK(clone);
    if(LSLP_WAIT_OK != _insertWorkNode(clone)) {
      _LSLP_INSERT(clone, &destroyList);
    }
    clone = msgList.next;
  }

  clone = destroyList.next;
  while(! _LSLP_IS_HEAD(clone)) {
    _LSLP_UNLINK(clone);
    clone->txerr(clone);
    clone = destroyList.next;
  }
  return;
}


lslpWork *lslpCreateSrvReg(lslpSrvRegList *reg, SOCKADDR_IN *rmtAddr) 
{

  static int8 workBuf[4096 - LSLP_MIN_HDR];
  lslpWork *temp; 
  int16 tempLen;
  time_t create_time = ((time_t)0);
  int16 len = (1432 - LSLP_MIN_HDR);
  int8 *bptr = workBuf; 
  assert((reg != NULL) && (! _LSLP_IS_HEAD(reg)));
  if(NULL != (temp = allocWorkNode())) {
    temp->hdr.data = &(temp->na.static_data[0]);
    temp->type = LSLP_Q_SLP_OUT;
    temp->status = LSLP_STATUS_RQ | LSLP_STATUS_UDP;
    temp->na.version = LSLP_NAMEADDR_VERSION;
    temp->na.rmtAddr.sin_family = AF_INET;
    temp->na.rmtAddr.sin_port = htons(lslp_config_opport);
    temp->na.rmtAddr.sin_addr.s_addr = rmtAddr->sin_addr.s_addr;
    temp->hdr.ver = LSLP_PROTO_VER;
    temp->hdr.msgid = LSLP_SRVREG;
    temp->hdr.flags = LSLP_FLAGS_FRESH;
    temp->hdr.nextExt = 0;
    temp->hdr.xid = _lslp_get_xid();
    assert(temp->hdr.xid != (uint16)-1);
    temp->hdr.errCode = LSLP_OK;
    temp->hdr.langLen = strlen(LSLP_EN_US) + 2;
    memcpy(temp->hdr.lang, LSLP_EN_US, strlen(LSLP_EN_US));
    memset(workBuf, 0x00, (1432 - LSLP_MIN_HDR));
    /* now that work->hdr is initialized, init the buf */
    /* give the remote guy the true remaining life of this registration */
    if(reg->url->lifetime != 0) {
      reg->url->lifetime -= time(&create_time);
    }
    if(TRUE == lslpStuffURL(&bptr, &len, reg->url)) {
      assert(reg->srvType != NULL);
      tempLen = strlen(reg->srvType);
      _LSLP_SETSHORT(bptr, (tempLen + 1), 0);
      bptr += sizeof(int16);
      len -= sizeof(int16);
      memcpy(bptr, reg->srvType, tempLen);
      bptr += (tempLen + 1);
      len -= (tempLen + 1);
      if(TRUE == lslpStuffScopeList(&bptr, &len, reg->scopeList)) {
	if(TRUE == lslpStuffAttrList(&bptr, &len, reg->attrList, NULL)) {
	  if(TRUE == lslpStuffAuthList(&bptr, &len, reg->authList)) {
	    if(NULL != (temp->hdr.data = (void *)formSLPMsg(&(temp->hdr),
					     workBuf, bptr - workBuf, FALSE))) {
	      reg->url->lifetime += create_time;
	      return(temp);
	    }
	  } /* if we stuffed the auth list */
	} /* if we stuffed the attr list */
      } /* if we stuffed the scope list */
    } /* if we stuffed the reg->url */
    temp->txerr(temp);
  } /* if we alloced our work node */
  reg->url->lifetime += create_time;
  return(NULL);
}

/* input parameter is a worknode containing a service registration message */
/* output is a new worknode that contains a srvack with the appropriate error code set */
void lslpHandleSrvReg(lslpWork *work ) 
{
  int8 *bptr;
  int16 len, tempLen, errCode;
  int32 ccode;
  
  lslpSrvRegList *reg;
  BOOL isLinked = FALSE;

  
  assert(work != NULL && work->hdr.data != NULL);
  bptr = work->hdr.data;
  len = (work->hdr.len - (_LSLP_HDRLEN_WORK(work)));
  bptr += _LSLP_HDRLEN_WORK(work);
  if(len > 12)  {
    if (NULL != (reg = lslpAllocSrvReg())) {


      lslpFreeURL(reg->url); 
      if(NULL != (reg->url = lslpUnstuffURL(&bptr, &len, &errCode))) {
	tempLen = _LSLP_GETSHORT(bptr, 0);
	bptr += sizeof(int16);
	len -= sizeof(int16);
	if(tempLen && (tempLen <= len)) {
	  if(NULL != (reg->srvType = (int8 *)calloc(1, tempLen + 1))) {
	    memcpy(reg->srvType, bptr, tempLen);
	    bptr += tempLen;
	    len -= tempLen;
	    lslpFreeScopeList(reg->scopeList);
	    reg->scopeList = lslpUnstuffScopeList(&bptr, &len, &errCode); 
	    if(errCode == 0) {
	      lslpFreeAttrList(reg->attrList, TRUE);
	      reg->attrList = lslpUnstuffAttr(&bptr, &len, &errCode);
	      if(errCode == 0) {
		lslpFreeAuthList(reg->authList);
		reg->authList = lslpUnstuffAuthList(&bptr, &len, &errCode);
		if(errCode == 0) {

		  /* see if there is a meshfwd extension */
		  {
		    mesh_fwd_ext *mesh_ext = mslpExtractFwdExt(work);
		    if(mesh_ext) {
		      int len ;
		      reg->version_timestamp = mesh_ext->version_timestamp;
		      reg->accept_entry.timestamp = mesh_ext->accept_entry.timestamp;
		      len = reg->accept_entry.url_len = mesh_ext->accept_entry.url_len;
		      reg->accept_entry.url = (uint8*)calloc(len + 1, sizeof(uint8));
		      if(reg->accept_entry.url )
			memcpy(reg->accept_entry.url, mesh_ext->accept_entry.url, len);
		      else 
			reg->accept_entry.url_len = 0;
		      
		    } else 
		      {
			reg->version_timestamp = reg->accept_entry.timestamp = mslp_init_64bit_timestamp();
			reg->accept_entry.url_len = 0;
		      }
		  }

		  /* link the srv reg to one our our local agent lists */
		  if(lslp_config_is_da) {
		    if( 0 == (ccode = _lslpCheckScopeAndLink(reg, (lslpDirAgent *)&lslpDAs)))
		      isLinked = TRUE; 
		  } else {
		    if( 0 == (ccode  = _lslpCheckScopeAndLink(reg, (lslpDirAgent *)&lslpSAs)))
		      isLinked = TRUE;
		  }
		} else { printf("error parsing auth list\n"); }/* we got the auth list */
	      } else { printf("error parsing attr list\n");} /* we got the attr list */
	    } else { printf("error parsing scope list\n"); } /* we got the scope list */
	  } else{errCode = LSLP_INTERNAL_ERROR;} /* we alloced the srv type buf */
	} else{errCode = LSLP_PARSE_ERROR;} /* len checks out ok  */
      } else {errCode = LSLP_PARSE_ERROR; printf("error parsing url\n");} /* we got the url */
    } else {errCode = LSLP_INTERNAL_ERROR;} /* we alloced the new srvreg */
  } else {errCode = LSLP_PARSE_ERROR; printf("bad length\n");} /* not enough room - or malicious msg */
  
  if(isLinked == TRUE ) {
    printf("\n\nwe have mesh peers...forwarding the registration\n");
    _lslpMeshFwdSrvReg(work) ;
  }
  
  if(isLinked == FALSE && reg != NULL) {
    lslpFreeSrvReg(reg);
  }
  errCode = ccode;
  
  if( NULL != lslpMakeSrvACK(work, errCode)) {
    work->txerr = stream_txerr;
    if(0 != _insertWorkNode(work)) {  
      if(! ((work->status & LSLP_STATUS_BCAST)  ||
	    (work->status & LSLP_STATUS_MCAST) ||
	    (work->status & LSLP_STATUS_CONVERGE))) {
	if(work->status & LSLP_STATUS_TCP) 
	  tcpInternalErr(work->ses_idx, work->hdr.xid, LSLP_INTERNAL_ERROR);
	else
	  udpInternalErr(&(work->na.rmtAddr), &(work->na.myAddr), work->hdr.xid, LSLP_INTERNAL_ERROR);
      }
      work->status = 0;
      work->txerr(work);
    }
  } 
  return;
}


int8 *lslpMakeSrvACK(lslpWork *work, int16 errCode)

{
  int16 errCodeSave = errCode;
  
  work->status |= LSLP_STATUS_RP;
  work->status &= (~LSLP_STATUS_RQ);
  if(errCode != 0) {
    if(errCode == LSLP_INTERNAL_ERROR)
      work->status |= LSLP_STATUS_INTERNAL_ERROR;
    else
      work->status |= LSLP_STATUS_PARSE_ERROR;
  }
  if(work->hdr.data == work->na.dyn_data ) {
    free(work->hdr.data);
  } else if (work->hdr.data != &(work->na.static_data[0])) {
    free(work->hdr.data);
  } else { 
    assert(work->hdr.data == &(work->na.static_data[0]));
  }
  work->type = LSLP_Q_SLP_OUT;
  work->hdr.data = &(work->na.static_data[0]);
  work->hdr.msgid = LSLP_SRVACK;
  work->hdr.errCode = errCodeSave;
  work->hdr.flags = 0;
  printf("forming SRVACK, error code = %d\n", errCodeSave);
  
  return(formSLPMsg(&(work->hdr), &errCodeSave, sizeof(int16), FALSE));
}


/* incremental registrations have not been used in implementations */
/* so we don't support them here. When we are processing a re-registration, */
/* we remove the existing registration and replace it with a new one. */ 


/* if I ever support more than one DA (like apache virtual domains) */ 
/* then I need to modify this routine to traverse the list (h->next) */ 

uint32 _lslpCheckScopeAndLink(  lslpSrvRegList *reg,  lslpDirAgent *h)
{
  uint32 ccode;
  lslpSrvRegList *temp;
  int8 *url = NULL;
  assert((reg != NULL) && (! (_LSLP_IS_HEAD(reg))));
  assert(h != NULL && (_LSLP_IS_HEAD(h)));
  assert(reg->url != NULL && reg->url->url != NULL);

  /*   lslp_foldString(reg->url->url); */
  if(TRUE == lslp_scope_intersection(reg->scopeList, h->next->scopeList)) {

    /* make sure the mesh variables are set correctly */
    if(reg->accept_entry.url_len == 0 ) {
      /* find our own url and initialize the reg with it */ 
      if((! _LSLP_IS_HEAD(h->next) && (NULL != h->next->url) && (NULL != h->next->url->url))) {
	int len =  strlen(h->next->url->url);
	if(len > 255) len = 255;
	reg->accept_entry.url_len = len + 1;
	reg->accept_entry.url = (uint8*)calloc(len + 1, sizeof(uint8));
	if(reg->accept_entry.url) {
	  memcpy(reg->accept_entry.url, h->next->url->url, len);
	}
	else 
	  reg->accept_entry.url_len = 0;
      } /* da url is good */
    } /* no mesh fwd accept entry - provide our own */
    
    /* see if the reg already exists */
    do { _LSLP_WAIT_SEM(h->writeSem, LSLP_Q_WAIT, &ccode);}
    while(ccode != LSLP_WAIT_OK) ;
    temp = (lslpSrvRegList *)h->next->srvRegList;
    while(! _LSLP_IS_HEAD(temp->next)) {
      temp = temp->next;
      if( lslpCompareURL(reg->url, temp->url)) {
	/* see if the version timestamp of the new url is later */ 
	if( reg->version_timestamp == 0 || reg->version_timestamp > temp->version_timestamp ) {
	  
	  /* remove the existing srvreg */
	  _LSLP_UNLINK(temp);
	  
	  lslpFreeSrvReg(temp);
	}
	
	break;
      } 
    } /* search reg loop */
    if(reg != NULL) {
      reg->url->lifetime += time(NULL);
      _LSLP_INSERT(reg, (lslpSrvRegList *)h->next->srvRegList);
      ccode = LSLP_OK;
      if(pipe_open == TRUE)
	url = strdup(reg->url->url);
    }
    _LSLP_SIGNAL_SEM(h->writeSem);
  }

  /* if the reg is linked, pipe the url to the notification file */
  if((url != NULL ) && (pipe_open == TRUE)) {
    size_t bytes_written, bytes_remaining;
    int8 *bptr;
    bytes_remaining = strlen(url);
    bptr = url;
    while(bytes_remaining > 0) {
      if((bytes_written = write(pipe_fd, bptr, bytes_remaining) ) < 0) {
	if(errno == EINTR)
	  bytes_written = 0;
	else
	  break;
      }
      bytes_remaining -= bytes_written;
      bptr += bytes_written;
    }
    /* terminate the record with a newline */
    do { bytes_written = write(pipe_fd, "\n", 1); }
    while((bytes_written) < 0 && (errno == EINTR)) ;
  }

  if(url != NULL)
    free(url);
  return(ccode);
}

void lslpHandleSrvACK(lslpWork *work ) 
{
  /* look for the matching xid on the pending q */
  /* if found, remove and free the xid worknode */
  /* always free the in parameter */
  lslpCleanPendingQ(work);
  work->txerr(work);
  
  return;
}
 

void lslpHandleSrvDereg(lslpWork *work)
{

  /* traverse the appropriate reg list and look */
  /* for a srvtype match. if found, unlink and free */
  /* then dereg with remote das. */

  /* when comparing urls, use the hash codes */

  work->txerr(work);
}


BOOL lslpCompareURL(lslpURL *a, lslpURL *b)
{

  lslpAtomizedURL *aa, *ab;
  assert(a != NULL);
  assert(b != NULL);
  assert(! _LSLP_IS_HEAD(a));
  assert(! _LSLP_IS_HEAD(b));
  if((a->atomized == NULL) ||  (b->atomized == NULL)) {
    assert(a->url != NULL && b->url != NULL);
    if(! strcasecmp(a->url, b->url)) {
      return(TRUE);
    }
  } else {
    aa = a->atomized->next;
    ab = b->atomized->next;
    if(aa->urlHash == ab->urlHash) {
      if( ! strcasecmp(aa->url, ab->url)) { 
	return(TRUE);
      }
    }
  }
  return(FALSE);
}


uint16 _lslp_get_xid(void)
{
  static uint16 xid;
  uint16 temp;
  int32 ccode;

  _LSLP_WAIT_SEM(xid_sem, LSLP_Q_WAIT, &ccode);
  if(ccode != LSLP_WAIT_OK) {
    return(-1);
  } else {
    temp = ++xid;
    if(temp == 0) {
      temp = ++xid;
    }
    _LSLP_SIGNAL_SEM(xid_sem);
  }
  return(temp);
}

/* routine does no semaphore locking - caller must have list sems */
/* locked */
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
      if(! lslp_string_compare(a->scope, b->scope)) {
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
  if( TRUE == lslp_pattern_match(s1, s2, FALSE))
    return 0;
  return -1;
}

/* return 1 if char is legal for scope value, 0 otherwise */
int lslp_isscope(int c) 
{  
  int i;
  static int8 scope_reserved[] = { '(', ')', ',', 0x5c, '!', '<', '=', '>', '~', ';', '*', '+', 0x7f };
  if( ! (_LSLP_ISASCII(c)))
      return(0);
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


#define lslp_to_lower(c)  (((c) > 0x40 && (c) < 0x5b) ? ((c) + 0x20) : (c))
/* based upon TclStringCaseMatch */
#define MAX_RECURSION  20
BOOL lslp_pattern_match(const int8 *string, const int8 *pattern, BOOL case_sensitive)
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
      if (*string == '\0')  //jeb
          return TRUE;
      else
          return FALSE;
//jeb      return (*string == '\0');
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
      if (*pattern == '\0') {
	recursion_level--;
	return FALSE;
//jeb	return 0;
      }
    }
    s = *(string++);
    p = *(pattern++);
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


lslpScopeList *lslpScopeStringToList(int8 *s, int16 len) 
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

/* a is an attribute list, while b is a string representation of an ldap filter  */
BOOL lslp_predicate_match(lslpAttrList *a, int8 *b)
{
  BOOL ccode;
  lslpLDAPFilter *ftree;
  if(a == NULL)
    return FALSE;
  assert(_LSLP_IS_HEAD(a));
  if(b == NULL || ! strlen(b))
    return(TRUE);		/*  no predicate - aways tests TRUE  */
  if(NULL != (ftree = _lslpDecodeLDAPFilter(b))) {
    ccode = lslpEvaluateFilterTree(ftree, a);
    lslpFreeFilterTree(ftree);
    return(ccode);
  }
  return(FALSE);
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

/* returns a null-terminated array of nameAddr structures */
/* addresses are returned in the myAddr field !! */
struct nameAddr *lslpGetAddrFromURL(lslpAtomizedURL *url)
{
  lslpAtomList *siteList;
  int8 *bptr;
  uint32 addr;
  struct nameAddr *na = NULL;
  uint16 port = lslp_config_opport;
  assert(url != NULL);
  if(_LSLP_IS_HEAD(url)) {
    if(_LSLP_IS_EMPTY(url)) {
      return(NULL);
    }
    siteList = &(url->next->site);
  } else {
    siteList = &(url->site);
  }

  assert(siteList != NULL);
  if(! _LSLP_IS_HEAD(siteList->next)) {
    siteList = siteList->next;
    /* look for an alternate port */
    assert(siteList->str != NULL);
    if(siteList->str == NULL)
      return(NULL);
    if(NULL != (bptr = strstr(siteList->str, ":"))) {
      *bptr = 0x00;
      bptr++;
      port = (int16)strtoul(bptr, NULL, 0) ;
    }

    /* look for the (unexpected) user@host production */
    if(NULL != (bptr = strstr(siteList->str, "@"))) 
      bptr++;
    else {
      bptr = siteList->str;
      while(*bptr == '/')
	bptr++;
    }
    /* first try inet_addr to see of the address is in dotted decimal form */
    if(INADDR_NONE == (addr = inet_addr(bptr))) {
      /* probably a host name */
      na = GetHostAddresses(bptr);
    } else {
      if(NULL != (na = (struct nameAddr *)calloc(2, sizeof(struct nameAddr)))) {
	na->myAddr.sin_addr.s_addr = addr;
	na->myAddr.sin_port = htons(port);
	na->myAddr.sin_family = AF_INET;
      }
    }
  }
  return(na);
}

/* Attribute Reply:                                                       */
/*       0                   1                   2                   3    */
/*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1  */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |       Service Location header (function = AttrRply = 7)       | */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |         Error Code            |      length of <attr-list>    | */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |                         <attr-list>                           \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
/*      |# of AttrAuths |  Attribute Authentication Block (if present)  \ */
/*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */


void lslpHandleAttrReq(lslpWork *work)
{

  int8 *bptr;
  lslpURL *url;
  lslpScopeList *scopes;
  lslpAttrList *attr_tags, *attr_return;
  lslpSrvRegList *regs;
  
  int16 str_len, buf_len, errCode = 0, parse_err;
  int32 total_len, purported_len, ccode;

  lslpDirAgent *head, *a;
  int16 rply_len = 0 ;
  int8* rply_buf = NULL;
  int8* pr_list_copy = NULL;
  

  assert(work != NULL && work->hdr.data != NULL);
  
  /* read the length from the slp header */
  purported_len = work->hdr.len;
  total_len = _LSLP_HDRLEN_WORK(work);
  bptr = (int8*)work->hdr.data + _LSLP_HDRLEN_WORK(work);
  if(total_len < purported_len){
    /* get the length of the pr list and look at it */
    str_len = _LSLP_GETSHORT(bptr, 0);
    if((str_len + 2 + total_len) < purported_len) {
      if(str_len ) {
	/* allocate a copy of the pr list */
	pr_list_copy = (int8*)calloc(str_len + 1, sizeof(int8));
	if(pr_list_copy) {memcpy(pr_list_copy, bptr + 2, str_len);}
      }
      

      if(FALSE == lslpPreviousResponder((int16)str_len, pr_list_copy)) {
	  
	/* adjust pointer and length */
	bptr += str_len + 2;
	total_len += str_len + 2;
	  
	/* decode the url */

	str_len = _LSLP_GETSHORT(bptr, 0);

	  
	if(( str_len + total_len + 2) < purported_len ) {
	  /* allocate a url structure */
	  if( (url = lslpAllocURL()) ) {
	    if( (url->url = (int8*)calloc(str_len + 1, sizeof(int8))) ) {
	      memcpy(url->url, bptr + 2, str_len);
	      url->atomized = _lslpDecodeURLs((int8**)&(url->url), 1);
		
	      /* adjust pointer and length */
	      bptr += str_len + 2;
	      total_len += str_len + 2;
	      /* extract the scope string */
	      buf_len = purported_len - total_len;
	      str_len = _LSLP_GETSHORT(bptr, 0);
	      if(str_len < buf_len) {
		scopes = lslpUnstuffScopeList(&bptr, &buf_len, &parse_err);
		total_len += str_len + 2;
		/* decode the attribute tags */
		str_len = _LSLP_GETSHORT(bptr, 0);
		  
		if(str_len + 2 + total_len < purported_len ) {
		    
		  attr_tags = lslpUnstuffAttr(&bptr, &buf_len, &parse_err);
		  total_len += str_len + 2;
		  attr_return = NULL;
		
		  /* traverse the list of registrations and find a matching url */
		  if(lslp_config_is_da)
		    head = (lslpDirAgent*)&lslpDAs;
		  else
		    head = (lslpDirAgent*)&lslpSAs;
		  assert(head);
		  _LSLP_WAIT_SEM(head->writeSem, LSLP_Q_WAIT, &ccode);
		  assert(ccode == LSLP_WAIT_OK);
		  if(ccode == LSLP_WAIT_OK) {
		  
		    a = head->next;
		    regs = a->srvRegList->next;
		    /* see if this reg has expired */ 
		    while(! _LSLP_IS_HEAD(regs)) {
		      if((0 != regs->url->lifetime) && (regs->url->lifetime < time(NULL))) {
			lslpSrvRegList* temp_reg = regs;
			regs = regs->next;
			_LSLP_UNLINK(temp_reg);
			lslpFreeSrvReg(temp_reg);
			continue;
		      } /* expired registration - remove it */
			/* compare the url */
		      if(! lslp_string_compare(regs->url->url, url->url)) {
			/* this is the url - now make sure scope matches */ 
			if(TRUE == lslp_scope_intersection(regs->scopeList, scopes)) {
			  /* bingo - now capture the attribute list */ 
			  /* allocate a buffer of LSLP_MTU size, init to error code, 
			     then unstuff the attr list. there will be no auths */
			  rply_buf = (int8*)calloc(lslp_config_mtu, sizeof(int8));
			  rply_len = lslp_config_mtu;
			  if(rply_buf) {
			    bptr = rply_buf + 2;
			    if(TRUE == lslpStuffAttrList(&bptr, &rply_len, regs->attrList, attr_tags)) {
			      _LSLP_SETSHORT(rply_buf, LSLP_OK, 0);
			    } else {
			      _LSLP_SETSHORT(rply_buf, LSLP_INTERNAL_ERROR, 0);
			    }
			  } else {    /* allocated reply buffer */
			    if(work->status & LSLP_STATUS_TCP)
			      tcpInternalErr(work->ses_idx, work->hdr.xid, LSLP_INTERNAL_ERROR);
			    else 
			      udpInternalErr(&(work->na.rmtAddr), 
					     &(work->na.myAddr), 
					     work->hdr.xid, LSLP_INTERNAL_ERROR);
			  } /* emergency response - hope it gets there */
			  break;
			} /* scopes intersect */
		      } /* urls match */
		      regs = regs->next;
		    } /* while traversing registrations */
		      /* release semaphore */
		    _LSLP_SIGNAL_SEM(head->writeSem);
		  } /* obtained da reg list semaphore */
		
		    /* if we were successful, we should have data in the rply_buf. */
		    /* if not, just construct a reply buffer with no data */ 
		  if(rply_buf) {
		    /* construct a reply */ 
		  
		    work->hdr.msgid = LSLP_ATTRRPLY;
		    work->hdr.errCode = LSLP_OK;
		    /* get the length of the attr list */
		    str_len = _LSLP_GETSHORT(rply_buf, 2);
		    /* calculate the length of the reply buffer */
		    /* 2 bytes err code, 2 bytes len, 1 byte # aths */ 
		    rply_len = str_len + 5;
		  
		    if(NULL != lslpMakeRply(work, (void*)rply_buf, rply_len, 0L)) {
		      work->txerr = stream_txerr;
		      if(LSLP_WAIT_OK != _insertWorkNode(work)) {
			errCode = LSLP_INTERNAL_ERROR;
		      }/* reply successfully initialized */ 
		    } else { errCode = LSLP_INTERNAL_ERROR; } 
		    free(rply_buf);
		  } else { errCode = LSLP_INTERNAL_ERROR; } 
		  /* sanity check on attr string length */
		} else { errCode = LSLP_INTERNAL_ERROR; }
		
		/* sanity check on scope string length */
	      } else { errCode = LSLP_INTERNAL_ERROR; } 
	      /* allocated url string buffer */
	    } else { errCode = LSLP_INTERNAL_ERROR; } 
	    lslpFreeURL(url);
	  } else { errCode = LSLP_INTERNAL_ERROR; }  /* allocated our url - remember to free it */
	} else { errCode = LSLP_PARSE_ERROR; } /* sanity check on url length */
	  
	if(errCode) {
	  if(! ((work->status & LSLP_STATUS_BCAST)  ||
		(work->status & LSLP_STATUS_MCAST) ||
		(work->status & LSLP_STATUS_CONVERGE))) {
	    if(work->status & LSLP_STATUS_TCP)
	      tcpInternalErr(work->ses_idx, work->hdr.xid, (uint8)errCode);
	    else 
	      udpInternalErr(&(work->na.rmtAddr), 
			     &(work->na.myAddr), 
			     work->hdr.xid, (uint8)errCode);
	  }
	  work->txerr(work);
	}

      } /* not on the previous responder list */
      if(pr_list_copy ) {
	free(pr_list_copy);
      } /* allocated our pr list copy  - remember to free it */
    } /* pr list length sanity check */
  } /* initial header sanity check */
    return;
}


/* upon entry, new peers will have the LSLP_NEEDS_PEER_INDICATION bit set */
/* will be called from the slpinthread */
/* call the mesh routines in a round-robin */ 
/* so we never hold up incoming messages longer than necessary */

/* make this an independent thread that does not lock the rmt da list */
/* should be able to make all calls with no locks ! */

void *lslpMeshServiceThread(void *parm) 
{

/*   << Tue May 25 11:13:42 2004 mdd >> */
/*     disable while rewriting mesh function to comply with rfc 3528 */


  
/*   while (! dieNow) { */
      /* run the tx half of the initialization state machine */
/*       _lslpMeshConnectPeers(&connectHead, &streamInHead ); */
/*       _lslpRegisterLocalServices(&streamInHead) ; */
/*       _lslpMeshInitTXPeers(&streamInHead, &connectHead) ; */
/*   } */  
  _LSLP_ENDTHREAD(0);
  return(0L);
} 


/* create a daadvert and forward it to all our peers */
void _lslpMeshFwdDAAdv(lslpDirAgent *list, lslpStream *peers) 
{
  int32  ccode;
  lslpDirAgent *subject;
  lslpStream *target;
  lslpWork *msg;
  LSLP_CONFIG_ADV *cfg ;
  LSLP_CONFIG_ADV configList = {&configList, &configList, TRUE};
  lslpWork msgList = {&msgList, &msgList, TRUE};
  
  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));  
  if(list == NULL || ! (_LSLP_IS_HEAD(list)))
    return;
  assert(peers != NULL);
  assert(_LSLP_IS_HEAD(peers));
  if(peers == NULL || ! (_LSLP_IS_HEAD(peers)))
    return;

  /* build a temporary list of peer addresses and flags */
  _LSLP_WAIT_SEM(peers->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    target = peers->next;
    while(! _LSLP_IS_HEAD(target)) {
      if(target->status & LSLP_DA_PEER_CONNECTED) {
	if(NULL != (cfg = (LSLP_CONFIG_ADV *)calloc(1, sizeof(LSLP_CONFIG_ADV)))) {
	  cfg->flag = (LSLP_STATUS_TCP | LSLP_STATUS_RP | 
		       LSLP_STATUS_KEEP_SOCKET | 
		       LSLP_STATUS_DONT_LINGER | 
		       LSLP_STATUS_PEER_SOCKET ) ;
	  cfg->addr = target->peerAddr.sin_addr.s_addr;
	  /* overload the mask field to store the peer socket */
	  cfg->mask = (uint32) target->peerSock;
	  _LSLP_INSERT(cfg, &configList);
	} /* if we allocated the cfg node */
      } /* if we found a target */
      target = target->next;
    } /* while we are looking for target das */
    _LSLP_SIGNAL_SEM(peers->sem);
  } /* if we locked the peer list */

  /* now look for DAs that need to be forwarded to our peers */
  _LSLP_WAIT_SEM(list->writeSem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    subject = list->next;
    while(! _LSLP_IS_HEAD(subject)) {
      if(subject->status & LSLP_DA_FWD_DAADV) {
	cfg = configList.next;
	while(! _LSLP_IS_HEAD(cfg)) {
	  if(NULL != (msg = lslpCreateDAAdvert(subject, subject->url->url, NULL, cfg))) {
	    /* make certain we send this on the peer socket */
	    msg->ses_idx = (uint32)(SOCKET)cfg->mask;
	    /* set the message destructor to the special mesh destructor */
	    msg->txerr = stream_txerr;
	    /* put the message into the temporary list */
	    _LSLP_INSERT(msg, &msgList);
	  } /* if we created or da advert */
	  cfg = cfg->next;
	} /* while we are traversing the target list */
	/* clear the DA fwd bit */
	subject->status &= (~LSLP_DA_FWD_DAADV);
      } /* if we found a subject */
      subject = subject->next;
    } /* while we are looking for subject das */
    _LSLP_SIGNAL_SEM(list->writeSem);
  } /* if we locked the subject da list */
  /* destroy the temporary target list */
  cfg = configList.next;
  while(! _LSLP_IS_HEAD(cfg)) {
    _LSLP_UNLINK(cfg);
    free(cfg);
    cfg = configList.next;
  }
  /* now go through the temporary message list and put each message into the real list */
  msg = msgList.next;
  while(! _LSLP_IS_HEAD(msg)) {
    _LSLP_UNLINK(msg);
    if(LSLP_WAIT_OK != _insertWorkNode(msg)) {
      msg->txerr(msg);
    }
    msg = msgList.next;
  }
  return;
}

uint32 _lslpMeshConnectPeers( lslpStream *connectList, lslpStream *peerList) 
{
  lslpStream *mi;
  int32 ccode, count;
  time_t now;
  assert(connectList != NULL);
  assert(peerList != NULL); 
  assert(_LSLP_IS_HEAD(connectList)) ;
  assert(_LSLP_IS_HEAD(peerList)) ;
  time(&now);
  _LSLP_WAIT_SEM(connectList->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    count = 0;
    mi = connectList->next;
    while(! _LSLP_IS_HEAD(mi)) { 

      if((now - mi->in_time) > peerWait.tv_sec) {
	count++;
	/* look for nodes that have a pending transmission state */
	_LSLP_UNLINK(mi);
	_LSLP_SIGNAL_SEM(connectList->sem);
	_mesh_trans[mi->state][mi->next_state](mi, NULL);
	/* see if we connected to the peer */
	if(mi->status & LSLP_DA_PEER_CONNECTED) {
	  /* insert the node into the list */
	  _LSLP_WAIT_SEM(peerList->sem, LSLP_Q_WAIT, &ccode);
	  assert(ccode == LSLP_WAIT_OK);
	  if(ccode == LSLP_WAIT_OK) {
	    _LSLP_INSERT(mi, peerList);

	    _LSLP_SIGNAL_SEM(peerList->sem);
	  } else {
	    lslpFreeStream(mi);
	  }
	} else {
	  lslpFreeStream(mi);
	}
	_LSLP_WAIT_SEM(connectList->sem, LSLP_Q_WAIT, &ccode);
	assert(ccode == LSLP_WAIT_OK);
      } else { mi = mi->next ; continue ;}
      mi = connectList->next;
    }
    _LSLP_SIGNAL_SEM(connectList->sem);
  }
  if(count == 0) {
    _LSLP_SLEEP(waitInterval); 
  }
  return(0L);
}

/* some of these state transitions can take a long time. others lock */
/* secondary lists, which may allow a deadlock. so to compensate for */
/* these problems we unlink the node and unlock the list before the */
/* long state transitions */
uint32 _lslpMeshInitTXPeers(lslpStream *list, lslpStream *connectList)
{

  /*  rewrite this routine to do a timeout for all streams, but to only enter
      the mesh state switch for mesh peers. this is due to the condition
      wher some streams will be clients and other streams will be mesh peers. 
  */
  lslpStream *mi;
  int32 ccode;
  lslpStream timeoutList = {&timeoutList, &timeoutList, TRUE};
  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list)) ;
  if((list == NULL) || (! _LSLP_IS_HEAD(list)))
    return(0L);
  _LSLP_WAIT_SEM(list->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    time_t now;
    time(&now);
    mi = list->next;
    while(! _LSLP_IS_HEAD(mi)) { 
      /* look for peers  that have a pending transmission state */
      switch(mi->next_state) {
      case MESH_CONNECT_STATE:
	_LSLP_UNLINK(mi);

	_LSLP_SIGNAL_SEM(list->sem);
	_LSLP_WAIT_SEM(connectList->sem, LSLP_Q_WAIT, &ccode);
	assert(ccode == LSLP_WAIT_OK);
	if(ccode == LSLP_WAIT_OK) {
	  _LSLP_INSERT(mi, connectList);
	  _LSLP_SIGNAL_SEM(connectList->sem);
	} else { lslpFreeStream(mi); }
	return(0L);
      case MESH_TX_PEER_STATE:
      case MESH_TX_DA_STATE:
	_mesh_trans[mi->state][mi->next_state](mi, NULL) ;
	mi = mi->next;
	continue;
      default: 
	/* scrounge around and see if we need to send a data indication */
	if( mi->state == MESH_SYNC_STATE && mesh_data_sync == FALSE) {
	  time_t now;
	  time(&now);
	  if( (now -  mi->last_read ) >= peerWait.tv_sec) {

	    if(LSLP_WAIT_OK == lslpMeshCtrl(mi, LSLP_MESHCTRL_DATA)) {
	      mesh_data_sync = TRUE;
	    }
	  }
	} 
	/*  here is another place where we need to differentiate between stream clients and mesh peers */
	/*  if a stream client times out, we can just dump him. if a mesh peer times out, we should  */
	/* try to re-establish the mesh relationship.  */
	/* see if we need to perform keepalive for this peer */
	if((now - mi->last_read) > (lslp_config_close_conn + 10)) {
	  /* he appears to have timed out */
	  _report_event(LSLP_SEV_IDX(67, LSLP_SEVERITY_MESH), NULL, 0, 
		       inet_ntoa(mi->peerAddr.sin_addr), 
		       mi->state, mi->next_state, "Keep Alive routine detected timeout.");
	  if(mi->status & LSLP_DA_TO_RTX) {
	    _LSLP_UNLINK(mi);
	    
	    /* we've already sent a timeout, put him on the the holding list */
	    mi->status = LSLP_DA_DOWN;
	    mi->state = MESH_START_STATE;
	    mi->next_state = MESH_CONNECT_STATE;
	    _LSLP_INSERT(mi, &timeoutList);
	  } else {
	    /* send a keepalive and see what happens */
	    mi->status |= LSLP_DA_TO_RTX;
	    /* give him some extra time */
	    mi->last_read += 10;

	    lslpMeshCtrl(mi, LSLP_MESHCTRL_KEEPALIVE);
	  }
	  mi = list->next;
	  continue;
	} else if ((now - mi->last_write) > (lslp_config_close_conn - 1)) {
	  /* we need to send him a keepalive */
	  lslpMeshCtrl(mi, LSLP_MESHCTRL_KEEPALIVE);
	  mi = mi->next;
	  continue;
	}
	break;
      } /* tx switch */
      mi = mi->next;
    }
    _LSLP_SIGNAL_SEM(list->sem);
  }
  /* see if we have anything on the holding list that we need to put on the connect list */
  /* add a check here to bypass the whole locking, inserting, unlocking routine if the */
  /* timeout list is empty  */
  if(! _LSLP_IS_EMPTY(&timeoutList) ) {
    _LSLP_WAIT_SEM(connectList->sem, LSLP_Q_WAIT, &ccode);
    assert(ccode == LSLP_OK);
    if(ccode == LSLP_OK) {
      mi = timeoutList.next;
      while(! _LSLP_IS_HEAD(mi)) {
	_LSLP_UNLINK(mi);
	_LSLP_INSERT(mi, connectList);
	mi = timeoutList.next;
      }
      _LSLP_SIGNAL_SEM(connectList->sem);
    }
  }
  return(0L);
}

/* diragent lists must be held exclusively by the caller */
int8 *lslpStreamIntersectingDAList(int8 action,
				  lslpDirAgent *list, 
				  lslpDirAgent *us, 
				  lslpStream *rmtDA, 
				  int32 *retLen) 
{
  lslpDirAgent *temp;
  int8 *bptr;
  lslpAtomizedURL *tempURL;
  int8 *buf = NULL;
  int16 bufSize = 256, count = 0, len = 3, urlLen;
  BOOL checked_us = FALSE;

  assert(list != NULL);
  assert(us != NULL);
  assert(rmtDA != NULL);
  assert(retLen != NULL);
  assert(_LSLP_IS_HEAD(list));
  assert(_LSLP_IS_HEAD(us));

  temp = list->next;
  while(! _LSLP_IS_HEAD(temp)) {
  try_intersecting_da_again:
    if(!_LSLP_IS_HEAD(temp)) {
      if(temp->addr.sin_addr.s_addr != rmtDA->peerAddr.sin_addr.s_addr) {
	if(lslp_scope_intersection(temp->scopeList, rmtDA->scopeList)) {
	  if(buf == NULL) {
	    if(NULL == (buf = (int8 *)malloc(bufSize)))
	      return(NULL);
	    else
	      bptr = buf + 3; /* leave room for the number of urls */
	  }
	  tempURL = temp->url->next;
	  while(! _LSLP_IS_HEAD(tempURL)) {
	    urlLen = strlen(tempURL->url);
	    while((bufSize - len) < (urlLen + 2)) {
	      bufSize *= 2; /* double the buffer size */
	      if(NULL == (buf = realloc(buf, bufSize)))
		return(NULL);
	      bptr = buf + len;
	    }
	    _LSLP_SETSHORT(bptr, urlLen, 0);
	    memcpy(bptr + 2, tempURL->url, urlLen);
	    bptr += (2 + urlLen);
	    len += (2 + urlLen);
	    count++;
	    tempURL = tempURL->next;
	  } /* while traversing the DA url list */
	} /* if scopes intersect */
      } /* if this is not the remote target */
    } /* if not a list head */
    temp = temp->next;
  } /* while we are traversing the list */
  if(checked_us == FALSE) {
    checked_us = TRUE;
    /* see if the local DA has any URLs that need to go on the list */
    temp = us->next;
    if(! _LSLP_IS_HEAD(temp))
      goto try_intersecting_da_again;
  }
  if(buf == NULL) {
    if(NULL == (buf = (int8 *)malloc(bufSize)))
      return(NULL);
  }
  /* set the action code */
  _LSLP_SETBYTE(buf, action, 0);
  /* set the url count */
  _LSLP_SETSHORT(buf, count, 1);
  *retLen = len ;
  return(buf);
}



/* a DA indication action code locks the rmt da list  */
/* a caller that has either of these lists locked will force a deadlock */
uint32 lslpMeshCtrl(lslpStream *mi, int8 action)
{
  int32 len;
  lslpWork *msg;

  int8 workBuf[3] = {0, 0, 0};
  int8 *bptr = NULL;
  uint32 tempAdditiveBits = 0L;
  int32 ccode = LSLP_WAIT_OK;
  assert((action > 0) && (action <= LSLP_MESHCTRL_KEEPALIVE));
  if((action <= 0) || (action > LSLP_MESHCTRL_KEEPALIVE))
    return(-1);
  if(NULL != (msg = allocWorkNode())) {
    msg->type = LSLP_Q_SLP_OUT;
    msg->na.version = LSLP_NAMEADDR_VERSION;
    msg->hdr.ver = LSLP_PROTO_VER;
    msg->hdr.msgid = LSLP_MESHCTRL;
    msg->hdr.nextExt = 0;
    msg->hdr.errCode = LSLP_OK;
    msg->hdr.langLen = LSLP_EN_US_LEN;
    memcpy(msg->hdr.lang, LSLP_EN_US, LSLP_EN_US_LEN);
    msg->status = (LSLP_STATUS_RQ |  LSLP_STATUS_RP |
		   LSLP_STATUS_TCP |  LSLP_STATUS_DONT_LINGER | 
		   LSLP_STATUS_KEEP_SOCKET | LSLP_STATUS_PEER_SOCKET ); 
    msg->hdr.xid = _lslp_get_xid();
    msg->na.sock = mi->peerSock;
    msg->ses_idx = mi->peerSock;
    len = sizeof(SOCKADDR_IN);
    getsockname(mi->peerSock, (struct sockaddr *)&(msg->na.myAddr), &len);
    memcpy(&(msg->na.rmtAddr), &(mi->peerAddr), sizeof(SOCKADDR_IN));
    switch(action) {
    case LSLP_MESHCTRL_DATA:

      _LSLP_SETBYTE(&workBuf[0], action, 0);
      bptr = &workBuf[0];
      len = 3;
      break;
    case LSLP_MESHCTRL_PEER_IND:

      _LSLP_SETBYTE(&workBuf[0], action, 0);
      bptr = &workBuf[0];
      len = 3;
      break;
    case LSLP_MESHCTRL_NULL:
    case LSLP_MESHCTRL_KEEPALIVE:  /* message consists only of an action code */
      _LSLP_SETBYTE(&workBuf[0], action, 0);
      bptr = &workBuf[0];
      len = 3;
      break;
    case LSLP_MESHCTRL_DA_IND: { /* message consists of an action code and a list of DA URLs */
      _LSLP_WAIT_SEM(lslpRemoteDAs.writeSem, LSLP_Q_WAIT, &ccode); 
      assert(ccode == LSLP_WAIT_OK);
      if(ccode == LSLP_WAIT_OK) {
	bptr = lslpStreamIntersectingDAList(action, 
					   (lslpDirAgent *)&lslpRemoteDAs,
					   (lslpDirAgent *)&lslpDAs,
					   mi, 
					   &len );
	_LSLP_SIGNAL_SEM(lslpRemoteDAs.writeSem);
      } else {len = 0 ;} 

      break; 
    }
    case 1:  /* invalid code according to the draft spec */
    default: /* invalid code - leave bptr NULL */
      len = 0;
	break;
    } /* action switch */
    if((NULL != bptr) && (len > 0)) {
      if(NULL != ( msg->hdr.data = (void *)formSLPMsg(&(msg->hdr), 
						       bptr, len, 
						       TRUE))) {
	msg->type = LSLP_Q_SLP_OUT;
	msg->txerr = stream_txerr;
	/* link the new msg to the slp out Q */
	if(LSLP_WAIT_OK == (ccode = _insertWorkNode(msg))) {
	  mi->status |= tempAdditiveBits;
	  /* update the keepalive timer for the rmtDA */
	  time(&(mi->last_write));
	} else { msg->txerr(msg); }
      }
      /* free bptr if we sent a da indication */
      if(bptr != &workBuf[0])
	free(bptr);
    }
  } /* if we alloc'd our work node */
  return(ccode);
}


/* caller must NOT have mesh list locked */

/*  right now this is called by the dispatch thread */
/*  reading a peer should suceed or fail immediately, because */
/*  we don't try to read unless we have already selected on the socket */
void  _lslpMeshReadPeers(lslpStream *list) 
{

  lslpStream *min, *temp;
  lslpStream ready = {&ready, &ready, TRUE };
  int32 ccode;
  assert(list != NULL);
  if(list == NULL)
    return;
  assert(_LSLP_IS_HEAD(list));
  if(! _LSLP_IS_HEAD(list))
    return;
  /* now traverse the mesh incoming list */
  _LSLP_WAIT_SEM(list->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_OK);
  if(ccode == LSLP_OK) {
    fd_set fds;
    int32 ccode ;
      min = list->next;
    while(! _LSLP_IS_HEAD(min)) {
      struct timeval tv = {0, 0}; /* don't wait around */
      FD_ZERO(&fds);
      FD_SET(min->peerSock, &fds);
      do {
	ccode = select(FD_SETSIZE, &fds, NULL, NULL, &tv);
      } while ((ccode == SOCKET_ERROR) && (errno == EINTR)) ;
      if(ccode > 0 ) {
	/* make a copy of this stream so we can read the data at our leisure */
	/* without keeping the stream list locked  */
	if(NULL != (temp = lslpAllocStream(&(min->peerAddr)))) {
	  time(&(min->last_read));
	  temp->peerSock = min->peerSock;
	  temp->status = min->status ;
	  temp->state = min->state;
	  temp->next_state = min->next_state;
	  temp->in_time = min->in_time;
	  temp->last_write = min->last_write;
	  temp->last_read = min->last_read;
	  temp->close_interval = min->close_interval;
	  temp->reconnect_interval = min->reconnect_interval ;
	  temp->reconnect_count = min->reconnect_count;
	  _LSLP_INSERT(temp, &ready);
	}
      }
      min = min->next;
    }
    _LSLP_SIGNAL_SEM(list->sem);
  }
  temp = ready.next;
  while(! _LSLP_IS_HEAD(temp)) {
    dispatchTCP(temp, temp->peerSock); 
    _LSLP_UNLINK(temp);
    /* not a "real" stream, so just free it */
    free(temp);
    temp = ready.next;
  }
  return;
}

lslpStream *lslpAllocStream(SOCKADDR_IN *ra)
{
  lslpStream *mi;

  if(NULL != (mi = (lslpStream *)calloc(1, sizeof(lslpStream)))) {
    if(ra != NULL) {
      memcpy(&(mi->peerAddr), ra, sizeof(SOCKADDR_IN));
      time(&(mi->last_read));
    }
  }
  return( mi );
}

void lslpFreeStream(lslpStream *mi)
{
  assert(mi != NULL);
  if(mi == NULL)
    return;
  if(mi->isHead == TRUE ) {
    _LSLP_CLOSE_SEM(mi->sem);
  }
  if(! (mi->status &  LSLP_DA_KEEP_SOCKET)) {
    shutdown(mi->peerSock, SD_BOTH);
    _LSLP_CLOSESOCKET(mi->peerSock);
  }
  if(mi->scopeList != NULL)
    lslpFreeScopeList(mi->scopeList);
  free(mi);
  return;
}


void lslpDestroyStreamList(lslpStream *l, int32 flag)
{
  lslpStream *temp;
  assert(l != NULL);
  assert(_LSLP_IS_HEAD(l));
  while(! _LSLP_IS_HEAD(l->next)) {
    temp = l->next;
    _LSLP_UNLINK(temp);
    lslpFreeStream(temp);
  }
  if(flag) {
    lslpFreeStream(l);
  }
  else {
    _LSLP_CLOSE_SEM(l->sem);
  }
  return;
}

uint32 _lslpStreamCreateAndLinkIn(lslpStream *l, 
				  SOCKADDR_IN *rmtAddr, 
				  uint32 status )
{
  uint32 ccode;
  lslpStream *temp, *head;
  if(l == NULL)
    head = &streamInHead;
  else
    head = l;
  assert(rmtAddr != NULL);
  assert(_LSLP_IS_HEAD(head));

  if(NULL != (temp = lslpAllocStream(rmtAddr))) {
    time(&(temp->in_time));
    temp->status = status;
    memcpy(&(temp->peerAddr), rmtAddr, sizeof(SOCKADDR_IN));
    _LSLP_WAIT_SEM(head->sem, LSLP_Q_WAIT, &ccode);
    assert(ccode == LSLP_WAIT_OK);
    if(ccode == LSLP_WAIT_OK) {
      _LSLP_INSERT(temp, head);
      _LSLP_SIGNAL_SEM(head->sem);
    } else { ccode = SOCKET_ERROR ; lslpFreeStream(temp);  }
  } else { ccode = LSLP_MALLOC_ERROR ; }  
  return(ccode) ;
}

/* this routine allocates a socket ! */
uint32 _lslpMeshCreateAndLinkIn(lslpStream *l, 
				SOCKADDR_IN *rmtAddr, 
				uint32 status, 
				int8 state,
				int8 next_state,
				int8 *scopes)
{
  uint32 ccode;
  lslpStream *temp, *head;
  if(l == NULL)
    head = &streamInHead;
  else
    head = l;
  assert(rmtAddr != NULL);
  assert(_LSLP_IS_HEAD(head));

  if(NULL != (temp = lslpAllocStream(rmtAddr))) {
    time(&(temp->in_time));
    temp->status = status;
    temp->state = state;
    temp->next_state = next_state;
    memcpy(&(temp->peerAddr), rmtAddr, sizeof(SOCKADDR_IN));
    if(NULL != (temp->scopeList = lslpScopeStringToList(scopes, strlen(scopes)))) {
      if(SOCKET_ERROR != (temp->peerSock = socket(AF_INET, SOCK_STREAM, 0))) {
	_LSLP_WAIT_SEM(head->sem, LSLP_Q_WAIT, &ccode);
	assert(ccode == LSLP_WAIT_OK);
	if(ccode == LSLP_WAIT_OK) {
	  _LSLP_INSERT(temp, head);
	  _LSLP_SIGNAL_SEM(head->sem);
	}else {	ccode = SOCKET_ERROR ; 	lslpFreeStream(temp);  }
      } else {	ccode = SOCKET_ERROR ; 	lslpFreeStream(temp);  }
    } else { ccode = LSLP_MALLOC_ERROR; lslpFreeStream(temp); }
  } else { ccode = LSLP_MALLOC_ERROR ; }  
  return(ccode);
}

lslpStream *_lslpFindUnlinkStream(lslpStream *l, SOCKADDR_IN *rmtAddr)
{
  uint32 ccode;
  lslpStream *temp = NULL;
  assert(l != NULL);
  assert(_LSLP_IS_HEAD(l));
  assert(rmtAddr != NULL);
  if(rmtAddr == NULL)
    return(NULL);
  _LSLP_WAIT_SEM(l->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    temp = l->next;
    while(! _LSLP_IS_HEAD(temp)) {
      if(rmtAddr->sin_addr.s_addr  == temp->peerAddr.sin_addr.s_addr) { 
	_LSLP_UNLINK(temp);
	break;
      }
      temp = temp->next;
    } /* while traversing the list */
    if(_LSLP_IS_HEAD(temp))
      temp = NULL;
    _LSLP_SIGNAL_SEM(l->sem);
  } /* we locked the list */
  return(temp);
}

/* no da lists are locked upon entry */
void _lslpMeshHandleDAIndication(lslpWork *w) 
{
  int8 *bptr, **list, **lptr;
  int16 len, msgLen, urls;
  int32 i;
  BOOL err = FALSE;
  /* advance the pointer to the da indication data */
  msgLen = (w->hdr.len - (_LSLP_HDRLEN_WORK(w)));
  bptr = w->hdr.data;
  bptr += (_LSLP_HDRLEN_WORK(w) + 1); 
  urls = _LSLP_GETSHORT(bptr, 0);
  bptr += sizeof(int16);
  if((urls > 0) && (NULL != (lptr = (list = (int8 **)calloc( urls + 1, sizeof(int8 *)))))) {
    for(i = 0; (i < urls) && (msgLen > 0); i++) {
      len = _LSLP_GETSHORT(bptr, 0);
      bptr += sizeof(int16);
      msgLen -= len;
      if(len > 0 && msgLen > 0) {
	/* copy each url from the indication */
	if(NULL != (lptr[i] = malloc(len + 1))) {
	  memcpy(lptr[i], bptr, len);
	  lptr[i][len] = 0x00;
	} else { err = TRUE ; break; }
	bptr += len;
      } else { err = TRUE; break; }  /* if the length bits check out */
    }/* for each url in the da indication */
    if(err == FALSE) {
      /* parse the urls into a list of decomposed url structures */
      lslpAtomizedURL *urlList;
      SOCKADDR_IN myAddr;
      myAddr.sin_family = AF_INET;
      myAddr.sin_port = 0;
      myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      if(NULL != (urlList = _lslpDecodeURLs(lptr, urls))) {
	lslpDirAgent *rmtDA;
	lslpAtomizedURL *temp;
	int32 ccode;
	temp = urlList->next;
	while(! _LSLP_IS_HEAD(temp)) {
	  /* now we need to lock the rmtDA list and see which of these */
	  /* URLs we don't already know about */
	  _LSLP_WAIT_SEM(lslpRemoteDAs.writeSem, LSLP_Q_WAIT, &ccode);
	  assert(ccode == LSLP_WAIT_OK);
	  if(ccode == LSLP_WAIT_OK) {
	    rmtDA = lslpRemoteDAs.next;
	    while(! _LSLP_IS_HEAD(rmtDA)) {
	      if(temp->url != NULL && rmtDA->url->url != NULL) {
		if(temp->urlHash == rmtDA->url->urlHash) {
		  if(! strcasecmp(temp->url, rmtDA->url->url)) {
		    /* mark this guy as someone we already know of */
		    temp->urlHash = -1; 
		  }
		}
	      }
	      rmtDA = rmtDA->next;
	    } /* while we are traversing the locked DA list */
	    _LSLP_SIGNAL_SEM(lslpRemoteDAs.writeSem);
	  } /* if we locked the DA list */
	  temp = temp->next;
	} /* while we are traversing the list of decoded urls */
	/* now that we marked all the new das and we don't have the da list locked, */
	/* generate srvreq msgs to all the new das */
	temp = urlList->next;
	while(! _LSLP_IS_HEAD(temp)) {
	  if(temp->urlHash != -1) {
	    /* we don't know about this DA. send him a srvreq */
	    struct nameAddr *na, *temp_na;
	    lslpWork *msg ;
	    if(NULL != (na = lslpGetAddrFromURL(temp))) {
	      temp_na = na;
	      while(temp_na->myAddr.sin_addr.s_addr != 0) {
		/* change the prototype for dasrvreq to allow NULL scope and */
		/* spi lists */
		if(NULL != (msg = lslpCreateDASrvReq(&(temp_na->myAddr), 
						      &myAddr, 
						      NULL,
						      NULL))) {
		  if (LSLP_WAIT_OK !=  _insertWorkNode(msg)) {
		    msg->txerr(msg);
		  }
		} /* if we formed our message successfully */
		temp_na++;
	      } /* while we are traversing the array of addresses */
	      free(na);	     
	    } /* if we got the array of addresses */
	  } /* if we don't already know about this rmt da */
	  temp = temp->next;
	} /* while we are generating srvreqs */
	lslpFreeAtomizedURLList(urlList, 1);
	/* awaking the outgoing dispatcher */
      } /* if we decoded all the URLS */
    } /* if we extracted all the URLs */
    for(i = 0; i < urls; i++) {
      if(list[i] != NULL)
	free(list[i]);
    }    
    free(list);
  } /* if we got our list of url pointers */
  return;
}


/* called by slp_in dispatch. DA lists are NOT LOCKED */
/* handle incoming mesh ctrl messages */
void _lslpMeshCtrlInHandler(lslpWork *w)
{
  int8 *bptr, action;
  int16 len;
  lslpStream *newIn = NULL;
/* << Tue Jun 29 17:05:45 2004 mdd >> rfc 3528   */
  w->txerr(w);
  return;


  assert((w != NULL) && (w->hdr.data != NULL) ) ;
  assert(w->status & LSLP_STATUS_TCP);
  /* we only handle Mesh Ctrl messages if they come in */
  /* on  a TCP socket */
  if(! (w->status & LSLP_STATUS_TCP)) {
    w->txerr(w);
    return;
  }
  /* looks good so far - let's unwrap the message */
  bptr = w->hdr.data;
  len = (w->hdr.len - (_LSLP_HDRLEN_WORK(w)));
  bptr += _LSLP_HDRLEN_WORK(w);
  action = _LSLP_GETBYTE(bptr, 0);
  if((len > 0) && (action >= LSLP_MESHCTRL_NULL)  && (action <= LSLP_MESHCTRL_KEEPALIVE)) {
    /* action code is valid */
    SOCKADDR_IN cnct;
    w->status |= LSLP_STATUS_KEEP_SOCKET;
    memcpy(&cnct, &(w->na.rmtAddr), sizeof(SOCKADDR_IN));
    /* if this peer is already on our connect list, remove him */
/*     if(NULL != (newIn = _lslpFindUnlinkStream(&connectHead, &cnct))) */
/*       free(newIn); */
    if(NULL != (newIn = _lslpStreamLockFind(&streamInHead, &(w->na.rmtAddr)))) {
      if(newIn->status & LSLP_STATUS_CLIENT_SOCKET) {
	/* convert this stream from a client to a peer */
	newIn->status &= (~LSLP_STATUS_CLIENT_SOCKET);
	newIn->status |= (LSLP_STATUS_PEER_SOCKET | LSLP_STATUS_KEEP_SOCKET | 
			      LSLP_DA_MESH_ENHANCED |  LSLP_DA_PEER_CONNECTED | 
			      LSLP_DA_INITIATED_PEER_CONN    ) ;
	newIn->state = MESH_ACCEPT_STATE;
	newIn->next_state = MESH_RX_PEER_STATE;
      }
      switch(action) {
      case LSLP_MESHCTRL_PEER_IND:

	_mesh_trans[newIn->state][MESH_RX_PEER_STATE](newIn, w);


	_LSLP_SIGNAL_SEM(streamInHead.sem) ;
	break;
      case LSLP_MESHCTRL_DA_IND: 
	_mesh_trans[newIn->state][MESH_RX_DA_STATE](newIn, w);
	_LSLP_SIGNAL_SEM(streamInHead.sem);
	/* put call to handle da indication here to avoid potential deadlock Thu Apr  5 10:46:07 2001 */
	_lslpMeshHandleDAIndication( w );
	break;
      case LSLP_MESHCTRL_DATA: 
	newIn->status |= (LSLP_DA_NEEDS_REGS );
      case LSLP_MESHCTRL_KEEPALIVE:
      case LSLP_MESHCTRL_NULL: 
      default:
	_LSLP_SIGNAL_SEM(streamInHead.sem) ;
	break;
      } /* action switch */

    }
  }

}


void *_stream_txerr_thread(void *work)
{
/* << Tue Jun 29 17:06:29 2004 mdd >> rfc 3528 */

  /* check here for client sockets, which can be streams created by a client connect */
  /*  a client socket is different from a peer socket in the sense that we can just */
  /*  throw a client socket away when it errs out. We don't need to try to reconnect */
  /*  also, a client socket does not have "mesh states. "  */
  lslpStream *err_mesh ;
  freeWorkNode( (lslpWork *)work  ) ;
  return(0) ;

  ((lslpWork *)work)->status &= (~LSLP_STATUS_KEEP_SOCKET);
  if((((lslpWork *)work)->status & LSLP_STATUS_TCP ) && (NULL != (err_mesh = _lslpFindUnlinkStream(&streamInHead, &(((lslpWork *)work)->na.rmtAddr))))) {
    shutdown(err_mesh->peerSock, SD_BOTH);
    _LSLP_CLOSESOCKET(err_mesh->peerSock);
/*     if(((lslpWork *)work)->status & LSLP_STATUS_PEER_SOCKET )  { */
/*       int32 ccode; */
/*       _report_event(LSLP_SEV_IDX(64, LSLP_SEVERITY_MESH), NULL, 0,  */
/* 		    inet_ntoa(err_mesh->peerAddr.sin_addr),  */
/* 		    err_mesh->state, err_mesh->next_state, "mesh node unlinked"); */
/*       err_mesh->status = LSLP_DA_DOWN; */
/*       err_mesh->state = MESH_START_STATE; */
/*       err_mesh->next_state = MESH_CONNECT_STATE; */
/*       _LSLP_WAIT_SEM(connectHead.sem, LSLP_Q_WAIT, &ccode); */
/*       if(ccode == LSLP_WAIT_OK) { */
/* 	_LSLP_INSERT(err_mesh, &connectHead); */
/* 	_LSLP_SIGNAL_SEM(connectHead.sem); */
/*       } else {  lslpFreeStream(err_mesh); } */
/*     } else {  lslpFreeStream(err_mesh); } */
  }

}

/* if this is the result of a socket error with a peer, try once to put the peer */
/* on the connect list */

void stream_txerr(lslpWork *w) 
{
  if( (w->status & LSLP_STATUS_TCP)   && ( w->hdr.errCode == -1 )  ) {
    /* let a worker thread do this to avoid deadlocks  */
    _LSLP_BEGINTHREAD( _stream_txerr_thread, 0, w  ) ;
  } else {   freeWorkNode(w) ; }
}

/* WARNING -- DO NOT call this from any routine that has a */
/* lock on the MeshIn List !!! */

BOOL _lslpIsMeshPeerSock(SOCKET s)
{
  return(_lslpIsStreamSock(&streamInHead, s, LSLP_STATUS_PEER_SOCKET));
}

BOOL _lslpIsClientSock(SOCKET s)
{
  return(_lslpIsStreamSock(&streamInHead, s, LSLP_STATUS_CLIENT_SOCKET));
}

BOOL _lslpIsStreamSock(lslpStream *list, SOCKET s, uint32 status)
{
  int32 ccode;
  lslpStream *mi;
  BOOL found = FALSE;
  assert(_LSLP_IS_HEAD(list));
  _LSLP_WAIT_SEM(list->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    mi = list->next;
    while(! _LSLP_IS_HEAD(mi)) {
      if((mi->status & status )&& (mi->peerSock == s)) {
	found = TRUE;
	break;
      }
      mi = mi->next;
    }
    _LSLP_SIGNAL_SEM(list->sem);
  }
  return(found);
}


/* call iteratively to extract extensions from messages during dispatch */
int8 * lslpGetNextExtension(lslpWork *work, int32 *offset)
{
  int8 *bptr; 

  assert(work != NULL);
  assert(offset != NULL); 
  if(work == NULL || offset == NULL)
    return(NULL);
  bptr = work->hdr.data;
  if(0 == ((*offset) = _LSLP_GETNEXTEXT(bptr, *offset)))
    return(NULL);
  if((*offset) > work->na.data_len )
    return NULL;
  
  return(bptr + (*offset));
}

/* if this routine returns non-NULL, the list is LOCKED !!! */
lslpStream * _lslpStreamLockFindSock (lslpStream *list, SOCKET sock)
{
  int32 ccode;
  lslpStream *temp ;
  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  if(list == NULL || ! (_LSLP_IS_HEAD(list)))
    return(NULL);
  _LSLP_WAIT_SEM(list->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    temp = list->next;
    while(! _LSLP_IS_HEAD(temp)) {
      if(sock  == temp->peerSock) {
	return(temp);
      }
      temp = temp->next;
    }
    _LSLP_SIGNAL_SEM(list->sem);
  }
  return(NULL);
}

/* if this routine returns non-NULL, the list is LOCKED !!! */
lslpStream * _lslpStreamLockFind(lslpStream *list, SOCKADDR_IN *addr)
{
  int32 ccode;
  lslpStream *temp ;
  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  if(list == NULL || ! (_LSLP_IS_HEAD(list)))
    return(NULL);
  assert(addr != NULL);
  if(addr == NULL)
    return(NULL);
  _LSLP_WAIT_SEM(list->sem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    temp = list->next;
    while(! _LSLP_IS_HEAD(temp)) {
      if(addr->sin_addr.s_addr == temp->peerAddr.sin_addr.s_addr) {
	return(temp);
      }
      temp = temp->next;
    }
    _LSLP_SIGNAL_SEM(list->sem);
  }
  return(NULL);
}

BOOL _lslpMeshTestSetScope(lslpStream *list, SOCKADDR_IN *addr, int8 *scopes)
{

  lslpStream *mi = _lslpStreamLockFind(list, addr) ;
  if(mi != NULL) {
    mi->scopeList = lslpScopeStringToList(scopes, strlen(scopes));
    _LSLP_SIGNAL_SEM(list->sem);
    return(TRUE);
  }
  return(FALSE);
}

#define LSLP_CLEAR_BITS 0
#define LSLP_SET_BITS 1
BOOL _lslpMeshSetStatusBits(lslpStream *list, SOCKADDR_IN *addr, uint32 bits, int32 op)
{
  lslpStream *mi = _lslpStreamLockFind(list, addr) ;
  if(mi != NULL) {
    if(op) { mi->status |= bits; } else { mi->status &= (~bits); }
    _LSLP_SIGNAL_SEM(list->sem);
    return(TRUE);
  }
  return(FALSE);
}


BOOL _lslpMeshTestStatusBits(lslpStream *list, SOCKADDR_IN *addr, uint32 bits)
{
  BOOL rcode = FALSE;
  lslpStream *mi = _lslpStreamLockFind(list, addr) ;
  if(mi != NULL) {
    if(mi->status & bits) { rcode = TRUE; }
    _LSLP_SIGNAL_SEM(list->sem);
    return(rcode);
  }
  return(FALSE);
}


#define LSLP_RX_TIME 0
#define LSLP_TX_TIME 1
BOOL _lslpMeshSetTime(lslpStream *list, SOCKADDR_IN *addr, int32 op)
{
  lslpStream *mi = _lslpStreamLockFind(list, addr) ;
  if(mi != NULL) {
    if(op == LSLP_RX_TIME)
      time(&(mi->last_read));
    else
      time(&(mi->last_write));
    _LSLP_SIGNAL_SEM(list->sem);
    return(TRUE);
  }
  return(FALSE);
}
 
void _mesh_link_ret(lslpStream *mi, void *parm) 
{
  int ccode;
  _LSLP_WAIT_SEM(streamInHead.sem, LSLP_Q_WAIT, &ccode);
  assert(LSLP_WAIT_OK == ccode);
  if(LSLP_WAIT_OK == ccode) {
    _LSLP_INSERT(mi, &streamInHead);
    _LSLP_SIGNAL_SEM(streamInHead.sem);
  } else { 
    lslpFreeStream(mi); 
    shutdown(((lslpWork *)parm)->ses_idx, SD_BOTH);
    ((lslpWork *)parm)->status &= (~ LSLP_STATUS_KEEP_SOCKET);
  }	
  return;
}

void _mesh_start_start(lslpStream *mi, void *parm)
{
  return;
}

void _mesh_error(lslpStream *mi, void *parm)
{
  _report_event(LSLP_SEV_IDX(60, LSLP_SEVERITY_ERROR), NULL, 0);
  assert(0);
  return;
}

void _mesh_invalid(lslpStream *mi, void *parm)
{
  _report_event(LSLP_SEV_IDX(59, LSLP_SEVERITY_ERROR), NULL, 0, 
	       mi->state, mi->next_state );
  return;
}

void _mesh_start_connect(lslpStream *mi, void *parm) 
{
  int ccode; 
  lslpDirAgent *da;
  struct sockaddr_in iface;

  assert(! (mi->status &  LSLP_DA_INITIATED_PEER_CONN));
  _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "i am initiating ");
  ccode = 51000;
  setsockopt(mi->peerSock, SOL_SOCKET, SO_RCVBUF, (int8 *)&ccode, sizeof(ccode));
  ccode = 51000;
  setsockopt(mi->peerSock, SOL_SOCKET, SO_SNDBUF, (int8 *)&ccode, sizeof(ccode));
  ccode = 1;
  ccode = setsockopt(mi->peerSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&ccode, sizeof(ccode));
  da = lslpDAs.next;
  /* always connect using the DA interface */
  iface.sin_family = AF_INET;
  iface.sin_port = htons(lslp_config_opport);
  iface.sin_addr.s_addr = da->addr.sin_addr.s_addr;
  ccode = bind(mi->peerSock, (struct sockaddr *)&iface, sizeof(struct sockaddr));
  if(0 == connect(mi->peerSock, (struct sockaddr *)&(mi->peerAddr), sizeof(SOCKADDR_IN))) {
    mi->state = MESH_CONNECT_STATE;
    mi->next_state = MESH_TX_PEER_STATE;
    mi->status |= (LSLP_DA_PEER_CONNECTED | LSLP_STATUS_KEEP_SOCKET);
    time(&mi->last_write);
    _report_event(LSLP_SEV_IDX(63, LSLP_SEVERITY_MESH), NULL, 0, 
		 inet_ntoa(mi->peerAddr.sin_addr), 
		 mi->state, mi->next_state, "i initiated");
  } else { 
    mi->status &= (~(LSLP_DA_PEER_CONNECTED | LSLP_STATUS_KEEP_SOCKET));
    _report_event(LSLP_SEV_IDX(66, LSLP_SEVERITY_MESH), NULL, 0, 
		 inet_ntoa(mi->peerAddr.sin_addr), 
		 mi->state, mi->next_state, "will destroy peer");
  }
  return ;
}


void _mesh_connect_txpeer(lslpStream *mi, void *parm)
{
  assert(! (mi->status & LSLP_DA_INITIATED_PEER_CONN));
  _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "i initiated ");
  if(LSLP_WAIT_OK == lslpMeshCtrl(mi, LSLP_MESHCTRL_PEER_IND)) {
    mi->state = MESH_TX_PEER_STATE;
    mi->next_state = MESH_TX_DA_STATE;
    _report_event(LSLP_SEV_IDX(63, LSLP_SEVERITY_MESH), NULL, 0, 
		 inet_ntoa(mi->peerAddr.sin_addr), 
		 mi->state, mi->next_state, "i initiated");
  }
  return ;
}

void _mesh_txpeer_txda(lslpStream *mi, void *parm)
{
  assert(! (mi->status & LSLP_DA_INITIATED_PEER_CONN));
  _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "i initiated ");
  if(LSLP_WAIT_OK == lslpMeshCtrl(mi, LSLP_MESHCTRL_DA_IND)) {
    mi->state = MESH_TX_DA_STATE;
    mi->next_state = MESH_RX_DA_STATE;
    _report_event(LSLP_SEV_IDX(63, LSLP_SEVERITY_MESH), NULL, 0, 
		 inet_ntoa(mi->peerAddr.sin_addr), 
		 mi->state, mi->next_state, "i initiated");
  }
  return ;
}

void _mesh_txda_rxda(lslpStream *mi, void *parm)
{
  _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "i initiated ");
  assert(! (mi->status & LSLP_DA_INITIATED_PEER_CONN));
  assert(parm != NULL);

  _lslpMeshHandleDAIndication((lslpWork *)parm);
  mi->state = MESH_SYNC_STATE;
  mi->next_state = MESH_START_STATE;

  _report_event(LSLP_SEV_IDX(63, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "i initiated");
  return;
}

/* void _mesh_rxdata_sync(lslpStream *mi, void *parm) */
/* { */
/*   assert(! (mi->status & LSLP_DA_INITIATED_PEER_CONN)); */
/*   _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0,  */
/* 	       inet_ntoa(mi->peerAddr.sin_addr),  */
/* 	       mi->state, mi->next_state, "i initiated "); */
/*   if((mi->status & LSLP_DA_RX_DATA) && (mi->status & LSLP_DA_TX_DATA)) { */
/*     mi->state = MESH_SYNC_STATE; */
/*     mi->next_state = MESH_START_STATE; */
/*     _report_event(LSLP_SEV_IDX(63, LSLP_SEVERITY_MESH), NULL, 0,  */
/* 		 inet_ntoa(mi->peerAddr.sin_addr),  */
/* 		 mi->state, mi->next_state, "i initiated"); */
/*   } */
/*   return ; */
/* } */

void _mesh_start_accept(lslpStream *mi, void *parm)
{
  _mesh_accept_rxpeer(mi, parm);
  return;
}

void _mesh_accept_rxpeer(lslpStream *mi, void *parm)
{
  /* mesh list locked upon entry ! */
  assert(mi->status & LSLP_DA_INITIATED_PEER_CONN) ;
  assert(parm != NULL) ;
  mi->state = MESH_RX_PEER_STATE;
  mi->next_state = MESH_RX_DA_STATE; 
  mi->status |= LSLP_DA_PEER_CONNECTED;
  _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "he initiated ");
  return;
}

void _mesh_rxpeer_rxda(lslpStream *mi, void *parm)
{
  /* mesh list locked upon entry ! */
  /* possible deadlock -- mdd Tue Apr  3 10:58:03 2001*/
  /* removed call to _lslpMeshHandleDAIndication  Thu Apr  5 10:20:44 2001  */
  assert(mi->status & LSLP_DA_INITIATED_PEER_CONN);
  assert(parm != NULL);
  _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "he initiated");

  mi->state = MESH_RX_DA_STATE;
  mi->next_state = MESH_TX_DA_STATE; 

  _report_event(LSLP_SEV_IDX(63, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "he initiated");
  return;
}


void _mesh_rxda_txda(lslpStream *mi, void *parm)
{
  assert(mi->status & LSLP_DA_INITIATED_PEER_CONN );
  _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "he initiated");
  if(LSLP_WAIT_OK == lslpMeshCtrl(mi, LSLP_MESHCTRL_DA_IND)) {
    mi->state = MESH_SYNC_STATE;
    mi->next_state = MESH_START_STATE;
    _report_event(LSLP_SEV_IDX(63, LSLP_SEVERITY_MESH), NULL, 0, 
		 inet_ntoa(mi->peerAddr.sin_addr), 
		 mi->state, mi->next_state, "he initiated");
  }
  return ;
}

void _mesh_connect_rxpeer(lslpStream *mi, void *parm) 
{
  _mesh_race(mi, parm);
}
void _mesh_txpeer_rxpeer(lslpStream *mi, void *parm)
{
  _mesh_race(mi, parm) ;
}
void _mesh_txda_rxpeer(lslpStream *mi, void *parm)
{
  _mesh_race(mi, parm) ;
}
void _mesh_txdata_rxpeer(lslpStream *mi, void *parm)
{
   _mesh_race(mi, parm) ;
}

void _mesh_race(lslpStream *mi, void *parm) 
{
  /* this peer is already connected but he is sending us a peer */
  /* indication. */
  assert(parm != NULL);
  _report_event(LSLP_SEV_IDX(62, LSLP_SEVERITY_MESH), NULL, 0, 
	       inet_ntoa(mi->peerAddr.sin_addr), 
	       mi->state, mi->next_state, "mesh protocol race condition");
  _LSLP_UNLINK(mi);

  mi->status = (LSLP_DA_MESH_ENHANCED | LSLP_DA_INITIATED_PEER_CONN);
  mi->state = MESH_ACCEPT_STATE;
  mi->next_state = MESH_RX_PEER_STATE;
  _mesh_accept_rxpeer(mi, parm) ;
  return;
}

void _mesh_sync_start(lslpStream *mi, void *parm)
{
  shutdown(mi->peerSock, SD_BOTH);
  _LSLP_CLOSESOCKET(mi->peerSock);
  mi->state = MESH_START_STATE;
  mi->next_state = MESH_START_STATE;
  mi->status = 0L;
  return;
}
