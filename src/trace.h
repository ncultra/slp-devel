
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

#ifndef __LSLP_TRACE_H
#define __LSLP_TRACE_H


#ifdef TRACE_ENABLED 

 #ifdef _WIN32
  #define SLP_TRACE printf("\nSLP: (file=%s, line=%d) ", __FILE__, __LINE__); \
                        printf
 #else
  #define SLP_TRACE(fmt, va...) printf("SLP: (file=%s, line=%d) " fmt "\n", \
     __FILE__, __LINE__, ## va )

 #endif // win32

#else 
 #ifdef _WIN32
   #define SLP_TRACE
 #else
   #define SLP_TRACE(fmt, va...) ((void)0)
#endif
#endif // trace enabled

#endif // defined



