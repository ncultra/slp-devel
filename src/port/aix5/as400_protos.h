/*** START HEADER FILE SPECIFICATIONS ********************************/
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



/*                                                                   */
/* Header File Name       : as400_protos.h                           */
/*                                                                   */
/* Descriptive Name       : OS/400 PASE program runtime functions    */
/*                                                                   */
/*    5769-SS1, 5722-SS1                                             */
/*    (C) Copyright IBM Corp. 1997, 2001                             */
/*    All rights reserved.                                           */
/*    US Government Users Restricted Rights -                        */
/*    Use, duplication or disclosure restricted                      */
/*    by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                   */
/* Licensed Materials-Property of IBM                                */
/*                                                                   */
/* Description            : Declares OS/400-unique extentions to     */
/*                          to AIX interfaces that are provided by   */
/*                          OS/400 PASE runtime                      */
/*                                                                   */
/* Change Activity        :                                          */
/*                                                                   */
/* CFD List               :                                          */
/*                                                                   */
/* Flag Reason   Level   Date   Pgmr      Change Description         */
/* ---- -------- ------- ------ --------- -------------------------  */
/* $00  D95693   v4r3m0  971031 ROCH      New file                   */
/* $01  P3681905 v4r4m0  981031 timms     Code cleanup and add new   */
/*                                        _ILELOAD, _ILESYM support  */
/* $02  P9905729 v4r5m0  990923 timms     Add _CVTSPP support        */
/* $03  P9912784 v4r5m0  000115 timms     Add systemCL support and   */
/*                                        OS/400 extensions for AIX  */
/*                                        system calls               */
/* $04  P9940306 v5r1m0f 010103 timms     Add _CVTERRNO,_STRLEN_SPP, */
/*                                        and _STRNCPY_SPP           */
/* $05  D98736   v5r2m0  010322 timms    _RSLOBJ, _PGMCALL, and      */
/*                                       _ILECALLX support           */
/* $06  D99127   v5r3m0  021001 timms    _SETSPPM support            */
/* $08  D99127   v5r3m0  030414 timms    8-byte teraspace pointers   */
/*                                                                   */
/* End CFD List.                                                     */
/*********************************************************************/

#ifndef _AS400_PROTOS_H
#define _AS400_PROTOS_H

#ifndef _H_STANDARDS
#include <standards.h>
#endif

#include <sys/types.h>
#include <as400_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _CVTERRNO converts an ILE errno value to an equivalent
 *  OS/400 PASE errno value
 */
extern
int
_CVTERRNO(int	errno_ile);

/*
 *  _SETCCSID sets a new OS/400 PASE CCSID and/or returns
 *  the prior OS/400 PASE CCSID value
 */
extern
int				/* Old CCSID (or -1 for error) */
_SETCCSID(int	ccsid);		/* New CCSID (or -1 for no change) */

/*
 *  _SETSPP builds a tagged space pointer to the teraspace
 *  address equivalent of an OS/400 PASE memory address.
 *  _SETSPP_TS64 builds a tagged space pointer to memory
 *  addressed by a 64-bit teraspace pointer.
 *  _SETSPPM builds multiple tagged space pointers.
 *  _CVTSPP returns the OS/400 PASE memory address equivalent
 *  of a tagged pointer, or a null pointer (zero)
 */
extern
void
_SETSPP(ILEpointer	*target, /* Space pointer result */
	const void	*source);
extern
void
_SETSPP_TS64(ILEpointer	*target, /* Space pointer result */
	     ts64_t	source);
extern
void
_SETSPPM(ILEpointer *const *target); /* Array of space pointers */
extern
void*
_CVTSPP(const ILEpointer *source);

/*
 *  _GETTS64 returns the 64-bit teraspace pointer equivalent
 *  of an OS/400 PASE memory address.
 *  _GETTS64_SPP returns a 64-bit teraspace pointer to the
 *  memory addressed by a tagged space pointer.
 *  _GETTS64M converts multiple OS/400 PASE memory addresses
 *  to 64-bit teraspace pointers.
 *  _CVTTS64 returns the OS/400 PASE memory address equivalent
 *  of a 64-bit teraspace pointer, or a null pointer (zero)
 */
extern
ts64_t
_GETTS64(const void	*source);
extern
ts64_t
_GETTS64_SPP(const ILEpointer *source);
void
_GETTS64M(ts64_t	*list, /* Array of pointers */
	  unsigned	count);
extern
void*
_CVTTS64(ts64_t	source);

/*
 *  _MEMCPY_WT and _MEMCPY_WT2 copy memory, preserving any
 *  aligned 16-byte tagged pointers completely copied
 */
extern
void*
_MEMCPY_WT(void		*target,
	   const void	*source,
	   size_t	length);
extern
void
_MEMCPY_WT2(const ILEpointer *target,
	    const ILEpointer *source,
	    size_t	length);

/*
 *  _STRLEN_SPP and _STRNCPY_SPP operate on null-terminated
 *  character strings addressed by 16-byte space pointers
 */
extern
size_t
_STRLEN_SPP(const ILEpointer *source);
extern
void
_STRNCPY_SPP(const ILEpointer *target,
	     const ILEpointer *source,
	     size_t	maxlength);

/*
 *  _RSLOBJ and _RSLOBJ2 resolve a symbolic name to a
 *  16-byte tagged system pointer to an OS/400 object
 */
extern
int
_RSLOBJ(ILEpointer	*sysptr,	/* result system pointer */
	const char	*path,
	char		*objtype);	/* caller provides buffer */
extern
int
_RSLOBJ2(ILEpointer	*sysptr,	/* result system pointer */
	 unsigned short	type_subtype,
	 const char	*objname,
	 const char	*libname);

/*
 *  _PGMCALL allows an OS/400 PASE program to call a
 *  program object (object type *PGM)
 */
extern
int
_PGMCALL(const ILEpointer *target,	/* System pointer to program */
	 void		**argv,		/* array of argument pointers */
	 unsigned	flags);

/*
 *  _RETURN returns from an OS/400 PASE program without
 *  destroying the environment (leaving the program active)
 */
extern
int
_RETURN(void);

/*
 *  _ILELOADX, _ILESYMX, and _ILECALLX, allow an
 *  OS/400 PASE program to load (activate) ILE bound
 *  programs and to access exported data and procedures
 */
extern
unsigned long long			/* activation mark */
_ILELOADX(const void	*path,		/* bound pgm name or sysptr */
	  unsigned	flags);		/* option flags */
extern
int					/* symbol type */
_ILESYMX(ILEpointer	*exportPtr,	/* ILEpointer result */
	 unsigned long long actmark,	/* activation mark */
	 const char	*symbol);	/* symbol name */
extern
int
_ILECALLX(const ILEpointer *target,	/* Procedure pointer */
	  ILEarglist_base *ILEarglist,	/* ILE argument list */
	  const arg_type_t *signature,	/* List of argument types */
	  result_type_t	result_type,
	  int		flags);
extern
int					/* activation mark (old form) */
_ILELOAD(const void	*path,		/* bound pgm name or sysptr */
	 unsigned	flags);		/* option flags */
extern
int					/* symbol type */
_ILESYM(ILEpointer	*exportPtr,	/* ILEpointer result */
	int		actmark,	/* activation mark (old form) */
	const char	*symbol);	/* symbol name */
extern
int
_ILECALL(const ILEpointer *target,	/* Procedure pointer */
	 ILEarglist_base *ILEarglist,	/* ILE argument list */
	 const arg_type_t *signature,	/* List of argument types */
	 result_type_t	result_type);

/*
 *  size_ILEarglist returns the size of an ILE argument
 *  list for a particular function signature.
 *  build_ILEarglist builds an ILE argument list from
 *  an equivalent OS/400 PASE argument list
 */
extern
size_t
size_ILEarglist(const arg_type_t *signature);

extern
size_t
build_ILEarglist(ILEarglist_base *ILEarglist, /* ILE arglist buffer */
		 const void	*PASEarglist, /* OS/400 PASE arglist */
		 const arg_type_t *signature); /* List of argument types */

#ifdef OLD_build_ILEarglist
#define build_ILEarglist(ile, pase, sig, rtype, rdummy) \
(((ILEarglist_base*)(ile))->result.r_aggregate.s.addr = \
   (((rtype) > 0) ? (address64_t)(((void**)(pase))[-1]) : (address64_t)0 ), \
 (build_ILEarglist)((ile), (pase), (sig)))
#endif /* OLD_build_ILEarglist */

/*
 *  systemCL runs a CL command
 */
extern
int
systemCL(const char	*cmd,
	 int		flags);

/*
 *  SQLOverrideCCSID400 sets the OS/400 PASE CCSID value
 *  used by libdb400.a for ASCII/EBCDIC conversions
 */
extern
int
SQLOverrideCCSID400(int	newCCSID);

/*  Retrieve OS/400 PASE and job default CCSID values */
extern
int Qp2paseCCSID();
extern
int Qp2jobCCSID();

/*  Set ILE environment variables */
extern
int Qp2setenv_ile(const char *const *env,
		  const char *conflict);

/*
 *  fork400 and f_fork400 are variants of fork and f_fork
 *  that allow control of OS/400 job attributes
 */
extern
int fork400(const char *jobname,
	    unsigned resourceID);
extern
int f_fork400(const char *jobname,
	      unsigned resourceID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AS400_PROTOS_H */
