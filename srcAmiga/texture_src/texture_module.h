/***************
 * NAME:          texture_module.h
 * DESCRIPTION:   Definitions for the texture libraries
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    31.03.97 ah    initial release
 ***************/

#ifndef TEXTURE_MODULE_H
#define TEXTURE_MODULE_H

#ifndef TEXTURE_H
#include "rtxt:rtexture.h"
#endif

#ifdef __PPC__
#if __STORM__ == 39
extern "library=TextureBase"
{
	TEXTURE_INFO *texture_init();
}
#else
extern "AmigaLib" TextureBase
{
	TEXTURE_INFO *texture_init_(struct Library*) = -0x1e;
}

inline TEXTURE_INFO *texture_init()
{
	extern struct Library *TextureBase;
	return texture_init_(TextureBase);
}
#endif
#else
extern "AMIGA" TextureBase
{
	TEXTURE_INFO *texture_init() = -0x1e;
}
#endif

#endif
