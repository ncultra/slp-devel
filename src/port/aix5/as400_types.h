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
/* Header File Name       : as400_types.h                            */
/*                                                                   */
/* Descriptive Name       : OS/400 PASE program data types           */
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
/* Description            : Declares data types for OS/400-unique    */
/*                          extensions to AIX interfaces that are    */
/*                          provided by OS/400 PASE runtime          */
/*                                                                   */
/* CFD List               :                                          */
/*                                                                   */
/* Flag Reason   Level   Date   Pgmr     Change Description          */
/* ---- -------- ------- ------ -------- -------------------------   */
/* $00  D95693   v4r3m0  971031 ROCH     New file                    */
/* $01  P3681905 v4r4m0  981031 timms    Code cleanup and add new    */
/*                                       _ILELOAD, _ILESYM support   */
/* $02  P9912784 v4r5m0  000115 timms    Update for AIX 4.3.3        */
/* $03  P9914068 v4r5m0  000131 timms    Add some API constants      */
/* $04  P9935054 v5r1m0  001020 timms    Add SYSTEMCL_ENVIRON        */
/* $05  P9905729 v5r1m0f 010103 timms    Add F_MAP_XPFFD             */
/* $06  D98736   v5r2m0  010322 timms    _RSLOBJ, _PGMCALL, and      */
/*                                       _ILECALLX support           */
/* $07  D99127   v5r3m0  020612 timms    Add PGMCALL_NOMAXARGS       */
/* $08  D99127   v5r3m0  030414 timms    8-byte teraspace pointers   */
/* $09  D99127   v5r3m0  030930 timms    Add PGMCALL_ASCII_STRINGS   */
/*                                                                   */
/* End CFD List.                                                     */
/*********************************************************************/

#ifndef _AS400_TYPES_H
#define _AS400_TYPES_H

#if defined(__AIXxSLIC__) && !defined(KERNEL)
#define KERNEL
#endif /* defined(__AIXxSLIC__) && !defined(KERNEL) */

#include <sys/types.h>

#ifndef KERNEL
#ifndef _H_INTTYPES	/* in AIX /usr/include/sys/inttypes.h */
typedef signed char		int8;
typedef signed short		int16;
typedef	signed int		int32;
typedef	signed long long	int64;
#endif /* _H_INTTYPES */
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef	unsigned int		uint32;
typedef	unsigned long long	uint64;
#endif /* !defined(_H_INTTYPES) && !defined(KERNEL) */

typedef float		float32;
typedef double		float64;

typedef uint64		address64_t;
#ifndef _H_INTTYPES	/* in AIX /usr/include/sys/inttypes.h */
typedef uint64		size64_t;
#endif /* _H_INTTYPES */

typedef uint32		address32_t;
typedef uint32		size32_t;

/*
 * Quadword (aligned) area for a tagged ILE pointer
 */
typedef union _ILEpointer {
#if !(defined(_AIX) || defined(KERNEL))
#pragma pack(1,16,_ILEpointer)	/* Force sCC quadword alignment */
#endif
    long double		align;	/* Force xlc quadword alignment
				   (with -qldbl128 -qalign=natural) */
#ifndef _AIX
    void		*openPtr; /* MI open pointer (tagged quadword) */
#endif
    struct {
	char		filler[16-sizeof(uint64)];
	address64_t	addr;	/* (PASE) memory address */
    } s;
} ILEpointer;

typedef uint64		ts64_t;	/* 64-bit teraspace pointer */

/*
 * Base structure of an ILE argument list.  Arguments
 * are stored (with natural alignment, based on type/length)
 * immediately following the end of this structure.  See
 * MI Architecture documentation for details.
 *
 * The _ILECALL and _ILECALLX interfaces always pass arguments
 * and function results in memory (in the argument list
 * structure), while MI architecture allows ILE to sometimes
 * pass arguments and function results in registers (making
 * parts of the memory argument list unused).
 */
typedef struct {
    ILEpointer		descriptor; /* Operational descriptor */
    union {
	ILEpointer	r_aggregate; /* Aggregate result pointer */
	struct {
	    char	filler[7];
	    int8	r_int8;
	} s_int8;
	struct {
	    char	filler[7];
	    uint8	r_uint8;
	} s_uint8;
	struct {
	    char	filler[6];
	    int16	r_int16;
	} s_int16;
	struct {
	    char	filler[6];
	    uint16	r_uint16;
	} s_uint16;
	struct {
	    char	filler[4];
	    int32	r_int32;
	} s_int32;
	struct {
	    char	filler[4];
	    uint32	r_uint32;
	} s_uint32;
	int64		r_int64;
	uint64		r_uint64;
	float64		r_float64;
    } result;			/* Function result */
} ILEarglist_base;

#define RSLOBJ_OBJTYPE_MAXLEN	11	/* max objtype length returned by _RSLOBJ */

/* MI type and subtype values for _RSLOBJ2 */
#define RSLOBJ_TS_PGM		0x0201	/* object type *PGM */
#define RSLOBJ_TS_SRVPGM	0x0203	/* object type *SRVPGM */

/* _ILELOAD function flags argument mask values */
#define ILELOAD_PATH		0x00000000 /* IFS path name */
#define ILELOAD_LIBOBJ		0x00000001 /* library/object name */
#define ILELOAD_PGMPTR		0x00000002 /* system pointer to program */

/* _ILESYM symbol type return values */
#define ILESYM_PROCEDURE	1
#define	ILESYM_DATA		2

/*
 * Argument data types for calls to ILE procedures
 */
typedef int16	arg_type_t;
/* positive arg_type_t values are length of aggregate argument */
#define ARG_END		0	/* end-of-list sentinel */
#define ARG_INT8	-1
#define ARG_UINT8	-2
#define	ARG_INT16	-3
#define ARG_UINT16	-4
#define ARG_INT32	-5
#define ARG_UINT32	-6
#define ARG_INT64	-7
#define ARG_UINT64	-8
#define ARG_FLOAT32	-9
#define ARG_FLOAT64	-10
#define ARG_MEMPTR	-11	/* Caller provides PASE memory address */
#define ARG_SPCPTR	-12	/* Caller provides tagged pointer */
#define ARG_OPENPTR	-13	/* Caller provides tagged pointer */
#define ARG_MEMTS64	-14	/* Caller provides PASE memory address */
#define ARG_TS64PTR	-15	/* Caller provides teraspace pointer */

/*
 * Function result types returned by ILE procedures
 */
typedef int16	result_type_t;
/* positive result_type_t values are length of aggregate result */
#define RESULT_VOID	0
#define RESULT_INT8	-1
#define RESULT_UINT8	-2
#define	RESULT_INT16	-3
#define RESULT_UINT16	-4
#define RESULT_INT32	-5
#define RESULT_UINT32	-6
#define RESULT_INT64	-7
#define RESULT_UINT64	-8
#define RESULT_FLOAT64	-10

/* _ILECALL and _ILECALLX function result values */
#define ILECALL_NOERROR		0
#define ILECALL_INVALID_ARG	1	/* Invalid argument type */
#define ILECALL_INVALID_RESULT	2	/* Invalid result type */
#define ILECALL_INVALID_FLAGS	3	/* Invalid flags */

/* _ILECALLX flag values */
#define ILECALL_NOINTERRUPT	0x00000004

/* _PGMCALL flag values */
#define PGMCALL_DIRECT_ARGS	0x00000001
#define PGMCALL_DROP_ADOPT	0x00000002
#define PGMCALL_NOINTERRUPT	0x00000004
#define PGMCALL_NOMAXARGS	0x00000008
#define PGMCALL_ASCII_STRINGS	0x00000010

#define PGMCALL_MAXARGS 255		/* _PGMCALL default maximum arguments */

/*
 *  OS/400 extensions for standard AIX interfaces
 */

/*  OS/400-unique command values for statx and fstatx  */
#define STX_XPFFD_PASE  (0x10000000) /* ILE file descriptor number (fstatx only) */
#define STX_XPFSS_PASE  (0x20000000) /* ILE stat structure */

/*  Special values returned for fstatx STX_XPFFD_PASE  */
#define XPFFD_PASE_STDIN	(-2)
#define XPFFD_PASE_STDOUT	(-3)
#define XPFFD_PASE_STDERR	(-4)

/*  OS/400-unique command values for fcntl  */
#define F_MAP_XPFFD  (0x10000000)	/* Map ILE file descriptor to OS/400 PASE */

/*  structure returned for STX_XPFSS_PASE */
struct stat64_ILE {
    uint32	st_mode;	/* File mode */
    uint32	st_ino;		/* File serial number */
    uint32	st_uid;		/* User ID of the owner of file */
    uint32	st_gid;		/* Group ID of the group of file */
    int64	st_size;	/* For regular files, the file size in bytes */
    int32	st_atime;	/* Time of last access */
    int32	st_mtime;	/* Time of last data modification */
    int32	st_ctime;	/* Time of last file status change */
    uint32	st_dev;		/* ID of device containing file */
    uint32	st_blksize;	/* Size of a block of the file */
    uint16	st_nlink;	/* Number of links */
    uint16	st_codepage;	/* Object data codepage */
    uint64	st_allocsize;	/* Allocation size of the file */
    uint32	st_ino_gen_id;	/* File serial number generation id */
    char	st_objtype[11];	/* OS/400 object type */
    char	st_reserved2[5]; /* Reserved */
    uint32	st_rdev;	/* Device ID (if character or block special file) */
    uint64	st_rdev64;	/* Device ID - 64 bit form */
    uint64	st_dev64;	/* ID of device containing file -  64 bit form */
    uint32	st_nlink32;	/* Number of links-32 bit */
    char	st_reserved1[26]; /* Reserved */
    uint16	st_ccsid;	/* Object data ccsid */
};

/* flags for systemCL */
#define SYSTEMCL_MSG_STDOUT	0x00000001 /* non-error msgs to stdout */
#define SYSTEMCL_MSG_STDERR	0x00000002 /* error msgs to stderr */
#define SYSTEMCL_NOMSGID	0x00000004 /* no msgid in stdout/err msgs */
#define SYSTEMCL_SPOOL_STDOUT	0x00000008 /* pipe spool to stdout */
#define SYSTEMCL_SPOOL_KEEP	0x00000010 /* keep spool files */
#define SYSTEMCL_FILTER_STDIN	0x00000020 /* ASCII->EBCDIC filter stdin */
#define SYSTEMCL_FILTER_STDOUT	0x00000040 /* EBCDIC->ASCII filter stdout */
#define SYSTEMCL_FILTER_STDERR	0x00000080 /* EBCDIC->ASCII filter stderr */
#define SYSTEMCL_SPAWN		0x00000100 /* run command in spawned job */
#define SYSTEMCL_SPAWN_JOBLOG	0x00000200 /* force joblog in spawned job */
#define SYSTEMCL_ENVIRON	0x00000400 /* set ILE environ from OS/400 PASE */

#define SYSTEMCL_VALID_FLAGS \
  (SYSTEMCL_MSG_STDOUT		| \
   SYSTEMCL_MSG_STDERR		| \
   SYSTEMCL_NOMSGID		| \
   SYSTEMCL_SPOOL_STDOUT	| \
   SYSTEMCL_SPOOL_KEEP		| \
   SYSTEMCL_FILTER_STDIN	| \
   SYSTEMCL_FILTER_STDOUT	| \
   SYSTEMCL_FILTER_STDERR	| \
   SYSTEMCL_SPAWN		| \
   SYSTEMCL_SPAWN_JOBLOG	| \
   SYSTEMCL_ENVIRON)

/* internal-use flag for __Qp2RunCL */
#define SYSTEMCL_INSPAWN	0x80000000 /* running in spawned job */

/*  OS/400-unique authorizations in struct acl_entry member ace_access  */
#define PASE_AUTH_OBJ_CTRL   0x2000
#define PASE_AUTH_OBJ_MGMT   0x1000
#define PASE_AUTH_POINTER    0x0800
#define PASE_AUTH_SPACE      0x0400
#define PASE_AUTH_RETRIEVE   0x0200
#define PASE_AUTH_INSERT     0x0100
#define PASE_AUTH_DELETE     0x0080
#define PASE_AUTH_UPDATE     0x0040
#define PASE_AUTH_EXECUTE    0x0020
#define PASE_AUTH_ALTER      0x0010
#define PASE_AUTH_REF        0x0008
#define PASE_AUTH_ALL       (PASE_AUTH_OBJ_CTRL | PASE_AUTH_OBJ_MGMT | \
                             PASE_AUTH_POINTER  | PASE_AUTH_SPACE    | \
                             PASE_AUTH_RETRIEVE | PASE_AUTH_INSERT   | \
                             PASE_AUTH_DELETE   | PASE_AUTH_UPDATE   | \
                             PASE_AUTH_EXECUTE  | PASE_AUTH_ALTER    | \
                             PASE_AUTH_REF)

#ifdef _AIX
#include <signal.h>
/*
 *  OS/400-unique ucontext_t (sigcontext) structure passed
 *  as an argument to signal handlers
 */
typedef struct ucontext_t_os400 {
    ucontext_t	uc;
    int32	__pad[3];	/* reserved */
    int32	msgkey;		/* OS/400 message key (or zero) */
} ucontext_t_os400;
#endif /* _AIX */

#endif /* _AS400_TYPES_H */
