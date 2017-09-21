/* .                             */
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



/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                        */
/*                                                                        */
/*                                       */
/*                                                                        */
/*           */
/*                                                     */
/*                                                                        */
/*          */
/*            */
/* .                              */
/*                                                                        */
/*                                                      */



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
