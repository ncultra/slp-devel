
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

#undef YYLMAX
#define YYLMAX 2048
#define YY_NEVER_INTERACTIVE 1
#undef YY_INPUT
#define YY_INPUT(b, r, m) abort()
#define exit(i) abort()
#undef ECHO
#define ECHO  urlerror(yytext)
#include "slp_client.h"
#include "y_url.h"
#include <stdarg.h>

#define urlless yyless
void urlerror(int8 *, ...);

static int16 heapIndex;
static int8 *heap;
static int8 *buf;
/* special counter to exit the appletalk state */
int8 atalk_state = 0;

static int8 *_lslp_strdup(int8 *s);
void url_close_lexer(void *handle);
void *url_init_lexer(int8 *s);
int urllex_destroy(void);
#if defined(__GNUC__)
static void yyunput (int c, register char * yy_bp ) __attribute__((unused));
#endif

%}

 /* {}	*/
digit			[0-9]
upalpha			[A-Z]
lowalpha		[a-z]
alpha			[a-zA-Z]
alphanum 		[a-zA-Z0-9]
hex			    [a-fA-F0-9]
safe			[-$_.~]
escaped			("%"{hex}{2} | "\\"{hex}{2})
mark 			[-_.!~*'()]
unreserved		({alphanum}|{mark})
reserved		[;/?:@&=+$,]
uric			({reserved} | {unreserved} | {escaped})

%x IP_SITE
%x AT_SITE
%x ATTRIBUTE
%p 	 9000
%n    300 
%e    700 
%option noyywrap prefix="url"
%%

 /* the ip user @ host syntax is a special state because some reserved */
 /* characters are valid - but the user string itself is optional so we can't */
 /* build it into a full-time rule */

"//"			{
                          BEGIN IP_SITE;
			  urlless(0);
			}
 /* return the slash as a token */
<IP_SITE>"/"		{urllval._i = *urltext; return(urllval._i);}


 /* an ipv6 address */
<*>\[[a-fA-F0-9:%.]{2,}\] {
            BEGIN INITIAL;
 			if(NULL != (urllval._s	= _lslp_strdup(urltext))) {
				return(_IP6ADDR);
              }
			else
				return(0L);
			}

 /* an ip address */
<*>(({digit}{1,3})"."({digit}{1,3})"."({digit}{1,3})"."({digit}{1,3})) {
            BEGIN INITIAL;
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_IPADDR);
			else
				return(0L);
			}

 /* this next rule needs to kick in even if it matches a zero-length string */
 /* i.e., it needs to be guaranteed even if there is no user @ host production */
<IP_SITE>[-_.!~*'()a-zA-Z0-9;+&=\%]* {
			BEGIN INITIAL;
			if (urlleng > 0)
			{
                            if(NULL != (urllval._s = _lslp_strdup(urltext)))
                                return(_RESNAME);
			    else
				return(0L);
			}
		}

 /* appletalk syntax includes three reserved characters - give the lexer a chance */
 /* to match an appletalk production before it starts returning reserved */
 /* chars as tokens. */

"/"[aA]{1}[tT]{1}"/" 	{
			BEGIN AT_SITE;
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_AT);
			else
				return(0L);
			}

<AT_SITE>(([-a-zA-Z0-9$_.~])|("%"([a-fA-F0-9]{2,2}))){1,31} {
 			atalk_state++;
			if (atalk_state == 3)
				{BEGIN INITIAL;}
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_ZONE);
			else
				return(0L);
			}

<AT_SITE>":"		{urllval._i = *urltext; return(urllval._i);}
<AT_SITE>"/"		{BEGIN INITIAL; urlless(0);}

 /* ipx syntax includes two reserved characters - give the lexer a chance */
 /* to match an ipx production before it starts returning reserved chars */
 /* as tokens */

"/"[iI]{1}[pP]{1}[xX]{1}"/"({hex}{8})":"({hex}{12})":"({hex}{4}) {
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_IPX);
			else
				return(0L);
			}


 /* a string consisting of only hex digits */
{hex}+			{
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_HEXDIG);
			else
				return(0L);
			}



 /* resource names start with alpha and include alphanum and '+' or '-' */
 /* but '+' is reserved and must be escaped */
[a-zA-Z]+[-a-zA-Z0-9.]* 	{
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_RESNAME);
			else
				return(0L);
			}

 /* reserved characters  - always a token in normal state unless part of */
 /* one of the productions above */
";"			{BEGIN ATTRIBUTE; urllval._i = *urltext; return(urllval._i);}
"/"			{urllval._i = *urltext; return(urllval._i);}
"?"			{urllval._i = *urltext; return(urllval._i);}
":"			{urllval._i = *urltext; return(urllval._i);}
"@"			{urllval._i = *urltext; return(urllval._i);}
"&"			{urllval._i = *urltext; return(urllval._i);}
"="			{urllval._i = *urltext; return(urllval._i);}
"+"			{urllval._i = *urltext; return(urllval._i);}
"$"			{urllval._i = *urltext; return(urllval._i);}
","			{urllval._i = *urltext; return(urllval._i);}

 /* anything else that is not reserved */
<ATTRIBUTE>[!-~]+ {
 			BEGIN INITIAL;
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_ELEMENT);
			else
				return(0L);
			}

 /* anything else that is not reserved */
[^;/?:@&=+$,]+		{
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_ELEMENT);
			else
				return(0L);
			}

 /* anything else is an error */

%%


void url_close_lexer(void *handle)
{
  assert(handle != 0);
  free(heap);
  free(buf);
  url_delete_buffer(handle);
  urllex_destroy();
}


void *url_init_lexer(int8 *s)
{
  buf = (int8 *)calloc(2052, sizeof(int8));
  heap = (int8 *)calloc(2052, sizeof(int8));
  if (!buf || !heap)
    abort();
  heapIndex = 0;
  strncpy(&buf[0], s, 2048);
  return(url_scan_buffer(&buf[0], strlen(s) + 2));
}

static int8 *_lslp_strdup(int8 *s)
{
  int8 *p = &heap[heapIndex];
  do { heap[heapIndex++] = *s; } while ((*s != 0x00) && (heapIndex < 2049) && (++s));
  return(p);
}

void urlerror(int8 *s, ...)
{
 return;
}	
