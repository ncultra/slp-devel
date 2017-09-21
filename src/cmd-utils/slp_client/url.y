%{

#include "slp_client.h"



  /* prototypes and globals go here */
  void urlerror(int8 *, ...);
  int32 urlwrap(void);
  int32 urllex(void);   
  int32 urlparse(void);
  void url_close_lexer(void *handle);
  void *url_init_lexer(int8 *s);
  void print_atomized_url(lslpAtomizedURL *url);
  
  lslpAtomizedURL urlHead = 
    {
      &urlHead, &urlHead, TRUE, NULL, 0
    };

  static lslpAtomList srvcHead = {&srvcHead, &srvcHead, TRUE, NULL, 0};
  static lslpAtomList siteHead = {&siteHead, &siteHead, TRUE, NULL, 0};
  static lslpAtomList userHead = {&userHead, &userHead, TRUE, NULL, 0};
  static lslpAtomList portHead = {&portHead, &portHead, TRUE, NULL, 0};
  static lslpAtomList pathHead = {&pathHead, &pathHead, TRUE, NULL, 0};
  static lslpAtomList attrHead = {&attrHead, &attrHead, TRUE, NULL, 0};

  %}

/* definitions for ytab.h */
%name-prefix="url"  

   %union {
     int32 _i;
     int8 *_s;
     lslpAtomList *_atl;
     lslpAtomizedURL *_aturl;
   }

%token<_i> _RESERVED
%token<_s> _HEXDIG _STAG _RESNAME _NAME _ELEMENT _IPADDR  _IP6ADDR _IPX
%token<_s> _AT _ZONE

 /* typecast the non-terminals */

 /* %type <_i> */
%type <_s> ip_site ipx_site at_site hostport host service_id
%type <_aturl> url
%type <_atl> service_list service site path_list path_el attr_list attr_el
%type <_atl> url_part sap

%%
url: service_list sap {
  if (NULL != ($$ = (lslpAtomizedURL *)calloc(1, sizeof(lslpAtomizedURL)))) {
    int32 urlLen = 12;
    lslpAtomList *temp = srvcHead.next;
    init_atomized_url($$);
    while (! _LSLP_IS_HEAD(temp) && temp->str != NULL && urlLen > 0 && urlLen < 0x800 ) {
      urlLen += strlen(temp->str);
      temp = temp->next;
    }
    temp = siteHead.next;
    while (! _LSLP_IS_HEAD(temp) && temp->str != NULL && urlLen > 0 && urlLen < 0x800) {
      urlLen += strlen(temp->str) + 2;
      temp = temp->next;
    }
    temp = pathHead.next;
    while (! _LSLP_IS_HEAD(temp) && temp->str != NULL && urlLen > 0 && urlLen < 0x800) {
      urlLen += strlen(temp->str) + 1;
      temp = temp->next;
    }
    temp = attrHead.next;
    while (! _LSLP_IS_HEAD(temp) && temp->str != NULL && urlLen > 0 && urlLen < 0x800) {
      urlLen += strlen(temp->str) + 1;
      temp = temp->next;
    }
      
    if ( urlLen > 0 && urlLen < 0x800 && 
	 NULL != ($$->url = (int8 *)calloc(urlLen + 1, sizeof(int8)))) {
      temp = srvcHead.next;
      if (! _LSLP_IS_HEAD(temp) && temp->str != NULL) {
	strncpy($$->url, temp->str, urlLen);
	_LSLP_UNLINK(temp);
	_LSLP_INSERT_BEFORE(temp, &$$->srvcs);
	temp = srvcHead.next;
      }
      while (! _LSLP_IS_HEAD(temp) && temp->str != NULL) {
	int len = urlLen - strlen($$->url);
	if (len > 0)
	  strncat($$->url, temp->str, len);
	_LSLP_UNLINK(temp);
	_LSLP_INSERT_BEFORE(temp, &$$->srvcs);
	temp = srvcHead.next;
      }
      temp = siteHead.next;
      while (! _LSLP_IS_HEAD(temp) && temp->str != NULL) {
	int len = urlLen - strlen($$->url);
	if (len > 0)
	  strncat($$->url, temp->str, len);
	_LSLP_UNLINK(temp);
	_LSLP_INSERT_BEFORE(temp, &$$->site);
	temp = siteHead.next;
      }
      temp = pathHead.next;
      while (! _LSLP_IS_HEAD(temp) && temp->str != NULL) {
	int len = urlLen - strlen($$->url);
	if (len-- > 1) {
	  strncat($$->url, "/", 1);
	  strncat($$->url, temp->str, len);
	    
	}
	_LSLP_UNLINK(temp);
	_LSLP_INSERT_BEFORE(temp, &$$->path);
	temp = pathHead.next;
      }
      temp = attrHead.next;
      while (! _LSLP_IS_HEAD(temp) && temp->str != NULL) {
	int len = urlLen - strlen($$->url);
	if (len-- > 1) {
	  strncat($$->url, ";", 1);
	  strncat($$->url, temp->str, len);
	}
	_LSLP_UNLINK(temp);
	_LSLP_INSERT_BEFORE(temp, &$$->attrs);
	temp = attrHead.next;
      }
      $$->urlHash = lslpCheckSum($$->url, strlen($$->url));
      _LSLP_INSERT_BEFORE($$, &urlHead);
    } else {
      lslpFreeAtomizedURL($$);
    }
  }
}
;
service_list: service	{
  _LSLP_INSERT_BEFORE($1, &srvcHead);
}

| service_list service {
    _LSLP_INSERT_BEFORE($2, &srvcHead);
}
;

service:  _RESNAME ':'	{
  if (NULL != ($$ = lslpAllocAtom())) {
    int len = 2 + strlen($1);
    if (len > 2 && len < 0x800 ) {
      $$->next = $$->prev = $$;
      if (NULL != ($$->str = (int8 *)calloc(len, sizeof(int8)))) {
	strncpy($$->str, $1, len - 2);
	strncat($$->str, ":", 1);
	$$->hash = lslpCheckSum($$->str, strlen($$->str));
      } else {
	free($$);
	$$ = NULL;
      }
    }
  }
}

|	  _RESNAME '.' _RESNAME ':'	{
  if (NULL != ($$ = lslpAllocAtom())) {
    int len = 3 + strlen($1) + strlen($3);
    $$->next = $$->prev = $$;
    if (len > 0 && len < 0x800 && 
	NULL != ($$->str = (int8 *)calloc(len, sizeof(int8)))) {
      strncpy($$->str, $1, len);
      strncat($$->str, ".", 1);
      strncat($$->str, $3, --len - strlen($$->str));
      strncat($$->str, ":", 1);	
      $$->hash = lslpCheckSum($$->str, strlen($$->str));
      _LSLP_INSERT_BEFORE($$, &srvcHead);
    } else {
      free($$);
      $$ = NULL;
    }
  }
}
;

sap:  site {
  $$ = &siteHead;
  if ($1 != NULL)
    {
      _LSLP_INSERT_BEFORE($1, $$);
    }
}
| 	site url_part 	{
  $$ = &siteHead;
  /* url_part is already linked into the path list, only link the site */
  if ($1 != NULL) {
      _LSLP_INSERT_BEFORE($1, $$);
  }
}
;

site:  ip_site {
  if ($1 != NULL) {
    if(NULL != ($$ = lslpAllocAtom())) {
      $$->next = $$->prev = $$;
      $$->str = $1;
      $$->hash = lslpCheckSum($$->str, strlen($$->str));
    }
  } else
    $$ = NULL;
}
|	ipx_site {
  if ($1 != NULL) {
    int len = strlen($1);
    if (len > 0 && len < 0x800) {
      if(NULL != ($$ = lslpAllocAtom())) {
	$$->str = $1;
	$$->hash = lslpCheckSum($$->str, len);
      }
    }
  } else
    $$ = NULL;
}
|	at_site {
  if ($1 != NULL) {
    int len = strlen($1);
    if (len > 0 && len < 0x800) {
      if(NULL != ($$ = lslpAllocAtom())) {
	$$->str = $1;
	$$->hash = lslpCheckSum($$->str, len);
      }
    }
  } else
    $$ = NULL;
}
 
|       service_id {
  
  if ($1 != NULL) {
    int len = strlen($1);
    if (len > 0 && len < 0x800) {
      if(NULL != ($$ =  lslpAllocAtom())) {
	$$->str = $1;
	$$->hash = lslpCheckSum($$->str, len);
      }
    }
  } else
    $$ = NULL;
}

;

/* for ip_site only, we also store away the user name, host, and port */
/* separately. These items are duplicated in the site production, but */
/* we need to process them independently later so we might as well */
/* separate them now. We also need the site production as a string, so */
/* we end up storing redundant data for ip sites. */ 

ip_site: '/''/' {
  $$ = strdup("//localhost");
}
|   '/''/' hostport {
  int len = strlen($3) + 3;
  if(len > 0 && len < 0x800 && 
     (NULL !=($$ = (int8 *)calloc(len + 1, sizeof(int8))))) {
    snprintf($$, len, "//%s", $3);
  }
}
|   '/''/' _RESNAME '@' hostport {
  int len;
  
  lslpAtomList *user = lslpAllocAtom();
  if (user) {
    user->str = strdup($3);
    _LSLP_INSERT_BEFORE(user, &userHead);
  }
  
  len = strlen($3) + strlen($5) + 3;
  if (len > 0 && len < 0x800) {
    if(NULL != ($$ = (int8 *)calloc(len + 1, sizeof(int8)))) {
      snprintf($$, len, "//%s@%s", $3, $5);
    }
  }
}
;

service_id: _RESNAME {
  int len = strlen($1);
  if (len > 0 && len < 0x800)
    $$ = strdup($1);
}
;

ipx_site:  _IPX {
  int len = strlen($1);
  if (len > 0 && len < 0x800)
    $$ = strdup($1);
  else
    $$ = NULL;
}
;

at_site:  _AT _ZONE ':' _ZONE ':' _ZONE {
  int len = 3 + strlen($1) + strlen($2) + strlen($4) + strlen($6);
  if( len > 0 && len < 0x800 && NULL != ($$ = (int8 *)calloc(len + 1, sizeof(int8))))
    snprintf($$, len, "%s%s:%s:%s", $1, $2, $4, $6);
}
;

hostport: host {
  $$ = $1;
}
|	host ':' _HEXDIG {
  int len;
  
  len = 3 + strlen($1) + strlen($3);
  if(len > 0 && len < 0x800 && NULL != ($$ = (int8 *)calloc(len, sizeof(int8)))) {
    strncpy($$, $1, len);
    strncat($$, ":", len - strlen($$));
    strncat($$, $3, len - strlen($$));
  }
}

;

host: _RESNAME   {
  int len = strlen($1);
  if (len > 0 && len < 0x800)
    $$ = $1;
}
|  _IPADDR {
  int len = strlen($1);
  if (len > 0 && len < 0x800)
    $$ = $1;
}

| _IP6ADDR {
  int len = strlen($1);
  if (len > 0 && len < 0x800)
    $$ = $1;
}

;

/* url_part is a dummy element that simplifies the grammar. */
/* the actual path and attribute components are already linked to pathHead */
url_part: path_list {
  ;	
}
|	attr_list {
  ;		
}
|	path_list attr_list {
  ;
}
;

path_list:	path_el {
  $$ = &pathHead;
  if ($1 != NULL)
    {
      _LSLP_INSERT_BEFORE($1, $$);
    }
}
| path_list path_el {
  $$ = &pathHead;
  if ($2 != NULL)
    {
      _LSLP_INSERT_BEFORE($2, $$);
    }
}

;

path_el: '/' {
  /* dangling path slash - do nothing */
  $$ = NULL;
}
|    '/' _ELEMENT  {
  int len = 1 + strlen($2);
  
  if(len > 0 && len < 0x800 && 
     NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList)))) {
    $$->prev = $$->next = $$;
    if(NULL != ($$->str = (int8 *)calloc(len, sizeof(int8)))) {
      strncpy($$->str, $2, len);
      $$->hash = lslpCheckSum($$->str, strlen($$->str));
    } else {
      free($$);
      $$ = NULL;
    }
  }
}
|	 '/' _RESNAME  {
  int len = 1 + strlen($2);
  if( len > 0 && len < 0x800 && 
      NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList)))) {
    $$->prev = $$->next = $$;
    if(NULL != ($$->str = (int8 *)calloc(len, sizeof(int8)))) {
      strncpy($$->str, $2, len);
      $$->hash = lslpCheckSum($$->str, strlen($$->str));
    } else {
      free($$);
      $$ = NULL;
    }
  }
}
| 	'/' _HEXDIG {
  int len = 1 + strlen($2);
  if(len > 0 && len < 0x800 && 
     NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList)))) {
    $$->prev = $$->next = $$;
    if(NULL != ($$->str = (int8 *)calloc(len, sizeof(int8)))) {
      strncpy($$->str, $2, len);
      $$->hash = lslpCheckSum($$->str, strlen($$->str));
    } else {
      free($$);
      $$ = NULL;
    }
  }
}
;

attr_list: attr_el {
  $$ = &attrHead;
  if ($1 != NULL)
    {
      _LSLP_INSERT_BEFORE($1, $$);
    }
}
| attr_list attr_el {
  $$ = &attrHead;
  if ($2 != NULL)
    {
      _LSLP_INSERT_BEFORE($2, $$);
    }
}
;

attr_el: ';' _ELEMENT  {
  int len = 1 + strlen($2);
  if(len > 0 && len < 0x800 && 
     NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList)))) {
    $$->prev = $$->next = $$;
    if(NULL != ($$->str = (int8 *)calloc(len, sizeof(int8)))) {
      strncpy($$->str, $2, len);
      $$->hash = lslpCheckSum($$->str, strlen($$->str));
    } else {
      free($$);
      $$ = NULL;
    }
  }
}

|  ';' _ELEMENT '=' _ELEMENT {
  int len = 2 + strlen($2) + strlen($4);
  
  if(len > 0 && len < 0x800 && 
     NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
    {
      $$->prev = $$->next = $$;
      if(NULL != ($$->str = (int8 *)calloc(len, sizeof(int8)))) {
	strncpy($$->str, $2, len);
	strncat($$->str, "=", len - strlen($$->str));
	strncat($$->str, $4, len - strlen($$->str));
	$$->hash = lslpCheckSum($$->str, strlen($$->str));
      } else {
	free($$);
	$$ = NULL;
      }
    }
}
;

%% 

void print_atom_list(lslpAtomList *list)
{
  lslpAtomList *tmp = list->next;
  SLP_TRACE("print_atom_list %p\n", list);
  SLP_TRACE("atom->isHead: %i next: %p prev: %p string: %p\n", 
	    list->isHead, list->next, list->prev, list->str);
  while (tmp && !_LSLP_IS_HEAD(tmp)) {
    if (tmp->str)  printf("atom: %s\n", tmp->str);
    tmp = tmp->next;
  }
  
}


void print_atomized_url(lslpAtomizedURL *url)
{
  SLP_TRACE("print_atomized_url %p\n", url);
  if (url) {
    if (url->url) printf("url: %s\n", url->url);
    print_atom_list(&url->srvcs);
    print_atom_list(&url->site);
    print_atom_list(&url->host);
    print_atom_list(&url->user);
    print_atom_list(&url->port);
    print_atom_list(&url->path);
    print_atom_list(&url->attrs);
  }
}

lslpAtomList *lslpAllocAtom(void)
{
  lslpAtomList *temp = (lslpAtomList *)calloc(1, sizeof(lslpAtomList));
  if (temp != NULL) {
    temp->next = temp->prev = temp;
  }
  return(temp);
}	

lslpAtomList *lslpAllocAtomList(void)
{
  lslpAtomList *temp =lslpAllocAtom(); 
  if (temp != NULL)
    temp->isHead = TRUE;
  return(temp);
}	

void lslpFreeAtom(lslpAtomList *a )
{
  assert(a != NULL);
  if (a->str != NULL) {
    free(a->str);
  }
  
  free(a);
  return;
}	

void lslpFreeAtomList(lslpAtomList *l, int32 flag)
{
  
  lslpAtomList *temp;
  assert(l != NULL);
  assert(_LSLP_IS_HEAD(l));
  while (! _LSLP_IS_EMPTY(l)) {
    temp = l->next;
    _LSLP_UNLINK(temp);
    lslpFreeAtom(temp);
  }
  if (flag)
    lslpFreeAtom(l);
  return;
}	

lslpAtomizedURL *lslpAllocAtomizedURL(void)
{
  lslpAtomizedURL *temp = (lslpAtomizedURL *)calloc(1, sizeof(lslpAtomizedURL));
  if (temp != NULL)
    {
      temp->prev = temp->next = temp;
    }
  return(temp);
}	

lslpAtomizedURL *lslpAllocAtomizedURLList(void)
{
  lslpAtomizedURL *temp = lslpAllocAtomizedURL();
  if (temp != NULL)
    {
      temp->isHead = TRUE;
      temp->next = temp->prev = temp;
      
    }
  return(temp);
}	

void lslpFreeAtomizedURL(lslpAtomizedURL *u)
{
  assert(u != NULL);
  if (u->url != NULL)
    free(u->url);
  if (! _LSLP_IS_EMPTY(&(u->srvcs)))
    lslpFreeAtomList(&(u->srvcs), 0);
  if (! _LSLP_IS_EMPTY(&(u->site)))
    lslpFreeAtomList(&(u->site), 0);
  if (! _LSLP_IS_EMPTY(&(u->host)))
    lslpFreeAtomList(&(u->host), 0);
  if (! _LSLP_IS_EMPTY(&(u->user)))
    lslpFreeAtomList(&(u->user), 0);
  if (! _LSLP_IS_EMPTY(&(u->port)))
    lslpFreeAtomList(&(u->port), 0);
  if (! _LSLP_IS_EMPTY(&(u->path)))
    lslpFreeAtomList(&(u->path), 0);
  if (!  _LSLP_IS_EMPTY(&(u->attrs)))
    lslpFreeAtomList(&(u->attrs), 0);
  free(u);
  return;
}	

void lslpFreeAtomizedURLList(lslpAtomizedURL *l, int32 flag)
{

  lslpAtomizedURL *temp;
  assert(l != NULL);
  assert(_LSLP_IS_HEAD(l));
  while (!_LSLP_IS_EMPTY(l)) {
    temp = l->next;
    _LSLP_UNLINK(temp);
    lslpFreeAtomizedURL(temp);
  }
  if (flag)
    free(l);
  return;
}	

void lslpInitAtomLists(void)
{
  _LSLP_INIT_HEAD(&srvcHead);
  _LSLP_INIT_HEAD(&siteHead);
  _LSLP_INIT_HEAD(&userHead);
  _LSLP_INIT_HEAD(&portHead);
  _LSLP_INIT_HEAD(&pathHead);
  _LSLP_INIT_HEAD(&attrHead);
  return;
}	

void lslpCleanUpURLLists(void)
{
 
  lslpFreeAtomList(&srvcHead, 0);
  lslpFreeAtomList(&siteHead, 0);
  lslpFreeAtomList(&userHead, 0);
  lslpFreeAtomList(&portHead, 0);
  lslpFreeAtomList(&pathHead, 0);
  lslpFreeAtomList(&attrHead, 0);
  lslpFreeAtomizedURLList(&urlHead, 0);
}

lslpAtomizedURL *_lslpDecodeURLs(int8 *u[], int32 count)
{
  int32 i;
  void *lexer ;
  lslpAtomizedURL *temp = NULL;
  
  assert(u != NULL && u[count - 1] != NULL);
  lslpInitAtomLists();
  _LSLP_INIT_HEAD(&urlHead);
  for (i = 0; i < count; i++) {
    if (NULL == u[i])
      break;
    if((0 != (lexer = url_init_lexer(u[i])))) {
      if(urlparse())
	lslpCleanUpURLLists();
      url_close_lexer(lexer);
    }
  }
  if (! _LSLP_IS_EMPTY(&urlHead)) {
    lslpAtomizedURL *parsed = urlHead.next;
    temp = lslpAllocAtomizedURLList();
    _LSLP_UNLINK(parsed);
    _LSLP_INSERT(parsed, temp->next);
  }
  lslpCleanUpURLLists();
  
  return(temp);	
}	 

/*
 * Local variables:
 *  c-file-style: "gnu"
 * End:
 */
