/***************
 * NAME:          pichand_lib.h
 * DESCRIPTION:   Definitions for the picture handler libraries
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.02.97 ah    initial release
 *    26.10.97 ah    removed picInit() and picCleanup()
 ***************/

#ifndef PICHAND_LIB_H
#define PICHAND_LIB_H

#ifdef __PPC__
#if __STORM__ == 39
extern "library=PicHandBase"
{
	char *picRead(PICTURE*) { };
}
#else
extern "AmigaLib" PicHandBase
{
	char *picRead(PICTURE*) = -0x1e;
}
#endif
#else
extern "AMIGA" PicHandBase
{
	char* picRead(PICTURE *) = -0x1e;
	char* picWrite(PICTURE *) = -0x24;
}
#endif // __PPC__

#endif
