/*************
* MODULE:		IFF-parser
* NAME:			iffparse.c
* VERSION:		1.2 12.3.1997
* DESCRIPTION:	iffparse emulates the functions from the "iffparse.library"
*					from Commodore Amiga
*					This module contains methods to read and write iff-files.
*					This library is a light version of the iffparse.library from Commodore.
*					Not all of the original function have been implemented and it won't
*					always behave the same as the original.
* AUTHORS:		Mike Hesser
* LITERATURE:	AMIGA ROM Kernel Reference Manual LIBRARIES Addison Wesley, Page 777
* BUGS:			no known bugs
* TO DO:			-	
* HISTORY:
*		DATE		NAME	COMMENT
*		25.06.95	mh		first release
*							IFFHandle, AllocIFF, OpenIFF, ParseIFF,
*							ReadChunkBytes, CloseIFF, FreeIFF, Swap
*		29.06.95	mh		bugfixes in ParseIFF
*		26.07.95	mh		bugfixes in CloseIFF, ParseIFF
*		28.07.95	mh		began PushChunk, WriteChunkBytes
*		30.07.95	mh		AddChunk, PopChunk
*		31.07.95	mh		chunks can be written with user defined size
*							comments
*		03.09.95	mh		Open returns NULL if file does not exist
*		18.09.95	mh		bugfix with Open (MODE_NEWFILE)
*		10.12.95	mh		bugfix with PushChunk (FORM ILBM),
*		04.04.96	mh		bugfix with PopChunk (adding one pad byte)
*		09.06.96	mh		cpp -> c
*		30.06.96	mh		bugfix: OpenIFF didn't return FALSE when file could not be created
*		08.07.96 mh		return IFFERR_EOC when leaving context
*		09.07.96 mh		bugfix: written chunksize was too long, bugfix: no pad byte when not IFFSIZE_UNKNOWN
*		11.07.96 mh		bugfix: feof didn't work properly, cn_Type is returned
*		19.09.96 mh		bugfix: CurrentChunk returns null when end of file is reached
*		25.09.96	mh		now Open_ returns NULL if file can't be open for writing (i.e. is uses by another program)
*		12.03.97 mh		bugfix: cn.cn_Size must be set to zero when leaving context
*		14.03.97 mh		additional test if read was successful (ReadChunkBytes)
*		07.06.97 mh		read past the end of the chunk are truncated now
*		15.09.97	mh		set buffer to NULL on cleanup
************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iffparse.h"

static struct	ContextNode cn;
static char		*buffer;
static LONG		position;
static struct	ChunkNode *chunkroot;
static struct	PushPopNode *pushpoproot;
static char		filename[256];

// prototypes
LONG PopChunk(struct IFFHandle *iff);

/*************
* FUNCTION:		InitIFFasDOS
* DESCRIPTION:	dummy procedure
* INPUT:			iff  handle structure
* OUTPUT:		-
*************/
void InitIFFasDOS(struct IFFHandle *iff)
{
}

/*************
* FUNCTION:		Open
* DESCRIPTION:	much of a dummy function, but tests if file exists
* INPUT:			name  name of file
*					mode  access mode
* OUTPUT:		file handle
*************/
FILE *Open_(char *name, LONG mode)
{
	FILE *exists;
	
	strcpy(filename, name);			// open stream in OpenIFF, because read or write is not determined
	if (mode != MODE_NEWFILE)
		exists = fopen(name, "rb");
	else
		exists = fopen(name, "wb");

	if (exists)
	{
		fclose(exists);
		return (FILE *)1;
	}
	else
		return NULL;
}

/*************
* FUNCTION:		Close
* DESCRIPTION:	close iff stream
* INPUT:			file	file handle
* OUTPUT:		-
*************/
void Close(FILE *file)
{
	fclose(file);
}

/***************
* FUNCTION:		AllocIFF
* DESCRIPTION:	Allocates and initializes a new IFFHandle
*					structure. This function is the only supported
*					way to create an IFFHandle structure since
*					there are private fields that need to be initialized.
* INPUT:			-
* OUTPUT:		IFFHandle
***************/
struct IFFHandle *AllocIFF()
{
	struct IFFHandle *iff;

	iff = (struct IFFHandle *)malloc(sizeof(struct IFFHandle));
	if (!iff)
		return NULL;

	iff->iff_Flags = 0;
	iff->iff_Depth = 0;
	cn.cn_Type = ID_NULL;
	cn.cn_ID = ID_NULL;
	cn.cn_Size = 0;
	cn.cn_Scan = 0;

	return iff;
}

/*************
* FUNCTION:		OpenIFF
* DESCRIPTION:	Initializes an IFFHandle structure for a new read or write. The
*					direction of the I/O is given by the value of rwMode, which can be
*					either IFFF_READ or IFFF_WRITE.
* INPUT:			iff		IFF-Handle
*					mode	access mode
* OUTPUT:		0L if ok else error number
*************/
LONG OpenIFF(struct IFFHandle *iff, LONG mode)
{
	if (mode == IFFF_WRITE)
		iff->iff_Stream = fopen(filename, "wb");
	else
		iff->iff_Stream = fopen(filename, "rb");

	if (!iff->iff_Stream)
		return 1L;

	iff->iff_Flags = mode;
	iff->iff_Depth = 0;
	buffer = NULL;
	position = 0;
	chunkroot = NULL;
	pushpoproot = NULL;

	return 0L;
}

/*************
* FUNCTION:		CloseIFF
* DESCRIPTION:	Completes an IFF read or write operation by closing the IFF context
*					established for this IFFHandle structure. The IFFHandle structure
*					itself is left ready for re-use and a new context can be opened with
*					OpenIFF(). This function can be used for cleanup if a read or write
*					fails partway through.
*					This operation is NOT permitted to fail;  any error code returned
*					will be ignored (best to return 0, though). DO NOT write to this
*					structure.
* INPUT:			iff		IFF Handle
* OUTPUT:		-
*************/
void CloseIFF(struct IFFHandle *iff)
{
	struct ChunkNode *p;
	if (buffer)
	{
		free(buffer);
		buffer = NULL;
	}	
	// free list
	p = chunkroot;
	while (p)
	{
		p = chunkroot->cn_Node;
		if (p && p->cn_Data)
			free(p->cn_Data);	// free buffer
		free(chunkroot);
		chunkroot = p;
	}
	while (!PopChunk(iff));	// close all chunks
}

/*************
* FUNCTION:		FreeIFF
* DESCRIPTION:	Deallocates all resources associated with
*					this IFFHandle structure.
*					The structure MUST have already been closed
*					by CloseIFF().
*					but tests if file exists
* INPUT:			iff		IFF-Handle
* OUTPUT:		-
*************/
void FreeIFF(struct IFFHandle *iff)
{
	if (iff)
		free(iff);
}

/*************
* FUNCTION:		AddChunk
* DESCRIPTION:	AddChunk is called from PropChunk, StopChunk & StopOnExit.
*					It adds a new description of a chunk to a simple
*					linked list.
* INPUT:			iff		IFF-handle
*					type		type of chunk (i.e. ILBM)
*					id			chunk name (i.e. BODY)
*					stop		stop mode
* OUTPUT:      0L if ok else error number
*************/
LONG AddChunk(struct IFFHandle *iff, long type, long id, long stop)
{
	struct ChunkNode *p;

	p = (struct ChunkNode *)malloc(sizeof(struct ChunkNode));
	if (!p)
		return IFFERR_NOMEM;
	p->cn_Node = chunkroot;
	p->cn_Stop = stop;
	p->cn_ID   = id;
	p->cn_Type = type;
	p->cn_Size = 0;
	p->cn_Data = NULL;
	chunkroot = p;

	return 0L;
}	

/*************
* FUNCTION:		PropChunk
* DESCRIPTION:	a prop chunk will be loaded completely into
*					memory by ParseIFF
* INPUT:			iff		IFF-handle
*					type	type of chunk (i.e. ILBM)
*					id		chunk name (i.e. BODY)
* OUTPUT:		0L if ok else error number
*************/
LONG PropChunk(struct IFFHandle *iff, long type, long id)
{
	return AddChunk(iff, type, id, CHNK_PROP);
}

/*************
* FUNCTION:		PropChunks
* DESCRIPTION:	With this function you can set many prop chunks at once
* INPUT:			iff			IFF-handle
*					propArray	array of chunk ids
*					pairs			number of chunk ids
* OUTPUT:		0L if ok else error number
*************/
LONG PropChunks(struct IFFHandle *iff, LONG *propArray, long pairs)
{
	int i;
	
	for (i = 0; i < pairs; i++)
		PropChunk(iff, propArray[i<<1], propArray[(i<<1) + 1]);
	return 0L;
}

/*************
* FUNCTION:		StopChunk
* DESCRIPTION:	ParseIFF will stop at this type of chunk
* INPUT:			iff		IFF-handle
*					type		type of chunk (i.e. ILBM)
*					id			chunk name (i.e. BODY)
* OUTPUT:		0L if ok
*************/
LONG StopChunk(struct IFFHandle *iff, long type, long id)
{
	return AddChunk(iff, type, id, CHNK_STOP);
}

/*************
* FUNCTION:		StopOnExit
* DESCRIPTION:	ParseIFF stops when leaving a chunk of this type
* INPUT:			iff		IFF-handle
*					type		type of chunk (i.e. ILBM)
*					id			chunk name (i.e. BODY)
* OUTPUT:		0L if ok else error number
*************/
LONG StopOnExit(struct IFFHandle *iff, long type, long id)
{
	return AddChunk(iff, type, id, CHNK_STOPONEXIT);
}

/*************
* FUNCTION:		FindProp
* DESCRIPTION:	FindProp searches for a certain chunk and returns
*					a pointer to a StoredProperty structure which keeps the
*					relevant data of chunk
* INPUT:			iff		IFF-handle
*					type		type of chunk (i.e. ILBM)
*					id			chunk name (i.e. BODY)
* OUTPUT:		pointer to StoredProperty
*************/
struct StoredProperty *FindProp(struct IFFHandle *iff, long type, long id )
{
	struct ChunkNode *p = chunkroot;
	
	while (p && (p->cn_ID != id || p->cn_Type != type))
		p = p->cn_Node;
		
	// note the typecast
	return (struct StoredProperty *)p;  // NULL if chunk wasn't found
}

/*************
* FUNCTION:		ParseIFF
* DESCRIPTION:	see below
* INPUT:			iff		IFF-handle
*					mode	access mode
* OUTPUT:		0L if ok else error number
*************/
/*
	ParseIFF()

	This is the biggie.

	Traverses a file opened for read by pushing chunks onto the context
	stack and popping them off directed by the generic syntax of IFF
	files. As it pushes each new chunk, it searches the context stack
	for handlers to apply to chunks of that type. If it finds an entry
	handler it will invoke it just after entering the chunk. If it finds
	an exit handler it will invoke it just before leaving the chunk.
	Standard handlers include entry handlers for pre-declared
	property chunks and collection chunks and entry and exit handlers for
	for stop chunks - that is, chunks which will cause the ParseIFF()
	function to return control to the client. Client programs can also
	provide their own custom handlers.

	The control flag can have three values:

	IFFPARSE_SCAN:
		In this normal mode, ParseIFF() will only return control to
		the caller when either:
			1) an error is encountered,
			2) a stop chunk is encountered, or a user handler
			   returns the special IFF_RETURN2CLIENT code, or
			3) the end of the logical file is reached, in which
			   case IFFERR_EOF is returned.

		ParseIFF() will continue pushing and popping chunks until one
		of these conditions occurs. If ParseIFF() is called again
		after returning, it will continue to parse the file where it
		left off.

	IFFPARSE_STEP and _RAWSTEP:
		In these two modes, ParseIFF() will return control to the
		caller after every step in the parse, specifically, after
		each push of a context node and just before each pop. If
		returning just before a pop, ParseIFF() will return
		IFFERR_EOC, which is not an error, per se, but is just an
		indication that the most recent context is ending. In STEP
		mode, ParseIFF() will invoke the handlers for chunks, if
		any, before returning. In RAWSTEP mode, ParseIFF() will not
		invoke any handlers and will return right away. In both
		cases the function can be called multiple times to step
		through the parsing of the IFF file.
*/
LONG ParseIFF(struct IFFHandle *iff, LONG mode)
{
	struct ChunkNode *p;
	struct PushPopNode *pp;	

	do
	{
		if (position == 0)
			fseek(iff->iff_Stream, cn.cn_Size, SEEK_CUR);  // move relative to current position

		// are we about to leave a context ?
		if (pushpoproot)
		{
			if (ftell(iff->iff_Stream) >= (pushpoproot->Filepos + pushpoproot->Length - 4))
			{
				cn.cn_ID = pushpoproot->ID;
				cn.cn_Type = pushpoproot->Type;
				cn.cn_Size = 0;
				pp = pushpoproot->Node;
				free(pushpoproot);
				pushpoproot = pp;		

				return IFFERR_EOC;			
			}
		}	

		if (fread(&cn.cn_ID, 4, 1, iff->iff_Stream) == 0)
		{
			cn.cn_ID = 0L;
			return IFFERR_EOF;									// read chunk ID
		}

		fread(&cn.cn_Size, 4, 1, iff->iff_Stream);		// read chunk size
#ifdef __SWAP__
		cn.cn_Size = Swap(cn.cn_Size);						// endian conversion
#endif
		if (cn.cn_Size & 1)
			cn.cn_Size++;											// make even
			
		// entering a context ? write type to context node
		if (cn.cn_ID == ID_FORM)
		{
			pp = (struct PushPopNode *)malloc(sizeof(struct PushPopNode));
			if (!pp)
				return IFFERR_NOMEM;

			fread(&cn.cn_Type, 4, 1, iff->iff_Stream);

			// add new chunk to simple linked list
			pp->Node 	= pushpoproot;
			pp->Filepos = ftell(iff->iff_Stream);	// remember position of chunksize
			pp->ID	   = cn.cn_ID;
			pp->Type		= cn.cn_Type;
			pp->Length	= cn.cn_Size;
			pp->CLength = 0;
			pushpoproot = pp;

			cn.cn_Size = 0;
		}		
		cn.cn_Scan = ftell(iff->iff_Stream);			// current position in file
		if (buffer)
		  free(buffer);										// delete old buffer
		buffer = NULL;
		position = 0L;
		
		// in scan mode
		if (mode == IFFPARSE_SCAN)
		{
			p = chunkroot;
			while (p && (p->cn_ID != cn.cn_ID || p->cn_Type != cn.cn_Type))
				p = p->cn_Node;
			if (p)
			{
				if (p->cn_Stop == CHNK_PROP)	// load complete chunk into memory
				{
					p->cn_Data = (void*)malloc(cn.cn_Size);
					p->cn_Size = cn.cn_Size;
					if (p->cn_Data)
					{
						fread(p->cn_Data, 1, cn.cn_Size, iff->iff_Stream);
						position = cn.cn_Size;
					}
					else
						return IFFERR_NOMEM;
				}
			}	
		}
	} while (mode == IFFPARSE_SCAN && !(p && p->cn_Stop == CHNK_STOP));

	return 0L;
}


/*************
* FUNCTION:		ReadChunkBytes
* DESCRIPTION:	Reads the IFFHandle stream into the buffer for
*					the specified number of bytes. Reads are limited to the
*					size of the current chunk and attempts to read past the
*					end of the chunk will truncate. This function returns
*					positive number of bytes read or a negative error code.
*
* INPUT:			iff		IFF-handle
*					buf		pointer to buffer
*					len		number of bytes to read		
* OUTPUT:		0L if ok else error number
*************/
LONG ReadChunkBytes(struct IFFHandle *iff, void *buf, LONG len)
{
	if (!buffer)	// buffer is empty? fill it up
	{
		buffer = (char *)malloc(cn.cn_Size);
		if (!buffer)
			return IFFERR_NOMEM;

		if (fread(buffer, cn.cn_Size, 1, iff->iff_Stream) != 1)
			return IFFERR_READ;
		position = 0;
	}
	if (len > cn.cn_Size)
		len = cn.cn_Size;

	memcpy(buf, &buffer[position], len);
	position += len;

	return len; 
}

/*************
* FUNCTION:		CurrentChunk
* DESCRIPTION:	Returns the top context node for the given
*					IFFHandle structure. The top context node corresponds
*					to the chunk most recently pushed on the stack, which
*					is the chunk where the stream is currently positioned.
*					The ContextNode structure contains information on the
*					type of chunk currently being parsed (or written), its
*					size and the current position within the chunk.
*
* INPUT:			iff		IFF-handle
* OUTPUT:		pointer to current chunk node
*************/
struct ContextNode *CurrentChunk(struct IFFHandle *iff)
{
	if (cn.cn_ID)
		return &cn;
	else
		return NULL;
}

/*************
* FUNCTION:		WriteChunkBytes
* DESCRIPTION:	Write cBytes of the buffer buf to stream
*					If a certain chunk size was specified with PushChunk, all attempts
*					to write past this size will be truncated
* INPUT:			iff		IFF handle
*					buf		pointer to buffer
*					cBytes	size of buffer
* OUTPUT:		0L if ok else error number
*************/
LONG WriteChunkBytes(struct IFFHandle *iff, APTR buf, long cBytes)
{
	if (!pushpoproot)
		return IFFERR_WRITE;
		
	// truncate if someone attempts to write past the given chunk size
	if (pushpoproot->Length != IFFSIZE_UNKNOWN &&
		(pushpoproot->CLength + cBytes) > pushpoproot->Length)
	{
		cBytes = pushpoproot->Length - pushpoproot->CLength;
		if (cBytes < 0)
			cBytes = 0;
	}
	
	pushpoproot->CLength+= cBytes;

	return fwrite(buf, 1, cBytes, iff->iff_Stream);
}

/*************
* FUNCTION:    PushChunk
* DESCRIPTION: Begin a new chunk. If the final chunk size is not known,
*					set <size> to IFFSIZE_UNKNOWN and the chunk size is set automatically
*					(when finishing a chunk with PopChunk)
*					Usually you always specify IFFSIZE_UNKNOWN
*
* INPUT:			iff		IFF - handle
*					type		type of chunk (i.e. ILBM)
*					id			chunk name (i.e. BODY)
*					size		size of chunk
* OUTPUT:		0L if ok else error number
*************/
LONG PushChunk(struct IFFHandle *iff, long type, long id, long size)
{
	struct PushPopNode *p;

	p = (struct PushPopNode *)malloc(sizeof(struct PushPopNode));
	if (!p)
		return IFFERR_NOMEM;
	fwrite(&id, 1, 4, iff->iff_Stream);	// write chunk id
	// add new chunk to simple linked list
	p->Node 		= pushpoproot;
	p->Filepos 	= ftell(iff->iff_Stream);	// remember position of chunksize
	p->ID	   	= id;
	p->Length	= size;
	p->CLength 	= 0;
	pushpoproot = p;
#ifdef __SWAP__
	size = Swap(size);							// do endian conversion again
#endif
	fwrite(&size, 1, 4, iff->iff_Stream);	// write size
	if (id == ID_FORM)
		fwrite(&type, 1, 4, iff->iff_Stream);
	
	return 0L;
}

/*************
* FUNCTION:		PopChunk
* DESCRIPTION:	PopChunks finishes a chunk. When IFFSIZE_UNKNOWN
*					is specified, PopChunk writes the final chunksize.
*
* INPUT:			iff		IFF - handle
* OUTPUT:		0L if ok else error number
*************/
LONG PopChunk(struct IFFHandle *iff)
{
	LONG act, size;
	struct PushPopNode *p = pushpoproot;

	if (!pushpoproot)
		return IFFERR_SYNTAX;

	act = ftell(iff->iff_Stream);
	size = act - p->Filepos - 4;

	if (size & 1)
		fwrite("\0", 1, 1, iff->iff_Stream);			// add one pad byte

	if (pushpoproot->Length != IFFSIZE_UNKNOWN)
	{
		if (pushpoproot->Length != pushpoproot->CLength)
			return IFFERR_WRITE;
	}
	else
	{	
		fseek(iff->iff_Stream, p->Filepos, SEEK_SET);	// move to position of chunk size
#ifdef __SWAP__
		size = Swap(size);										// endian
#endif
		fwrite(&size, 1, 4, iff->iff_Stream);				// write chunk size
		fseek(iff->iff_Stream, 0, SEEK_END);	  			// move back to end
	}
	p = pushpoproot->Node;
	free(pushpoproot);
	pushpoproot = p;
	
	return 0L;
} 

