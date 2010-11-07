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
/* Header File Name       : os400msg.h                               */
/*                                                                   */
/* Descriptive Name       : OS/400 message APIs for OS/400 PASE      */
/*                                                                   */
/*    5722-SS1                                                       */
/*    (C) Copyright IBM Corp. 2000, 2000                             */
/*    All rights reserved.                                           */
/*    US Government Users Restricted Rights -                        */
/*    Use, duplication or disclosure restricted                      */
/*    by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                   */
/* Licensed Materials-Property of IBM                                */
/*                                                                   */
/* Description            : Declares function prototypes and         */
/*                          related data structures to send          */
/*                          and receive OS/400 messages in an        */
/*                          OS/400 PASE program                      */
/*                                                                   */
/* Change Activity        :                                          */
/*                                                                   */
/* CFD List               :                                          */
/*                                                                   */
/* Flag Reason   Level   Date   Pgmr      Change Description         */
/* ---- -------- ------- ------ --------- -------------------------  */
/* $00  D97947   v5r1m0  000921 ROCH      New file                   */
/*                                                                   */
/* End CFD List.                                                     */
/*********************************************************************/

#ifndef _OS400MSG_H
#define _OS400MSG_H

#ifndef _H_STANDARDS
#include <standards.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Character string input arguments are null-terminated strings in
 *  the current OS/400 PASE CCSID.  The system converts input strings
 *  to the (EBCDIC) job default CCSID and pads with blanks (as needed)
 *  for use by OS/400 messaging interfaces.  An error (non-zero result)
 *  is returned if any input string is too long.  Character results
 *  that are not tagged with a specific CCSID are in the (EBCDIC)
 *  job default CCSID
 */

/*
 *  Send to a nonprogram message queue
 */
int
QMHSNDM(const char	*msgid,		/* Message identifier */
	const char	*msgf,		/* Qualified message file name */
	const void	*msgdata,	/* Message data or text */
	int		msgdataLen,	/* Length of message data or text */
	const char	*msgtype,	/* Message type */
	const char	*msgqList,	/* Qualified message queue list */
	int		msgqCount,	/* Number of message queues */
	const char	*rpyq,		/* Qualified reply message queue */
	int		*msgkey,	/* Message key */
	void		*errcode);	/* Error code */

int
QMHSNDM1(const char	*msgid,		/* Message identifier */
	 const char	*msgf,		/* Qualified message file name */
	 const void	*msgdata,	/* Message data or text */
	 int		msgdataLen,	/* Length of message data or text */
	 const char	*msgtype,	/* Message type */
	 const char	*msgqList,	/* Qualified message queue list */
	 int		msgqCount,	/* Number of message queues */
	 const char	*rpyq,		/* Qualified reply message queue */
	 int		*msgkey,	/* Message key */
	 void		*errcode,	/* Error code */
	 int		ccsid);		/* Message data CCSID */

/*
 *  Send to a program message queue
 */
int
QMHSNDPM(const char	*msgid,		/* Message identifier */
	 const char	*msgf,		/* Qualified message file name */
	 const void	*msgdata,	/* Message data or text */
	 int		msgdataLen,	/* Length of message data or text */
	 const char	*msgtype,	/* Message type */
	 const char	*pgmq,		/* Call stack entry */
	 int		pgmqDelta,	/* Call stack counter */
	 int		*msgkey,	/* Message key */
	 void		*errcode);	/* Error code */

int
QMHSNDPM1(const char	*msgid,		/* Message identifier */
	  const char	*msgf,		/* Qualified message file name */
	  const void	*msgdata,	/* Message data or text */
	  int		msgdataLen,	/* Length of message data or text */
	  const char	*msgtype,	/* Message type */
	  const char	*pgmq,		/* Call stack entry */
	  int		pgmqDelta,	/* Call stack counter */
	  int		*msgkey,	/* Message key */
	  void		*errcode,	/* Error code */
	  int		pgmqLen,	/* Length of call stack entry */
	  const char	*pgmqQual,	/* Call stack entry qualification */
	  int		extwait);	/* *EXT pgmq wait time */

int
QMHSNDPM2(const char	*msgid,		/* Message identifier */
	  const char	*msgf,		/* Qualified message file name */
	  const void	*msgdata,	/* Message data or text */
	  int		msgdataLen,	/* Length of message data or text */
	  const char	*msgtype,	/* Message type */
	  const void	*pgmq,		/* Call stack entry */
	  int		pgmqDelta,	/* Call stack counter */
	  int		*msgkey,	/* Message key */
	  void		*errcode,	/* Error code */
	  int		pgmqLen,	/* Length of call stack entry */
	  const char	*pgmqQual,	/* Call stack entry qualification */
	  int		extwait,	/* *EXT pgmq wait time */
	  const char	*pgmqType,	/* Call stack entry data type */
	  int		ccsid);		/* Message data CCSID */

/*
 *  Receive from a nonprogram message queue
 */
int
QMHRCVM(void		*msginfo,	/* Message information buffer */
	int		msginfoLen,	/* Length of message info buffer */
	const char	*format,	/* Format name */
	const char	*msgq,		/* Qualified message queue name */
	const char	*msgtype,	/* Message type */
	int		*msgkey,	/* Message key */
	int		wait,		/* Wait time */
	const char	*action,	/* Message action */
	void		*errcode);	/* Error code */

int
QMHRCVM1(void		*msginfo,	/* Message information buffer */
	 int		msginfoLen,	/* Length of message info buffer */
	 const char	*format,	/* Format name */
	 const char	*msgq,		/* Qualified message queue name */
	 const char	*msgtype,	/* Message type */
	 int		*msgkey,	/* Message key */
	 int		wait,		/* Wait time */
	 const char	*action,	/* Message action */
	 void		*errcode,	/* Error code */
	 int		ccsid);		/* Message data CCSID */

/*
 *  Receive from a program message queue
 */
int
QMHRCVPM(void		*msginfo,	/* Message information buffer */
	 int		msginfoLen,	/* Length of message info buffer */
	 const char	*format,	/* Format name */
	 const char	*pgmq,		/* Call stack entry */
	 int		pgmqDelta,	/* Call stack counter */
	 const char	*msgtype,	/* Message type */
	 int		*msgkey,	/* Message key */
	 int		wait,		/* Wait time */
	 const char	*action,	/* Message action */
	 void		*errcode);	/* Error code */
int
QMHRCVPM1(void		*msginfo,	/* Message information buffer */
	  int		msginfoLen,	/* Length of message info buffer */
	  const char	*format,	/* Format name */
	  const char	*pgmq,		/* Call stack entry */
	  int		pgmqDelta,	/* Call stack counter */
	  const char	*msgtype,	/* Message type */
	  int		*msgkey,	/* Message key */
	  int		wait,		/* Wait time */
	  const char	*action,	/* Message action */
	  void		*errcode,	/* Error code */
	  int		pgmqLen,	/* Length of call stack entry */
	  const char	*pgmqQual);	/* Call stack entry qualification */
int
QMHRCVPM2(void		*msginfo,	/* Message information buffer */
	  int		msginfoLen,	/* Length of message info buffer */
	  const char	*format,	/* Format name */
	  const void	*pgmq,		/* Call stack entry */
	  int		pgmqDelta,	/* Call stack counter */
	  const char	*msgtype,	/* Message type */
	  int		*msgkey,	/* Message key */
	  int		wait,		/* Wait time */
	  const char	*action,	/* Message action */
	  void		*errcode,	/* Error code */
	  int		pgmqLen,	/* Length of call stack entry */
	  const char	*pgmqQual,	/* Call stack entry qualification */
	  const char	*pgmqType,	/* Call stack entry data type */
	  int		ccsid);		/* Message data CCSID */

/*
 * NOTE: The following structure types only declare the fixed
 *       portion of each format.  Any varying length fields must
 *       be declared by the user
 */

/*
 *  Structures for formats RCVM0100 and RCVM0200 returned by
 *  QMHRCVM and QMHRCVM1
 */
#pragma options align=packed
typedef struct Qmh_Rcvm_RCVM0100
{
    int  Bytes_Returned;
    int  Bytes_Available;
    int  Message_Severity;
    char Message_Id[7];
    char Message_Type[2];
    char Message_Key[4];
    char Reserved[7];
    int  CCSID_Convert_Status;
    int  CCSID_Returned;
    int  Data_Returned;
    int  Data_Available;
     /*char Message_Data[]; */    /* Varying length */
} Qmh_Rcvm_RCVM0100_t;
#pragma options align=reset

#pragma options align=packed
typedef struct Qmh_Rcvm_RCVM0200
{
    int  Bytes_Returned;
    int  Bytes_Available;
    int  Message_Severity;
    char Message_Id[7];
    char Message_Type[2];
    char Message_Key[4];
    char Message_File_Name[10];
    char Message_File_Library[10];
    char Message_Library_Used[10];
    char Send_Job[10];
    char Send_User_Profile[10];
    char Send_Job_Number[6];
    char Send_Program_Name[12];
    char Reserved1[4];
    char Send_Date[7];
    char Send_Time[6];
    char Reserved2[17];
    int  Text_CCSID_Convert_Status;
    int  Data_CCSID_Convert_Status;
    char Alert_Option[9];
    int  Text_CCSID_Returned;
    int  Data_CCSID_Returned;
    int  Length_Data_Returned;
    int  Length_Data_Available;
    int  Length_Message_Returned;
    int  Length_Message_Available;
    int  Length_Help_Returned;
    int  Length_Help_Available;
     /*char Message_Text[]; */	/* Varying length */
     /*char Message[]; */		/* Varying length */
     /*char Message_Help[]; */	/* Varying length */
} Qmh_Rcvm_RCVM0200_t;
#pragma options align=reset

/*
 *  Structures for formats RCVM0100, RCVM0200, and RCVM0300
 *  returned by QMHRCVPM, QMHRCVPM1, and QMHRCVPM2
 */
#pragma options align=packed
typedef struct Qmh_Rcvpm_RCVM0100
{
    int  Bytes_Returned;
    int  Bytes_Available;
    int  Message_Severity;
    char Message_Id[7];
    char Message_Type[2];
    char Message_Key[4];
    char Reserved[7];
    int  CCSID_Convert_Status;
    int  CCSID_Returned;
    int  Data_Returned;
    int  Data_Available;
     /*char Message_Data[]; */	/* Varying length */
} Qmh_Rcvpm_RCVM0100_t;
#pragma options align=reset

#pragma options align=packed
typedef struct Qmh_Rcvpm_RCVM0200
{
    int  Bytes_Returned;
    int  Bytes_Available;
    int  Message_Severity;
    char Message_Id[7];
    char Message_Type[2];
    char Message_Key[4];
    char Message_File_Name[10];
    char Message_File_Library[10];
    char Message_Library_Used[10];
    char Send_Job[10];
    char Send_User_Profile[10];
    char Send_Job_Number[6];
    char Send_Program_Name[12];
    char Send_Instruction_Num[4];
    char Send_Date[7];
    char Send_Time[6];
    char Receive_Program_Name[10];
    char Receive_Instruction_Num[4];
    char Send_Type[1];
    char Receive_Type[1];
    char Reserved[1];
    int  Text_CCSID_Convert_Status;
    int  Data_CCSID_Convert_Status;
    char Alert_Option[9];
    int  Text_CCSID_Returned;
    int  Data_CCSID_Returned;
    int  Length_Data_Returned;
    int  Length_Data_Available;
    int  Length_Message_Returned;
    int  Length_Message_Available;
    int  Length_Help_Returned;
    int  Length_Help_Available;
     /*char Message_Text[]; */	/* Varying length */
     /*char Message[]; */		/* Varying length */
     /*char Message_Help[]; */	/* Varying length */
} Qmh_Rcvpm_RCVM0200_t;
#pragma options align=reset

#pragma options align=packed
typedef struct Qmh_Rcvpm_RCVM0300
{
    int  Bytes_Returned;
    int  Bytes_Available;
    int  Message_Severity;
    char Message_Id[7];
    char Message_Type[2];
    char Message_Key[4];
    char Message_File_Name[10];
    char Message_File_Library[10];
    char Message_Library_Used[10];
    char Alert_Option[9];
    int  Text_CCSID_Convert_Status;
    int  Data_CCSID_Convert_Status;
    int  Text_CCSID_Returned;
    int  Data_CCSID_Returned;
    int  Length_Data_Returned;
    int  Length_Data_Available;
    int  Length_Message_Returned;
    int  Length_Message_Available;
    int  Length_Help_Returned;
    int  Length_Help_Available;
    int  Length_Send_Returned;
    int  Length_Send_Available;
     /*char Message_Text[]; */	/* Varying length */
     /*char Message[]; */		/* Varying length */
     /*char Message_Help[]; */	/* Varying length */
     /*Qmh_Rcvpm_Sender_t Send_Information; */	/* Varying length */
} Qmh_Rcvpm_RCVM0300_t;
#pragma options align=reset

/*
 *  Structure for sender information returned in format RCVM0300
 *  returned by QMHRCVPM, QMHRCVPM1, and QMHRCVPM2
 */
#pragma options align=packed
typedef struct Qmh_Rcvpm_Sender
{
    char Send_Job[10];
    char Send_User_Profile[10];
    char Send_Job_Number[6];
    char Send_Date[7];
    char Send_Time[6];
    char Send_Type[1];
    char Receive_Type[1];
    char Send_Program_Name[12];
    char Send_Module_Name[10];
    char Send_Procedure_Name[256];
    char Reserved[1];
    int  Send_Statements_Available;
    char Send_Statements[30];
    char Receive_Program_Name[10];
    char Receive_Module_Name[10];
    char Receive_Procedure_Name[256];
    char Reserved2[10];
    int  Receive_Statements_Available;
    char Receive_Statements[30];
    char Reserved3[2];
    int  Displacement_Long_Snd_Pgm;
    int  Length_Long_Snd_Pgm;
    int  Displacement_Long_Snd_Proc;
    int  Length_Long_Snd_Proc;
    int  Displacement_Long_Rcv_Proc;
    int  Length_Long_Rcv_Proc;
     /*char Reserved4[]; */		/* Varying length */
     /*char Long_Snd_Pgm[]; */	/* Varying length */
     /*char Long_Snd_Proc[]; */	/* Varying length */
     /*char Long_Rcv_Proc[]; */	/* Varying length */
} Qmh_Rcvpm_Sender_t;
#pragma options align=reset

#ifdef __cplusplus
}
#endif

#endif /* _OS400MSG_H */
