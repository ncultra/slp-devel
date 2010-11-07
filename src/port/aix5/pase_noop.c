/* begin_generated_IBM_copyright_prolog                             */
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



/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ---------------------------------------------------------------- */
/* IBM Confidential                                                 */
/*                                                                  */
/* OCO Source Materials                                             */
/*                                                                  */
/* Product(s):                                                      */
/*     5733-SC1                                                     */
/*                                                                  */
/* (C)Copyright IBM Corp.  2005.                                    */
/*                                                                  */
/* The Source code for this program is not published  or otherwise  */
/* divested of its trade secrets,  irrespective of what has been    */
/* deposited with the U.S. Copyright Office.                        */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

/* Module Description ***********************************************/
/*                                                                  */
/* OS/400 Source File Name: pase_noop.c                             */
/*                                                                  */
/* Module Name: pase_noop                                           */
/*                                                                  */
/* Service Program Name:                                            */
/*                                                                  */
/* Source File Description: This file contains a no-op program,     */
/*    whose sole purpose is to test for proper installation of      */
/*    BOSS Option 33 (PASE - Portable Applications Solutions        */
/*    Environment).  This will allow a calling ILE/C program to     */
/*    detect and log error conditions where PASE is not installed.  */
/*                                                                  */
/* Function List:                                                   */
/*                                                                  */
/* Change Activity:                                                 */
/*                                                                  */
/* CFD List:                                                        */
/*                                                                  */
/* flg reason    vers date   user  comment                          */
/* --- --------- ---- ------ ----  -------------------------------- */
/* $A0 Dxxxxx    5D30 050218 JSS : Inititial creation               */
/*                                                                  */
/* End CFD List.                                                    */
/*                                                                  */
/*  Additional notes about the Change Activity                      */
/*                                                                  */
/* A0 - Initial release                                             */
/*                                                                  */
/* End Change Activity.                                             */
/*                                                                  */
/* End Module Description *******************************************/

static char ibmid[] = "5733-SC1 (C) Copyright IBM Corp. 2005 \
   All rights reserved. \
   US Government Users Restricted Rights - \
   Use, duplication or disclosure restricted \
   by GSA ADP Schedule Contract with IBM Corp. \
   Licensed Materials - Property of IBM ";

#include <stdio.h>
#include <stdlib.h>

static char rcsHeader[] = "$Header: /var/lib/cvsroot/ldap_slp/src/port/aix5/pase_noop.c,v 1.2 2005/03/29 21:01:49 mdday Exp $";

int main(int argc, char *argv[], char *envp[])
{
  char *pszProgramName = "pase_noop";
  /* Only post a message if help requested...otherwise just exit */
  if ((argc > 1) && (!memcmp(argv[1],"-h",2))) {
    printf("%s: PASE no-op program executed successfully.\n",
           pszProgramName); /* because argv[0] is ILE caller */
  }

  return(0);

} /* end main */
