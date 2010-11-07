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

#include "lslp-common-defs.h"

extern uint32 dieNow;
extern uint32 waitInterval; 
extern LSLP_CONFIG_ADV lslpConfigAdv;
extern LSLP_CONFIG_ADV lslpConfigDisc;
extern LSLP_CONFIG_ADV lslpConfigTransport; /* transport configuration */
extern lslpStream streamInHead ;
/* extern lslpStream connectHead ; << Tue Jun 29 16:59:00 2004 mdd >> rfc 3528 */
extern BOOL fwd_DA_necessary ;
extern time_t up_time;
/* the list of directory agents */
lslpDirAgentHead lslpDAs = 
{
  (struct lslp_dir_agent *)&lslpDAs, 
  (struct lslp_dir_agent *)&lslpDAs, 
  TRUE, 0
};

int32 lslp_num_remote_das = 0; 
lslpDirAgentHead lslpRemoteDAs = 
{
  (struct lslp_dir_agent *)&lslpRemoteDAs, 
  (struct lslp_dir_agent *)&lslpRemoteDAs, 
  TRUE, 0	
};

lslpDirAgentHead lslpSAs = 
{
  (struct lslp_dir_agent *)&lslpSAs, 
  (struct lslp_dir_agent *)&lslpSAs, 
  TRUE, 0
};

lslpDirAgentHead lslpRemoteSAs = 
{
  (struct lslp_dir_agent *)&lslpRemoteSAs, 
  (struct lslp_dir_agent *)&lslpRemoteSAs, 
  TRUE, 0
};


/*****************************************************/
/***** da structure constructors and destructors *****/
/*****************************************************/

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

BOOL lslpStuffScopeList(int8 **buf, int16 *len, lslpScopeList *list)
{
  lslpScopeList *scopes;
  int16 scopeLen = 0, lenSave;
  int8 *bptr;
  BOOL ccode = FALSE;
  assert(buf != NULL);
  assert(len != NULL && *len > 3);
  assert(list != NULL);
  if (buf == NULL || len == NULL || *len < 3 || list == NULL)
    return(FALSE);
  /* always return true on an empty list so we can continue to build the */
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
      if (scopeLen + (int16)strlen(scopes->scope) < *len)
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


lslpScopeList *lslpUnstuffScopeList(int8 **buf, int16 *len, int16 *err)
{
  int16 tempLen;
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


lslpSPIList *lslpUnstuffSPIList(int8 **buf, int16 *len, int16 *err)
{
  return((lslpSPIList *)lslpUnstuffScopeList(buf, len, err));
}


BOOL lslpStuffSPIList(int8 **buf, int16 *len, lslpSPIList *list)
{
  return(lslpStuffScopeList(buf, len, (lslpScopeList *)list));
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
  lslpDestroyAcceptIDEntry(&(s->accept_entry), FALSE);
  
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

uint32 _lslpInitSAList(void)
{
  if (0 == (_LSLP_CREATE_SEM(lslpSAs.writeSem, 1))) {
    if (0 != (_LSLP_CREATE_SEM(lslpRemoteSAs.writeSem, 1))){
      _LSLP_CLOSE_SEM(lslpSAs.writeSem);
      return(0L);
    }
  }
  else
    return(0L);
  return(0xffffffff);
}	

uint32 _lslpInitDAList(void)
{
  if(0 == (_LSLP_CREATE_SEM(lslpDAs.writeSem, 1))) {
    if (0L != (_LSLP_CREATE_SEM(lslpRemoteDAs.writeSem, 1))) {
      _LSLP_CLOSE_SEM(lslpDAs.writeSem);
      return(0L);
    }
  }
  else
    return(0L);
  return(0xffffffff);
}	


BOOL lslpIsValidDAAddress(uint32 addr)
{
  if((addr == INADDR_ANY) || (addr == 0xFFFFFFFF))
    return(FALSE);
  return(TRUE);
}

BOOL _lslpLinkDA(lslpDirAgent *da, lslpDirAgent *l)
{
  int32 ccode;
  assert(da != NULL);
  assert(l != NULL);
  assert(_LSLP_IS_HEAD(l));
  _LSLP_WAIT_SEM(l->writeSem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if (ccode == LSLP_WAIT_OK)
    {
      _LSLP_INSERT_BEFORE(da, l);
      _LSLP_SIGNAL_SEM(l->writeSem);
    }
  else
    {
      return(FALSE);
    }
  return(TRUE);
}	

BOOL _lslpUnlinkDA(lslpDirAgent *da, lslpDirAgent *l)
{
  int32 ccode;
  assert(da != NULL);
  _LSLP_WAIT_SEM(l->writeSem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if (ccode == LSLP_WAIT_OK)
    {
      /* link to the end of the list */
      _LSLP_UNLINK(da);
      _LSLP_SIGNAL_SEM(l->writeSem);
    }
  else
    {
      return(FALSE);
    }
  return(TRUE);
}	


/* create a directory or a service agent agent */
#define LSLP_FLAG_SA	0
#define LSLP_FLAG_DA	1
uint32 lslpCreateDA(uint32 minRefresh, 
		     int8 *logPath,
		     int8 *url,
		     SOCKADDR_IN *hostAddr,
		     int8 *scopes,
		     int8 *spis,
		     int8 *attrs, 
		     int32 flag)
{
  uint32 ccode = LSLP_OK;
  lslpDirAgent *da = (lslpDirAgent *)calloc(1, sizeof(lslpDirAgent));
  if (da != NULL) {
    if(0 == (_LSLP_CREATE_SEM(da->writeSem, 1))) {
      da->minRefresh = minRefresh;
      da->logPath = strdup(logPath);
      if (lslpValidateDAURL(da, url)) {
	time(&(da->statelessBoot));
	memcpy(&da->addr, hostAddr, sizeof(SOCKADDR_IN));
	if (flag == LSLP_FLAG_SA || lslpValidateDAScopeList(da, scopes)) {
	  if (lslpValidateDASPIList(da, spis)) {
	    if (lslpValidateDAAttrList(da, attrs)) {
	      if (NULL != (da->srvRegList = lslpAllocSrvRegList())) {
		/* create the auth list */
		if (lslpValidateDAAuthList(da)) {
		  /* register ourselves as a service on our local srv reg list */
		  lslpSrvRegList *reg = lslpAllocSrvReg();
		  if(NULL != reg) {
		    int8 *p, *pSave;
		    int16 err;
		    int16 len, plen;
		    if(flag == LSLP_FLAG_SA)
		      reg->srvType = strdup(lslp_sa_url);
		    else
		      reg->srvType = strdup(lslp_da_url);
		    p = strstr(reg->srvType, "//");
		    if(p != NULL)
		      *(p - 1) = 0x00;
		    p = strdup(url);
		    if(p != NULL) {
		      pSave = p;
		      reg->url->url = strdup(p);
		      reg->url->len = strlen(p);
		      reg->url->atomized = _lslpDecodeURLs(&p, 1);
		      free(pSave);
		    }
		    plen = strlen(scopes) + 1;
		    p = calloc(1, plen  + 3);
		    if (p != NULL) {
		      lslpScopeList *temp;
		      _LSLP_SETSHORT(p, plen, 0);
		      memcpy(p + 2, scopes, plen);
		      pSave = p;
		      len = plen + 3;
		      temp = lslpUnstuffScopeList(&p, &len, &err);
		      if(temp != NULL) {
			lslpFreeScopeList(reg->scopeList);
			reg->scopeList = temp;
		      }  
		      free(pSave); 
		    }
		    plen = strlen(attrs) + 1;
		    p = calloc(1, plen + 3);
		    if(p != NULL) {
		      lslpAttrList *temp;		      
		      pSave = p;
		      len = plen + 3;
		      _LSLP_SETSHORT(p, plen, 0);
		      memcpy(p + 2, attrs, plen);
		      temp = lslpUnstuffAttr(&p, &len, &err);
		      if(temp != NULL) {
			lslpFreeAttrList(reg->attrList, TRUE);
			reg->attrList = temp;
		      }
		      free(pSave); 
		    }
		    /* no auths for now */
		    /* put an explicit srvreg for ourselves on ourself */
		    _LSLP_INSERT(reg, (lslpSrvRegList *)da->srvRegList);
		  }

		  if (flag == LSLP_FLAG_SA) {
		    if (_lslpLinkDA(da,  (lslpDirAgent *)&lslpSAs))
		      return(ccode);
		  }
		  /* link this da to the list and return OK */
		  if (_lslpLinkDA(da,  (lslpDirAgent *)&lslpDAs)) {
		    return(ccode);
		  }
		}
	      }  else { ccode = LSLP_INTERNAL_ERROR; }
	      lslpFreeAttrList(da->attrList, TRUE);
	    } else {ccode = LSLP_PARSE_ERROR;}
	    lslpFreeSPIList(da->spiList);
	  } else { ccode = LSLP_PARSE_ERROR;}
	  lslpFreeScopeList(da->scopeList);
	} else {ccode = LSLP_PARSE_ERROR;}
      lslpFreeAtomizedURLList(da->url, 1);
      } else { ccode = LSLP_PARSE_ERROR;}
      /* free the semaphore */
      _LSLP_CLOSE_SEM(da->writeSem);
    } else {ccode = LSLP_INTERNAL_ERROR;}
    free(da);
  } else { ccode = LSLP_MALLOC_ERROR;}
  return(ccode);
}

void lslpFreeDA(lslpDirAgent *da)
{
  assert(da != NULL);
  if (da->logPath != NULL)
    free(da->logPath);
  if (da->url != NULL)
    lslpFreeAtomizedURLList(da->url, 1);
  if (da->scopeList != NULL)
    lslpFreeScopeList(da->scopeList);
  if (da->spiList != NULL)
    lslpFreeSPIList(da->spiList);
  if (da->attrList != NULL)
    lslpFreeAttrList(da->attrList, TRUE);
  if(da->authList != NULL)
    lslpFreeAuthList(da->authList);
  if (da->srvRegList != NULL) {
    lslpFreeSrvRegList(da->srvRegList);
  }
    _LSLP_CLOSE_SEM(da->writeSem);
  free(da);
}	

/* a da can virtualize itself and pose as multiple da's by having a list */
/* of urls. to destroy a da we unlink a url from its list. if the url list is empty */
/* we actually free the da. */

static BOOL _internalDestroyDA(int8 *url, lslpDirAgent *head) 
{

  int32 ccode;
  BOOL rCode = FALSE;

  assert(url != NULL);
  assert((head != NULL) && (_LSLP_IS_HEAD(head)));
  _LSLP_WAIT_SEM(head->writeSem, LSLP_Q_WAIT, &ccode);
 assert(ccode == LSLP_WAIT_OK);
  if (ccode == LSLP_WAIT_OK)
    {
      lslpDirAgent *temp;
      temp = head->next;
      while (! _LSLP_IS_HEAD(temp)) {
	lslpAtomizedURL *tau = temp->url->next;
	while(! _LSLP_IS_HEAD(tau)) {
	  if(! strcasecmp(url, tau->url)) {
	    _LSLP_UNLINK(tau);
	    lslpFreeAtomizedURL(tau);
	    tau = temp->url->next;
	  } else {tau = tau->next; }
	}
	if ((temp->url != NULL) && (_LSLP_IS_EMPTY(temp->url))) {
	  _LSLP_UNLINK(temp);
	  lslpFreeDA(temp);
	  rCode = TRUE;
	  break;
	}
	temp = temp->next;
      }	
      _LSLP_SIGNAL_SEM(head->writeSem);
    }
  return(rCode);
}

BOOL _lslpDestroyDA(int8 *url)
{
  return(_internalDestroyDA(url, (lslpDirAgent *)&lslpDAs) );
}	

BOOL _lslpDestroyRemoteDA(int8 *url)
{
  return(_internalDestroyDA(url, (lslpDirAgent *)&lslpRemoteDAs) );
}

/* only call this routine to completely destroy a list of DAs */
  /* it doesn't look for URL's. */
void _lslpDestroyDAList(lslpDirAgent *l, BOOL dynFlag)
{
  int32 ccode;

  assert(l != NULL);
  assert(_LSLP_IS_HEAD(l));
  _LSLP_WAIT_SEM(l->writeSem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  if (ccode == LSLP_WAIT_OK)
    {
      lslpDirAgent *temp;
      temp = l->next;
      while (! _LSLP_IS_HEAD(temp))
	{
	  _LSLP_UNLINK(temp);
	  lslpFreeDA(temp);
	  temp = l->next;
	}
    }
  _LSLP_CLOSE_SEM(l->writeSem);
  if(TRUE == dynFlag)
  lslpFreeDA(l);
  return;
}	

/* ensure the url is well formed and does not contain any illegal characters */
/* if sucessful, creates an atomized URL */
BOOL lslpValidateDAURL(lslpDirAgent *da, int8 *url)
{
  assert(da != NULL);
  assert(url != NULL);
  
  if(NULL != (da->url = _lslpDecodeURLs(&url, 1)))
    return(TRUE);
  return(FALSE);
}
	
BOOL lslpValidateDAScopeList(lslpDirAgent *da, int8 *scopes)
{
  assert(da != NULL);

  if(NULL != (da->scopeList = lslpScopeStringToList(scopes,strlen(scopes)))) {
    return(TRUE);
  }
  return(FALSE);
}	

BOOL lslpValidateDASPIList(lslpDirAgent *da, int8* spis)
{
  assert(da != NULL);
  if(NULL != (da->spiList = lslpScopeStringToList(spis, strlen(spis)))) {
    return(TRUE);
  }
  return(FALSE);
}	

BOOL lslpValidateDAAttrList(lslpDirAgent *da, int8 *attr)
{
  assert(da != NULL);
  if (attr == NULL)
    return(TRUE);
  if (NULL != (da->attrList = _lslpDecodeAttrString(attr)))
    return(TRUE);
  return(FALSE);
}	

BOOL lslpValidateDAAuthList(lslpDirAgent *da)
{
  assert(da != NULL);
  return TRUE;
  
  /* << Tue May 18 14:48:17 2004 mdd >>

  in anticipation of deprecating authorization blocks, 
  leave the DA with a NULL authList
  */ 
/*   if (NULL != (da->authList = lslpAllocAuthList())) { */
/*     lslpAuthBlock *temp; */
/*     lslpSPIList *spis = da->spiList->next; */
/*     while (! _LSLP_IS_HEAD(spis) && spis->scope != NULL) { */
/*       if (NULL != (temp = lslpAllocAuthBlock())) { */
/* 	temp->spiLen = strlen(spis->scope) + 1; */
/* 	if (NULL != (temp->spi = strdup(spis->scope))) { */
/* 	  time( &(temp->timestamp) ); */
/* 	  temp->descriptor = 0x8000; */
/* 	  temp->len = 10 + temp->spiLen + 0; */
/* 	  _LSLP_INSERT_BEFORE(temp, da->authList); */
/* 	} else { */
/* 	  lslpFreeAuthBlock(temp); */
/* 	  break;	 */
/* 	} */
/*       } */
/*       else */
/* 	break; */
/*       spis = spis->next; */
/*     } */
/*     if (_LSLP_IS_HEAD(spis)) */
/*       return(TRUE); */
/*   } */
/*   return(FALSE); */
}	


/* we never lock the local da list while generating da adverts */
/* however, this means we cannot dereference the srvreg list */
void lslp_daAdvertThread(void )
{
  static time_t last, now, last_disc;
  static BOOL once ;
  LSLP_CONFIG_ADV *configList ;
  lslpDirAgent *da;
  lslpWork *adv;

  /* first, check to see if the user did not configure the da to do */
  /* advs or srvrqs. If not, cause the da to configure itself using */
  /* the existing transport interfaces */


  if (_LSLP_IS_EMPTY(&lslpConfigAdv) || _LSLP_IS_EMPTY(&lslpConfigDisc) ) {
    BOOL disc, find;
    LSLP_CONFIG_ADV *dup;
    LSLP_CONFIG_ADV *t = lslpConfigTransport.next;
    disc = _LSLP_IS_EMPTY(&lslpConfigDisc);
    find = _LSLP_IS_EMPTY(&lslpConfigAdv);

    while(! _LSLP_IS_HEAD(t)) {
      if((t->mask != 0xffffffff) && (t->flag & IFF_BROADCAST)) {
	if(NULL != (configList = (LSLP_CONFIG_ADV *)calloc(1, sizeof(LSLP_CONFIG_ADV)))) {
	  configList->iface = t->iface;
	  configList->addr = t->addr;
	  configList->flag = LSLP_STATUS_BCAST;
	  if(disc == TRUE && find == TRUE) {
	    configList->frequency = lslp_config_da_beat;
	    if(NULL != (dup  = (LSLP_CONFIG_ADV *)malloc(sizeof(LSLP_CONFIG_ADV)))) {
	      memcpy(dup, configList, sizeof(LSLP_CONFIG_ADV));
	      dup->frequency = lslp_da_find ;
	      _LSLP_INSERT(dup, &lslpConfigDisc);
	    }
	    _LSLP_INSERT(configList, &lslpConfigAdv) ;
	  } else {
	    if(disc == TRUE) {
	      configList->frequency = lslp_da_find;
	      _LSLP_INSERT(dup, &lslpConfigDisc);
	    } else {
	      configList->frequency = lslp_config_da_beat;
	      _LSLP_INSERT(configList, &lslpConfigAdv) ;
	    }
	  }
	}
      }
      t = t->next;
    }
  }

  /* what time is it now ? */
  time(&now);
  /* see if we need to generate any recurring da advertisements */
  configList = lslpConfigAdv.next;
  while( ! _LSLP_IS_HEAD(configList)) {
    if((once == FALSE) || (configList->last == 0) || ( (now - configList->last) > configList->frequency)) {
      time(&configList->last);
      da = lslpDAs.next;
      while (! _LSLP_IS_HEAD(da)) {
	lslpAtomizedURL *tau = da->url;
	while(tau != NULL && (! _LSLP_IS_HEAD(tau->next))) {
	  if (NULL != (adv = lslpCreateDAAdvert(da, tau->next->url, NULL, configList))) {
	    /* for multi-homed DAs force a bind to the correct sending interface */
	    adv->na.myAddr.sin_family = AF_INET;
	    adv->na.myAddr.sin_port = htons(lslp_config_opport);
	    adv->na.myAddr.sin_addr.s_addr = configList->iface;
	    if(LSLP_WAIT_OK != _insertWorkNode(adv)) {
	      adv->txerr(adv);
	    } 
	  }
	  tau = tau->next;
	}
	da = da->next;
      }
    }
    configList = configList->next;
  }

  /* see if we need to generate any recurring da service requests */
  if((now - up_time ) > lslp_config_start_wait) {
    configList = lslpConfigDisc.next;
    while( ! _LSLP_IS_HEAD(configList)) {
      if((once == FALSE) || (configList->last == 0) || ( (now - configList->last) > configList->frequency)) {
	SOCKADDR_IN to, from;
	time(&configList->last);
	/* the first time through we defer sending the request */
	if(once == FALSE) {
	  configList = configList->next;
	  continue;
	}
	to.sin_family = from.sin_family = AF_INET;
	to.sin_port = from.sin_port = htons(lslp_config_opport);
	to.sin_addr.s_addr = configList->addr;
	from.sin_addr.s_addr = configList->iface;
	if(NULL != (adv = lslpCreateDASrvReq(&to, &from, NULL, NULL))) {
	  adv->status = configList->flag;
	  adv->status |= LSLP_STATUS_RQ ;
	  if(( adv->status & LSLP_STATUS_MCAST) || (adv->status & LSLP_STATUS_BCAST)) {
	    int8 flags;
	    flags = _LSLP_GETFLAGS(((int8 *)adv->hdr.data));
	    flags |= LSLP_FLAGS_MCAST;
	    _LSLP_SETFLAGS(((int8 *)adv->hdr.data), flags);
	  }
	  if(LSLP_WAIT_OK != _insertWorkNode(adv)) {
	    adv->txerr(adv);
	  }
	}
      }
      configList = configList->next;
    }
  }

  /* do our regular multicast da advert per the spec */
  if(once == FALSE || (now - last) >= lslp_config_da_beat) {
    time(&last);
    da = lslpDAs.next;
    while (! _LSLP_IS_HEAD(da)) {
      lslpAtomizedURL *tau = da->url;

      while(tau != NULL && (! _LSLP_IS_HEAD(tau->next))) {
	if (NULL != (adv = lslpCreateDAAdvert(da, tau->next->url, NULL, NULL))) {
	  int8 flags;
	  flags = _LSLP_GETFLAGS(((int8 *)adv->hdr.data));
	  flags |= LSLP_FLAGS_MCAST;
	  _LSLP_SETFLAGS(((int8 *)adv->hdr.data), flags);
	  adv->status |= LSLP_STATUS_MCAST;
	  adv->na.myAddr.sin_family = AF_INET;
	  adv->na.myAddr.sin_port = htons(lslp_config_opport);
	  if(LSLP_WAIT_OK != _insertWorkNode(adv)) {
	    adv->txerr(adv);
	  } 
	}
	tau = tau->next;
      }
      da = da->next;
    }
  }

  /* do our regular multicast da discovery  per the spec */
  if((now - up_time ) > lslp_config_start_wait) {
    if(once == FALSE || (now - last_disc) >= lslp_da_find) {
      SOCKADDR_IN to, from;
      /* don't send any requests out the first time through */
      time(&last_disc);
      if(once == TRUE) {
	to.sin_family = from.sin_family = AF_INET;
	to.sin_port = from.sin_port = htons(lslp_config_opport);
	to.sin_addr.s_addr =  _LSLP_MCAST ;
	from.sin_addr.s_addr = INADDR_ANY;
	if(NULL != (adv = lslpCreateDASrvReq(&to, &from, NULL, NULL))) {
	  int8 flags;
	  flags = _LSLP_GETFLAGS(((int8 *)adv->hdr.data));
	  flags |= LSLP_FLAGS_MCAST;
	  _LSLP_SETFLAGS(((int8 *)adv->hdr.data), flags);
	  adv->status = (LSLP_STATUS_MCAST | LSLP_STATUS_RQ );
	  if(LSLP_WAIT_OK != _insertWorkNode(adv)) {
	    adv->txerr(adv);
	  }
	}
      }
    }
  }
  once = TRUE;
  return ;
}

lslpWork *lslpCreateDASrvReq(SOCKADDR_IN *addr_to,
			      SOCKADDR_IN *addr_from,
			      lslpScopeList *scopeList, 
			      lslpSPIList *spiList )
{
  lslpWork *msg;
  int8 *workBuf;
  assert(addr_to !=  NULL);
  assert(addr_from != NULL);

  if(NULL != (workBuf = (int8 *)calloc((1432 - LSLP_MIN_HDR), sizeof(int8)))) {
    int8 *bptr = workBuf;
    int16 len = (1432 - LSLP_MIN_HDR);
    if(NULL != (msg = allocWorkNode())) {
      msg->type = LSLP_Q_SLP_OUT;
      msg->na.version = LSLP_NAMEADDR_VERSION;
      memcpy(&(msg->na.myAddr), addr_from, sizeof(SOCKADDR_IN));
      memcpy((&msg->na.rmtAddr), addr_to, sizeof(SOCKADDR_IN));
      msg->hdr.ver = LSLP_PROTO_VER;
      msg->hdr.msgid = LSLP_SRVRQST;
      msg->hdr.nextExt = 0;
      msg->hdr.errCode = LSLP_OK;
      msg->hdr.langLen = LSLP_EN_US_LEN;
      memcpy(msg->hdr.lang, LSLP_EN_US, LSLP_EN_US_LEN);
      msg->status = LSLP_STATUS_RQ | LSLP_STATUS_UDP;
      msg->hdr.xid = _lslp_get_xid();
      msg->ses_idx = _getSession(&(msg->na));
      /* build the data portion of the srvreq message */
      /* set the pr responder list to nul */
      _LSLP_SETSHORT(bptr, 0, 0);

      /* set the length of the srv type string */
      _LSLP_SETSHORT(bptr, 23, 2);
      /* copy the srv type string */
      memcpy(bptr + 4, lslp_da_url, 23);
      bptr += ((2 * sizeof(int16)) + 23) ; 
      len -= ((2 * sizeof(int16)) + 23);

      /* stuff the scope list */
      if(scopeList != NULL) {
	lslpStuffScopeList(&bptr, &len, scopeList);
      } else {
	/* stuff a zero length scope list */
	_LSLP_SETSHORT(bptr, 0, 0);
	bptr += sizeof(int16); len -= sizeof(int16);
      }
      /* stuff a zero-length predicate */
      _LSLP_SETSHORT(bptr, 0, 0);
      bptr += sizeof(int16); len -= sizeof(int16);
      /* stuff the spi */
      if(spiList != NULL) {
	lslpStuffSPIList(&bptr, &len, spiList);
      } else {
	/* stuff a zero-length spi list */
	_LSLP_SETSHORT(bptr, 0, 0);
	bptr += sizeof(int16); len -= sizeof(int16);
      }
      msg->hdr.data = (void *)&(msg->na.static_data[0]);
      msg->hdr.data = (void *)formSLPMsg(&(msg->hdr), 
					  workBuf, 
					  (1432 - LSLP_MIN_HDR) - len, 
					  FALSE);
	
      /* if we allocated or work node */
      if(msg->hdr.data == NULL) {
	msg->txerr(msg);
	msg = NULL;
      }
    } /* if we allocated our work node */
    free(workBuf);
  } /* if we allocated our working buffer */
  return(msg);
}


lslpWork *lslpCreateDAAdvert(lslpDirAgent *da, int8 *url,
			      lslpWork *r, 
			      LSLP_CONFIG_ADV *config)
{
  int8 workBuf[1432 - LSLP_MIN_HDR];
  lslpWork *temp;
  int16 len = (1432 - (LSLP_MIN_HDR + LSLP_EN_US_LEN));
  int8 *bptr = workBuf;
  assert(da != NULL);
  if (NULL != (temp = allocWorkNode()) ) {
    temp->type = LSLP_Q_SLP_OUT;
    temp->na.version = LSLP_NAMEADDR_VERSION;
    memcpy(&(temp->na.myAddr), &(da->addr), sizeof(SOCKADDR_IN));
    temp->hdr.ver = LSLP_PROTO_VER;
    temp->hdr.msgid = LSLP_DAADVERT;
    temp->hdr.nextExt = 0;
    temp->hdr.errCode = LSLP_OK;
    temp->hdr.langLen = LSLP_EN_US_LEN;	
    memcpy(temp->hdr.lang, LSLP_EN_US, LSLP_EN_US_LEN);
    if(config != NULL) {
      temp->hdr.xid = 0;
      temp->status = config->flag;
      temp->status &= (~LSLP_STATUS_RQ);
      temp->status |= (LSLP_STATUS_RP | LSLP_STATUS_DONT_LINGER | LSLP_STATUS_UDP );
      temp->na.rmtAddr.sin_family = AF_INET;
      temp->na.rmtAddr.sin_port = htons(lslp_config_opport);
      if(config->flag & LSLP_STATUS_BCAST) {
	temp->hdr.flags = LSLP_FLAGS_MCAST;
	temp->status |= LSLP_STATUS_BCAST ;
	temp->na.rmtAddr.sin_addr.s_addr = config->addr;
      } else {
	temp->na.rmtAddr.sin_addr.s_addr = config->addr;
	if(config->flag &  LSLP_STATUS_MCAST) {
	  temp->hdr.flags = LSLP_STATUS_MCAST;
	  temp->status |=  LSLP_STATUS_MCAST ;
	} else {
	  temp->ses_idx = _getSession(&(temp->na));
	}
      }
    } else {
      if(r == NULL) {
	temp->status = LSLP_STATUS_MCAST  ;
	temp->na.rmtAddr.sin_family = AF_INET;
	temp->na.rmtAddr.sin_port =  htons(lslp_config_opport);
	temp->na.rmtAddr.sin_addr.s_addr = _LSLP_MCAST;
 	temp->hdr.flags = LSLP_FLAGS_MCAST;
	temp->status &= (~LSLP_STATUS_RQ);
	temp->status |= (LSLP_STATUS_RP | LSLP_STATUS_DONT_LINGER | LSLP_STATUS_UDP );
	temp->hdr.xid = 0;
      } else {
	temp->status = r->status;
	temp->status &= (~LSLP_STATUS_RQ);
	temp->status |= LSLP_STATUS_RP;
	temp->ses_idx = r->ses_idx;
	temp->na.sock = r->na.sock;
	memcpy(&(temp->na.rmtAddr), &(r->na.rmtAddr), sizeof(SOCKADDR_IN));
	temp->hdr.xid = r->hdr.xid;
      }
    }

    memset(workBuf, 0x00, (1432 - LSLP_MIN_HDR));
    /* error code */
    _LSLP_SETSHORT(bptr, 0, 0);
    bptr += sizeof(int16); len -= sizeof(int16);
    /* stateless boot */
    _LSLP_SETLONG(bptr, da->statelessBoot, 0);
    bptr += sizeof(int32); len -= sizeof(int32);
    /* url len */
    _LSLP_SETSHORT(bptr, strlen(url), 0);
    bptr += sizeof(int16); len -= sizeof(int16);
    if (len  >= (int16)(strlen(url))) {
      memcpy(bptr, url, strlen(url));
      bptr += strlen(url);
      len -= (strlen(url));
      if (lslpStuffScopeList( &bptr, &len, da->scopeList)) {
	if (lslpStuffAttrList(&bptr, &len, da->attrList, NULL)) {
	  if (lslpStuffSPIList(&bptr, &len, da->spiList)) {
	    /* stuff a hard-coded 0 for the auth block count */
 	    if(lslpStuffAuthList(&bptr, &len, da->authList)) { 
	      if(temp->status & LSLP_STATUS_TCP) {
	      temp->hdr.data = (void *)formSLPMsg(&(temp->hdr),
						  workBuf, bptr - workBuf, TRUE);
	      } else {
		temp->hdr.data = (void *)(&temp->na.static_data[0]);
		temp->hdr.data = (void *)formSLPMsg(&(temp->hdr), 
						    workBuf, bptr - workBuf , FALSE);
	      }
	      temp->na.data_len = (int16) _LSLP_GETLENGTH((int8 *)temp->hdr.data);
	      return(temp);
	    }
	  } /* if we stuffed the spi list */
	} /* if we stuffed the attr list */
      } /* if we stuffed the scopelist */
    } /* if we copied the url */
    temp->txerr(temp);
  }
  return(NULL);
}	


/* if return == NULL, we already know about this guy and we need to do nothing */
/* (or the message was malformed) */
/* if return != NULL, register our services with the url returned */
BOOL _lslpCreateDirAgentFromAdvert(lslpWork *work)
{
  int16 tempLen, msgLen;
  LSLP_CONFIG_ADV *t;
  time_t stateless_boot, temp_boot = 0;
  lslpDirAgent *tempDA = NULL, *existing_da = NULL;
  lslpAuthBlock *list = NULL, *temp = NULL;
  int8 *url = NULL, *scopes = NULL, *spis = NULL, *attrs = NULL, *bytePtr;
  uint32 da_status = LSLP_DA_NOT_PEER;
  

  /* determine if this remote da is actually the local da */
  /* see if the daadvert originated from one of our local addresses */
  t = lslpConfigTransport.next;
  while(! _LSLP_IS_HEAD(t)) {
    if(t->iface == work->na.rmtAddr.sin_addr.s_addr)
      return(FALSE);
    t= t->next;
  }

  /* work->hdr is already initialized */
  msgLen = work->hdr.len;
  bytePtr = work->hdr.data;
  tempLen = _LSLP_GETSHORT(bytePtr, (_LSLP_HDRLEN_WORK(work)));  /* get and check the error code */
  if(tempLen != 0) {
    return(FALSE);
  }
  bytePtr += ((_LSLP_HDRLEN_WORK(work)) + 2); /* advance past the error code */
  msgLen -= ((_LSLP_HDRLEN_WORK(work)) + 2);
  if(msgLen > 4) {
    stateless_boot = _LSLP_GETLONG(bytePtr, 0);  /* get the stateless boot timestamp */
    bytePtr += 4;                               /* advance past the timestamp */
    msgLen -= 4;
    /* check for a stateless boot of zero - it means the da is going away */
    if(stateless_boot == 0 ) {
      /* reset his status and return */ 
      da_offline((lslpDirAgent *)&lslpRemoteDAs, work->ses_idx);
      return(FALSE);
    }

    tempLen = _LSLP_GETSHORT(bytePtr, 0);       /* get the url length */
    bytePtr += 2;                               /* advance past the url length */
    if((msgLen > (2 + tempLen)) && (tempLen > 32)) { /* if the url is the right size */
      msgLen -= (2 + tempLen);
      if (NULL != (url = (int8 *)calloc(tempLen + 1, sizeof(int8)))) {
	memcpy(url, bytePtr, tempLen);
      }  else 
	goto cdafa_errExit;

      /* check to make certain we're not processing our own da advert for some reason  */
      /* someone could send us a bogus da advert that described this local da */

      if(stateless_boot <= _lslpFindExistingDA(url, (lslpDirAgent *)&lslpDAs, TRUE)) { 
	/* someone sent us a daadvert that describes this da */ 
	/* free resources and return */
	free(url);
	return(FALSE);
      }
 
      bytePtr += tempLen;                          /* advance past the url */
      tempLen = _LSLP_GETSHORT(bytePtr, 0);        /* get the scope-list length */
      bytePtr += 2;                                /* advance to the scope-list */ 
      if((msgLen > (2 + tempLen)) && tempLen > 6) { /* must be at least "default" */
	msgLen -= (2 + tempLen);
	if(NULL != (scopes = (int8 *)calloc(tempLen + 1, sizeof(int8)))) {
	  memcpy(scopes, bytePtr, tempLen); 
	} else  goto cdafa_errExit;

	/* now we have the url and the boot timestamp, plus the scope string */ 
	/* determine if we already know about this remote da and that he did not restart. */
	/* if we do know him and he as not restarted, we might need to change the mesh state */

	da_status = mslp_existing_da_init(work, scopes);
	if(NULL != (existing_da = _lslpFindLockExistingDA(url, (lslpDirAgent *)&lslpRemoteDAs))) {
	  if(stateless_boot == existing_da->statelessBoot ) {
	    /* we already know about this guy. Update his mesh status */ 
	    if(da_status) {
	      existing_da->status |= da_status;
	      existing_da->socket = work->ses_idx ;

	      /* if this da is a peer, we need to snag his IP address */ 
	      if(da_status == LSLP_DA_PEER_CONNECTED) {
		memcpy(&(existing_da->addr), &(work->na.rmtAddr), sizeof(SOCKADDR_IN));
	      }
	    }
	    _LSLP_SIGNAL_SEM(((lslpDirAgent *)&lslpRemoteDAs)->writeSem);
	    free(url);
	    free(scopes);
	    return FALSE;
	  } else {
	    /* remote da has rebooted remove him and start over */ 
	    _LSLP_UNLINK(existing_da);
	    lslpFreeDA(existing_da);
	  }
	  
	  /* remember to unlock the remote da list */ 
	  _LSLP_SIGNAL_SEM(((lslpDirAgent *)&lslpRemoteDAs)->writeSem);
	}

	/* marshall the remainder of the information */ 
	bytePtr += tempLen;                          /* advance past the scopes-list */
	tempLen = _LSLP_GETSHORT(bytePtr, 0);        /* get the attr-list length */
	bytePtr += 2;                                /* advance to the attr-list */
	if(msgLen > 2 + tempLen) {
	  msgLen -= (2 + tempLen);
	  if((tempLen == 0) || (NULL != (attrs = (int8 *)calloc(tempLen + 1, sizeof(int8)))))
	    if(NULL != attrs)
	      memcpy(attrs, bytePtr, tempLen);
	  bytePtr += tempLen;                          /* advance past the attrs-list */
	  tempLen = _LSLP_GETSHORT(bytePtr, 0);        /* get the spi-list length */
	  bytePtr += 2;                                /* advance to the spi-list */
	  if(msgLen > 2 + tempLen) {
	    msgLen -= (2 + tempLen);
	    if((tempLen == 0) || (NULL != (spis = (int8 *)calloc(tempLen + 1, sizeof(int8)))))
	      if(NULL != spis)
		memcpy(spis, bytePtr, tempLen);
	    bytePtr += tempLen;                         /* advance past the spi-list */
	    tempLen = _LSLP_GETBYTE(bytePtr, 0);        /* get the number of auth-blocks */
	    bytePtr += 1;                               /* advance to the auth blocks */
	    msgLen -= 1;
	    if((tempLen == 0) || (NULL != (list = lslpAllocAuthList()))) {
	      if(list != NULL) {
		while((tempLen > 0) && (msgLen > 10)) {
		  if(NULL != (temp = lslpAllocAuthBlock())) {
		    temp->descriptor = _LSLP_GETSHORT(bytePtr, 0); /* get the block structure descriptor */
		    bytePtr += 2;                                  /* advance to the block length */
		    temp->len = _LSLP_GETSHORT(bytePtr, 0);        /* get the block length */
		    bytePtr += 2;                                  /* advance to the timestamp */
		    temp->timestamp = _LSLP_GETLONG(bytePtr, 0);   /* get the timestamp */
		    bytePtr += 4;                                  /* advance to the spi length */
		    temp->spiLen = _LSLP_GETSHORT(bytePtr, 0);     /* get the spi length */
		    bytePtr += 2;                                  /* advance to the spi */ 
		    msgLen -= 10;
		    if(msgLen >= (temp->spiLen)) {
		      if(NULL != (temp->spi = (uint8 *)calloc(temp->spiLen + 1, sizeof(uint8)))) {
			memcpy(temp->spi, bytePtr, temp->spiLen);  /* copy the spi */
			bytePtr += temp->spiLen;                   /* advance to the next block */
			if(temp->len > (temp->spiLen + 10)) { /* if there really is an auth block */
			  if (NULL != (temp->block = (uint8 *)calloc(temp->len + 1, sizeof(uint8)))) {
			    memcpy(temp->block, bytePtr, temp->len);
			    bytePtr += temp->len;
			  } else {msgLen = -1;} /* if we alloced the auth block buffer */
			}
			_LSLP_INSERT(temp, list);                /* insert the auth block into the list */
			temp = NULL;
		      } else { msgLen = -1;}/* if we alloced the spi buffer */
		    } else { msgLen = -1;} /* if there is enough room for this entire auth block */
		  }else { msgLen = -1;} /* if we alloced the auth block */
		  tempLen--; /* decrement the number of auth blocks */
		} /* while there is room and there are auth blocks to process */
	     	if(msgLen < 0) { /* oh oh - we ran out of message length */
		  if(temp != NULL) {
		    lslpFreeAuthBlock(temp);
		    temp = NULL; 
		  }
		  lslpFreeAuthList(list);
		  list = NULL;
		  goto cdafa_errExit; /* msg was malformed, damaged, or malicious */
		}
	      } /* if we alloced the auth block list head */
	      if(NULL != (tempDA = (lslpDirAgent *)calloc(1, sizeof(lslpDirAgent)))) {
		if(0 == _LSLP_CREATE_SEM(tempDA->writeSem, 1)) {
		  if(lslpValidateDAURL(tempDA, url)) { /* dups the url string, among other things */
		    struct nameAddr *url_addr;
		    tempDA->statelessBoot = stateless_boot;
		    if(NULL != (url_addr = lslpGetAddrFromURL(tempDA->url))) {
		      memcpy(&(tempDA->addr), &(url_addr->myAddr), sizeof(SOCKADDR_IN)) ;
		      if( (lslpValidateDAScopeList(tempDA, scopes)) &&  ( lslpIsValidDAAddress(tempDA->addr.sin_addr.s_addr))) {
			if(lslpValidateDASPIList(tempDA, spis)) {
			  free(spis);
			  spis = NULL;
			  if(lslpValidateDAAttrList(tempDA, attrs)) {
			    free(attrs);
			    attrs = NULL;
			    tempDA->authList = list;

			    /* if this is an existing da that has restarted */
			    /* we need to remove him from our list */
			    if (temp_boot != 0) {
			      /* if false, we just removed a url - the da still exists */
			      /* however, we should never have a remote da with multiple urls */
			      assert(_lslpDestroyRemoteDA(url));
			      lslp_num_remote_das--;
			    }
			    /* sets status bits ! */ 
			    lslpSetDAMeshEnhancedFlag(tempDA);
			    gettimeofday(&tempDA->last_adv, NULL);
			    /* set further status bits for the new remote DA */
			    if(da_status) {
			      tempDA->status |= da_status;
			      tempDA->socket = work->ses_idx;
			      if(da_status == LSLP_DA_PEER_CONNECTED) {
				memcpy(&(existing_da->addr), &(work->na.rmtAddr), sizeof(SOCKADDR_IN));
			      }
			    }
			    if (_lslpLinkDA(tempDA,  (lslpDirAgent *)&lslpRemoteDAs)) {
			      lslp_num_remote_das++;
			      if(url != NULL) {
				free(url);
				url = NULL;
			      }
			      if(scopes != NULL) {
				free(scopes);
				scopes = NULL;
			      }
			      free(url_addr);
			      return(TRUE);
			    }
			  } /* if the attrs are valid */
			} /* if the spis are valid */
		      }
		      if(scopes != NULL) {
			free(scopes);
			scopes = NULL;
		      } /* if the scopes are valid */
		      free(url_addr);
		    } /* if we got the host addresses from the url */
		    if(url != NULL) {
		      free(url);
		      url = NULL;
		    }
		  } /* if the url is valid */
		} /* if we got the da write semaphore */
	      } /* if we alloced the new da */
	    }
	  }
	}
      }
    }
  }
cdafa_errExit:
  if(url != NULL)
    free(url);
  if(scopes != NULL)
    free(scopes);
  if(attrs != NULL)
    free(attrs);
  if(spis != NULL)
    free(spis);
  if(temp != NULL)
    lslpFreeAuthBlock(temp);
  if(list != NULL)
    lslpFreeAuthList(list);
  if(tempDA != NULL)
    lslpFreeDA(tempDA);
  return(FALSE);
}

/* if we find the da, return its time stamp, otherwise return zero */
/* this has the side effect of updating the last_adv timestamp */ 
time_t _lslpFindExistingDA(int8 *url, lslpDirAgent *l, BOOL lock)
{
  int32 ccode;
  lslpAtomizedURL *tempURL;
  LSLP_SEM_T s;
  time_t stateless_boot = 0;
  
  assert(url != NULL);
  assert(l != NULL);
  assert(_LSLP_IS_HEAD(l));

  if(lock == TRUE) { 
    s = l->writeSem;
    _LSLP_WAIT_SEM(s, LSLP_Q_WAIT, &ccode);
    assert(ccode == LSLP_WAIT_OK);
  }

  l = l->next;
  while ((! _LSLP_IS_HEAD(l)) && (l->url != NULL)) {
    assert(l->url != NULL);
    tempURL = l->url->next;
    while(! _LSLP_IS_HEAD(tempURL)) {
      if(! strcasecmp(tempURL->url, url)) {
	stateless_boot = l->statelessBoot;
	gettimeofday(&(l->last_adv), NULL);
	if(lock == TRUE) {
	  _LSLP_SIGNAL_SEM(s);
	}
	return stateless_boot;
      } 
      tempURL = tempURL->next;
    }
    l = l->next;
  }
  if(lock == TRUE) {
    _LSLP_SIGNAL_SEM(s); 
  }
  return stateless_boot;
}

lslpDirAgent* _lslpFindLockExistingDA(uint8* url, lslpDirAgent* l)
{
  int32 ccode;
  lslpAtomizedURL *tempURL;
  lslpDirAgent *temp;
  
  
  assert(url != NULL);
  assert(l != NULL);
  assert(_LSLP_IS_HEAD(l));

  _LSLP_WAIT_SEM(l->writeSem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);

  temp = l->next;
  while ((! _LSLP_IS_HEAD(temp)) && (temp->url != NULL)) {
    assert(temp->url != NULL);
    tempURL = temp->url->next;
    while(! _LSLP_IS_HEAD(tempURL)) {
      if(! strcasecmp(tempURL->url, url)) {

	return temp;
      } 
      tempURL = tempURL->next;
    }
    temp = temp->next;
  }
  _LSLP_SIGNAL_SEM(l->writeSem);
  
  return NULL;
  
}


/*****************************************************************
 * if da has the mesh-enhanced keyword attribute, this routine will
 * set the status flag accordingly. Otherwise, it will do nothing.
 * 
 *****************************************************************/
void lslpSetDAMeshEnhancedFlag(lslpDirAgent *l)
{
  lslpAttrList *temp ;
  assert(l != NULL);
  assert(l->attrList != NULL);
  if(l == NULL || l->attrList == NULL)
    return;
  temp = l->attrList->next;
  while(! _LSLP_IS_HEAD(temp)) {
    if(! strcasecmp(temp->name, "mesh-enhanced")) {
      l->status |= LSLP_DA_MESH_ENHANCED;
      return;
    }
    temp = temp->next;
  } /* while we are traversing the attribute list */
  return;
}


/*****************************************************************
 * Given an DA socket, returns the stream associated with that DA, or NULL.
 *
 * Parameters: list - stream list head to be searched
 *             socket - the socket descriptor in use by the da 
 * 
 * Returns: pointer to the stream associated with the DA, or NULL if none found
 * NOTE: list is LOCKED upon a non-null return. Caller MUST UNLOCK IT
 *
 ******************************************************************/

lslpStream * _get_da_stream(lslpStream *list, uint32 socket) 
{
  return _lslpStreamLockFindSock(list, socket);
}

void set_da_stream(SOCKADDR_IN *addr, uint32 socket, lslpDirAgent *da)
{
  assert(addr != NULL);
  assert(da != NULL);
  memcpy(&(da->addr), addr, sizeof(SOCKADDR_IN));
  da->socket = socket;
  return ;
}


/*****************************************************************
 * Given a stream socket, return a pointer to the associated DA, or NULL
 *
 * Parameters: list - da list to be searched
 *             socket - socket we are looking for
 *
 * Returns: pointer to the DA containing the socket
 * NOTE: da list is LOCKED upon non-null return. Caller MUST UNLOCK IT 
 *****************************************************************/

lslpDirAgent * _get_stream_da(lslpDirAgent *list, uint32 socket)
{
  uint32 ccode;
  lslpDirAgent *temp ;

  assert(list != NULL);
  assert(_LSLP_IS_HEAD(list));
  
  _LSLP_WAIT_SEM(list->writeSem, LSLP_Q_WAIT, &ccode);
  assert(ccode == LSLP_WAIT_OK);
  
  temp = list->next;
  while( ! _LSLP_IS_HEAD(temp)) {
    if(temp->socket == socket)
      return temp;
    temp = temp->next;
  }
  _LSLP_SIGNAL_SEM(list->writeSem);
  return NULL;
}



BOOL da_offline(lslpDirAgent *list, uint32 socket) 
{

  lslpDirAgent *temp;
  
  if(NULL != (temp = _get_stream_da(list, socket))) {
    temp->status |= LSLP_DA_DOWN;
    temp->socket = 0xffffffff;
    memset(&(temp->addr), 0x00, sizeof(SOCKADDR_IN));
    _LSLP_SIGNAL_SEM(list->writeSem);
    return TRUE;
  }
  return FALSE;
}

BOOL da_online(lslpDirAgent *list, uint32 socket, SOCKADDR_IN *addr)
{
  lslpDirAgent *temp;
  
  if(NULL != (temp = _get_stream_da(list, socket))) {
    temp->status &= ~(LSLP_DA_DOWN);
    temp->socket = socket;
    memcpy(&(temp->addr), addr, sizeof(SOCKADDR_IN));
    _LSLP_SIGNAL_SEM(list->writeSem);
    return TRUE;
  }
  return FALSE;
}
