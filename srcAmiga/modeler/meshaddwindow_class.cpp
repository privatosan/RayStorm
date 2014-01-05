/***************
 * PROGRAM:       Modeler
 * NAME:          meshaddwindow_class.cpp
 * DESCRIPTION:   Functions for mesh add window class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.09.96 ah    initial release
 ***************/

#include <stdlib.h>
#include <stdio.h>

#include <pragma/utility_lib.h>
#ifdef __PPC__
#include <clib/powerpc_protos.h>
#endif

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef MESHADDWINDOW_CLASS_H
#include "meshaddwindow_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

/*************
 * DESCRIPTION:   do initialations for mesh add window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct MeshAddWindow_Data *data,databuf;
	Object *content;
	char s1[30],s2[30],s3[30],s4[30],s5[30];

	databuf.type = (int)WARPOS_PPC_FUNC(GetTagData)(MUIA_MeshAddWindow_Type,0,msg->ops_AttrList);
	switch(databuf.type)
	{
		case MESHADD_CUBE:
			Float2String(global.cube_size.x, s1);
			Float2String(global.cube_size.y, s2);
			Float2String(global.cube_size.z, s3);
			content =
				ColGroup(2),
				GroupFrameT("Size"),
					Child, KeyFloatString(databuf.fs_vect1x,"X",s1,'x'), End,
					Child, KeyFloatString(databuf.fs_vect1y,"Y",s2,'y'), End,
					Child, KeyFloatString(databuf.fs_vect1z,"Z",s3,'z'), End,
				End;
			break;
		case MESHADD_SPHERE:
			Float2String(global.sphere_radius, s1);
			sprintf(s2, "%d", global.sphere_divisions);
			sprintf(s3, "%d", global.sphere_slices);
			content =
				ColGroup(2),
					Child, KeyFloatString(databuf.fs_float1,"Radius",s1,'r'), End,
					Child, KeyNumericString(databuf.ns_int1,"Divisions",s2,'d'), End,
					Child, KeyNumericString(databuf.ns_int2,"Slices",s3,'s'), End,
				End;
			break;
		case MESHADD_PLANE:
			Float2String(global.plane_size.x, s1);
			Float2String(global.plane_size.y, s2);
			Float2String(global.plane_size.z, s3);
			sprintf(s4, "%d", global.plane_divisions_x);
			sprintf(s5, "%d", global.plane_divisions_y);
			content =
				VGroup,
					Child, ColGroup(2),
					GroupFrameT("Size"),
						Child, KeyFloatString(databuf.fs_vect1x,"X",s1,'x'), End,
						Child, KeyFloatString(databuf.fs_vect1y,"Y",s2,'y'), End,
						Child, KeyFloatString(databuf.fs_vect1z,"Z",s3,'z'), End,
					End,
					Child, ColGroup(2),
					GroupFrameT("Divisions"),
						Child, KeyNumericString(databuf.ns_int1,"X Divisions",s4,'d'), End,
						Child, KeyNumericString(databuf.ns_int2,"Z Divisions",s5,'i'), End,
					End,
				End;
			break;
		case MESHADD_TUBE:
			Float2String(global.tube_radius, s1);
			Float2String(global.tube_height, s2);
			sprintf(s3, "%d", global.tube_divisions);
			sprintf(s4, "%d", global.tube_slices);
			content =
				VGroup,
					Child, ColGroup(2),
						Child, KeyFloatString(databuf.fs_float1,"Radius",s1,'r'), End,
						Child, KeyFloatString(databuf.fs_float2,"Height",s2,'h'), End,
						Child, KeyNumericString(databuf.ns_int1,"Divisions",s3,'d'), End,
						Child, KeyNumericString(databuf.ns_int2,"Slices",s4,'s'), End,
					End,
					Child, ColGroup(3),
						Child, KeyLabel2("Close top",'t'), Child, databuf.cm_flag1 = CheckMarkNew(global.tube_close_top,'t'), Child, HSpace(0),
						Child, KeyLabel2("Close bottom",'b'), Child, databuf.cm_flag2 = CheckMarkNew(global.tube_close_bottom,'b'), Child, HSpace(0),
					End,
				End;
			break;
		case MESHADD_CONE:
			Float2String(global.cone_radius, s1);
			Float2String(global.cone_height, s2);
			sprintf(s3, "%d", global.cone_divisions);
			sprintf(s4, "%d", global.cone_slices);
			content =
				VGroup,
					Child, ColGroup(2),
						Child, KeyFloatString(databuf.fs_float1,"Radius",s1,'r'), End,
						Child, KeyFloatString(databuf.fs_float2,"Height",s2,'h'), End,
						Child, KeyNumericString(databuf.ns_int1,"Divisions",s3,'d'), End,
						Child, KeyNumericString(databuf.ns_int2,"Slices",s4,'s'), End,
					End,
					Child, ColGroup(3),
						Child, KeyLabel2("Close bottom",'b'), Child, databuf.cm_flag1 = CheckMarkNew(global.cone_close_bottom,'b'), Child, HSpace(0),
					End,
				End;
			break;
		case MESHADD_TORUS:
			Float2String(global.torus_radius, s1);
			Float2String(global.torus_thickness, s2);
			sprintf(s3, "%d", global.torus_divisions);
			sprintf(s4, "%d", global.torus_slices);
			content =
				ColGroup(2),
					Child, KeyFloatString(databuf.fs_float1,"Ring radius",s1,'r'), End,
					Child, KeyFloatString(databuf.fs_float2,"Ring thickness",s2,'t'), End,
					Child, KeyNumericString(databuf.ns_int1,"Divisions",s3,'d'), End,
					Child, KeyNumericString(databuf.ns_int2,"Slices",s4,'s'), End,
				End;
			break;
	}

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title,"Add mesh object",
		MUIA_Window_ID,MAKE_ID('M','A','D','D'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, content,
			Child, HGroup, MUIA_Group_SameWidth, TRUE,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
			End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct MeshAddWindow_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object *)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

		// close methods
		DOMETHOD(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_MeshAddWindow_Finish, obj, FALSE);
		DOMETHOD(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_MeshAddWindow_Finish, obj, FALSE);

		// other methods
		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_MeshAddWindow_Ok);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct MeshAddWindow_Data *data = (struct MeshAddWindow_Data *)INST_DATA(cl,obj);
	VECTOR *v;
	char *buf;
	ULONG num;

	switch (msg->opg_AttrID)
	{
		case MUIA_MeshAddWindow_Type:
			*(int*)(msg->opg_Storage) = data->type;
			return TRUE;
		case MUIA_MeshAddWindow_Vector1:
			v = (VECTOR*)(msg->opg_Storage);
			GetAttr(MUIA_String_Contents,data->fs_vect1x,(ULONG*)&buf);
			v->x = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_vect1y,(ULONG*)&buf);
			v->y = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_vect1z,(ULONG*)&buf);
			v->z = atof(buf);
			return TRUE;
		case MUIA_MeshAddWindow_Float1:
			GetAttr(MUIA_String_Contents,data->fs_float1,(ULONG*)&buf);
			*(float*)(msg->opg_Storage) = atof(buf);
			return TRUE;
		case MUIA_MeshAddWindow_Float2:
			GetAttr(MUIA_String_Contents,data->fs_float2,(ULONG*)&buf);
			*(float*)(msg->opg_Storage) = atof(buf);
			return TRUE;
		case MUIA_MeshAddWindow_Int1:
			GetAttr(MUIA_String_Contents,data->ns_int1,(ULONG*)&buf);
			*(int*)(msg->opg_Storage) = atoi(buf);
			return TRUE;
		case MUIA_MeshAddWindow_Int2:
			GetAttr(MUIA_String_Contents,data->ns_int2,(ULONG*)&buf);
			*(int*)(msg->opg_Storage) = atoi(buf);
			return TRUE;
		case MUIA_MeshAddWindow_Flag1:
			GetAttr(MUIA_Selected, data->cm_flag1, &num);
			*(BOOL*)(msg->opg_Storage) = num ? TRUE : FALSE;
			return TRUE;
		case MUIA_MeshAddWindow_Flag2:
			GetAttr(MUIA_Selected, data->cm_flag2, &num);
			*(BOOL*)(msg->opg_Storage) = num ? TRUE : FALSE;
			return TRUE;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   ok button pressed -> accept new settings
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct MeshAddWindow_Data *data = (struct MeshAddWindow_Data*)INST_DATA(cl,obj);
	ULONG num;
	char *buf;

	switch(data->type)
	{
		case MESHADD_CUBE:
			GetAttr(MUIA_String_Contents,data->fs_vect1x,(ULONG*)&buf);
			global.cube_size.x = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_vect1y,(ULONG*)&buf);
			global.cube_size.y = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_vect1z,(ULONG*)&buf);
			global.cube_size.z = atof(buf);
			break;
		case MESHADD_SPHERE:
			GetAttr(MUIA_String_Contents,data->fs_float1,(ULONG*)&buf);
			global.sphere_radius = atof(buf);
			GetAttr(MUIA_String_Contents,data->ns_int1,(ULONG*)&buf);
			global.sphere_divisions = atoi(buf);
			GetAttr(MUIA_String_Contents,data->ns_int2,(ULONG*)&buf);
			global.sphere_slices = atoi(buf);
			break;
		case MESHADD_PLANE:
			GetAttr(MUIA_String_Contents,data->fs_vect1x,(ULONG*)&buf);
			global.plane_size.x = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_vect1y,(ULONG*)&buf);
			global.plane_size.y = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_vect1z,(ULONG*)&buf);
			global.plane_size.z = atof(buf);
			GetAttr(MUIA_String_Contents,data->ns_int1,(ULONG*)&buf);
			global.plane_divisions_x = atoi(buf);
			GetAttr(MUIA_String_Contents,data->ns_int2,(ULONG*)&buf);
			global.plane_divisions_y = atoi(buf);
			break;
		case MESHADD_TUBE:
			GetAttr(MUIA_String_Contents,data->fs_float1,(ULONG*)&buf);
			global.tube_radius = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_float2,(ULONG*)&buf);
			global.tube_height = atof(buf);
			GetAttr(MUIA_String_Contents,data->ns_int1,(ULONG*)&buf);
			global.tube_divisions = atoi(buf);
			GetAttr(MUIA_String_Contents,data->ns_int2,(ULONG*)&buf);
			global.tube_slices = atoi(buf);
			GetAttr(MUIA_Selected, data->cm_flag1, &num);
			global.tube_close_top = num ? TRUE : FALSE;
			GetAttr(MUIA_Selected, data->cm_flag2, &num);
			global.tube_close_bottom = num ? TRUE : FALSE;
			break;
		case MESHADD_CONE:
			GetAttr(MUIA_String_Contents,data->fs_float1,(ULONG*)&buf);
			global.cone_radius = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_float2,(ULONG*)&buf);
			global.cone_height = atof(buf);
			GetAttr(MUIA_String_Contents,data->ns_int1,(ULONG*)&buf);
			global.cone_divisions = atoi(buf);
			GetAttr(MUIA_String_Contents,data->ns_int2,(ULONG*)&buf);
			global.cone_slices = atoi(buf);
			GetAttr(MUIA_Selected, data->cm_flag1, &num);
			global.cone_close_bottom = num ? TRUE : FALSE;
			break;
		case MESHADD_TORUS:
			GetAttr(MUIA_String_Contents,data->fs_float1,(ULONG*)&buf);
			global.torus_radius = atof(buf);
			GetAttr(MUIA_String_Contents,data->fs_float2,(ULONG*)&buf);
			global.torus_thickness = atof(buf);
			GetAttr(MUIA_String_Contents,data->ns_int1,(ULONG*)&buf);
			global.torus_divisions = atoi(buf);
			GetAttr(MUIA_String_Contents,data->ns_int2,(ULONG*)&buf);
			global.torus_slices = atoi(buf);
			break;
	}

	DOMETHOD(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_MeshAddWindow_Finish, obj, TRUE);

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(MeshAddWindow_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case MUIM_MeshAddWindow_Ok:
			return(Ok(cl,obj,msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
