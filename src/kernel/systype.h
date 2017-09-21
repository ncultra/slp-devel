
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




/*
 * Figure out the type of system that we're running on.
 *
 * Try to determine the environment automatically from the C compiler's
 * predefined symbols.
 * The following can be determined automatically:
 * BSD VAX, Pyramid, Xenix, AT&T 3b1, AT&T 80386, Celerity and MS-DOS.
 * If this doesn't work on some new system, ifdef this out, and set it
 * by hand.
 */


/* added by mday */
/* we can essentially comment this entire file out because the correct definitions */
/* are included in the winsock2.h file that we include for windows 32 compiles     */

#ifdef _WIN32 

/* #define	rindex		strrchr        */
/* #define	index		strchr         */
/*                                     */
/* #define	u_char		uint8  */
/* #define	u_short		uint16 */
/* #define	u_int		uint32   */
/* #define	u_long		uint32  */
/*                                     */
#else  /* ! _WIN32 */

#ifdef	unix		/* true for most UNIX systems, BSD and Sys5 */
			/* but not for Xenix !! */
#define	UNIX	1	/* OS type */

#ifdef	vax		/* true for BSD on a VAX */
/* also true for VAX Sys5, but we don't have to worry about that (for now) */
#define	VAX	1	/* hardware */
#define	BSD	1	/* OS type */
#else
#ifdef	pyr

#define	PYRAMID	1	/* hardware */
#define	BSD	1	/* OS type */
#else
#ifdef	mc68k		/* assume AT&T UNIX pc, aka 7300 or 3b1 */
			/* what about other 68000 unix systems ?? */
#define	UNIXPC	1	/* hardware */
#define	SYS5	1	/* OS type */
#else
#ifdef	i386		/* AT&T System V Release 3.2 on the Intel 80386 */

#define	IBMPC	 1	/* hardware */
#define	SYS5	 1	/* OS type */
#else
#ifdef	accel

#define	CELERITY 1	/* hardware */
#define	BSD	 1	/* OS type */
#else
#define UNIX 1
#define UNIXPC 1
/* what type of unix system is this */
#endif	/* accel */
#endif	/* i386 */
#endif	/* mc68k */
#endif	/* pyr */
#endif	/* vax */

#endif	/* unix */

#ifdef	M_XENIX		/* true for SCO Xenix */
#define	UNIX	1	/* OS type */
#define	XENIX	1	/* OS type */
#define	SYS5	1	/* OS type */
#define	IBMPC	1	/* hardware */
#endif	/* M_XENIX */

#ifdef	MSDOS		/* true for Microsoft C and Lattice, assume former */
#define	IBMPC		1	/* hardware */
#define	MICROSOFT	1	/* C compiler type */
#endif	/* MSDOS */

/*
 * Define replacement names for the BSD names that we use.
 */

#ifdef	SYS5
#define	rindex		strrchr
#define	index		strchr

#define	u_char		unchar
#define	u_short		ushort
#define	u_int		uint
#define	u_long		uint32
#endif

#ifdef	MICROSOFT
#define	rindex		strrchr
#define	index		strchr

#define	u_char		unchar
#define	u_short		ushort
#define	u_int		uint
#define	u_long		uint32
#endif

#endif /* win32 */



