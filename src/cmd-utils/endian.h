
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
