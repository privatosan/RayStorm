/*************
 * MODULE:			fileutil
 * NAME:				fileutil.c
 * VERSION:			1.0 13.12.1995
 * DESCRIPTION:	help functions for file name handling
 * AUTHORS:			Mike Hesser
 * BUGS:				none
 * TO DO:			all
 * HISTORY:			DATE		NAME	COMMENT
 *						11.02.95	mh		first release
 *						09.09.95	mh		AddPart, FilePart full function
 *						27.09.95	mh		FileExt added
 * 					13.12.95 mh		new comments
 *						30.06.96	mh		cpp -> c
 *						07.09.96 mh		added AddFileExt
 *						07.10.96 mh		added DirPart
 *						26.05.97 mh		bugfix in DirPart
 *************/

#include <stdio.h>
#include <string.h>
#include "fileutil.h"

#ifdef __AMIGA__
	#ifdef __SASC
		#include <proto/dos.h>
	#endif
	#if defined __MAXON__ || __STORM__
		#include <pragma/dos_lib.h>
	#endif
	#ifdef __GNUC__
		#include <inline/dos.h>
	#endif
#endif

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef __AMIGA__

/*************
 * FUNCTION:		FilePart
 * DESCRIPTION:	returns file name of file path
 * INPUT:			name   file path
 * OUTPUT:			pointer to file name
 *************/
char *FilePart(char *name)
{
	char *h;

	h = strrchr(name, '\\');
	if(!h)
		h = strrchr(name, '/');
	if(h)
		return h + 1;
	h = strrchr(name, ':');
	if(h)
		return h + 1;
	else
		return name;
}

/*************
 * FUNCTION:		DirPart
 * DESCRIPTION:	returns path of a path + filename
 * INPUT:			name   buffer to put path into
 *						path	 a full path to get the path from
 *						maxlen length of buffer
 * OUTPUT:			TRUE if path exists else FALSE
 *************/
int DirPart(char *buf, char *path, int maxlen)
{
	char *h;
	int l;
	
	h = strrchr(path, '\\');
	if(!h)
	{
		h = strrchr(path, '/');
		if(!h)
			h = strrchr(path, ':');
	}
	if(h)
	{
		l = (int)(h - path + 1);
		if (l > maxlen - 1)
			l = maxlen - 1;

		strncpy(buf, path, l);
		buf[l] = '\0';
		return 1;
	}
	else
		return 0;
}

/*************
 * FUNCTION:    AddPart
 * DESCRIPTION: <path> is a file path and <name> is a name
 *              of a file. AddPart puts them together to a
 *              new file path which length does not exceed l
 * INPUT:       path   file path
 *              name   file path
 *              maxlen maximum length of resulting string
 * OUTPUT:      -
 *************/
void AddPart(char *path, char *name, int maxlen)
{
	char *h;
	int cnt;

	if (strlen(path) + 1 > (size_t)maxlen)
		return;

	if (path[strlen(path) - 1] != DIRECTORYSLASH && path[0])
	{
		path[strlen(path) + 1] = '\0';
		path[strlen(path)] = DIRECTORYSLASH;
	}

	h = FilePart(name);
	cnt = strlen(h) + 1;
	if (strlen(path) + cnt > (size_t)maxlen)
		cnt = maxlen - strlen(path);

	strncpy(&path[strlen(path)], h, cnt);
}

/*************
 * FUNCTION:		FileExt
 * DESCRIPTION:	returns the extension of a given file name
 * INPUT:			filename   file name
 * OUTPUT:			pointer to extension
 *************/
char *FileExt(char *filename)
{
	char *pos;
	pos = strrchr(FilePart(filename), '.');
	if (pos)
		return pos + 1;
	else
		return NULL;
}

/**********
 * FUNCTION:		AddFileExt
 * DESCRIPTION:	adds an extension to a given filename, i.e. "test" to "test.dat"
 *						if an extension already exists, it is replaced by the new extension
 *						the resulting string has no more than maxlen characters
 * INPUT:			file    name of file
 *						ext     extension
 *						maxlen  maximum length of string	
 * OUTPUT:			1 when successful otherwise 0
 **********/
int AddFileExt(char *file, char *ext, int maxlen)
{
	char *pos;
	int cnt;

	/* skip extension point eventually given */
	if (ext[0] == '.')
		ext++;

	/* cut a given extension */
	pos = FileExt(file);
	if (pos)
		*(pos - 1) = '\0';

	if (strlen(file) + 1 > (size_t)maxlen)
		return 0;
		
	strcat(file, ".");

	cnt = strlen(ext) + 1;
	if (strlen(file) + cnt > (size_t)maxlen)
		cnt = maxlen - strlen(file);

	/* user wants to trick us ! */
	if (cnt < 0)
		return 0;

	strncpy(&file[strlen(file)], ext, cnt);

	return 1;
}

#endif

/**********
 * FUNCTION:		ExpandPath
 * DESCRIPTION:	Test if a file can be found, uses a pathlist of the form
 *						'path1;path2;...;pathn' to find the file.
 * INPUT:			paths   path string
 *						file    name of file to test
 *						buffer  buffer for the the result (at least 256 chars long)
 * OUTPUT:			TRUE if found else FALSE
 **********/
int ExpandPath(char *paths, char *file, char *buffer)
{
	FILE *test;
	char *pathptr = paths, *help;

	// test without paths
	test = fopen(file, "r");
	// file found
	if (test)
	{
		fclose(test);
		strcpy(buffer,file);
		return 1;
	}

	while (pathptr)
	{
		// next path
		help = strchr(pathptr, (int)';');
		if (help)
		{
			strncpy(buffer, pathptr, (size_t)(help - pathptr));
			buffer[help-pathptr] = 0;
			pathptr = &help[1];
		}
		else
		{
			strcpy(buffer, pathptr);
			pathptr = NULL;
		}
		AddPart(buffer, file, 256);
		test = fopen(buffer, "r");

		// file found
		if (test)
		{
			fclose(test);
			return 1;
		}
	}
	return 0;
}
