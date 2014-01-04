/***************
 * MODUL:         ILBM-handler definitions
 * NAME:          ilbm.h
 * DESCRIPTION:   In this module are all the definitions to read and write
 *		ILBM-files
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		12.11.95	ah		initial release
 ***************/

#ifndef ILBM_H
#define ILBM_H

#ifndef PICLOAD_H
#include "picload.h"
#endif

// ILBM-section
// IFF types we may encounter
#define	ID_ILBM		MAKE_ID('I','L','B','M')
// ILBM Chunk ID's we may encounter
#define	ID_BMHD		MAKE_ID('B','M','H','D')
#define	ID_CMAP		MAKE_ID('C','M','A','P')
#define	ID_CRNG		MAKE_ID('C','R','N','G')
#define	ID_CCRT		MAKE_ID('C','C','R','T')
#define	ID_GRAB		MAKE_ID('G','R','A','B')
#define	ID_SPRT		MAKE_ID('S','P','R','T')
#define	ID_DEST		MAKE_ID('D','E','S','T')
#define	ID_CAMG		MAKE_ID('C','A','M','G')
#define	ID_BODY		MAKE_ID('B','O','D','Y')

/* Convert image width to even number of BytesPerRow for ILBM save.
 * Do NOT use this macro to determine the actual number of bytes per row
 * in an Amiga BitMap.  Use BitMap->BytesPerRow for scan-line modulo.
 * Use your screen or viewport width to determine width. Or under
 * V39, use GetBitMapAttr().
 */
#define RowBytes(w)	((((w) + 15) >> 4) << 1)
#define RowBits(w)	((((w) + 15) >> 4) << 4)

/*  Masking techniques  */
#define	mskNone			0
#define	mskHasMask		1
#define	mskHasTransparentColor	2
#define	mskLasso		3

/*  Compression techniques  */
#define	cmpNone			0
#define	cmpByteRun1		1

/* ---------- BitMapHeader ---------------------------------------------*/
/*  Required Bitmap header (BMHD) structure describes an ILBM */
typedef struct
{
	UWORD	w, h;				/* Width, height in pixels */
	WORD	x, y;				/* x, y position for this bitmap  */
	UBYTE	nPlanes;			/* # of planes (not including mask) */
	UBYTE	masking;			/* a masking technique listed above */
	UBYTE	compression;	/* cmpNone or cmpByteRun1 */
	UBYTE	flags;			/* as defined or approved by Commodore */
	UWORD	transparentColor;
	UBYTE	xAspect, yAspect;
	WORD	pageWidth, pageHeight;
} BitMapHeader;

#endif // ILBM_H
