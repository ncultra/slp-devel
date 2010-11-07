
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
#undef YY_INPUT
#include "lslp-common-defs.h"
#include "y_url.h"
#include <stdarg.h>

#define urlless yyless
void urlerror(int8 *, ...);

/*
extern char *lslp_logPath;
extern unsigned int lslpPrintSem;

#define yyerror(s) 	_lslpMsgOut(lslp_logPath, lslpPrintSem, LSLP_LOG_ERRORS, NULL, 0, (s), __FILE__, __LINE__);
*/
static int16 heapIndex;
static int8 heap[2052];
static int8 buf[2052];
/* special counter to exit the appletalk state */
int8 atalk_state = 0;

static int8 *_lslp_strdup(int8 *s);
void url_close_lexer(uint32 handle);
uint32 url_init_lexer(int8 *s);

%}

 /* {}	*/
digit			[0-9]
upalpha			[A-Z]
lowalpha		[a-z]
alpha			[a-zA-Z]
alphanum 		[a-zA-Z0-9]
hex			[a-fA-F0-9]
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
%option noyywrap
%%

 /* the ip user @ host syntax is a special state because some reserved */
 /* characters are valid - but user the string itself is optional so we can't */
 /* build it into a full-time rule */

"//"			{
                          BEGIN IP_SITE;
			  urlless(0);
			}
 /* return the slash as a token */
<IP_SITE>"/"		{urllval._i = *urltext; return(urllval._i);}

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

 /* a string consisting of only hex digits */
{hex}+			{
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_HEXDIG);
			else
				return(0L);
			}

 /* an ipv4 address */
({hex}{1,3})"."({hex}{1,3})"."({hex}{1,3})"."({hex}{1,3}) {
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_IPADDR);
			else
				return(0L);
			}

 /* resource names start with alpha and include alphanum and '+' or '-' */
 /* but '+' is reserved and must be escaped */
[-a-zA-Z0-9.]* 	{
 			if(NULL != (urllval._s	= _lslp_strdup(urltext)))
				return(_RESNAME);
			else
				return(0L);
			}


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

%%


void url_close_lexer(uint32 handle)
{
  assert(handle != 0);
  url_delete_buffer((YY_BUFFER_STATE)handle);
}


uint32 url_init_lexer(int8 *s)
{
  memset(&buf[0], 0x00, 2052);
  memset(&heap[0], 0x00, 2052);
  heapIndex = 0;
  strncpy(&buf[0], s, 2048);
  return((uint32)url_scan_buffer(&buf[0], strlen(s) + 2));
}


static int8 *_lslp_strdup(int8 *s)
{
  int8 *p = &heap[heapIndex];
  do { heap[heapIndex++] = *s; } while ((*s != 0x00) && (heapIndex < 2049) && (++s));
  return(p);
}

void urlerror(int8 *s, ...)
{
#ifdef TEST_HARNESS
	va_list va;
	va_start(va, s);
	printf(s, va);
#endif
	return;
}	

/*
int main(int argc, char *argv[])
{

}	

*/

