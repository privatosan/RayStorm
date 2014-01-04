/*************
 * MODUL:         fileutil
 * NAME:          fileutil.h
 * DESCRIPTION:   definitions
 * TO DO:         -
 * HISTORY:       DATE     NAME  COMMENT
 *                11.02.95 mh    first release
 *                27.09.95 mh    FileExt
 *************/
#ifndef FILEUTIL_H
#define FILEUTIL_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif // TYPEDEFS_H

#ifndef __AMIGA__
	#define DIRECTORYSLASH '\\'
#else
	#define DIRECTORYSLASH '/'
#endif

#ifndef __AMIGA__
void AddPart(char *, char *, int);
char *FilePart(char *);
int DirPart(char *, char *, int);
char *FileExt(char *);
int AddFileExt(char *, char *, int);
#endif
BOOL ExpandPath(char *paths, char *file, char *buffer);

#endif
