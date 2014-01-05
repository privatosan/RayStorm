/***************
 * PROGRAM:       Modeler
 * NAME:          plugins.h
 * DESCRIPTION:   functions for RayStorm plugins
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.09.98 ah    initial release
 ***************/

#ifndef PLUGINS_H
#define PLUGINS_H

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

#ifndef PLUIGN_MODULE_H
#include "ray:plugins/plugin_module.h"
#endif // PLUIGN_MODULE_H

struct PluginWindow_Data
{
	Object *originator;
	PLUGIN_INFO *plugininfo;
	void *plugindata;
	Object *dialog;
	Object *b_ok,*b_cancel;
};

SAVEDS ASM ULONG PluginWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

class PLUGIN : public SLIST
{
	public:
		struct Library *base;
		PLUGIN_INFO *info;
		void *data;

		PLUGIN();
		virtual ~PLUGIN();
};

BOOL ScanPlugins(Object*, Object*);
void FreePlugins();

#endif // PLUGINS_H
