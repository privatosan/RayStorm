/**********************************************************
 * MODUL:         IFF-parser.h
 * NAME:          iffparse.h
 * VERSION:       0.95 10.12.1995
 * DESCRIPTION:	iffparse.h defines types for iffparse.cpp
 * AUTHORS:			Mike Hesser
 * BUGS:				none
 * TO DO:
 * HISTORY:
 *						DATE		NAME	COMMENT
 *						25.06.95	mh		first release
 *						13.11.95	mh		directory slash
 *********************************************************/

#ifndef IFF_IFFPARSE_H
#define IFF_IFFPARSE_H

#include <stdio.h>

#ifndef TYPEDEFS_H
	#include "typedefs.h"
#endif

#ifndef FILEUTIL_H
	#include "fileutil.h"
#endif

/*
	Structure associated with an active IFF stream.
	"iff_Stream" is a value used by the client's read/write/seek functions -
	it will not be accessed by the library itself and can have any value
	(could even be a pointer or a BPTR).
  
	This structure can only be allocated by iffparse.library
*/

struct IFFHandle
{
    FILE  *iff_Stream;
    ULONG iff_Flags;
    LONG  iff_Depth;	//  Depth of context stack 
};


/*
	A node associated with a context on the iff_Stack. Each node
	represents a chunk, the stack representing the current nesting
	of chunks in the open IFF file. Each context node has associated
	local context items in the (private) LocalItems list.  The ID, type,
	size and scan values describe the chunk associated with this node.
  
	This structure can only be allocated by iffparse.library
*/
struct ContextNode
{
	LONG cn_ID;
	LONG cn_Type;
	LONG cn_Size;	/*  Size of this chunk		   */
	LONG cn_Scan;	/*  # of bytes read/written so far */
};


struct StoredProperty
{
	LONG sp_Size;
	APTR sp_Data;
};

/*
	this structure is needed by ParseIFF in order to know when to stop
*/
struct ChunkNode
{
	LONG cn_Size;		// note that size & data have the same position as in StoredProperty
	APTR cn_Data;		// this is necessary for typecast (dirty but it works)
	struct ChunkNode  *cn_Node;	 	
	LONG cn_Stop;		// how to stop (Prop/Stop/StopOnExit)			   	
	LONG cn_ID;
	LONG cn_Type;
};	


/*
	this structure records the current nesting
*/
struct PushPopNode
{
	struct	PushPopNode *Node;
	LONG		Filepos;
	LONG		ID;
	LONG		Type;
	LONG		Length;
	LONG		CLength;	// current length of chunk
};


// bit masks for "iff_Flags" field
#define IFFF_READ			0L			 				// read mode - default
#define IFFF_WRITE		1L			 				// write mode		   
#define IFFF_RWBITS		(IFFF_READ | IFFF_WRITE) 	// read/write bits
#define IFFF_FSEEK		(1L<<1)		 				// forward seek only	   
#define IFFF_RSEEK		(1L<<2)		 				// random seek	  
#define IFFF_RESERVED	0xFFFF0000L		 			// Don't touch these bits


/*
   IFF return codes. Most functions return either zero for success or
   one of these codes. The exceptions are the read/write functions which
   return positive values for number of bytes or records read or written,
   or a negative error code. Some of these codes are not errors per sae,
   but valid conditions such as EOF or EOC (End of Chunk).
*/

#define IFFERR_EOF			-1L		// Reached logical end of file	
#define IFFERR_EOC			-2L		// About to leave context	
#define IFFERR_NOSCOPE		-3L		// No valid scope for property	
#define IFFERR_NOMEM			-4L		// Internal memory alloc failed 
#define IFFERR_READ			-5L		// Stream read error		
#define IFFERR_WRITE			-6L		// Stream write error	
#define IFFERR_SEEK			-7L		// Stream seek error		
#define IFFERR_MANGLED		-8L		// Data in file is corrupt	
#define IFFERR_SYNTAX		-9L		// IFF syntax error		
#define IFFERR_NOTIFF		-10L	// Not an IFF file		
#define IFFERR_NOHOOK		-11L	// No call-back hook provided	
#define IFF_RETURN2CLIENT	-12L	// Client handler normal return 

// Control modes for ParseIFF() function
#define IFFPARSE_SCAN		0L
#define IFFPARSE_STEP		1L
#define IFFPARSE_RAWSTEP	2L

//*****************************************************************************

#ifdef __SWAP__
	#define MAKE_ID(a,b,c,d)	\
		((ULONG) (d)<<24 | (ULONG) (c)<<16 | (ULONG) (b)<<8 | (ULONG) (a))
#else
	#define MAKE_ID(d,c,b,a)	\
		((ULONG) (d)<<24 | (ULONG) (c)<<16 | (ULONG) (b)<<8 | (ULONG) (a))
#endif

// Universal IFF identifiers
#define ID_FORM	MAKE_ID('F','O','R','M')
#define ID_LIST	MAKE_ID('L','I','S','T')
#define ID_CAT		MAKE_ID('C','A','T',' ')
#define ID_PROP	MAKE_ID('P','R','O','P')
#define ID_NULL	MAKE_ID(' ',' ',' ',' ')	

#define TAG_DONE  		NULL
#define MODE_OLDFILE 	1005
#define MODE_NEWFILE 	1006
#define IFFSIZE_UNKNOWN 0

#define CHNK_PROP		1L
#define CHNK_STOP		2L
#define CHNK_STOPONEXIT	3L

// function prototypes
#ifdef __cplusplus
extern "C"
{
#endif
void 				InitIFFasDOS(struct IFFHandle *);
FILE				*Open_(char *, LONG);
void				Close(FILE *);
struct IFFHandle 	*AllocIFF();
LONG 				OpenIFF(struct IFFHandle *, LONG);
LONG 				ParseIFF(struct IFFHandle *, LONG);
LONG 				ReadChunkBytes(struct IFFHandle *, void *, LONG);
struct ContextNode 	*CurrentChunk(struct IFFHandle *);
void 				CloseIFF(struct IFFHandle *);
void 				FreeIFF(struct IFFHandle *);
LONG				PropChunk(struct IFFHandle *, long, long);
LONG 				PropChunks(struct IFFHandle *, LONG *, long);
LONG 				StopChunk(struct IFFHandle *, long, long);
LONG 				StopOnExit(struct IFFHandle *, long, long);
struct StoredProperty *FindProp(struct IFFHandle *, long, long);
LONG 				WriteChunkBytes(struct IFFHandle *, APTR, long);
LONG 				PushChunk(struct IFFHandle *, long, long, long);
LONG 				PopChunk(struct IFFHandle *);
#ifdef __cplusplus
}
#endif

#endif // IFFPARSE_H
