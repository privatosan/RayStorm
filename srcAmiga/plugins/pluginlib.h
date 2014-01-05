/***************
 * MODUL:         texture library definition file
 * NAME:          pluginlib.h
 * DESCRIPTION:   definitions for library
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    12.09.98 ah    initial release
 ***************/

#include <exec/libraries.h>

struct PluginBase
{
	struct Library base;
};

#pragma libbase PluginBase

