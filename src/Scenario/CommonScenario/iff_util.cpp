/***************
 * PROGRAM:       Modeler
 * NAME:          iff_util.cpp
 * DESCRIPTION:   Functions which are useful for reading and writing iff-files
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    31.05.96 ah    initial release
 ***************/

#ifdef __AMIGA__
	#include <pragma/dos_lib.h>
#endif

#ifndef IFF_UTIL_H
#include "iff_util.h"
#endif

/*************
 * FUNCTION:      WriteChunk
 * DESCRIPTION:   writes a chunk of data to an iff file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL WriteChunk(struct IFFHandle *iff, LONG id, APTR data, LONG size)
{
	LONG error;

	error = PushChunk(iff, 0, id, size);
	if(error)
		return FALSE;

	if(WriteChunkBytes(iff, data, size) < 0)
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * FUNCTION:      WriteLongChunk
 * DESCRIPTION:   writes a chunk of LONG data to an iff file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL WriteLongChunk(struct IFFHandle *iff, LONG id, APTR data, int cnt)
{
	LONG error;

	error = PushChunk(iff, 0, id, cnt * sizeof(ULONG));
	if(error)
		return FALSE;
#ifdef __SWAP__
	int i;
	for(i = 0; i < cnt; i++)
		((ULONG*)data)[i] = Swap(((ULONG*)data)[i]);
#endif
	if(WriteChunkBytes(iff, data, cnt * sizeof(ULONG)) < 0)
		return FALSE;
#ifdef __SWAP__
	for(i = 0; i < cnt; i++)  // swap back again to keep the data unchanged
		((ULONG*)data)[i] = Swap(((ULONG*)data)[i]);
#endif

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * FUNCTION:      WriteWordChunk
 * DESCRIPTION:   writes a chunk of WORD data to an iff file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL WriteWordChunk(struct IFFHandle *iff, LONG id, APTR data, int cnt)
{
	LONG error;

	error = PushChunk(iff, 0, id, cnt * sizeof(UWORD));
	if(error)
		return FALSE;
#ifdef __SWAP__
	int i;
	for(i = 0; i < cnt; i++)
		((UWORD*)data)[i] = SwapW(((UWORD*)data)[i]);
#endif
	if(WriteChunkBytes(iff, data, cnt * sizeof(UWORD)) < 0)
		return FALSE;
#ifdef __SWAP__
	for(i = 0; i < cnt; i++)  // swap back again to keep the data unchanged
		((UWORD*)data)[i] = SwapW(((UWORD*)data)[i]);
#endif

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * FUNCTION:      ReadInt
 * DESCRIPTION:   Read ints from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      number of ints
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
BOOL ReadInt(IFFHandle *h, int *buf, int cnt)
{
#ifdef __SWAP__
	BOOL res;
	int  i;

	res = (ReadChunkBytes(h, buf, sizeof(int) * cnt) == sizeof(int) * cnt);
	for(i = 0; i < cnt; i++)
		buf[i] = Swap(buf[i]);
	return res;
#else
	return (ReadChunkBytes(h, buf, sizeof(int) * cnt) == sizeof(int) * cnt);
#endif
}

/*************
 * FUNCTION:      ReadULONG
 * DESCRIPTION:   Read ULONGs from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      number of ULONGs
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
BOOL ReadULONG(IFFHandle *h, ULONG *buf, int cnt)
{
#ifdef __SWAP__
	BOOL res;
	int  i;

	res = (ReadChunkBytes(h, buf, sizeof(ULONG) * cnt) == sizeof(ULONG) * cnt);
	for(i = 0; i < cnt; i++)
		buf[i] = Swap(buf[i]);
	return res;
#else
	return (ReadChunkBytes(h, buf, sizeof(ULONG) * cnt) == sizeof(ULONG) * cnt);
#endif
}

/*************
 * FUNCTION:      ReadWord
 * DESCRIPTION:   Read words from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      number of words
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
BOOL ReadWord(IFFHandle *h, WORD *buf, int cnt)
{
#ifdef __SWAP__
	BOOL res;
	int  i;

	res = (ReadChunkBytes(h, buf, sizeof(WORD) * cnt) == sizeof(WORD) * cnt);
	for(i = 0; i < cnt; i++)
		buf[i] = SwapW(buf[i]);
	return res;
#else
	return (ReadChunkBytes(h, buf, sizeof(WORD) * cnt) == sizeof(WORD) * cnt);
#endif
}

/*************
 * FUNCTION:      ReadString
 * DESCRIPTION:   Read String from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      maximum string length
 * OUTPUT:        amount of read bytes
 *************/
int ReadString(IFFHandle *h, char *buf, int cnt)
{
	return ReadChunkBytes(h, buf, 256);
}

/*************
 * FUNCTION:      ReadFloat
 * DESCRIPTION:   Read floats from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      number of floats
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
BOOL ReadFloat(IFFHandle *h, float *buf, int cnt)
{
#ifdef __SWAP__
	BOOL res;
	int  i;
	ULONG *b;

	b = (ULONG*)buf;
	res = (ReadChunkBytes(h, b, sizeof(float) * cnt) == sizeof(float) * cnt);
	for (i = 0; i < cnt; i++)
		b[i] = Swap(b[i]);
	buf = (float*)b;
	return res;
#else
	return (ReadChunkBytes(h, buf, sizeof(float) * cnt) == sizeof(float) * cnt);
#endif
}

/*************
 * FUNCTION:      ReadUBYTE
 * DESCRIPTION:   Read UBYTEs from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      number of UBYTEs
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
BOOL ReadUBYTE(IFFHandle *h, UBYTE *buf, int cnt)
{
	return (ReadChunkBytes(h, buf, sizeof(UBYTE) * cnt) == sizeof(UBYTE) * cnt);
}

/*************
 * FUNCTION:      IFFCleanup
 * DESCRIPTION:   Close and cleanup iff-file
 * INPUT:         iff      file handle
 * OUTPUT:        -
 *************/
void IFFCleanup(struct IFFHandle *iff)
{
	CloseIFF(iff);
	if(iff->iff_Stream)
		Close(iff->iff_Stream);
	FreeIFF(iff);
}

