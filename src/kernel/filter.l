
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

#include "lslp-common-defs.h"
#include "y_filter.h"
#include <stdarg.h>


#define filterless yyless
void filtererror(int8 *, ...);

/*
extern char *lslp_logPath;

#define yyerror(s) 	_lslpMsgOut(lslp_logPath, lslpPrintSem, LSLP_LOG_ERRORS, NULL, 0, (s), __FILE__, __LINE__);
*/
static int16 heapIndex;
static int8 heap[2052];
static int8 buf[2052];

static int8 *_lslp_strdup(int8 *s);
void filter_close_lexer(uint32 handle);
uint32 filter_init_lexer(int8 *s);


%}

/* regex macros for the lexer */
hexdigit		[0-9a-fA-F]
reserved                [()\&|!=<>~\n] /* newline in reserved set for convinience */
not_reserved            [^()\&|!=<>~\n] 


/* special lexer states */
%x QUOTED_STRING

/* table size directives */

%option noyywrap
%%

"\""                          { BEGIN QUOTED_STRING; filterless(0); }
<QUOTED_STRING>[^()\&|!=<>~\n]+  {
                                 BEGIN INITIAL; 
                                 if(NULL != (filterlval.filter_string =  _lslp_strdup(filtertext)))
                                    return(OPERAND) ; 
                                 else 
                                    return(0L) ;
                              }


[ \t\v\f]*                    { ; }
"("                           { filterlval.filter_int = L_PAREN; return L_PAREN ; }
")"                           { filterlval.filter_int = R_PAREN; return R_PAREN ; }
"&"                           { filterlval.filter_int = OP_AND; return OP_AND ; }
"|"                           { filterlval.filter_int = OP_OR; return OP_OR ; }
"!"                           { filterlval.filter_int = OP_NOT; return OP_NOT ; }
"="                           { filterlval.filter_int = OP_EQU; return OP_EQU ; }
">="                          { filterlval.filter_int = OP_GT; return OP_GT ; }
"<="                          { filterlval.filter_int = OP_LT; return OP_LT ; }
"=*"                          { filterlval.filter_int = OP_PRESENT; return OP_PRESENT ; }
"~="                          { filterlval.filter_int = OP_APPROX; return OP_APPROX ;  }


[-+][0-9]+                  |
[-+]"0"[xX]{hexdigit}+      { 
                              filterlval.filter_int = strtol(filtertext, (char **) 0, 0) ;
                              return VAL_INT;
                            }

[0-9]+                      |
"0"[xX]{hexdigit}+          { 
                              filterlval.filter_int = strtoul(filtertext, (char **) 0, 0);
                              return VAL_INT;
                            }

[tT][rR][uU][eE]            { 
                               filterlval.filter_int = 1; return VAL_BOOL; 
                            }


[fF][aA][lL][sS][eE]        { 
                               filterlval.filter_int = 0; return VAL_BOOL; 
                            }


[^()\&|!=<>~ \t\v\f]+       {
                                 if(NULL != (filterlval.filter_string =  _lslp_strdup(filtertext)))
                                    return(OPERAND) ; 
                                 else 
                                    return(0L) ;
                            }


 
%%

void filter_close_lexer(uint32 handle)
{
  assert(handle != 0);
  filter_delete_buffer((YY_BUFFER_STATE)handle);
}


uint32 filter_init_lexer(int8 *s)
{
  memset(&buf[0], 0x00, 2052);
  memset(&heap[0], 0x00, 2052);
  heapIndex = 0;
  strncpy(&buf[0], s, 2048);
  return((uint32)filter_scan_buffer(&buf[0], strlen(s) + 2));
}


static int8 *_lslp_strdup(int8 *s)
{
  int8 *p = &heap[heapIndex];
  do { heap[heapIndex++] = *s; } while ((*s != 0x00) && (heapIndex < 2049) && (++s));
  return(p);
}

static int8 *_lslp_strdup_strip_quotes(int8 *s)
{
  int8 *p = &heap[heapIndex];
  do { if(*s != '"') heap[heapIndex++] = *s; } while ((*s != 0x00) && (heapIndex < 2049) && (++s));
  return(p);
}


void filtererror(int8 *s, ...)
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

