/***************
 * PROGRAM:       Modeler
 * NAME:          plugins.cpp
 * DESCRIPTION:   functions for RayStorm plugins
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.09.98 ah    initial release
 ***************/

#include <string.h>
#include <stdarg.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>

#include <pragma/utility_lib.h>

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef MAINWINDOW_CLASS_H
#include "mainwindow_class.h"
#endif

#ifndef PLUGINS_H
#include "plugins.h"
#endif

#ifndef DIALOG_H
#include "dialog.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

struct Library *PluginBase;

PLUGIN::PLUGIN()
{
	base = NULL;
}

PLUGIN::~PLUGIN()
{
	if(base)
	{
		PluginBase = base;
		info->cleanup(info);
		CloseLibrary(base);
	}
	if(data)
		delete data;
}

static PLUGIN *pluginroot = NULL;

/*************
 * DESCRIPTION:   This function is called from the plugin library to execute a specific function
 *    of e.g. buttons
 * INPUT:         strip    menu strip
 * OUTPUT:        FALSE if failed
 *************/
ULONG DoPluginMethod(void *o, ULONG msg, ...)
{
	Object *obj = (Object*)o;
	va_list list;

	if(!obj)
		return FALSE;

	switch(msg)
	{
		case PLUG_IMAGE_DISPLAY:
			va_start(list, msg);
			DoMethod(obj, MUIM_Render_ResetColors);
			SetAttrs(obj, MUIA_Render_Data, *((ULONG*)list), TAG_DONE);
			DoMethod(obj, MUIM_Render_DrawUpdate, 0);
			va_end(*list);
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   callback functions for plugin interface
 * INPUT:         
 * OUTPUT:
 *************/
void *MeshCreate()
{
	OBJECT *selobj;

	if(!sciCreateObject(OBJECT_MESH))
		return NULL;

	GetActiveObjects(&selobj, FALSE);

	return selobj;
}

void MeshPoints(void *_mesh, ULONG count, VECTOR *points)
{
	MESH *mesh = (MESH*)_mesh;
	ULONG index;

	if(!_mesh || !count || !points)
		return;

	index = mesh->AddPoint(count);

	memcpy(&mesh->points[index], points, sizeof(VECTOR)*count);

	mesh->CalcBBox();
}

void MeshEdges(void *_mesh, ULONG count, PLUGIN_EDGE *edges)
{
	MESH *mesh = (MESH*)_mesh;
	ULONG index;
	EDGE *edge;
	int i;

	if(!_mesh || !count || !edges)
		return;

	index = mesh->AddEdge(count);

	edge = &mesh->edges[index];
	for(i=0; i<count; i++)
	{
		edge->p[0] = edges->p1;
		edge->p[1] = edges->p2;
		edge++;
		edges++;
	}
}

void MeshTriangles(void *_mesh, ULONG count, PLUGIN_TRIANGLE *triangles)
{
	MESH *mesh = (MESH*)_mesh;
	ULONG index;
	TRIANGLE *triangle;
	int i;

	if(!_mesh || !count || !triangles)
		return;

	index = mesh->AddTria(count);

	triangle = &mesh->trias[index];
	for(i=0; i<count; i++)
	{
		triangle->e[0] = triangles->e1;
		triangle->e[1] = triangles->e2;
		triangle->e[2] = triangles->e3;
		triangle++;
		triangles++;
	}
}

/*************
 * DESCRIPTION:   Scan for available plugins
 * INPUT:         strip    menu strip
 * OUTPUT:        FALSE if failed
 *************/
BOOL ScanPlugins(Object *strip, Object *mainwin)
{
	struct FileInfoBlock *fiblock;
	BPTR dirlock;
	Object *prevmenu, *item;
	BOOL success = TRUE;
	PLUGIN *plugin;
	char name[256];
	ULONG id = 0;

	dirlock = Lock("plugins", ACCESS_READ);
	if(!dirlock)
		return FALSE;

	fiblock = (struct FileInfoBlock*)AllocDosObjectTags(DOS_FIB, TAG_DONE);
	if(!fiblock)
	{
		UnLock(dirlock);
		return FALSE;
	}

	prevmenu = (Object *)DOMETHOD(strip, MUIM_FindUData, MEN_PREVIEW);
	if(!prevmenu)
	{
		FreeDosObject(DOS_FIB, fiblock);
		UnLock(dirlock);
		return FALSE;
	}

	if(!Examine(dirlock, fiblock))
	{
		FreeDosObject(DOS_FIB, fiblock);
		UnLock(dirlock);
		return FALSE;
	}

	while(ExNext(dirlock, fiblock) && success)
	{
		plugin = new PLUGIN;
		if(!plugin)
		{
			success = FALSE;
			continue;
		}
		plugin->Insert((SLIST**)&pluginroot);
		strcpy(name, "plugins");
		AddPart(name, fiblock->fib_FileName, 256);
		plugin->base = OpenLibrary(name, 0L);
		if(!plugin->base)
		{
			success = FALSE;
			continue;
		}
		PluginBase = plugin->base;
		plugin->info = plugin_init();
		if(!plugin->info)
		{
			success = FALSE;
			continue;
		}
		plugin->data = new char[plugin->info->datasize];
		if(!plugin->data)
		{
			success = FALSE;
			continue;
		}
		// copy default data
		if(plugin->info->defaultdata)
			memcpy(plugin->data, plugin->info->defaultdata, plugin->info->datasize);
		item = MUI_MakeObject(MUIO_Menuitem, plugin->info->name, NULL, 0, id);
		if(!item)
		{
			success = FALSE;
			continue;
		}
		plugin->info->DoMethod = DoPluginMethod;
		plugin->info->MeshCreate = MeshCreate;
		plugin->info->MeshPoints = MeshPoints;
		plugin->info->MeshEdges = MeshEdges;
		plugin->info->MeshTriangles = MeshTriangles;
		DOMETHOD(strip, MUIM_Family_Insert, item, prevmenu);
		DOMETHOD(item, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, mainwin, 2, MUIM_MainWindow_OpenPlugin, plugin);
		id++;
	}

	DOMETHOD(strip, MUIM_Family_Remove, prevmenu);

	FreeDosObject(DOS_FIB, fiblock);
	UnLock(dirlock);

	return success;
}

/*************
 * DESCRIPTION:   Free plugins
 * INPUT:         -
 * OUTPUT:        -
 *************/
void FreePlugins()
{
	pluginroot->FreeList();
}

/*************
 * DESCRIPTION:   do initialations for plugin window
 * INPUT:         cl
 *                obj
 *                Msg
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct PluginWindow_Data databuf,*data;

	databuf.plugininfo = (PLUGIN_INFO*)WARPOS_PPC_FUNC(GetTagData)(MUIA_PluginWindow_Info, 0, msg->ops_AttrList);
	databuf.plugindata = (PLUGIN_INFO*)WARPOS_PPC_FUNC(GetTagData)(MUIA_PluginWindow_Data, 0, msg->ops_AttrList);
	if(!databuf.plugininfo || !databuf.plugindata)
		return 0;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Plugin",
		MUIA_Window_ID, MAKE_ID('P','L','U','G'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, databuf.dialog = (Object *)NewObject(CL_Dialog->mcc_Class,NULL,
				MUIA_Dialog_Dialog, databuf.plugininfo->dialog,
				MUIA_Dialog_Data, databuf.plugindata,
			End,
			Child, HGroup,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
			End,
		End,
		TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct PluginWindow_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object *)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);

		// button methods
		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj, 3, MUIM_PluginWindow_Close, obj, TRUE);

		// close methods
		DOMETHOD(data->b_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
			obj, 3, MUIM_PluginWindow_Close, obj, FALSE);
		DOMETHOD(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			obj, 3, MUIM_PluginWindow_Close, obj, FALSE);

		// do first update when window is openend
		DOMETHOD(obj, MUIM_Notify, MUIA_Window_Open, TRUE,
			obj, 1, MUIM_Parent_Update);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   close window
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Close(struct IClass *cl,Object *obj, struct MUIP_ChildWindow_Finish *msg)
{
	struct PluginWindow_Data *data = (struct PluginWindow_Data*)INST_DATA(cl,obj);

	if(msg->ok && data->plugininfo->update)
		data->plugininfo->work(data->plugininfo, data->plugindata);

	DOMETHOD(app,MUIM_Application_PushMethod,data->originator,
		3, MUIM_AttrsWindow_Finish, msg->win, msg->ok);

	return 0;
}

/*************
 * DESCRIPTION:   Let the plugin recalculate it's data
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Update(struct IClass *cl,Object *obj, Msg msg)
{
	struct PluginWindow_Data *data = (struct PluginWindow_Data*)INST_DATA(cl,obj);

	if(data->plugininfo->update)
		data->plugininfo->update(data->plugininfo, data->plugindata);

	return 0;
}

/*************
 * DESCRIPTION:   Dispatcher for plugin window class.
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG PluginWindow_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case MUIM_PluginWindow_Close:
			return(Close(cl,obj,(struct MUIP_ChildWindow_Finish*)msg));
		case MUIM_Parent_Update:
			return(Update(cl,obj,msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
