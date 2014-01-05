/***************
 * PROGRAM:       Modeler
 * NAME:          arexx.cpp
 * DESCRIPTION:   Functions for arexx interface
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    16.01.98 ah    initial release
 ***************/

#include <string.h>
#include <stdio.h>

#ifndef AREXX_H
#include "arexx.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

enum
{
	AREXX_ERR_NONE=0,
	AREXX_ERR_FAILED,
	AREXX_ERR_INVALID_ARGUMENT,
	AREXX_ERR_INVALID_VECTOR,
};

SAVEDS ASM ULONG f_getselobjname(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_getobjectpos(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_getobjectalign(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_getobjectsize(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_setobjectpos(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_setobjectalign(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_setobjectsize(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_createobject(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_select(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_selectbyname(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_redraw(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_camera2viewer(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_group(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_ungroup(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_zoom(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_display(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_view(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_undo(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_redo(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_cut(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_copy(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_paste(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_delete(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_show(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_new(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_open(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_save(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_loadobject(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_saveobject(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_addmaterial(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_deletematerial(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_assignmaterial(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_assignmaterialtoselected(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_loadmaterial(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_savematerial(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_render(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);

static struct Hook hooks[] =
{
	{{NULL, NULL},(HOOKFUNC)f_getselobjname,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_getobjectpos,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_getobjectalign,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_getobjectsize,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_setobjectpos,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_setobjectalign,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_setobjectsize,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_createobject,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_select,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_selectbyname,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_redraw,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_camera2viewer,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_group,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_ungroup,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_zoom,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_display,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_undo,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_redo,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_cut,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_copy,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_paste,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_delete,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_show,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_new,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_open,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_save,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_loadobject,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_saveobject,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_addmaterial,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_deletematerial,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_assignmaterial,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_assignmaterialtoselected,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_loadmaterial,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_savematerial,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_render,NULL,NULL},
};

enum cmds
{
	CMD_GETSELOBJNAME,
	CMD_GETOBJECTPOS,
	CMD_GETOBJECTALIGN,
	CMD_GETOBJECTSIZE,
	CMD_SETOBJECTPOS,
	CMD_SETOBJECTALIGN,
	CMD_SETOBJECTSIZE,
	CMD_CREATEOBJECT,
	CMD_SELECT,
	CMD_SELECTBYNAME,
	CMD_REDRAW,
	CMD_CAMERA2VIEWER,
	CMD_GROUP,
	CMD_UNGROUP,
	CMD_ZOOM,
	CMD_DISPLAY,
	CMD_VIEW,
	CMD_UNDO,
	CMD_REDO,
	CMD_CUT,
	CMD_COPY,
	CMD_PASTE,
	CMD_DELETE,
	CMD_SHOW,
	CMD_NEW,
	CMD_OPEN,
	CMD_SAVE,
	CMD_LOADOBJECT,
	CMD_SAVEOBJECT,
	CMD_ADDMATERIAL,
	CMD_DELETEMATERIAL,
	CMD_ASSIGNMATERIAL,
	CMD_ASSIGNMATERIALTOSELECTED,
	CMD_LOADMATERIAL,
	CMD_SAVEMATERIAL,
	CMD_RENDER,
};

struct MUI_Command arexxcmds[] =
{
	{"GETSELOBJNAME",    NULL,       0, &hooks[CMD_GETSELOBJNAME]},
	{"GETOBJECTPOS",     "NAME/A",   1, &hooks[CMD_GETOBJECTPOS]},
	{"GETOBJECTALIGN",   "NAME/A",   1, &hooks[CMD_GETOBJECTALIGN]},
	{"GETOBJECTSIZE",    "NAME/A",   1, &hooks[CMD_GETOBJECTSIZE]},
	{"SETOBJECTPOS",     "POS/A",    1, &hooks[CMD_SETOBJECTPOS]},
	{"SETOBJECTALIGN",   "ALIGN/A",  1, &hooks[CMD_SETOBJECTALIGN]},
	{"SETOBJECTSIZE",    "SIZS/A",   1, &hooks[CMD_SETOBJECTSIZE]},
	{"CREATEOBJECT",     "WHICH/A",  1, &hooks[CMD_CREATEOBJECT]},
	{"SELECT",           "WHAT/A",   1, &hooks[CMD_SELECT]},
	{"SELECTBYNAME",     "NAME/A,MULTI/S", 2, &hooks[CMD_SELECTBYNAME]},
	{"REDRAW",           NULL,       0, &hooks[CMD_REDRAW]},
	{"CAMERA2VIEWER",    NULL,       0, &hooks[CMD_CAMERA2VIEWER]},
	{"GROUP",            NULL,       0, &hooks[CMD_GROUP]},
	{"UNGROUP",          NULL,       0, &hooks[CMD_UNGROUP]},
	{"ZOOM",             "CMD/A",    1, &hooks[CMD_ZOOM]},
	{"DISPLAY",          "MODE/A",   1, &hooks[CMD_DISPLAY]},
	{"VIEW",             "MODE/A",   1, &hooks[CMD_VIEW]},
	{"UNDO",             NULL,       0, &hooks[CMD_UNDO]},
	{"REDO",             NULL,       0, &hooks[CMD_REDO]},
	{"CUT",              NULL,       0, &hooks[CMD_CUT]},
	{"COPY",             NULL,       0, &hooks[CMD_COPY]},
	{"PASTE",            NULL,       0, &hooks[CMD_PASTE]},
	{"DELETE",           NULL,       0, &hooks[CMD_DELETE]},
	{"SHOW",             "WHAT/A",   1, &hooks[CMD_SHOW]},
	{"NEW",              NULL,       0, &hooks[CMD_NEW]},
	{"OPEN",             "FILE/A",   1, &hooks[CMD_OPEN]},
	{"SAVE",             "FILE/A",   1, &hooks[CMD_SAVE]},
	{"LOADOBJECT",       "FILE/A",   1, &hooks[CMD_LOADOBJECT]},
	{"SAVEOBJECT",       "FILE/A",   1, &hooks[CMD_SAVEOBJECT]},
	{"ADDMATERIAL",      "MATERIAL/A",  1, &hooks[CMD_ADDMATERIAL]},
	{"DELETEMATERIAL",   "MATERIAL/A",  1, &hooks[CMD_DELETEMATERIAL]},
	{"ASSIGNMATERIAL",   "MATERIAL/A,OBJECT/A",   2, &hooks[CMD_ASSIGNMATERIAL]},
	{"ASSIGNMATERIALTOSELECTED",  "MATERIAL/A",   1, &hooks[CMD_ASSIGNMATERIALTOSELECTED]},
	{"LOADMATERIAL",     "NAME/A,FILE/A",  2, &hooks[CMD_LOADMATERIAL]},
	{"SAVEMATERIAL",     "NAME/A,FILE/A",  2, &hooks[CMD_SAVEMATERIAL]},
	{"RENDER",           NULL,       0, &hooks[CMD_RENDER]},
	{NULL,               NULL,       0, NULL},
};

/*************
 * DESCRIPTION:   read vector from string
 * INPUT:         v     pointer to vector
 *                s     vector string; format '<x,y,z>'
 * OUTPUT:        TRUE if ok else FALSE
 *************/
static BOOL read_vector(VECTOR *v, char *s)
{
	if(s[0] != '<')
		return FALSE;
	if(s[strlen(s)-1] != '>')
		return FALSE;
	v->x = float(atof(strtok(&s[1],",")));
	v->y = float(atof(strtok(NULL,",")));
	v->z = float(atof(strtok(NULL,">")));

	return TRUE;
}

void GetObjFunc(OBJECT *obj, void *data)
{
	if(obj && (((char*)data)[0] == 0))
		strcpy((char*)data, obj->GetName());
}

SAVEDS ASM ULONG f_getselobjname(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	char buffer[256];

	buffer[0] = 0;
	GetSelectedObjects(GetObjFunc, buffer, 0);

	SetAttrs(appl, MUIA_Application_RexxString, buffer, TAG_DONE);

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_getobjectpos(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	char buffer[256];
	OBJECT *obj;
	MATRIX matrix;
	VECTOR pos, orient_x,orient_y,orient_z;

	buffer[0] = 0;
	obj = GetObjectByName((char*)arg[0]);
	if(obj)
	{
		obj->GetObjectMatrix(&matrix);
		matrix.GenerateAxis(&orient_x,&orient_y,&orient_z, &pos);
		sprintf(buffer, "<%f,%f,%f>", pos.x, pos.y, pos.z);
	}

	SetAttrs(appl, MUIA_Application_RexxString, buffer, TAG_DONE);

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_getobjectalign(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	char buffer[256];
	OBJECT *obj;
	MATRIX matrix;
	VECTOR align, pos, orient_x,orient_y,orient_z;

	buffer[0] = 0;
	obj = GetObjectByName((char*)arg[0]);
	if(obj)
	{
		obj->GetObjectMatrix(&matrix);
		matrix.GenerateAxis(&orient_x,&orient_y,&orient_z, &pos);
		Orient2Angle(&align, &orient_x, &orient_y, &orient_z);
		align.y = -align.y;
		sprintf(buffer, "<%f,%f,%f>", align.x, align.y, align.z);
	}

	SetAttrs(appl, MUIA_Application_RexxString, buffer, TAG_DONE);

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_getobjectsize(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	char buffer[256];
	OBJECT *obj;

	buffer[0] = 0;
	obj = GetObjectByName((char*)arg[0]);
	if(obj)
		sprintf(buffer, "<%f,%f,%f>", obj->size.x, obj->size.y, obj->size.z);

	SetAttrs(appl, MUIA_Application_RexxString, buffer, TAG_DONE);

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_createobject(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	int command = -1;

	if(!strcmp((char*)arg[0], "BOX"))
		command = SCIM_CREATEBOX;
	else if(!strcmp((char*)arg[0], "SPHERE"))
		command = SCIM_CREATESPHERE;
	else if(!strcmp((char*)arg[0], "CYLINDER"))
		command = SCIM_CREATECYLINDER;
	else if(!strcmp((char*)arg[0], "CONE"))
		command = SCIM_CREATECONE;
	else if(!strcmp((char*)arg[0], "SOR"))
		command = SCIM_CREATESOR;
	else if(!strcmp((char*)arg[0], "PLANE"))
		command = SCIM_CREATEPLANE;
	else if(!strcmp((char*)arg[0], "POINTLIGHT"))
		command = SCIM_CREATEPOINTLIGHT;
	else if(!strcmp((char*)arg[0], "SPOTLIGHT"))
		command = SCIM_CREATESPOTLIGHT;
	else if(!strcmp((char*)arg[0], "DIRECTIONALLIGHT"))
		command = SCIM_CREATEDIRECTIONALLIGHT;
	else if(!strcmp((char*)arg[0], "CAMERA"))
		command = SCIM_CREATECAMERA;
	else if(!strcmp((char*)arg[0], "CSG"))
		command = SCIM_CREATECSG;
	else if(!strcmp((char*)arg[0], "AXIS"))
		command = SCIM_CREATEAXIS;

	if(command == -1)
		return AREXX_ERR_INVALID_ARGUMENT;

	if(!sciDoMethod(command))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_select(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	int command = -1;

	if(!strcmp((char*)arg[0], "ALL"))
		command = SCIM_SELALL;
	else if(!strcmp((char*)arg[0], "NONE"))
		command = SCIM_DESELALL;
	else if(!strcmp((char*)arg[0], "NEXT"))
		command = SCIM_SELNEXT;
	else if(!strcmp((char*)arg[0], "PREV"))
		command = SCIM_SELPREV;

	if(command == -1)
		return AREXX_ERR_INVALID_ARGUMENT;

	if(!sciDoMethod(command))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_selectbyname(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciSelectByName((char*)arg[0], arg[1]))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_redraw(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_REDRAW))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_camera2viewer(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_CAMERA2VIEWER))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_group(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_GROUP))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_ungroup(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_UNGROUP))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_zoom(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	int command = -1;

	if(!strcmp((char*)arg[0], "FIT"))
		command = SCIM_ZOOMFIT;
	else if(!strcmp((char*)arg[0], "FOCUS"))
		command = SCIM_FOCUS;
	else if(!strcmp((char*)arg[0], "IN"))
		command = SCIM_ZOOMIN;
	else if(!strcmp((char*)arg[0], "OUT"))
		command = SCIM_ZOOMOUT;

	if(command == -1)
		return AREXX_ERR_INVALID_ARGUMENT;

	if(!sciDoMethod(command))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_setobjectpos(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	VECTOR pos, align, size;
	UNDO_TRANSFORM *undo;
	OBJECT *object;

	if(!read_vector(&pos, (char*)arg[0]))
		return AREXX_ERR_INVALID_VECTOR;

	GetActiveObjects(&object);
	if(!object)
		return AREXX_ERR_FAILED;

	undo = new UNDO_TRANSFORM;
	if(!undo)
		return AREXX_ERR_FAILED;
	undo->AddSelectedObjects();

	SetVector(&align, 0.f, 0.f, 0.f);
	SetVector(&size, 1.f, 1.f, 1.f);

	sciSetActiveObjects(undo, object,
		&pos, FALSE, &size, TRUE, &align, TRUE);

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_setobjectalign(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	VECTOR pos, align, size;
	UNDO_TRANSFORM *undo;
	OBJECT *object;

	if(!read_vector(&align, (char*)arg[0]))
		return AREXX_ERR_INVALID_VECTOR;

	GetActiveObjects(&object);
	if(!object)
		return AREXX_ERR_FAILED;

	undo = new UNDO_TRANSFORM;
	if(!undo)
		return AREXX_ERR_FAILED;
	undo->AddSelectedObjects();

	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&size, 1.f, 1.f, 1.f);

	sciSetActiveObjects(undo, object,
		&pos, TRUE, &size, TRUE, &align, FALSE);

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_setobjectsize(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	VECTOR pos, align, size;
	UNDO_TRANSFORM *undo;
	OBJECT *object;

	if(!read_vector(&size, (char*)arg[0]))
		return AREXX_ERR_INVALID_VECTOR;

	GetActiveObjects(&object);
	if(!object)
		return AREXX_ERR_FAILED;

	undo = new UNDO_TRANSFORM;
	if(!undo)
		return AREXX_ERR_FAILED;
	undo->AddSelectedObjects();

	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&align, 0.f, 0.f, 0.f);

	sciSetActiveObjects(undo, object,
		&pos, TRUE, &size, FALSE, &align, TRUE);

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_display(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	int command = -1;

	if(!strcmp((char*)arg[0], "BBOX"))
		command = SCIM_DISPLAYBBOX;
	else if(!strcmp((char*)arg[0], "WIRE"))
		command = SCIM_DISPLAYWIRE;
	else if(!strcmp((char*)arg[0], "SOLID"))
		command = SCIM_DISPLAYSOLID;

	if(command == -1)
		return AREXX_ERR_INVALID_ARGUMENT;

	if(!sciDoMethod(command))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_view(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	int command = -1;

	if(!strcmp((char*)arg[0], "CAMERA"))
		command = SCIM_VIEWCAMERA;
	else if(!strcmp((char*)arg[0], "FRONT"))
		command = SCIM_VIEWFRONT;
	else if(!strcmp((char*)arg[0], "RIGHT"))
		command = SCIM_VIEWRIGHT;
	else if(!strcmp((char*)arg[0], "TOP"))
		command = SCIM_VIEWTOP;
	else if(!strcmp((char*)arg[0], "PERSPECTIVE"))
		command = SCIM_VIEWPERSP;
	else if(!strcmp((char*)arg[0], "QUAD"))
		command = SCIM_VIEWQUAD;

	if(command == -1)
		return AREXX_ERR_INVALID_ARGUMENT;

	if(!sciDoMethod(command))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_undo(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_UNDO))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_redo(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_REDO))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_cut(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_CUT))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_copy(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_COPY))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_paste(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_PASTE))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_delete(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciDoMethod(SCIM_DELETE))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_show(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	int command = -1;

	if(!strcmp((char*)arg[0], "NAMES"))
		command = SCIM_SHOWNAMES;
	else if(!strcmp((char*)arg[0], "ALL"))
		command = SCIM_SHOWALL;
	else if(!strcmp((char*)arg[0], "CAMERAS"))
		command = SCIM_SHOWCAMERA;
	else if(!strcmp((char*)arg[0], "BRUSHES"))
		command = SCIM_SHOWBRUSHES;
	else if(!strcmp((char*)arg[0], "TEXTURES"))
		command = SCIM_SHOWTEXTURES;
	else if(!strcmp((char*)arg[0], "LIGHTS"))
		command = SCIM_SHOWLIGHT;
	else if(!strcmp((char*)arg[0], "MESHES"))
		command = SCIM_SHOWMESH;
	else if(!strcmp((char*)arg[0], "SPHERES"))
		command = SCIM_SHOWSPHERE;
	else if(!strcmp((char*)arg[0], "BOXES"))
		command = SCIM_SHOWBOX;
	else if(!strcmp((char*)arg[0], "CYLINDERS"))
		command = SCIM_SHOWCYLINDER;
	else if(!strcmp((char*)arg[0], "CONES"))
		command = SCIM_SHOWCONE;
	else if(!strcmp((char*)arg[0], "SORS"))
		command = SCIM_SHOWSOR;
	else if(!strcmp((char*)arg[0], "PLANES"))
		command = SCIM_SHOWPLANE;
	else if(!strcmp((char*)arg[0], "CSGS"))
		command = SCIM_SHOWCSG;

	if(command == -1)
		return AREXX_ERR_INVALID_ARGUMENT;

	if(!sciDoMethod(command))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_new(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	sciNew();

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_open(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(sciOpen((char*)arg[0]))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_save(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(sciSave((char*)arg[0]))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_loadobject(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciLoadObject((char*)arg[0]))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_saveobject(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciSaveObject((char*)arg[0]))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_addmaterial(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	SURFACE *surf;

	surf = sciCreateSurface((char*)arg[0]);
	if(surf)
	{
		sciAddMaterial(surf);
		return AREXX_ERR_NONE;
	}

	return AREXX_ERR_FAILED;
}

SAVEDS ASM ULONG f_deletematerial(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	SURFACE *surf;

	surf = GetSurfaceByName((char*)arg[0]);
	if(surf)
	{
		if(sciRemoveMaterial(surf))
		{
			sciDeleteSurface(surf);
			return AREXX_ERR_NONE;
		}
	}
	return AREXX_ERR_FAILED;
}

SAVEDS ASM ULONG f_assignmaterial(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	SURFACE *surf;
	OBJECT *obj;

	surf = GetSurfaceByName((char*)arg[0]);
	if(!surf)
		return AREXX_ERR_FAILED;

	obj = GetObjectByName((char*)arg[1]);
	if(!obj)
		return AREXX_ERR_FAILED;

	if(!sciAssignSurface(surf, obj))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_assignmaterialtoselected(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	SURFACE *surf;

	surf = GetSurfaceByName((char*)arg[0]);
	if(!surf)
		return AREXX_ERR_FAILED;

	if(!sciAssignSurfaceToSelectedObjects(surf))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_loadmaterial(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	SURFACE *surf;

	surf = sciCreateSurface((char*)arg[0]);
	if(!surf)
		return AREXX_ERR_FAILED;

	if(surf->Load((char*)arg[1]))
	{
		sciDeleteSurface(surf);
		return AREXX_ERR_FAILED;
	}

	sciAddMaterial(surf);

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_savematerial(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	SURFACE *surf;

	surf = GetSurfaceByName((char*)arg[0]);
	if(!surf)
		return AREXX_ERR_FAILED;

	if(surf->Save((char*)arg[1]))
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

SAVEDS ASM ULONG f_render(REG(a0) struct Hook *hook, REG(a2) Object *appl, REG(a1) ULONG *arg)
{
	if(!sciRender())
		return AREXX_ERR_FAILED;

	return AREXX_ERR_NONE;
}

