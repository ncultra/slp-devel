
%{

#undef YYLMAX
#define YYLMAX 2048
#define YY_NEVER_INTERACTIVE 1
#undef YY_INPUT
#define YY_INPUT(b, r, m) abort()
#define exit(i) abort()
#define fread(a, b, c, d) abort()
#undef ECHO
#define ECHO  attrerror(yytext)
#include "slp_client.h"
#include "y_attr.h"

static int16 heapIndex;
static int8 heap[2052];
static int8 buf[2052];
static int8 *_lslp_strdup(int8 *s);
void attrerror(int8 *, ...);
void attr_close_lexer(uint32 handle);
int attrlex_destroy(void);
uint32 attr_init_lexer(int8 *s);

%}

/* {}	*/
digit			[0-9]
hexdigit		[0-9a-fA-F]
escaped			(\\([a-fA-F0-9]{2}))
reserved		[(),\\!<=>~\x00-\x1f]
res_tag			[(),\\!<>~\x00-\x1f*_\r\n\t]
res_val			[*_\r\n\t]
tag			[^(), \\!<=>~_*\x00-\x1f]
val			(("\\"([a-fA-F0-9]{2}))|[^(),!<=>~\x00-\x1f])

 /* 
	this lexer cycles through three states. in the initial state, 
	it is looking strictly for attribute tags. If, in the initial 
	state it finds a '(' it anticipates an attribute-value pair 
	and changes to ATTRIBUTE state .

	in the ATTRIBUTE state is is looking for either a closing ')'
	or a '='. If it sees a closing ')' it changes to the initial state. 
	if it sees an '=' it changes to the VALUE state and looks for 
	the attribute value. After seeing the attribute value, it looks
	for a either a ')' or a ','. If it sees a ')' it closes the attribute
	and returns to the initial state. 

	if, during the VALUE state the lexer sees a ',' it decides it is 
	looking at a multi-valued attribute and returns a special _MULTIVAL
	token to the parser. The parser uses this to construct additional
	attribute-value pairs for each value in the multi-val statement. 
 */ 
%option noyywrap prefix="attr"

%x ATTR
%x VALUE
%%

[ \t\v\f]*              { ; }

"," 			{ attrlval._i = *attrtext; return(attrlval._i); }
"("			{ BEGIN ATTR; attrlval._i = *attrtext; return(attrlval._i); }
{tag}+			{   
 			   if (attrleng > 0) {
			      if(NULL != (attrlval._s = _lslp_strdup(attrtext)))
                                 return(_TAG);
                              else
                                 return(0L);
                           }
                        }

 /* anything else in the initial state is an error */
.				{attrerror("error in lexer - initial state\n");}

<ATTR>[ \t\v\f]*        { ; }
<ATTR>"="	        {BEGIN VALUE; attrlval._i = *attrtext; return(attrlval._i);}
<ATTR>{tag}+	        {   
                          if (attrleng > 0 ) {
                             if(NULL != (attrlval._s = _lslp_strdup(attrtext)))
                                return(_TAG);
                             else
                                return(0L);
                           }
                        }

 /* make it legal to have just an attr tag enclosed in parens */
<ATTR>")"		{BEGIN INITIAL; attrlval._i = *attrtext; return(attrlval._i); }

 /* anything else in the attribute state is an error */
<ATTR>.			{attrerror("error in lexer - attribute state\n");}

<VALUE>[ \t\v\f]*       { ; }
<VALUE>","  		{attrlval._i = *attrtext; return(_MULTIVAL);}

<VALUE>[-+][0-9]+             |
<VALUE>[-+]"0"[xX]{hexdigit}+ { attrlval._i = strtol(attrtext, (char **) 0, 0); return _INT ;};

<VALUE>[0-9]+                 |
<VALUE>"0"[xX]{hexdigit}+     { attrlval._i = strtoul(attrtext, (char **)0, 0); return _INT; };


<VALUE>[tT][rR][uU][eE] {return(_TRUE);}
<VALUE>[fF][aA][lL][sS][eE]  {return(_FALSE);}
<VALUE>{val}+	        {	
                           if (attrleng > 0) {
                              if(NULL != (attrlval._s = _lslp_strdup(attrtext)))
                                 return(_STRING);
                              else
                                 return(0L);
                           }
                        }		

<VALUE>")"	        {BEGIN INITIAL; attrlval._i = *attrtext; return(attrlval._i);}

 /* anything else in the value state is an error */
<VALUE>.		{BEGIN INITIAL;}

%%

static int8 *_lslp_strdup(int8 *s)
{
  int8 *p = &heap[heapIndex];
  do { heap[heapIndex++] = *s; } while ((*s != 0x00) && (heapIndex < 2049) && (++s));
  return(p);
}

void attr_close_lexer(uint32 handle)
{
  assert(handle != 0);
  attr_delete_buffer((YY_BUFFER_STATE)handle);
  attrlex_destroy();
}

uint32 attr_init_lexer(int8 *s)
{
  memset(&buf[0], 0x00, 2052);
  memset(&heap[0], 0x00, 2052);
  heapIndex = 0;
  strncpy(&buf[0], s, 2052);
  return((uint32)attr_scan_buffer(&buf[0], strlen(s) + 2));
}


void attrerror(int8 *s, ...)
{
	
 /* (va, s);
	printf(s, va);
	return;
*/
}	

/*
int main(int argc, char *argv[])
{

}	

*/


