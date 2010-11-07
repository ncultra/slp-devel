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

/*******************************************************************
 *  Description: vsnmp memory suballocator interface header file  
 *
 *****************************************************************************/



#ifndef NO_SUBALLOC
#ifndef TEST_HARNESS
#ifndef SUBALLOC_INCLUDE
#define SUBALLOC_INCLUDE

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */



#ifdef DEBUG_ALLOC
typedef struct _suballocHandle {
	void *(*de_init)(uint32);
	void *(*malloc)(size_t, uint32, int8 *f, int32 l);
	void *(*calloc)(size_t, size_t, uint32, int8 *f, int32 l);
	void *(*realloc)(void *, size_t, uint32, int8 *f, int32 l);
	int8 *(*strdup)(const int8 *, uint32, int8 *f, int32 l);
	int16 *(*wcsdup)(const int16 *, uint32, int8 *f, int32 l);
	BOOL (*unfreed)(uint32, int8 *f, int32 l);
	int8 logpath[256];
}SUBALLOC_HANDLE;

#else
typedef struct _suballocHandle {
	void (*de_init)(uint32);
	void *(*malloc)(size_t, uint32);
	void *(*calloc)(size_t, size_t, uint32);
	void *(*realloc)(void *, size_t, uint32);
	int8 *(*strdup)(const int8 *, uint32);
	int16 *(*wcsdup)(const int16 *, uint32);
	BOOL (*unfreed)(uint32);
}SUBALLOC_HANDLE;

#define malloc(x) vs_malloc((x), (uint32)sa)
#define calloc(x, y) vs_calloc((x), (y), (uint32)sa)
#define realloc(x, y) vs_realloc((x), (y), (uint32)sa)
#define strdup(x) vs_strdup((x), (uint32)sa)
#define wcsdup(x) vs_wcsdup((x), (uint32)sa)
#define _de_init() sa->de_init((int32)sa)
#endif			 

#define _unfreed() sa->unfreed((int32) sa)
#define free(x) vs_free(x)



#ifdef DEBUG_CONSOLE
#define DEBUG_OUT(s) vs_DebugWrite((s), __FILE__, __LINE__)
#else
#define DEBUG_OUT(s) ((void)(0))
#endif

#ifdef DEBUG_ALLOC
SUBALLOC_HANDLE *InitializeProcessHeap(int8 *f);
void _CheckNode(void *m);
void DeInitSubAllocator(uint32 handle);
void *vs_malloc(size_t size, uint32 handle, int8 *f, int32 l);
void *vs_calloc(size_t num, size_t size, uint32 handle, int8 *f, int32 l);
void *vs_realloc(void *pblock, size_t newsize, uint32 handle, int8 *f, int32 l);
int8 * vs_strdup(const int8 *string, uint32 handle, int8 *f, int32 l);
int16 * vs_wcsdup(const int16 *string, uint32 handle, int8 *f, int32 l);
BOOL _UnfreedNodes(int32 vector, int32 index, uint32 handle);

#else
SUBALLOC_HANDLE *InitializeProcessHeap(void);
void DeInitSubAllocator(uint32 handle);
void *vs_malloc(size_t size, uint32 handle);
void *vs_calloc(size_t num, size_t size, uint32 handle);
void *vs_realloc(void *pblock, size_t newsize, uint32 handle);
int8 * vs_strdup(const int8 *string, uint32 handle);
int16* vs_wcsdup(const int16 *string, uint32 handle);
BOOL _UnfreedNodes(int32 vector, int32 index, uint32 handle);
#endif

extern BOOL InitializeSubAllocator(void);
extern void vs_free(void *m);
extern void vs_DebugWrite(const int8 *string, int8 *f, int32 l);

#ifdef __cplusplus
}            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#endif /* SUBALLOC_INCLUDE */
#endif /* TEST_HARNESS */
#endif /*  NO_SUBALLOCATOR */
