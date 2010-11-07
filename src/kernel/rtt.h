
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





#ifndef LSLP_RTT_INCLUDE
#define LSLP_RTT_INCLUDE
#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Definitions for RTT timing.
 */

#include	"systype.h"
#include	<stdio.h>

#ifdef BSD
#include	<sys/time.h>
#include	<sys/resource.h>
#endif

#ifdef SYS5
#include	<sys/times.h>	/* requires <sys/types.h> */
#include	<sys/param.h>	/* need the definition of HZ */
#define		TICKS	HZ	/* see times(2); usually 60 or 100 */
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <sys\timeb.h>
#include <time.h>
#include "lslp-windows.h"
#define err_sys printf
#endif

#ifdef _AIX     	// JSS
#include "lslp-aix.h"	// JSS
#else			// JSS
#ifdef __unix__
#include "lslp-linux.h"
#endif
#endif			// JSS

/*
 * Structure to contain everything needed for RTT timing.
 * One of these required per socket being timed.
 * The caller allocates this structure, then passes its address to
 * all the rtt_XXX() functions.
 */

struct rtt_struct {
  float	rtt_rtt;	/* most recent round-trip time (RTT), seconds */
  float	rtt_srtt;	/* smoothed round-trip time (SRTT), seconds */
  float	rtt_rttdev;	/* smoothed mean deviation, seconds */
  int16	rtt_nrexmt;	/* #times retransmitted: 0, 1, 2, ... */
  int16	rtt_currto;	/* current retransmit timeout (RTO), seconds */
  int16	rtt_nxtrto;	/* retransmit timeout for next packet, if nonzero */

#ifdef SYS5
  int32			time_start;	/* for elapsed time */
  int32			time_stop;	/* for elapsed time */
  struct tms		tms_start;	/* arg to times(2), but not used */
  struct tms		tms_stop;	/* arg to times(2), but not used */
#else
  struct timeval	time_start;	/* for elapsed time */
  struct timeval	time_stop;	/* for elapsed time */
#endif
};

#define	RTT_RXTMIN      2	/* min retransmit timeout value, seconds */
#define	RTT_RXTMAX    120	/* max retransmit timeout value, seconds */
#define	RTT_MAXNREXMT 	4	/* max #times to retransmit: must also
				   change exp_backoff[] if this changes */
#ifndef __linux__
#ifdef	SYS5
int32	times();		/* the system call */
#endif
#endif
extern int32	rtt_d_flag;	/* can be set nonzero by caller for addl info */





/* Timer routines for round-trip timing of datagrams.
 *
 *	rtt_init()	Called to initialize everything for a given
 *			"connection."
 *	rtt_newpack()	Called before each new packet is transmitted on
 *			a "connection."  Initializes retransmit counter to 0.
 *	rtt_start()	Called before each packet either transmitted or
 *			retransmitted.  Calculates the timeout value for
 *			the packet and starts the timer to calculate the RTT.
 *	rtt_stop()	Called after a packet has been received.
 *	rtt_timeout()	Called after a timeout has occurred.  Tells you
 *			if you should retransmit again, or give up.
 *
 * The difference between rtt_init() and rtt_newpack() is that the former
 * knows nothing about the "connection," while the latter makes use of
 * previous RTT information for a given "connection."
 */

void rtt_init(struct rtt_struct *);
void rtt_newpack(struct rtt_struct *);
int32 rtt_start(struct rtt_struct *);
void rtt_stop(struct rtt_struct *);
int32 rtt_timeout(struct rtt_struct *);
void rtt_debug(struct rtt_struct *);

#ifdef	__cplusplus
}
#endif
#endif

