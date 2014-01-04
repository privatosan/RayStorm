/***************
 * PROGRAM:       Modeler
 * NAME:          prefs.cpp
 * DESCRIPTION:   preferences save and read
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    19.02.96 ah    initial release
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifdef __AMIGA__
#include <prefs/prefhdr.h>
#include <pragma/iffparse_lib.h>
#include <pragma/dos_lib.h>
#else
#include "prefhdr.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#define PREFS_VERSION 0

static PREFS *root = NULL;

/*************
 * FUNCTION:      PREFS::PREFS
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
PREFS::PREFS()
{
	data = NULL;
}

/*************
 * FUNCTION:      PREFS::~PREFS
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
PREFS::~PREFS()
{
	if(data)
		delete data;
}

/*************
 * FUNCTION:      PREFS::AddPrefs
 * DESCRIPTION:   Inserts a preference in the list (it makes a copy of the
 *    object). If there is already one with the same id in the list the
 *    contents of the old is freed and the new data is inserted there.
 * INPUT:         none
 * OUTPUT:        TRUE if all ok else FALSE
 *************/
BOOL PREFS::AddPrefs()
{
	PREFS *p;
	PREFS *next, *prev;

	if(root)
	{
		prev = root;
		do
		{
			next = (PREFS*)prev->GetNext();
			if(prev->id == id)
			{
				// there is already one with the same id
				delete prev->data;
				prev->length = length;
				prev->data = (void*)new char[length];
				if(!prev->data)
					return FALSE;
				memcpy(prev->data,data,length);
				return TRUE;
			}
			prev = next;
		}
		while(next);
	}

	p = new PREFS;
	if(!p)
		return FALSE;

	p->id = id;
	p->length = length;
	p->data = (void*)new char[length];
	if(!p->data)
		return FALSE;
	memcpy(p->data,data,length);

	p->SLIST::Insert((SLIST**)&root);

	return TRUE;
}

/*************
 * FUNCTION:      PREFS::GetPrefs
 * DESCRIPTION:   Gets the preferences with the specified id. Caution: the
 *    data is not copied, you only get a pointer to this; you have to copy
 *    it yourself
 * INPUT:         none
 * OUTPUT:        TRUE if id found else FALSE
 *************/
BOOL PREFS::GetPrefs()
{
	PREFS *next, *prev;

	if(root)
	{
		prev = root;
		do
		{
			next = (PREFS*)prev->GetNext();
			if(prev->id == id)
			{
				// found prefs with the same id
				length = prev->length;
				data = prev->data;
				return TRUE;
			}
			prev = next;
		}
		while(next);
	}
	return FALSE;
}

/*************
 * FUNCTION:      FreePrefs
 * DESCRIPTION:   free preferences
 * INPUT:         none
 * OUTPUT:        none
 *************/
void FreePrefs()
{
	PREFS *next;

	while(root)
	{
		next = (PREFS*)root->GetNext();
		delete root;
		root = next;
	}
}

static void CleanupIFF(struct IFFHandle *iff)
{
	CloseIFF(iff);
	if(iff->iff_Stream)
		Close(iff->iff_Stream);
	FreeIFF(iff);
}

/*************
 * FUNCTION:      PrefsLoad
 * DESCRIPTION:   load preferences
 * INPUT:         file     file to read preferences from
 * OUTPUT:        TRUE if ok or FALSE if failed
 *************/
BOOL PrefsLoad(char *file)
{
	LONG error = 0;
	struct IFFHandle *iff;
	struct PrefHeader ph;
	PREFS p;
	struct ContextNode *cn;

	// Allocate IFF_File structure.
	iff = AllocIFF();
	if(!iff)
		return FALSE;

	// Set up IFF_File for AmigaDOS I/O.
#ifdef __AMIGA__
	iff->iff_Stream = Open(file, MODE_OLDFILE);
#else
	iff->iff_Stream = Open_(file, MODE_OLDFILE);
#endif
	if(!iff->iff_Stream)
	{
		CleanupIFF(iff);
		return FALSE;
	}
	InitIFFasDOS(iff);

	// Start the IFF transaction.
	if(OpenIFF(iff, IFFF_READ))
	{
		CleanupIFF(iff);
		return FALSE;
	}

	if(ParseIFF(iff, IFFPARSE_RAWSTEP))
	{
		CleanupIFF(iff);
		return FALSE;
	}
	cn = CurrentChunk(iff);
	// check for preference file
	if(cn->cn_Type != ID_PREF)
	{
		CleanupIFF(iff);
		return FALSE;
	}

	if(ParseIFF(iff, IFFPARSE_RAWSTEP))
	{
		CleanupIFF(iff);
		return FALSE;
	}
	// check for preference header
	cn = CurrentChunk(iff);
	if(cn->cn_ID != ID_PRHD)
	{
		CleanupIFF(iff);
		return FALSE;
	}
	// read preference header
	if(ReadChunkBytes(iff, &ph, sizeof(struct PrefHeader)) != sizeof(struct PrefHeader))
	{
		CleanupIFF(iff);
		return FALSE;
	}
	// check the version of the file
	if(ph.ph_Version != PREFS_VERSION)
	{
		CleanupIFF(iff);
		return FALSE;
	}

	while (!error || error == IFFERR_EOC)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if (error != IFFERR_EOC)
		{
			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(iff);
			if (cn)
			{
				p.id = cn->cn_ID;
				p.length = cn->cn_Size;
				p.data = new char[cn->cn_Size];
				if(!p.data)
				{
					CleanupIFF(iff);
					return FALSE;
				}
				if(ReadChunkBytes(iff, p.data, cn->cn_Size) < 0)
				{
					CleanupIFF(iff);
					return FALSE;
				}
				if(!p.AddPrefs())
				{
					CleanupIFF(iff);
					return FALSE;
				}
				delete p.data;
			}
		}
	}
	CleanupIFF(iff);
	p.data = NULL;
	return TRUE;
}

/*************
 * FUNCTION:      PrefsSave
 * DESCRIPTION:   save preferences
 * INPUT:         file     file to write preferences to
 * OUTPUT:        TRUE if ok or FALSE if failed
 *************/
BOOL PrefsSave(char *file)
{
	PREFS *next, *prev;
	struct IFFHandle *iff;
	struct PrefHeader ph = {PREFS_VERSION,0,0};

	// Allocate IFF_File structure.
	iff = AllocIFF();
	if(!iff)
		return FALSE;

	// Set up IFF_File for AmigaDOS I/O.
#ifdef __AMIGA__
	iff->iff_Stream = Open(file, MODE_NEWFILE);
#else
	iff->iff_Stream = Open_(file, MODE_NEWFILE);
#endif
	if(!iff->iff_Stream)
	{
		CleanupIFF(iff);
		return FALSE;
	}
	InitIFFasDOS(iff);

	// Start the IFF transaction.
	if(OpenIFF(iff, IFFF_WRITE))
	{
		CleanupIFF(iff);
		return FALSE;
	}

	if(PushChunk(iff, ID_PREF, ID_FORM, IFFSIZE_UNKNOWN))
	{
		CleanupIFF(iff);
		return FALSE;
	}
	if(PushChunk(iff, ID_PREF, ID_PRHD, IFFSIZE_UNKNOWN))
	{
		CleanupIFF(iff);
		return FALSE;
	}
	// write header
	if(WriteChunkBytes(iff, &ph, sizeof(struct PrefHeader)) < 0)
	{
		CleanupIFF(iff);
		return FALSE;
	}
	if(PopChunk(iff))
	{
		CleanupIFF(iff);
		return FALSE;
	}

	// write preference chunks
	if(root)
	{
		prev = root;
		do
		{
			next = (PREFS*)prev->GetNext();
			if(PushChunk(iff, ID_FORM, prev->id, IFFSIZE_UNKNOWN))
			{
				CleanupIFF(iff);
				return FALSE;
			}
			// write header
			if(WriteChunkBytes(iff, prev->data, prev->length) < 0)
			{
				CleanupIFF(iff);
				return FALSE;
			}
			if(PopChunk(iff))
			{
				CleanupIFF(iff);
				return FALSE;
			}
			prev = next;
		}
		while(next);
	}

	CleanupIFF(iff);
	return TRUE;
}
