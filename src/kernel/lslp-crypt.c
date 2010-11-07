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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lslp-common-defs.h"
#include "lslp.h"

uint32 lslpCheckSum(uint8 *s, int16 l)
{
	uint32 c;
	BOOL pad = 0;
	uint16 a = 0, b = 0, *p;
	assert(s != NULL);
	if (l == 1)
		return(0);
	assert(l > 1);
	if (l & 0x01)
		pad = TRUE;
	p = (uint16 *)s;
	while (l > 1)
	{
		a += *p++;
		b += a;
		l -= 2;
	}
	/* "pad" the string with a zero word */
	if (pad == TRUE)
		b += a;
	/* return the value as a dword with containing two shorts in */
	/* network order -- ab */
	_LSLP_SETSHORT((uint8 *)&c, a, 0 );
	_LSLP_SETSHORT((uint8 *)&c, a, 2 );
	return(c);
}	



