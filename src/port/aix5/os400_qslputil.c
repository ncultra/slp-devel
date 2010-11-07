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
/* OS/400 Source File Name: os400_qslputil.c                        */
/*                                                                  */
/* Module Name: os400_qslputil                                      */
/*                                                                  */
/* Service Program Name:                                            */
/*                                                                  */
/* Source File Description:  OS400 SLP Utilities                    */
/*                                                                  */
/*   The functions included in this module allow for some           */
/*   OS400-specific utility functions that can be called from       */
/*   within PASE (like converting ascii/ebcdic, the infamous        */
/*   qslp_IssueMsg() function, etc.)                                */
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

static char rcsHeader[] = "$Header: /var/lib/cvsroot/ldap_slp/src/port/aix5/os400_qslputil.c,v 1.2 2005/03/29 21:01:49 mdday Exp $";

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define _OS400_QSLPUTIL_C
#include "os400_qslputil.h"
#undef _OS400_QSLPUTIL_C

#ifdef OS400PASE_ONLY
#include <as400_types.h>
#include <as400_protos.h>
#include <os400msg.h>
#endif /* #ifdef OS400PASE_ONLY */

/********************************************************************/
/* PID & program names must match those in qslp.c in ILE/C!         */
/* If you change them here, you have to change them there and       */
/* recompile.                                                       */
/********************************************************************/
char *pszSlpDaPath     = "/QOpenSys/QIBM/ProdData/SC1/lslp_kernel";
char *pszSlpSaPath     = "/QOpenSys/QIBM/ProdData/SC1/slp_srvreg";
char *pszSlpUaPath     = "/QOpenSys/QIBM/ProdData/SC1/slp_query";
char *pszSlpAtPath     = "/QOpenSys/QIBM/ProdData/SC1/slp_attrreq";

char *pszSlpDaConfPath = "/QOpenSys/QIBM/UserData/SC1/slp.conf";

char *pszSlpSaPidFile  = "/QOpenSys/QIBM/UserData/SC1/sa.pid";
char *pszSlpDaPidFile  = "/QOpenSys/QIBM/UserData/SC1/da.pid";
char *pszSlpUaPidFile  = "/QOpenSys/QIBM/UserData/SC1/ua.pid";
char *pszSlpAtPidFile  = "/QOpenSys/QIBM/UserData/SC1/at.pid";


/********************************************************************/
/* Conversion between ASCII and EBCDIC using simple table machanism */
/* ASCII code page 819; EBCDIC code page 37                         */
/********************************************************************/

/********************************************************************/
/* Convert an EBCDIC null-terminated string into ASCII              */
/********************************************************************/
void ntstr_to_ascii(char * input_ebcdic_str)
{
  register char *cp;
  register int c;
  int index, str_len;

  cp = input_ebcdic_str;
  str_len = strlen(input_ebcdic_str);
  for (index = 0; index < str_len; index++) {
    c = *cp;
    *cp = to_ascii[c];
    cp++;
  }
  return;
}

/********************************************************************/
/* Convert an EBCDIC non null-terminated string into ASCII          */
/********************************************************************/
void str_to_ascii(char * input_ebcdic_str, int str_len)
{
  register char *cp;
  register int c;
  int index;

  cp = input_ebcdic_str;
  for (index = 0; index < str_len; index++) {
    c = *cp;
    *cp = to_ascii[c];
    cp++;
  }
  return;
}

/********************************************************************/
/* Convert a null_terminated ASCII string into EBCDIC               */
/********************************************************************/
void ntstr_to_ebcdic(char * input_ascii_str)
{
  register char *cp;
  register int c;
  int index, str_len;

  cp = input_ascii_str;
  str_len = strlen(input_ascii_str);
  for (index = 0; index < str_len; index++) {
    c = *cp;
    *cp = to_ebcdic[c];
    cp++;
  }
  return;
}

/********************************************************************/
/* Convert a non null_terminated ASCII string into EBCDIC           */
/********************************************************************/
void str_to_ebcdic(char * input_ascii_str, int str_len)
{
  register char *cp;
  register int c;
  int index;

  cp = input_ascii_str;
  for (index = 0; index < str_len; index++) {
    c = *cp;
    *cp = to_ebcdic[c];
    cp++;
  }
  return;
}


/* Function Specification *********************************************/
/*                                                                    */
/* Function Name: qslp_IssueMsg                                       */
/*                                                                    */
/* Descriptive Name:                                                  */
/*   Issue the specified message to the job log.                      */
/*                                                                    */
/* Notes:                                                             */
/*   It is pretty much the same, although I did make some changes     */
/*   and improvements.  Biggest difference is that I added a new      */
/*   input parm (see pszMsgDest below) where the caller can mandate   */
/*   where they want the message sent ... you can pass NULL, though,  */
/*   and we'll make a pretty good guess at the best way to send it.   */
/*   Also added code to automatically convert string type             */
/*   replacement data to ebcdic from ascii (rather than always        */
/*   making the caller do it before calling this).                    */
/*                                                                    */
/*   Messages: This function does not define any messages of its own, */
/*             but issues the message specified by the caller to the  */
/*             job log.                                               */
/*                                                                    */
/* Input:                                                             */
/*   pszMsgId: pointer to string containing the i5/OS message id for  */
/*          the message to be issued.                                 */
/*   pszMsgType: pointer to string containing the type of message     */
/*               ("*COMP     ", "*ESCAPE   ", "*DIAG     ",           */
/*                "*INFO     ", "*INQ      ", "*NOTIFY   ",           */
/*                "*REQ      ", or "*STATUS   ").  NOTE:  This MUST   */
/*               be a 10 char + NULL string.                          */
/*   pszMsgDest: pointer to string containing the destination of      */
/*               message ... this would be like:                      */
/*               ("*EXT      ", "*PGMBDY   ", etc.)                   */
/*               It can be sent NULL, in which case we'll take a      */
/*               guess at what would be best.                         */
/*   iNumRpls: number of replacement values for the message.          */
/*  *** NOTE ***: This function accepts a variable number of sets     */
/*                of replacement value parameters, with the number    */
/*                of sets determined by the numrpls argument.         */
/*  For each replacement value:                                       */
/*    - argument indicating the type of the replacement value; this   */
/*      argument is of type qslp_Substitute_t.                        */
/*    - if the argument type value is SUB_STRING or SUB_HEX, an       */
/*      integer argument which specifies the length of the            */
/*      substitution value AS DEFINED IN THE MESSAGE FILE.  For       */
/*      SUB_HEX, this must also match the size of the data provided.  */
/*      If the argument type value is SUB_VERBATIM, an integer        */
/*      argument which specifies the length of the (already           */
/*      prepared) substutution data.                                  */
/*    - argument containing the replacement value; the type of the    */
/*      argument may be string (pointer to character), short integer, */
/*      or long integer.  For SUB_VERBATIM, this is a pointer to      */
/*      the buffer containing the prepared substitution data.         */
/*    - if the argument type value is SUB_VERBATIM, a pointer to      */
/*      a null-terminated string contining the name of the message    */
/*      file containing the message to be issued.  (If this pointer   */
/*      is null, the QTCP message file will be used.                  */
/*                                                                    */
/* Output:                                                            */
/*   A message is issued to the job log.                              */
/*                                                                    */
/* Exit Normal:                                                       */
/*                                                                    */
/*                                                                    */
/* Exit Error:                                                        */
/*                                                                    */
/*                                                                    */
/* End Function Specification *****************************************/
void qslp_IssueMsg(
         char *   pszMsgId,     /* ptr to string with message id      */
         char *   pszMsgType,   /* message type system value string   */
         char *   pszMsgDest,   /* destination (*EXT, *PGMBDY, etc.)  */
         int      iNumRpls,     /* number of msg replacement values   */
         ...                    /* optional sets of replacement
                                     value types and repl. values     */
                      )         /* none                               */
{
#ifdef OS400PASE_ONLY /* This function will only work if run on OS400 */
                      /* and will not load correctly on AIX (will try */
                      /* to find apis that don't exist in AIX). If    */
                      /* we are building a version that can run both  */
                      /* places, this function will do nothing and    */
                      /* just return.                                 */

  #define ERRBUFSIZE 256
  #define REPLBUFFSIZE 2048
  const int iErrBufSize = ERRBUFSIZE; /* size of QMHSNDPM err buffer  */
  char pcErrBuf[ERRBUFSIZE];      /* error return buffer for QMHSNDPM */
  Qus_EC_t *pErrBuf;              /* ptr to error buffer (w/cast)     */
  char aszMsgFile[21];            /* name of message file             */
  char *pszMsgFile;               /* ptr to name of message file      */
  char pcMsgDest[11];             /* destination of message           */
  char iCallStack = 0;            /* how far back in stack to send    */
  char pcMsgKey[4];               /* msg key returned by QMHSNDPM     */
  int iReplBuffIndex;             /* index into repl. buffer          */
  int iReplArgIndex;              /* replacement argument index       */
  const int iReplBuffSize = REPLBUFFSIZE; /* size of repl buffer      */
  char pszReplBuff[REPLBUFFSIZE + 1];  /* replacement value buffer    */
  int iSubsStrLength;             /* length of replacement string     */
  qslp_Substitute_t SubType;      /* substitution data type           */
  long lSubLong;                  /* long int for substitution        */
  short sSubShort;                /* short int for substitution       */
  char *pszSubString;             /* ptr to string for substitution   */
  va_list ArgPtr;                 /* variable argument ptr            */
  char *pszProfile = NULL;        /* user profile for QMNSNDM         */
  char pcProfileQueue[21];        /* profile + *USER for QMHSNDM      */
  char pcMsgReply[20];            /* msg reply for QMHSNDM            */
  int  iNumMsgQs = 1;             /* array msgqs for QMHSNDM          */
  char *pszFunctionName = "qslp_IssueMsg";

  /*********************************************************************/
  /* inititialize message file to system if begins with TCP            */
  /*********************************************************************/
  if        (!memcmp("TCP", pszMsgId, 3)) {
    strcpy(aszMsgFile, QTCPMSG);   /* Stack/BOSS/Apps */
  } else if (!memcmp("CP", pszMsgId, 2)) {
    strcpy(aszMsgFile, QCPFMSG);   /* Most CP[A-F]nnnn */
  } else if (!memcmp("CEE", pszMsgId, 3)) {
    strcpy(aszMsgFile, QCEEMSG);   /* Exceptions */
  } else if (!memcmp("C2", pszMsgId, 2)) {
    strcpy(aszMsgFile, QC2MSGF);   /* ILE Signals */
  } else if (!memcmp("SLP", pszMsgId, 3)) {
    strcpy(aszMsgFile, QSLPMSG);   /* SLP */
  } else if (!memcmp("DNS", pszMsgId, 3)) {
    strcpy(aszMsgFile, QTOBMSGF);  /* DNS */
  } else {
    strcpy(aszMsgFile, QCPFMSG);   /* Default file */
  }

  /********************************************************************/
  /* Initialze variable function argument processing                  */
  /********************************************************************/
  va_start(ArgPtr, iNumRpls);
  iReplBuffIndex = 0;

  for (iReplArgIndex = 0; iReplArgIndex < iNumRpls; ++iReplArgIndex) {
    SubType = va_arg(ArgPtr, qslp_Substitute_t);

    switch ( SubType ) {

      case SUB_STRING: /* string length, string */
      iSubsStrLength = va_arg(ArgPtr, int);
      pszSubString = va_arg(ArgPtr, char *);
      /* Substitution string too long, adjust length */
      if   ( ( iSubsStrLength + iReplBuffIndex ) > iReplBuffSize ) {
        iSubsStrLength = iReplBuffSize - iReplBuffIndex;
      }
      qslp_PadCpy(&pszReplBuff[iReplBuffIndex], pszSubString, iSubsStrLength);
      iReplBuffIndex += iSubsStrLength;
      break;

      case SUB_SHORT:  /* short int gets promoted to long int */
      sSubShort = va_arg(ArgPtr, int);
      if ((sizeof(signed short int) + iReplBuffIndex) < iReplBuffSize) {
        memcpy(&pszReplBuff[iReplBuffIndex], &sSubShort,
               sizeof(signed short int));
        iReplBuffIndex += sizeof(signed short int);
      }
      break;

      case SUB_LONG: /* long int */
      lSubLong = va_arg(ArgPtr, long);
      if ((sizeof(signed long int) + iReplBuffIndex) < iReplBuffSize) {
        memcpy(&pszReplBuff[iReplBuffIndex], &lSubLong,
               sizeof(signed long int));
        iReplBuffIndex += sizeof(signed long int);
      }
      break;

      case SUB_HEX: /* hex length, hex string */
      iSubsStrLength = va_arg(ArgPtr, int);
      pszSubString = va_arg(ArgPtr, char *);
      if ((iSubsStrLength + iReplBuffIndex ) > iReplBuffSize) {
        iSubsStrLength = iReplBuffSize - iReplBuffIndex;
      }
      memcpy(&pszReplBuff[iReplBuffIndex], pszSubString, iSubsStrLength);
      iReplBuffIndex += iSubsStrLength;
      break;

      case SUB_VERBATIM:  /* verbatim length, verbatim prepared data  */
      iSubsStrLength = va_arg(ArgPtr, int);
      pszSubString = va_arg(ArgPtr, char *);
      if ((iSubsStrLength + iReplBuffIndex) > iReplBuffSize) {
        iSubsStrLength = iReplBuffSize - iReplBuffIndex;
      }
      memcpy(&pszReplBuff[iReplBuffIndex], pszSubString, iSubsStrLength);
      iReplBuffIndex += iSubsStrLength;
      pszMsgFile = va_arg(ArgPtr, char *);

      if (NULL != pszMsgFile) {
        strcpy(aszMsgFile, pszMsgFile);
      }
      break;

      case SUB_VSTRING: /* variable length string */
      pszSubString = va_arg(ArgPtr, char *);
      iSubsStrLength = strlen(pszSubString);

      if (iReplBuffIndex + sizeof(signed short int) <= iReplBuffSize) {
        if ((iSubsStrLength + iReplBuffIndex + sizeof(signed short int)) > iReplBuffSize) {
          iSubsStrLength = iReplBuffSize - iReplBuffIndex - sizeof(signed short int);
        }
        *(signed short int *)&pszReplBuff[iReplBuffIndex] = iSubsStrLength;
        iReplBuffIndex += sizeof(signed short int);
        memcpy(&pszReplBuff[iReplBuffIndex], pszSubString, iSubsStrLength);
        iReplBuffIndex += iSubsStrLength;
      }
      break;

      case SUB_PROFILE: /* user profile string */
      pszProfile = va_arg(ArgPtr, char *);
      qslp_PadCpy(pcProfileQueue, pszProfile, 10);
      /***************************************************************/
      /* Array of message queues are 20 chars each.  Use profile     */
      /* name followed by special value *USER for default msg queue. */
      /***************************************************************/
      qslp_PadCpy(&pcProfileQueue[10], "*USER     ", 10);
      break;

      default:
      break;
    } /* switch */
  } /* for */

  /********************************************************************/
  /* complete variable function argument processing (va_end macro)    */
  /********************************************************************/
  va_end(ArgPtr);

  /********************************************************************/
  /* If the destination was not supplied, then make a smart guess     */
  /********************************************************************/
  if (NULL == pszMsgDest) {
    if (!memcmp(pszMsgType, MSG_ESCAPE, 7)) {
      strcpy(pcMsgDest, MSQ_Q_PGM_BDY); /* Program boundary */
    } else {
      strcpy(pcMsgDest, MSQ_Q_CUR_PROG);/* Current program  */
    }
  } else {
    strncpy(pcMsgDest, pszMsgDest, 10); /* User specified */
  }

  memset(pcMsgKey, 0x00, sizeof(pcMsgKey));
  memset(pcMsgReply, 0x00, sizeof(pcMsgReply));

  /********************************************************************/
  /* Setup error handling                                             */
  /********************************************************************/
  pErrBuf = (Qus_EC_t *)pcErrBuf;
  pErrBuf->Bytes_Provided = 0;
  iCallStack = 0;

  /********************************************************************/
  /* If a profile argument was received, send a non-program message   */
  /* to that profiles message queue.                                  */
  /********************************************************************/
  if (NULL != pszProfile) {
    printf("%s: Non-program message %.7s from >%.10s< to profile %s\n",
            pszFunctionName, pszMsgId, aszMsgFile, pszProfile);
    QMHSNDM(pszMsgId, (void *)aszMsgFile, pszReplBuff, iReplBuffIndex,
             pszMsgType, pcProfileQueue, iNumMsgQs,
             pcMsgReply, (int *)pcMsgKey, pErrBuf);
  }

  /********************************************************************/
  /* Send a program message to the joblog.                            */
  /********************************************************************/
  printf("%s: Program message %.7s from >%.10s< to joblog\n",
          pszFunctionName, pszMsgId, aszMsgFile);

  QMHSNDPM(pszMsgId, (void *)aszMsgFile, pszReplBuff,
           iReplBuffIndex, pszMsgType, pcMsgDest, iCallStack,
           (int *)pcMsgKey, pErrBuf);

#endif /* #ifdef OS400PASE_ONLY */

  return;
}


/* Function Specification *********************************************/
/*                                                                    */
/* Function Name: qslp_PadCpy                                         */
/*                                                                    */
/* Descriptive Name:                                                  */
/*                                                                    */
/*   This function copies a source string to a destination string,    */
/*   padding the destination string (if necessary) to the specified   */
/*   length with spaces.  The destination string is also              */
/*   null-terminated.  This function operates like strncpy, such that */
/*   if the destination string is longer than the source string, only */
/*   the specified number of characters will be copied.               */
/*                                                                    */
/* Input:                                                             */
/*                                                                    */
/*   pszDest:   ptr to character array where string is to be copied   */
/*   pszSrc:    ptr to null-terminated string to copy                 */
/*   iDestLen:  desired length of destination string (not including   */
/*              the terminating null character which will be added)   */
/*                                                                    */
/* Output:                                                            */
/*                                                                    */
/*   Modified string or char array with NULL terminator added.        */
/*                                                                    */
/* Exit Normal:                                                       */
/*                                                                    */
/*   A pointer to the destination string is returned.                 */
/*                                                                    */
/* Exit Error:                                                        */
/*                                                                    */
/*   None.                                                            */
/*                                                                    */
/* End Function Specification *****************************************/
char *qslp_PadCpy(
         char *   pszDest,      /* ptr to destination string          */
         char *   pszSrc,       /* ptr to source string               */
         const int iDestLen     /* desired length of dest. string     */
                )               /* ptr to destination string          */
{
  int  iCpyLen;                 /* bytes to copy                      */

  iCpyLen = strlen(pszSrc);
  if (iCpyLen > iDestLen) {
    iCpyLen = iDestLen;
  }
  memcpy(pszDest, pszSrc, iCpyLen);
  if (iCpyLen < iDestLen) {
    memset(&pszDest[iCpyLen], ' ', iDestLen - iCpyLen);
  }

  /********************************************************************/
  /* Add null termination to string                                   */
  /********************************************************************/
  pszDest[iDestLen] = 0x00;

  return(pszDest);
}

/********************************************************************/
/* Create a file with PID number that can be used by kill() later   */
/********************************************************************/
void qslp_CreatePidFile(char *pszPidFile) {
  FILE *fp;
  int fd;
  char tmpSub1[256];
  char tmpSub2[256];

  fp = fopen(pszPidFile, "a+"); /* Allow more PIDs for multiple SAs  */
  if (fp == NULL) {
    (void)close(fd);
    printf("qslp_CreatePidFile: Error creating PID file %s\n", pszPidFile);
    sprintf(tmpSub2, "ERRNO %d : %s.", errno, strerror(errno));
    sprintf(tmpSub1, "Error creating pid file '%s'. ", pszPidFile);
    strcat(tmpSub1, tmpSub2);
    printf("qslp_CreatePidFile: %s\n", tmpSub1);
    qslp_IssueMsg("CPF9897", "*DIAG     ", "*EXT      ", 1, 
                  SUB_VSTRING, tmpSub1);
  } else {
    (void) fprintf(fp, "%ld\n", (long)getpid());
    (void) fclose(fp);
    printf("qslp_CreatePidFile: Created PID file:\n");
    printf("qslp_CreatePidFile:   >%s<\n", pszPidFile);
  }
  return;
}

/**********************************************************************/
/* @function qslp_Buffer()                                            */
/*                                                                    */
/* Parameters:                                                        */
/*                                                                    */
/* char *pcBuffer - points at buffer to dump                          */
/* int iBuffLen   - length of buffer to dump                          */
/*                                                                    */
/* Description:                                                       */
/*                                                                    */
/* Dumps out an buffer in both hex and readable form.                 */
/*                                                                    */
/*      SignalHandler: Dumping Signal.Ex_Data:                        */
/*          1934D8E3 D4E3E2D7 C3F0F0F2 40404040  |..QTGSPC002     |   */
/*          40404040 40404040 40404040 40404040  |                |   */
/*          00000000 00000000 00000000 00000000  |................|   */
/**********************************************************************/
void qslp_Buffer(char *pcBuffer, int iBuffLen)
{
  int i=0,
  j=0,
  k=0,
  numrows=0,
  iLineLen=0;
  unsigned char c;
  char acLogBuffer[132];
  char *pBuffPtr = acLogBuffer;
  char acLine[20];

  numrows = (iBuffLen + 15) / 16;
  for (i = 0; i < numrows; i++) {
    /******************************************************************/
    /* Offset into line by 4 blank characters                         */
    /******************************************************************/
    *pBuffPtr = ' '; pBuffPtr++;
    *pBuffPtr = ' '; pBuffPtr++;
    *pBuffPtr = ' '; pBuffPtr++;
    *pBuffPtr = ' '; pBuffPtr++;
    /******************************************************************/
    /* Print 16 bytes of the buffer as the hexadecimal dump section   */
    /******************************************************************/
    for (j = 0; j < 16; j++) {
      if (j + (i * 16) >= iBuffLen) {
        *pBuffPtr = ' '; pBuffPtr++;
        *pBuffPtr = ' '; pBuffPtr++;
      } else {
        sprintf(pBuffPtr, "%02X", pcBuffer[j + (i * 16)]);
        pBuffPtr+=2;
      } /* endif */
      /****************************************************************/
      /* Print a space every 4 bytes (4 sets of hex bytes)            */
      /****************************************************************/
      /* if (j % 4 == 3) { */
      if ((j - ((j/4)*4)) == 3) {
        *pBuffPtr = ' '; pBuffPtr++;
      } /* endif */
    } /* endfor */
    /******************************************************************/
    /* Print the same 16 bytes as the "readable" dump section         */
    /******************************************************************/
    *pBuffPtr = ' '; pBuffPtr++;
    *pBuffPtr = '|'; pBuffPtr++;                    /* Left frame bar */
    k=0;
    for (j=0; j < 16; j++) {
      if (j + (i * 16) >= iBuffLen) {
        *pBuffPtr = ' '; pBuffPtr++;
      } else {
        k++;                                       /* Count real data */
        c = pcBuffer[j + (i * 16)];
        if (isprint(c)) {
          *pBuffPtr = c; pBuffPtr++;
        } else if (c == 0x40) {
          *pBuffPtr = ' '; pBuffPtr++;
        } else {
          *pBuffPtr = '.'; pBuffPtr++;
        } /* endif */
      } /* endif */
    } /* endfor */
    *pBuffPtr = '|';                               /* Right frame bar */
    pBuffPtr++;
    memset(acLine, 0x00, sizeof(acLine));
    /******************************************************************/
    /* This is a bit more esoteric...useful matching up multi-line    */
    /* dumps for easy comparison.  The byte count helps you find the  */
    /* line you want quickly.                                         */
    /******************************************************************/
    sprintf(acLine, "  Byte %d\n", (i * 16) + k);
    iLineLen = strlen(acLine);
    memcpy(pBuffPtr, acLine, iLineLen);
    pBuffPtr += iLineLen;
    *pBuffPtr = 0x00;
    pBuffPtr = &acLogBuffer[0];
    printf(pBuffPtr);
  } /* endfor */
  return;
}
