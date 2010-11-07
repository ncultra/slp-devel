
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




%{

#include "lslp-common-defs.h"

/* prototypes and globals go here */





#ifdef TEST_HARNESS
 uint32 lslpCheckSum(uint8 *s, int16 l)
   {
     return 0;
   }
 
#endif

int32 urlparse(void);
void url_close_lexer(uint32 handle);
uint32 url_init_lexer(int8 *s);

lslpAtomizedURL urlHead = 
{
	&urlHead, &urlHead, TRUE
};

static lslpAtomList srvcHead = {&srvcHead, &srvcHead, TRUE, };
static lslpAtomList siteHead = {&siteHead, &siteHead, TRUE, };
static lslpAtomList pathHead = {&pathHead, &pathHead, TRUE, };
static lslpAtomList attrHead = {&attrHead, &attrHead, TRUE, };

%}

/* definitions for ytab.h */

%union {
	int32 _i;
	int8 *_s;
	lslpAtomList *_atl;
	lslpAtomizedURL *_aturl;
}

%token<_i> _RESERVED
%token<_s> _HEXDIG _STAG _RESNAME _NAME _ELEMENT _IPADDR  _IPX
%token<_s> _AT _ZONE

/* typecast the non-terminals */

/* %type <_i> */
%type <_s> ip_site ipx_site at_site hostport host service_id
%type <_aturl> url
%type <_atl> service_list service site path_list path_el attr_list attr_el
%type <_atl> url_part sap

%%
url: service_list sap {
			if (NULL != ($$ = (lslpAtomizedURL *)calloc(1, sizeof(lslpAtomizedURL))))
			{
				int32 urlLen = 1;
				lslpAtomList *temp = srvcHead.next;
				while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
				{
					urlLen += strlen(temp->str);
					temp = temp->next;
				}
				temp = siteHead.next;
				while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
				{
					urlLen += strlen(temp->str);
					temp = temp->next;
				}
				temp = pathHead.next;
				while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
				{
					urlLen += strlen(temp->str) + 1;
					temp = temp->next;
				}
				temp = attrHead.next;
				while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
				{
					urlLen += strlen(temp->str) + 1;
					temp = temp->next;
				}
				if (NULL != ($$->url = (int8 *)calloc(urlLen, sizeof(int8))))
				{
					temp = srvcHead.next;
					if (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcpy($$->url, temp->str);
						temp = temp->next;
					}
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat($$->url, temp->str);
						temp = temp->next;
					}
					temp = siteHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat($$->url, temp->str);
						temp = temp->next;
					}
					temp = pathHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat($$->url, "/");
						strcat($$->url, temp->str);
						temp = temp->next;
					}
					temp = attrHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat($$->url, ";");
						strcat($$->url, temp->str);
						temp = temp->next;
					}
					$$->urlHash = lslpCheckSum($$->url, (int16)strlen($$->url));
				}

				/* make certain the listheads are initialized */
				$$->srvcs.next = $$->srvcs.prev = &($$->srvcs);
				$$->srvcs.isHead = TRUE;
				$$->site.next = $$->site.prev = &($$->site);
				$$->site.isHead = TRUE;
				$$->path.next = $$->path.prev = &($$->path);
				$$->path.isHead = TRUE;
				$$->attrs.next = $$->attrs.prev = &($$->attrs);
				$$->attrs.isHead = TRUE;
				if (! _LSLP_IS_EMPTY(&srvcHead ))
				{
					_LSLP_LINK_HEAD(&($$->srvcs), &srvcHead);
				}
				if (! _LSLP_IS_EMPTY(&siteHead))
				{
					_LSLP_LINK_HEAD(&($$->site), &siteHead);
				}
				if (! _LSLP_IS_EMPTY(&pathHead))
				{
					_LSLP_LINK_HEAD(&($$->path), &pathHead);
				}
				if (! _LSLP_IS_EMPTY(&attrHead))
				{
					_LSLP_LINK_HEAD(&($$->attrs), &attrHead);
				}
				_LSLP_INSERT_BEFORE($$, &urlHead);
				lslpInitAtomLists();
			}
		}
	;
service_list: service	{
			$$ = &srvcHead;
			if ($1 != NULL)
			{
				_LSLP_INSERT_BEFORE($1, $$);
			}
		}

	| service_list service {
			$$ = &srvcHead;
			if ($2 != NULL)
			{
				_LSLP_INSERT_BEFORE($2, $$);
			}
		}
	;

service:  _RESNAME ':'	{
			if (NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				$$->next = $$->prev = $$;
				if (NULL != ($$->str = (int8 *)calloc(2 + strlen($1), sizeof(int8))))
				{
					strcpy($$->str, $1);
					strcat($$->str, ":");	
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
				else
				{
					free($$);
					$$ = NULL;
				}
			}

		}
	|	  _RESNAME '.' _RESNAME ':'	{
			if (NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				$$->next = $$->prev = $$;
				if (NULL != ($$->str = (int8 *)calloc(3 + strlen($1) + strlen($3), sizeof(int8))))
				{
					strcpy($$->str, $1);
					strcat($$->str, ".");
					strcat($$->str, $3);
					strcat($$->str, ":");	
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
				else
				{
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
			if ($1 != NULL)
			{
					_LSLP_INSERT_BEFORE($1, $$);
			}
		}
	;

site:  ip_site {
			if ($1 != NULL)
			{
				if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					$$->next = $$->prev = $$;
					$$->str = $1;
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
			}
			else
				$$ = NULL;
		}
	|	ipx_site {
			if ($1 != NULL)
			{
				if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					$$->next = $$->prev = $$;
					$$->str = $1;
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
			}
			else
				$$ = NULL;
		}
	|	at_site {
			if ($1 != NULL)
			{
				if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					$$->next = $$->prev = $$;
					$$->str = $1;
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
			}
			else
				$$ = NULL;
		}
	

|       service_id {
  
			if ($1 != NULL)
			{
				if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					$$->next = $$->prev = $$;
					$$->str = $1;
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
			}
			else
				$$ = NULL;
  
}
;

ip_site: '/''/' {
			$$ = strdup("//");
		}
	|   '/''/' hostport {
			if(NULL != $3 && (NULL !=($$ = (int8 *)calloc(3 + strlen($3), sizeof(int8)))))
			{
				strcpy($$, "//");
				strcat($$, $3);
			}

		}
	|   '/''/' _RESNAME '@' hostport {
			if(NULL != $5 && (NULL !=($$ = (int8 *)calloc(4 + strlen($3) + strlen($5), sizeof(int8)))))
			{
				strcpy($$, "//");
				strcat($$, $3);
				strcat($$, "@");
				strcat($$, $5);
			}
		}
	;

service_id: _RESNAME {
	  $$ = strdup($1);
	}
        ;


ipx_site: _IPX {
			$$ = $1;
		}
	;

at_site: _AT _ZONE ':' _ZONE ':' _ZONE {
			if(NULL != ($$ = (int8 *)calloc(strlen($1) + strlen($2) + strlen($4) + strlen($6) + 3, sizeof(int8))))
			{
				strcpy($$, $1);
				strcat($$, $2);
				strcat($$, ":");
				strcat($$, $4);
				strcat($$, ":");
				strcat($$, $6);
			}
		}
	;

hostport: host {
			$$ = $1;	
		}
	|	host ':' _HEXDIG {
			if ($1 != NULL)
			{
				if(NULL != ($$ = (int8 *)calloc(strlen($1) + strlen($3) + 2, sizeof(int8))))
				{
					strcpy($$, $1);
					strcat($$, ":");
					strcat($$, $3);
				}
			}
			else
				$$ = NULL;	
		}
	;

host: _RESNAME {
			$$ = $1;
		}
	|  _IPADDR {
			$$ = $1;
		}
	;

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
			if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				$$->prev = $$->next = $$;
				if(NULL != ($$->str = (int8 *)calloc(1 + strlen($2), sizeof(int8))))
				{
					strcpy($$->str, $2);
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));

				}
				else
				{
					free($$);
					$$ = NULL;
				}
			}

		}
	|	 '/' _RESNAME  {
			if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				$$->prev = $$->next = $$;
				if(NULL != ($$->str = (int8 *)calloc(1 + strlen($2), sizeof(int8))))
				{
					strcpy($$->str, $2);
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
				else
				{
					free($$);
					$$ = NULL;
				}
			}

		}
	| 	'/' _HEXDIG {
			if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				$$->prev = $$->next = $$;
				if(NULL != ($$->str = (int8 *)calloc(1 + strlen($2), sizeof(int8))))
				{
					strcpy($$->str, $2);
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
				else
				{
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
			if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				$$->prev = $$->next = $$;
				if(NULL != ($$->str = (int8 *)calloc(1 + strlen($2), sizeof(int8))))
				{
					strcpy($$->str, $2);
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
				else
				{
					free($$);
					$$ = NULL;
				}
			}
		}

	|  ';' _ELEMENT '=' _ELEMENT {
			if(NULL != ($$ = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				$$->prev = $$->next = $$;
				if(NULL != ($$->str = (int8 *)calloc(2 + strlen($2) + strlen($4), sizeof(int8))))
				{
					strcpy($$->str, $2);
					strcat($$->str, "=");
					strcat($$->str, $4);
					$$->hash = lslpCheckSum($$->str, (int16)strlen($$->str));
				}
				else
				{
					free($$);
					$$ = NULL;
				}
			}
		}
	;

%% 


lslpAtomList *lslpAllocAtom(void)
{
	lslpAtomList *temp = (lslpAtomList *)calloc(1, sizeof(lslpAtomList));
	if (temp != NULL)
	{
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

void lslpFreeAtom(lslpAtomList *a)
{
	assert(a != NULL);
	if (a->str != NULL)
		free(a->str);
	free(a);
	return;
}	

void lslpFreeAtomList(lslpAtomList *l, int32 flag)
{
	lslpAtomList *temp;
	assert(l != NULL);
	assert(_LSLP_IS_HEAD(l));
	while (! _LSLP_IS_EMPTY(l))
	{
		temp = l->next;
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		/*		_LSLP_UNLINK(temp);  */
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
	while (! (_LSLP_IS_HEAD(l->next)))
	{
		temp = l->next;
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		/*		_LSLP_UNLINK(temp); */
		lslpFreeAtomizedURL(temp);
	}
	if (flag)
		free(l);
	return;
}	

void lslpInitAtomLists(void)
{
	srvcHead.next = srvcHead.prev = &srvcHead;
	siteHead.next = siteHead.prev = &siteHead;
	pathHead.next = pathHead.prev = &pathHead;
	attrHead.next = attrHead.prev = &attrHead;
	return;
}	

void lslpInitURLList(void)
{
	urlHead.next = urlHead.prev = &urlHead;
	return;
}	



void lslpCleanUpURLLists(void)
{

  lslpFreeAtomList(&srvcHead, 0);
  lslpFreeAtomList(&siteHead, 0);
  lslpFreeAtomList(&pathHead, 0);
  lslpFreeAtomList(&attrHead, 0);
  lslpFreeAtomizedURLList(&urlHead, 0);
}

lslpAtomizedURL *_lslpDecodeURLs(int8 *u[], int32 count)
{
#ifndef TEST_HARNESS
  extern LSLP_SEM_T urlParseSem;
#endif
  int32 i, ccode = LSLP_WAIT_OK;
  uint32 lexer = 0;
  lslpAtomizedURL *temp = NULL;
  assert(u != NULL && u[count - 1] != NULL);
#ifndef TEST_HARNESS
  _LSLP_WAIT_SEM(urlParseSem, 10000, &ccode );
#endif
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    lslpInitURLList();
    lslpInitAtomLists();
    
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
      if(NULL != (temp = lslpAllocAtomizedURLList())) {
	_LSLP_LINK_HEAD(temp, &urlHead);
      }
    }
#ifndef TEST_HARNESS
    _LSLP_SIGNAL_SEM(urlParseSem);
#endif
  }
  return(temp);	
}	 

#ifdef TEST_HARNESS
/* test harness */
#define NUMBER_URLS 10
uint8 *urls[] = 
{

	"http:/at/12d:appletalk-local:apple%20web",
	"really bad @##$)(*094//index.html",
	"service:local-state.vendor:notification.vendor://192.168.2.1:81;load=10",
	"service:transducer:thermomoter://mdday@soft-hackle.net:87/resolution/farenheit;temp=354;expiration=1-1-45",	
	"http://www.usgs.org/index",
	"service:local-state.vendor:notification.vendor:/ipx/ffffaaaa:babebabebabe:1e2e;state=critical;alarm;situation=mylanta",
 	"service:lpr://printers/localprinters;postscript=yes",
	"http://www.kernel.org/LDP/HOWTO/",
	"ftp://ftp.isi.edu/in-notes/rfc1700.txt",
	"service:ftp://ftp.isi.edu/in-notes/rfc1700.txt",

	"", ""
};

int32 main(int32 argc, int8 *argv[])
{
	int32 i;
	lslpAtomizedURL *list;

	for (i = 0; i < NUMBER_URLS; i++)
	{
		printf("input: %s\n", urls[i]);
	}

	if (NULL != (list = _lslpDecodeURLs((int8 **)urls, NUMBER_URLS)))
	{
		while (! _LSLP_IS_HEAD(list->next))
		{
			printf("output: %s %u\n", list->next->url, list->next->urlHash);
			list = list->next;
		}
	}

	return(0);		
}	

#endif

