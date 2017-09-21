#ifndef BISON_URL_TAB_H
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



# define BISON_URL_TAB_H

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


extern YYSTYPE yylval;

#endif /* not BISON_URL_TAB_H */
