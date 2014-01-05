/***************
 * NAME:          plugin_module.h
 * DESCRIPTION:   Definitions for the plugin libraries
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.11.98 ah    initial release
 ***************/

#ifndef PLUIGN_MODULE_H
#define PLUGIN_MODULE_H

#ifndef TEXTURE_H
#include "ray:plugins/plugin.h"
#endif

#ifdef __PPC__
#if __STORM__ == 39
extern "library=PluginBase"
{
	PLUGIN_INFO *plugin_init();
}
#else
extern "AmigaLib" PluginBase
{
	PLUGIN_INFO *plugin_init_(struct Library*) = -0x1e;
}

inline PLUGIN_INFO *plugin_init()
{
	extern struct Library *PluginBase;
	return plugin_init_(PluginBase);
}
#endif
#else
extern "AMIGA" PluginBase
{
	PLUGIN_INFO *plugin_init() = -0x1e;
}
#endif

#endif
