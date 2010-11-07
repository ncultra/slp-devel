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
/* OS/400 Source File Name: os400_qslputil.h                        */
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
#ifndef _OS400_QSLPUTIL_H
#define _OS400_QSLPUTIL_H

#ifndef OS400PASE_ONLY
#define OS400PASE_ONLY /* This enables features that will ONLY run  */
                       /* under PASE on OS400 ... if you build with */
                       /* this defined and try to run the final     */
                       /* executables under AIX, they will likely   */
                       /* fail. If OS400PASE_ONLY is not defined,   */
                       /* then the executables you build will run   */
                       /* under both AIX and OS400 PASE, so just    */
                       /* comment this #define out if you want a    */
                       /* version that will run on AIX & PASE both. */
                       /* NOTE: This enables code for things like   */
                       /*       sending real OS400 messages, for    */
                       /*       which the apis would obviously not  */
                       /*       exist on AIX. So, if you build      */
                       /*       without OS400PASE_ONLY, it will     */
                       /*       still run under both, but when you  */
                       /*       run under PASE, you will no longer  */
                       /*       have the real OS400 messages in the */
                       /*       joblog. This is ok for testing, but */
                       /*       the production version shipped with */
                       /*       OS400 MUST have this OS400PASE_ONLY */
                       /*       flag defined.                       */
#endif /* #ifndef OS400PASE_ONLY */

/********************************************************************/
/* Turn on/off debugging printf's                                   */
/********************************************************************/
#define DEBUG  // turn on debugging...normally put this in compile

#ifndef DBG
  #ifdef DEBUG
    #define DBG(x) x
  #else
    #define DBG(x)
  #endif
#endif

/********************************************************************/
/* PID & program names must match those in qslp.c in ILE/C!         */
/* If you change them here, you have to change them there and       */
/* recompile.                                                       */
/********************************************************************/
extern char *pszSlpDaPath;
extern char *pszSlpSaPath;
extern char *pszSlpUaPath;
extern char *pszSlpAtPath;

extern char *pszSlpDaConfPath;

extern char *pszSlpSaPidFile;
extern char *pszSlpDaPidFile;
extern char *pszSlpUaPidFile;
extern char *pszSlpAtPidFile;

#ifdef _OS400_QSLPUTIL_C  /* Only want these items the module that */
                          /* has the actual functions.             */

/*******************************************************************/
/* conversion between ASCII and EBCDIC using simple table machnism */ 
/* ASCII code page 819; EBCDIC code page 37                        */
/*******************************************************************/

#define   CONVERSION_TABLE_SIZE    256  	

/********************************************************************/
/* Use EBCDIC hex value as offset index for the array, the value of */
/* the array is the same char in ascii hex value                    */
/* the characters that we don't care for conversion is set to 0     */
/********************************************************************/
const char to_ascii[CONVERSION_TABLE_SIZE] = {

/* 8 in a row; 32 rows; 8x32 = 256                                  */     

/********************************************************************/
/* The headings are the second possible digit: e.g. 08 means 0 or 8 */
/* second digit of index: odd rows are 0-7; even rows are 8-F       */
/********************************************************************/

    /* 08    19    2A   3B    4C    5D    6E    7F */  /* index     */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x00-0x07 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x08-0x0F */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x10-0x17 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x18-0x1F */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x20-0x27 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x28-0x2F */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x30-0x37 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x38-0x3F */

      0x20, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x40-0x47 */
      0x00, 0x00, 0x00, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,  /* 0x48-0x4F */

      0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x50-0x57 */
      0x00, 0x00, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0xAC,  /* 0x58-0x5F */

      0x2D, 0x2F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x60-0x67 */
      0xC7, 0x00, 0xA6, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,  /* 0x68-0x6F */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x70-0x77 */ 
      0x00, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,  /* 0x78-0x7F */

     /* 08    19    2A   3B    4C    5D    6E    7F */ /* index     */

      0x00, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,  /* 0x80-0x87 */ 
      0x68, 0x69, 0xAB, 0xBB, 0x00, 0x00, 0x00, 0xB1,  /* 0x88-0x8F */

      0xB0, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,  /* 0x90-0x97 */
      0x71, 0x72, 0x00, 0x00, 0xE6, 0x00, 0x00, 0xA4,  /* 0x98-0x9F */

      0xB5, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,  /* 0xA0-0xA7 */
      0x79, 0x7A, 0xA1, 0xBF, 0x00, 0x00, 0x00, 0xAE,  /* 0xA8-0xAF */

      0x5E, 0x00, 0xA5, 0xB7, 0xA9, 0x00, 0x00, 0x00,  /* 0xB0-0xB7 */
      0x00, 0x00, 0x5B, 0x5D, 0xAF, 0xA8, 0xB4, 0xD7,  /* 0xB8-0xBF */

      0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,  /* 0xC0-0xC7 */
      0x48, 0x49, 0xAD, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0xC8-0xCF */

      0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,  /* 0xD0-0xD7 */
      0x51, 0x52, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0xD8-0xDF */

      0x5C, 0xF7, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,  /* 0xE0-0xE7 */
      0x59, 0x5A, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0xE8-0xEF */ 

      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,  /* 0xF0-0xF7 */
      0x38, 0x39, 0xB3, 0x00, 0x00, 0x00, 0x00, 0x00   /* 0xF8-0xFF */

};  /* to_ascii */


/********************************************************************/
/* Use ASCII hex value as offset index for the array, the value of  */
/* the array is the same char in EBCDIC hex value                   */
/* the characters that we don't care for conversion is set to 0     */
/********************************************************************/
const char to_ebcdic[CONVERSION_TABLE_SIZE] = { 

/* 8 in a row; 32 rows; 8x32 = 256                                  */     

/********************************************************************/
/* The headings are the second possible digit: e.g. 08 means 0 or 8 */
/* second digit of index: odd rows are 0-7; even rows are 8-F       */
/********************************************************************/

    /* 08    19    2A   3B    4C    5D    6E    7F */  /* index     */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x00-0x07 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x08-0x0F */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x10-0x17 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x18-0x1F */

      0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,  /* 0x20-0x27 */
      0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,  /* 0x28-0x2F */

      0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,  /* 0x30-0x37 */
      0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,  /* 0x38-0x3F */

      0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,  /* 0x40-0x47 */
      0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,  /* 0x48-0x4F */

      0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,  /* 0x50-0x57 */
      0xE7, 0xE8, 0xE9, 0xBA, 0xE0, 0xBB, 0xB0, 0x6D,  /* 0x58-0x5F */

      0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,  /* 0x60-0x67 */
      0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,  /* 0x68-0x6F */

      0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,  /* 0x70-0x77 */
      0xA7, 0xA8, 0xA9, 0xC0, 0x4F, 0xD0, 0xA1, 0x00,  /* 0x78-0x7F */

     /* 08    19    2A   3B    4C    5D    6E    7F */ /* index     */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x80-0x87 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x88-0x8F */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x90-0x97 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0x98-0x9F */

      0x41, 0xAA, 0x00, 0x00, 0x00, 0xB2, 0x6A, 0x00,  /* 0xA0-0xA7 */
      0xBD, 0xB4, 0x00, 0x8A, 0x5F, 0xCA, 0xAF, 0xBC,  /* 0xA8-0xAF */

      0x90, 0x8F, 0xEA, 0xFA, 0xBE, 0xA0, 0x00, 0xB3,  /* 0xB0-0xB7 */
      0x00, 0xDA, 0x00, 0x8B, 0x00, 0x00, 0x00, 0xAB,  /* 0xB8-0xBF */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0xC0-0xC7 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0xC8-0xCF */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBF,  /* 0xD0-0xD7 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0xD8-0xDF */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x00,  /* 0xE0-0xE7 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* 0xE8-0xEF */

      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1,  /* 0xF0-0xF7 */
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   /* 0xF8-0xFF */

};  /* to_ebcdic */

#endif /* #ifdef _OS400_QSLPUTIL_C */

/********************************************************************/
/* Values passed to QMHSNDPM as part of sending a message.          */
/********************************************************************/
#define MSG_DIAG          "*DIAG     "
#define MSG_ESCAPE        "*ESCAPE   "
#define MSG_INFO          "*INFO     "
#define MSG_INQ           "*INQ      "
#define MSG_COMP          "*COMP     "
#define MSG_NOTIFY        "*NOTIFY   "
#define MSG_RQS           "*RQS      "
#define MSG_STATUS        "*STATUS   "
#define MSQ_Q_CUR_PROG    "*         "
#define MSQ_Q_PGM_BDY     "*PGMBDY   "
#define MSQ_Q_CTL_BDY     "*CTLBDY   "
#define MSQ_Q_EXTERN      "*EXT      "
#define MSQ_Q_SYSOPR      "*SYSOPR   "
#define QTCPMSG           "QTCPMSG   *LIBL     " /* Stack MSGF    */
#define QTCPMSGF          "QTCPMSGF  *LIBL     " /* Apps MSGF     */
#define QCPFMSG           "QCPFMSG   *LIBL     " /* Most CPFxxxx  */
#define QCEEMSG           "QCEEMSG   *LIBL     " /* CEE9901       */
#define QC2MSGF           "QC2MSGF   *LIBL     " /* ILE Signals   */
#define QSLPMSG           "QSLPMSG   *LIBL     " /* SLP MSGF      */
#define QTOBMSGF          "QTOBMSGF  *LIBL     " /* DNS MSGF      */

/************/
/* Typedefs */
/************/

/******************************************************************/
/* Substitution values for messages; used by IssueMsg             */
/******************************************************************/
typedef enum {
  SUB_STRING,      /* C-string (null-terminated array of char)    */
  SUB_SHORT,       /* short integer (16 bit, same as PL/MI BIN(2))*/
  SUB_LONG,        /* long integer (32 bit, same as PL/MI BIN(4)) */
  SUB_HEX,         /* memory block to dump in hex                 */
  SUB_VERBATIM,    /* already formatted substitution buffer       */
  SUB_VSTRING,     /* Variable length C-string (null-terminated)  */
  SUB_PROFILE      /* User profile receives as a non-program msg  */
} qslp_Substitute_t;

/******************************************************************/
/* Record structure for Error Code Parameter (from <qusec.h>)     */
/******************************************************************/
typedef struct Qus_EC {
      int  Bytes_Provided;
      int  Bytes_Available;
      char Exception_Id[7];
      char Reserved;
    /*char Exception_Data[];*/           /* Varying length        */
} Qus_EC_t;

typedef struct Qus_ERRC0200 {
      int Key;
      int Bytes_Provided;
      int Bytes_Available;
      char Exception_Id[7];
      char Reserved;
      int CCSID;
      int Offset_Exc_Data;
      int Length_Exc_Data;
    /*char *Reserved2;*/
    /*char Exception_Data[];*/           /* Varying Length        */
} Qus_ERRC0200_t;

/***********************/
/* Function prototypes */
/***********************/

/******************************************************************/
/* Convert an EBCDIC null-terminated string into ASCII            */
/******************************************************************/
void ntstr_to_ascii(char * );

/******************************************************************/
/* Convert an EBCDIC non null-terminated string into ASCII        */
/******************************************************************/
void str_to_ascii(char * , int );

/******************************************************************/
/* Convert a null_terminated ASCII string into EBCDIC             */
/******************************************************************/
void ntstr_to_ebcdic(char * );

/******************************************************************/
/* Convert a non null_terminated ASCII string into EBCDIC         */
/******************************************************************/
void str_to_ebcdic(char * , int );

/******************************************************************/
/* Issue ILE message with variable number of substitution values  */
/******************************************************************/
void qslp_IssueMsg(
         char *,            /* ptr to string with message id      */
         char *,            /* message type system value string   */
         char *,            /* destination (*EXT, *PGMBDY, etc.)  */
         int,               /* number of msg replacement values   */
         ...                /* optional sets of replacement
                                 value types and repl. values     */
                      );    /* none                               */

/******************************************************************/
/* Pad string with blank spaces                                   */
/******************************************************************/
char *qslp_PadCpy(
         char *,            /* ptr to destination string          */
         char *,            /* ptr to source string               */
         const int          /* desired length of dest. string     */
                );

/******************************************************************/
/* Create a pid file to store pid for kill()                      */
/******************************************************************/
void qslp_CreatePidFile(char *);

/******************************************************************/
/* Print out a buffer in both hex and ASCII text                  */
/******************************************************************/
void qslp_Buffer(char *Buffer, int iBuffLen);

#endif /* #ifndef _OS400_QSLPUTIL_H */
