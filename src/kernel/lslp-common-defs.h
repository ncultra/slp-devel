/* This is an automatically generated prolog.                             */
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



/*                                                                        */
/*                                                                        */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* OBJECT CODE ONLY SOURCE MATERIALS                                      */
/*                                                                        */
/* (C) COPYRIGHT International Business Machines Corp. 2000,2004          */
/* All Rights Reserved                                                    */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */



#ifndef _LSLP_COMMON_INCLUDE_
#define _LSLP_COMMON_INCLUDE_

#include "../../config.h"

#if !defined(powerpc)
	#define	__LITTLE_ENDIAN__	1234
	#define	__BIG_ENDIAN__	4321
	#define	__PDP_ENDIAN__	3412
#endif


#ifdef NUCLEUS
   #define __BYTE_ORDER__ __BIG_ENDIAN__
#else
   #ifdef WORDS_BIGENDIAN
      #define __BYTE_ORDER__ __BIG_ENDIAN__
   #else 
      #define __BYTE_ORDER__ __LITTLE_ENDIAN__
   #endif 
#endif

#include OS_PORT_INCLUDE

#include "lslp.h"
#include "rtt.h"
#include "lslp-config.h"
#include "lslp-kernel.h" 
#include "attr.h" 
#include "url.h" 

#include "lslp-crypt.h"
#include "lslp-da-sa.h"
#include "lslp-msg.h"

/*#include "lslp-ldap.h" */

#ifdef __cplusplus
extern "C" {
#endif


#define LSLP_HEXDUMP(c) ((((c) > 31) && ((c) < 128)) ? (c) : '.')
#define LSLP_MSG_STRINGS		4
#define LSLP_STRINGS_HEXDUMP 	1
#define LSLP_STRINGS_WORKDUMP   2
#define LSLP_STRINGS_NADUMP     3

#ifdef __cplusplus
}
#endif

#endif /* _LSLP_COMMON_INCLUDE_ */
