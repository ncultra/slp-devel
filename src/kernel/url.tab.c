/* A Bison parser, made from url.y
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



   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	_RESERVED	257
# define	_HEXDIG	258
# define	_STAG	259
# define	_RESNAME	260
# define	_NAME	261
# define	_ELEMENT	262
# define	_IPADDR	263
# define	_IPX	264
# define	_AT	265
# define	_ZONE	266

#line 28 "url.y"


#include "../../lslp-common/lslp-common-defs.h"

/* prototypes and globals go here */




#ifndef TEST_HARNESS
SUBALLOC_HANDLE *url_sa;
#ifndef _init_heap
#ifdef DEBUG_ALLOC
#define _init_heap(x)  url_sa = InitializeProcessHeap((x));
#define malloc(x) url_sa->malloc((x), (uint32)url_sa, __FILE__, __LINE__)
#define calloc(x, y) url_sa->calloc((x), (y), (uint32)url_sa, __FILE__, __LINE__)
#define realloc(x, y) url_sa->realloc((x), (y), (uint32)url_sa, __FILE__, __LINE__)
#define strdup(x) url_sa->strdup((x), (uint32)url_sa, __FILE__, __LINE__)
#define wcsdup(x) url_sa->wcsdup((x), (uint32)url_sa, __FILE__, __LINE__)
#define _de_init() url_sa->de_init((int32)url_sa)

#else

#define _init_heap(x)  url_sa = InitializeProcessHeap((x));
#define malloc(x) url_sa->malloc((x), (uint32)url_sa)
#define calloc(x, y) url_sa->calloc((x), (y), (uint32)url_sa)
#define realloc(x, y) url_sa->realloc((x), (y), (uint32)url_sa)
#define strdup(x) url_sa->strdup((x), (uint32)url_sa)
#define wcsdup(x) url_sa->wcsdup((x), (uint32)url_sa)
#define _de_init() url_sa->de_init((int32)url_sa)

#endif

#endif

#else
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
	&urlHead, &urlHead, TRUE, NULL, 0, NULL
};

static lslpAtomList srvcHead = {&srvcHead, &srvcHead, TRUE, };
static lslpAtomList siteHead = {&siteHead, &siteHead, TRUE, };
static lslpAtomList pathHead = {&pathHead, &pathHead, TRUE, };
static lslpAtomList attrHead = {&attrHead, &attrHead, TRUE, };


#line 89 "url.y"
#ifndef YYSTYPE
typedef union {
	int32 _i;
	int8 *_s;
	lslpAtomList *_atl;
	lslpAtomizedURL *_aturl;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		50
#define	YYFLAG		-32768
#define	YYNTBASE	19

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 266 ? yytranslate[x] : 34)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    14,    15,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    13,    17,
       2,    18,     2,     2,    16,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    16,    18,    21,    23,
      25,    27,    30,    34,    40,    42,    49,    51,    55,    57,
      59,    61,    63,    66,    68,    71,    73,    76,    79,    82,
      84,    87,    90
};
static const short yyrhs[] =
{
      20,    22,     0,    21,     0,    20,    21,     0,     6,    13,
       0,     6,    14,     6,    13,     0,    23,     0,    23,    29,
       0,    24,     0,    25,     0,    26,     0,    15,    15,     0,
      15,    15,    27,     0,    15,    15,     6,    16,    27,     0,
      10,     0,    11,    12,    13,    12,    13,    12,     0,    28,
       0,    28,    13,     4,     0,     6,     0,     9,     0,    30,
       0,    32,     0,    30,    32,     0,    31,     0,    30,    31,
       0,    15,     0,    15,     8,     0,    15,     6,     0,    15,
       4,     0,    33,     0,    32,    33,     0,    17,     8,     0,
      17,     8,    18,     8,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   109,   203,   211,   220,   238,   260,   267,   276,   289,
     302,   317,   320,   328,   339,   344,   357,   360,   375,   378,
     383,   386,   389,   394,   401,   411,   415,   433,   450,   468,
     475,   484,   501
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "_RESERVED", "_HEXDIG", "_STAG", "_RESNAME", 
  "_NAME", "_ELEMENT", "_IPADDR", "_IPX", "_AT", "_ZONE", "':'", "'.'", 
  "'/'", "'@'", "';'", "'='", "url", "service_list", "service", "sap", 
  "site", "ip_site", "ipx_site", "at_site", "hostport", "host", 
  "url_part", "path_list", "path_el", "attr_list", "attr_el", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    19,    20,    20,    21,    21,    22,    22,    23,    23,
      23,    24,    24,    24,    25,    26,    27,    27,    28,    28,
      29,    29,    29,    30,    30,    31,    31,    31,    31,    32,
      32,    33,    33
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     2,     1,     2,     2,     4,     1,     2,     1,     1,
       1,     2,     3,     5,     1,     6,     1,     3,     1,     1,
       1,     1,     2,     1,     2,     1,     2,     2,     2,     1,
       2,     2,     4
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     0,     2,     4,     0,    14,     0,     0,     3,
       1,     6,     8,     9,    10,     0,     0,    11,    25,     0,
       7,    20,    23,    21,    29,     5,     0,    18,    19,    12,
      16,    28,    27,    26,    31,    24,    22,    30,     0,     0,
       0,     0,     0,    18,    13,    17,    32,    15,     0,     0,
       0
};

static const short yydefgoto[] =
{
      48,     2,     3,    10,    11,    12,    13,    14,    29,    30,
      20,    21,    22,    23,    24
};

static const short yypact[] =
{
      -3,     5,    -5,-32768,-32768,     2,-32768,     4,     7,-32768,
  -32768,   -13,-32768,-32768,-32768,    10,    11,     6,     3,    12,
  -32768,   -13,-32768,     9,-32768,-32768,    13,    14,-32768,-32768,
      15,-32768,-32768,-32768,    16,-32768,     9,-32768,    18,     8,
      17,    19,    20,-32768,-32768,-32768,-32768,-32768,    29,    33,
  -32768
};

static const short yypgoto[] =
{
  -32768,-32768,    34,-32768,-32768,-32768,-32768,-32768,    -4,-32768,
  -32768,-32768,    21,    22,   -23
};


#define	YYLAST		43


static const short yytable[] =
{
      37,     1,    18,     1,    19,     6,     7,    31,    15,    32,
       8,    33,    27,    37,    43,    28,    16,    28,     4,     5,
      34,    45,    17,    25,    26,    38,    19,    46,    40,    49,
      39,    42,    47,    50,    41,    44,     9,     0,     0,     0,
       0,     0,    35,    36
};

static const short yycheck[] =
{
      23,     6,    15,     6,    17,    10,    11,     4,     6,     6,
      15,     8,     6,    36,     6,     9,    12,     9,    13,    14,
       8,     4,    15,    13,    13,    12,    17,     8,    13,     0,
      16,    13,    12,     0,    18,    39,     2,    -1,    -1,    -1,
      -1,    -1,    21,    21
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 109 "url.y"
{
			if (NULL != (yyval._aturl = (lslpAtomizedURL *)calloc(1, sizeof(lslpAtomizedURL))))
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
				if (NULL != (yyval._aturl->url = (int8 *)calloc(urlLen, sizeof(int8))))
				{
					temp = srvcHead.next;
					if (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcpy(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					temp = siteHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					temp = pathHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat(yyval._aturl->url, "/");
						strcat(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					temp = attrHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat(yyval._aturl->url, ";");
						strcat(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					yyval._aturl->urlHash = lslpCheckSum(yyval._aturl->url, (int16)strlen(yyval._aturl->url));
				}

				/* make certain the listheads are initialized */
				yyval._aturl->srvcs.next = yyval._aturl->srvcs.prev = &(yyval._aturl->srvcs);
				yyval._aturl->srvcs.isHead = TRUE;
				yyval._aturl->site.next = yyval._aturl->site.prev = &(yyval._aturl->site);
				yyval._aturl->site.isHead = TRUE;
				yyval._aturl->path.next = yyval._aturl->path.prev = &(yyval._aturl->path);
				yyval._aturl->path.isHead = TRUE;
				yyval._aturl->attrs.next = yyval._aturl->attrs.prev = &(yyval._aturl->attrs);
				yyval._aturl->attrs.isHead = TRUE;
				if (! _LSLP_IS_EMPTY(&srvcHead ))
				{
					_LSLP_LINK_HEAD(&(yyval._aturl->srvcs), &srvcHead);
				}
				if (! _LSLP_IS_EMPTY(&siteHead))
				{
					_LSLP_LINK_HEAD(&(yyval._aturl->site), &siteHead);
				}
				if (! _LSLP_IS_EMPTY(&pathHead))
				{
					_LSLP_LINK_HEAD(&(yyval._aturl->path), &pathHead);
				}
				if (! _LSLP_IS_EMPTY(&attrHead))
				{
					_LSLP_LINK_HEAD(&(yyval._aturl->attrs), &attrHead);
				}
				_LSLP_INSERT_BEFORE(yyval._aturl, &urlHead);
				lslpInitAtomLists();
			}
		;
    break;}
case 2:
#line 203 "url.y"
{
			yyval._atl = &srvcHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;
    break;}
case 3:
#line 211 "url.y"
{
			yyval._atl = &srvcHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;
    break;}
case 4:
#line 220 "url.y"
{
			if (NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->next = yyval._atl->prev = yyval._atl;
				if (NULL != (yyval._atl->str = (int8 *)calloc(2 + strlen(yyvsp[-1]._s), sizeof(int8))))
				{
					strcpy(yyval._atl->str, yyvsp[-1]._s);
					strcat(yyval._atl->str, ":");	
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}

		;
    break;}
case 5:
#line 238 "url.y"
{
			if (NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->next = yyval._atl->prev = yyval._atl;
				if (NULL != (yyval._atl->str = (int8 *)calloc(3 + strlen(yyvsp[-3]._s) + strlen(yyvsp[-1]._s), sizeof(int8))))
				{
					strcpy(yyval._atl->str, yyvsp[-3]._s);
					strcat(yyval._atl->str, ".");
					strcat(yyval._atl->str, yyvsp[-1]._s);
					strcat(yyval._atl->str, ":");	
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}

		;
    break;}
case 6:
#line 260 "url.y"
{
			yyval._atl = &siteHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;
    break;}
case 7:
#line 267 "url.y"
{
			yyval._atl = &siteHead;
			if (yyvsp[-1]._atl != NULL)
			{
					_LSLP_INSERT_BEFORE(yyvsp[-1]._atl, yyval._atl);
			}
		;
    break;}
case 8:
#line 276 "url.y"
{
			if (yyvsp[0]._s != NULL)
			{
				if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					yyval._atl->next = yyval._atl->prev = yyval._atl;
					yyval._atl->str = yyvsp[0]._s;
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
			}
			else
				yyval._atl = NULL;
		;
    break;}
case 9:
#line 289 "url.y"
{
			if (yyvsp[0]._s != NULL)
			{
				if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					yyval._atl->next = yyval._atl->prev = yyval._atl;
					yyval._atl->str = yyvsp[0]._s;
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
			}
			else
				yyval._atl = NULL;
		;
    break;}
case 10:
#line 302 "url.y"
{
			if (yyvsp[0]._s != NULL)
			{
				if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					yyval._atl->next = yyval._atl->prev = yyval._atl;
					yyval._atl->str = yyvsp[0]._s;
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
			}
			else
				yyval._atl = NULL;
		;
    break;}
case 11:
#line 317 "url.y"
{
			yyval._s = strdup("//");
		;
    break;}
case 12:
#line 320 "url.y"
{
			if(NULL != yyvsp[0]._s && (NULL !=(yyval._s = (int8 *)calloc(3 + strlen(yyvsp[0]._s), sizeof(int8)))))
			{
				strcpy(yyval._s, "//");
				strcat(yyval._s, yyvsp[0]._s);
			}

		;
    break;}
case 13:
#line 328 "url.y"
{
			if(NULL != yyvsp[0]._s && (NULL !=(yyval._s = (int8 *)calloc(4 + strlen(yyvsp[-2]._s) + strlen(yyvsp[0]._s), sizeof(int8)))))
			{
				strcpy(yyval._s, "//");
				strcat(yyval._s, yyvsp[-2]._s);
				strcat(yyval._s, "@");
				strcat(yyval._s, yyvsp[0]._s);
			}
		;
    break;}
case 14:
#line 339 "url.y"
{
			yyval._s = yyvsp[0]._s;
		;
    break;}
case 15:
#line 344 "url.y"
{
			if(NULL != (yyval._s = (int8 *)calloc(strlen(yyvsp[-5]._s) + strlen(yyvsp[-4]._s) + strlen(yyvsp[-2]._s) + strlen(yyvsp[0]._s) + 3, sizeof(int8))))
			{
				strcpy(yyval._s, yyvsp[-5]._s);
				strcat(yyval._s, yyvsp[-4]._s);
				strcat(yyval._s, ":");
				strcat(yyval._s, yyvsp[-2]._s);
				strcat(yyval._s, ":");
				strcat(yyval._s, yyvsp[0]._s);
			}
		;
    break;}
case 16:
#line 357 "url.y"
{
			yyval._s = yyvsp[0]._s;	
		;
    break;}
case 17:
#line 360 "url.y"
{
			if (yyvsp[-2]._s != NULL)
			{
				if(NULL != (yyval._s = (int8 *)calloc(strlen(yyvsp[-2]._s) + strlen(yyvsp[0]._s) + 2, sizeof(int8))))
				{
					strcpy(yyval._s, yyvsp[-2]._s);
					strcat(yyval._s, ":");
					strcat(yyval._s, yyvsp[0]._s);
				}
			}
			else
				yyval._s = NULL;	
		;
    break;}
case 18:
#line 375 "url.y"
{
			yyval._s = yyvsp[0]._s;
		;
    break;}
case 19:
#line 378 "url.y"
{
			yyval._s = yyvsp[0]._s;
		;
    break;}
case 20:
#line 383 "url.y"
{
			;	
		;
    break;}
case 21:
#line 386 "url.y"
{
			;		
		;
    break;}
case 22:
#line 389 "url.y"
{
			;
		;
    break;}
case 23:
#line 394 "url.y"
{
			yyval._atl = &pathHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;
    break;}
case 24:
#line 401 "url.y"
{
			yyval._atl = &pathHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;
    break;}
case 25:
#line 411 "url.y"
{
			 /* dangling path slash - do nothing */
			 yyval._atl = NULL;
			 ;
    break;}
case 26:
#line 415 "url.y"
{
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (int8 *)calloc(1 + strlen(yyvsp[0]._s), sizeof(int8))))
				{
					strcpy(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));

				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}

		;
    break;}
case 27:
#line 433 "url.y"
{
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (int8 *)calloc(1 + strlen(yyvsp[0]._s), sizeof(int8))))
				{
					strcpy(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}

		;
    break;}
case 28:
#line 450 "url.y"
{
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (int8 *)calloc(1 + strlen(yyvsp[0]._s), sizeof(int8))))
				{
					strcpy(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}
		;
    break;}
case 29:
#line 468 "url.y"
{
			yyval._atl = &attrHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;
    break;}
case 30:
#line 475 "url.y"
{
			yyval._atl = &attrHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;
    break;}
case 31:
#line 484 "url.y"
{
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (int8 *)calloc(1 + strlen(yyvsp[0]._s), sizeof(int8))))
				{
					strcpy(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}
		;
    break;}
case 32:
#line 501 "url.y"
{
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (int8 *)calloc(2 + strlen(yyvsp[-2]._s) + strlen(yyvsp[0]._s), sizeof(int8))))
				{
					strcpy(yyval._atl->str, yyvsp[-2]._s);
					strcat(yyval._atl->str, "=");
					strcat(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}
		;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 521 "url.y"
 


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

