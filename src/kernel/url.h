
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




#ifndef _URLDEFS_INCLUDE
#define _URLDEFS_INCLUDE
#ifdef	__cplusplus
extern "C" {
#endif
#ifndef __linux__
#pragma pack( push, lslp_defs )
#pragma pack(1)
#endif

typedef struct lslp_atom_list {
	struct lslp_atom_list *next;
	struct lslp_atom_list *prev;
	BOOL isHead;
	uint8 *str;
	uint32 hash;
}lslpAtomList;

typedef struct lslp_atomized_url {
	struct lslp_atomized_url *next;
	struct lslp_atomized_url *prev;
	BOOL isHead;
	uint8 *url;
	uint32 urlHash;
	lslpAtomList srvcs;
	lslpAtomList site;
	lslpAtomList path;
	lslpAtomList attrs;
}lslpAtomizedURL;


  /*prototypes */
  lslpAtomList *lslpAllocAtom(void);
  lslpAtomList *lslpAllocAtomList(void);
  void lslpFreeAtomList(lslpAtomList *l, int32 flag);
  void lslpFreeAtomList(lslpAtomList *l, int32 flag);
  lslpAtomizedURL *lslpAllocAtomizedURL(void);
  lslpAtomizedURL *lslpAllocAtomizedURLList(void);
  void lslpFreeAtomizedURL(lslpAtomizedURL *u);
  void lslpFreeAtomizedURLList(lslpAtomizedURL *l, int32 flag);
  void lslpInitAtomLists(void);
  void lslpInitURLList(void);
   
  lslpAtomizedURL *_lslpDecodeURLs(int8 *u[], int32 count);

#ifndef __linux__
#pragma pack( pop, lslp_defs )
#endif
#ifdef	__cplusplus
}
#endif

#endif /* _LSLPDEFS_INCLUDE */

