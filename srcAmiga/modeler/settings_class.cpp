/***************
 * PROGRAM:       Modeler
 * NAME:          settingswindow_class.cpp
 * DESCRIPTION:   the settings window is used to set
 *    - the position, orientation and size
 *    - object specific values (pages depend on objects)
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    06.03.96 ah    initial release
 *    04.11.96 ah    added lens flares and visible light source to light settings
 *    27.11.96 ah    added inner halo radius
 *    14.12.96 ah    added spike width
 *    30.01.97 ah    added star tilt
 *    07.02.97 ah    added 'No phong shading'
 *    09.02.97 ah    tilt and type are now global for all flares
 *    17.02.97 ah    flares can now be selected with the mouse
 *    06.03.97 ah    added shadowmap
 ***************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef BROWSER_CLASS_H
#include "browser_class.h"
#endif

#ifndef SETTINGS_CLASS_H
#include "settings_class.h"
#endif

#ifndef MUIUTILITY_H
#include "MUIutility.h"
#endif

#ifndef REQUVIEW_CLASS_H
#include "requview_class.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef STAR_H
#include "star.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

enum
{
	ERR_TRACKNOTFOUND,
	ERR_TRACKTOSELF
};

static char *errors[] =
{
	"Track object not found",
	"Can't track to myself"
};

static char *SettingPages[] =
{
	"Transform","Settings",NULL
};

static char *LightSettingPages[] =
{
	"Transform","Settings","Lens flares", "Visible light", NULL
};

static char *Pages[] =
{
	"Transform",NULL
};

static int lensvals[][2] =
{
	50,50,
	HFOV,VFOV,
	12,12
};

// lens presets
static char *lenses[] =
{
	"Fisheye",
	"Normal",
	"Tele",
	NULL
};

static char *flare_functions[] =
{
	"Plain", "Ring", "Radial", "Exponent 1", "Exponent 2", NULL
};

static char *flare_types[] =
{
	"Disc","Triangle","Rectangle","Poly 5","Poly 6","Poly 7","Poly 8",NULL
};

static char *shadowmap[] =
{
	"128x128",
	"256x256",
	"512x512",
	"768x768",
	NULL
};

static char *csg[] =
{
	"Union",
	"Intersection",
	"Difference",
	NULL
};

static char *sor[] =
{
	"Fast, but inaccurate",
	"Slow and accurate",
	NULL
};

static void UpdateFlareGadgets(struct Settings_Data *);
static void UpdateStarGadgets(struct Settings_Data *);
static void UpdateGadgets(struct Settings_Data *);

/*************
 * DESCRIPTION:   do initialations for settings window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Settings_Data *data,databuf;
	Object *pages, *set, *transform, *group;

	databuf.object = (OBJECT*)GetTagData(MUIA_Settings_Object,0,msg->ops_AttrList);

	transform = HGroup,
		Child, VGroup,
			GroupFrameT("Position"),
			Child, ColGroup(2),
				Child, FloatString(databuf.posx,"X",NULL), End,
				Child, FloatString(databuf.posy,"Y",NULL), End,
				Child, FloatString(databuf.posz,"Z",NULL), End,
			End,
			Child, HGroup,
				Child, KeyLabel2("Local", 0),
				Child, databuf.cm_poslocal = CheckMarkNew(FALSE, 0),
				Child, HSpace(0),
			End,
		End,
		Child, VGroup,
			GroupFrameT("Alignment"),
			Child, ColGroup(2),
				Child, FloatString(databuf.alignx,"X",NULL), End,
				Child, FloatString(databuf.aligny,"Y",NULL), End,
				Child, FloatString(databuf.alignz,"Z",NULL), End,
			End,
			Child, HGroup,
				Child, KeyLabel2("Local", 0),
				Child, databuf.cm_alignlocal = CheckMarkNew(FALSE, 0),
				Child, HSpace(0),
			End,
		End,
		Child, VGroup,
			GroupFrameT("Size"),
			Child, databuf.sizex = (Object*)NewObject(CL_Float->mcc_Class, NULL,
				MUIA_Float_Label, "X",
				MUIA_Float_Min, 0,
				MUIA_Float_LowerBound, TRUE,
			End,
			Child, databuf.sizey = (Object*)NewObject(CL_Float->mcc_Class, NULL,
				MUIA_Float_Label, "Y",
				MUIA_Float_Min, 0,
				MUIA_Float_LowerBound, TRUE,
			End,
			Child, databuf.sizez = (Object*)NewObject(CL_Float->mcc_Class, NULL,
				MUIA_Float_Label, "Z",
				MUIA_Float_Min, 0,
				MUIA_Float_LowerBound, TRUE,
			End,
			Child, HGroup,
				Child, KeyLabel2("Local", 0),
				Child, databuf.cm_sizelocal = CheckMarkNew(FALSE, 0),
				Child, HSpace(0),
			End,
		End,
	End;

	if(databuf.object->CSGAble())
	{
		set =
			VGroup,
				Child, HGroup,
					Child, ColGroup(2), GroupFrameT("Track"),
						Child, databuf.s_track = KeyString(NULL,256,'t'), End,
						Child, databuf.pb_track = PopButton(MUII_PopUp),
					End,
					Child, HGroup, GroupFrameT("CSG"),
						Child, databuf.cm_inverted = CheckMarkNew(FALSE,'i'),
						Child, KeyLabel2("Inverted", 'i'),
						HSpace(0),
					End,
				End,
				Child, transform,
			End;
	}
	else
	{
		set =
			VGroup,
				Child, ColGroup(2), GroupFrameT("Track"),
					Child, databuf.s_track = KeyString(NULL,256,'t'), End,
					Child, databuf.pb_track = PopButton(MUII_PopUp),
				End,
				Child, transform,
			End;
	}

	databuf.type = (int)GetTagData(MUIA_Settings_Type,0,msg->ops_AttrList);
	switch(databuf.type)
	{
		case SETTINGS_SETNONE:
			pages = set;
			break;
		case SETTINGS_SETCAMERA:
			pages =
				RegisterGroup(SettingPages),
					Child, set,
					Child, VGroup,
						Child, VGroup, GroupFrameT("Lens"),
							Child, VGroup,
								Child, databuf.fs_fdist = (Object*)NewObject(CL_Float->mcc_Class, NULL,
									MUIA_Float_Label, "Focal distance",
									MUIA_Float_ControlChar, 'f',
									MUIA_Float_Min, 0,
									MUIA_Float_LowerBound, TRUE,
								End,
								Child, databuf.fs_apert = (Object*)NewObject(CL_Float->mcc_Class, NULL,
									MUIA_Float_Label, "Aperture",
									MUIA_Float_ControlChar, 'a',
									MUIA_Float_Min, 0,
									MUIA_Float_LowerBound, TRUE,
									MUIA_Float_LowerInside, TRUE,
								End,
							End,
							Child, ColGroup(5),
								Child, databuf.cm_focustrack = CheckMarkNew(FALSE,'f'), Child, KeyLabel2("Focus track",'t'),
//                      Child, databuf.cm_fastdof = CheckMarkNew(FALSE,'d'), Child, KeyLabel2("Fast DOF",'d'),
								Child, HSpace(0),
							End,
						End,
						Child, VGroup, GroupFrameT("Field of View"),
							Child, HGroup,
								Child, VGroup,
									Child, databuf.fs_hfov = (Object*)NewObject(CL_Float->mcc_Class, NULL,
										MUIA_Float_Label, "Horizontal",
										MUIA_Float_ControlChar, 'h',
										MUIA_Float_Min, 0,
										MUIA_Float_Max, 360,
										MUIA_Float_LowerBound, TRUE,
										MUIA_Float_UpperBound, TRUE,
									End,
									Child, HSpace(0),
									Child, databuf.fs_vfov = (Object*)NewObject(CL_Float->mcc_Class, NULL,
										MUIA_Float_Label, "Vertical",
										MUIA_Float_ControlChar, 'v',
										MUIA_Float_Min, 0,
										MUIA_Float_Max, 360,
										MUIA_Float_LowerBound, TRUE,
										MUIA_Float_UpperBound, TRUE,
									End,
								End,
								Child, databuf.lv_lens = ListviewObject,
									MUIA_CycleChain,TRUE,
									MUIA_Listview_List, ListObject,
										InputListFrame,
										MUIA_List_SourceArray, lenses,
									End,
								End,
							End,
							Child, ColGroup(3),
								Child, databuf.cm_vfov = CheckMarkNew(FALSE,'r'), Child, KeyLabel2("Take VFOV from resolution",'r'), Child, HSpace(0),
							End,
						End,
					End,
				End;
			break;
		case SETTINGS_SETSPOTLIGHT:
		case SETTINGS_SETPOINTLIGHT:
		case SETTINGS_SETDIRECTIONALLIGHT:
			databuf.gr_lensflares =
				HGroup,
					Child, ColGroup(2),
						Child, Label2("Flare No."),
						Child, databuf.ls_flarenumber = SliderObject,
							MUIA_CycleChain, TRUE,
							MUIA_Slider_Min, 1,
						End,

						Child, KeyLabel2("Type",'t'),
						Child, databuf.cy_flaretype = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_ControlChar, 't',
							MUIA_Cycle_Entries, flare_types,
						End,

						Child, Label2("Tilt"),
						Child, databuf.ls_flaretilt = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
							MUIA_StringSlider_Min, 0,
							MUIA_StringSlider_Max, 359,
							MUIA_StringSlider_LowerBound, TRUE,
							MUIA_StringSlider_UpperBound, TRUE,
						End,

						Child, VSpace(0), Child, VSpace(0),

						Child, Label2("Color"),
						Child, databuf.cf_flarecolor = Colorfield,

						Child, Label2("Radius %"),
						Child, databuf.ls_flareradius = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
							MUIA_StringSlider_Min, 1,
							MUIA_StringSlider_Max, 100,
							MUIA_StringSlider_LowerBound, TRUE,
						End,

						Child, Label2("Position %"),
						Child, databuf.ls_flareposition = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
							MUIA_StringSlider_Min, -250,
							MUIA_StringSlider_Max, 250,
						End,

						Child, KeyLabel2("Function",'f'),
						Child, databuf.cy_flarefunction = CycleObject,
							MUIA_CycleChain, TRUE,
							MUIA_ControlChar, 'f',
							MUIA_Cycle_Entries, flare_functions,
						End,

						Child, VSpace(0), Child, VSpace(0),
					End,
					Child, VGroup,
						Child, databuf.flareview = (Object *)NewObject(CL_RequView->mcc_Class,NULL,
							MUIA_Background, MUII_BACKGROUND,
							TextFrame,
						End,
						Child, ColGroup(2),
							Child, databuf.b_flareadd = KeyTextButton("Add",'a'), TAG_DONE, End,
							Child, databuf.b_flareremove = KeyTextButton("Remove",'r'), TAG_DONE, End,
						End,
					End,
				End;
			databuf.gr_lightstar =
				HGroup,
					Child, VGroup, MUIA_Group_SameWidth, TRUE,
						Child, VGroup, GroupFrameT("Star"),
							Child, HGroup,
								Child, HGroup,
									Child, KeyLabel2("Enable",'e'), Child, databuf.cm_starenable = CheckMarkNew(FALSE,'e'), Child, HSpace(0),
								End,
								Child, HGroup,
									Child, KeyLabel2("Random spikes",'r'), Child, databuf.cm_starrandom = CheckMarkNew(FALSE,'r'), Child, HSpace(0),
								End,
							End,
							Child, ColGroup(2),
								Child, Label2("Radius %"),
								Child, databuf.ls_starradius = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
									MUIA_StringSlider_Min, 1,
									MUIA_StringSlider_Max, 100,
									MUIA_StringSlider_LowerBound, TRUE,
								End,

								Child, Label2("Spikes"),
								Child, databuf.ls_starspikes = SliderObject,
									MUIA_CycleChain, TRUE,
									MUIA_Slider_Min, 4,
									MUIA_Slider_Max, 100,
								End,

								Child, Label2("Range %"),
								Child, databuf.ls_starrange = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
									MUIA_StringSlider_Min, 1,
									MUIA_StringSlider_Max, 100,
									MUIA_StringSlider_LowerBound, TRUE,
									MUIA_StringSlider_UpperBound, TRUE,
								End,

								Child, Label2("Tilt"),
								Child, databuf.ls_startilt = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
									MUIA_StringSlider_Min, 0,
									MUIA_StringSlider_Max, 359,
									MUIA_StringSlider_LowerBound, TRUE,
									MUIA_StringSlider_UpperBound, TRUE,
								End,
							End,
							Child, HGroup,
								Child, databuf.fs_spikenoise = (Object*)NewObject(CL_Float->mcc_Class, NULL,
									MUIA_Float_Label, "Noise",
									MUIA_Float_ControlChar, 'n',
									MUIA_Float_Min, 0,
									MUIA_Float_LowerBound, TRUE,
								End,
								Child, databuf.fs_spikeintensity = (Object*)NewObject(CL_Float->mcc_Class, NULL,
									MUIA_Float_Label, "Intensity",
									MUIA_Float_ControlChar, 'i',
									MUIA_Float_Min, 0,
									MUIA_Float_LowerBound, TRUE,
								End,
							End,

							Child, HGroup,
								Child, Label2("Color"),
								Child, databuf.cf_starcolor = Colorfield,
							End,

							Child, VSpace(0),
						End,
						Child, VGroup, GroupFrameT("Ring"),
							Child, HGroup,
								Child, KeyLabel2("Enable",'n'), Child, databuf.cm_haloenable = CheckMarkNew(FALSE,'n'), Child, HSpace(0),
							End,

							Child, ColGroup(2),
								Child, Label2("Radius %"),
								Child, databuf.ls_haloradius = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
									MUIA_StringSlider_Min, 1,
									MUIA_StringSlider_Max, 100,
									MUIA_StringSlider_LowerBound, TRUE,
								End,

								Child, Label2("Inner %"),
								Child, databuf.ls_innerhaloradius = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
									MUIA_StringSlider_Min, 1,
									MUIA_StringSlider_Max, 100,
									MUIA_StringSlider_LowerBound, TRUE,
									MUIA_StringSlider_UpperBound, TRUE,
								End,
							End,

							Child, HGroup,
								Child, Label2("Color"),
								Child, databuf.cf_ringcolor = Colorfield,
							End,

							Child, VSpace(0),
						End,
					End,
					Child, databuf.starview = (Object *)NewObject(CL_RequView->mcc_Class,NULL,
						MUIA_Background, MUII_BACKGROUND,
						TextFrame,
					End,
				End;
			if(databuf.type == SETTINGS_SETSPOTLIGHT)
			{
				group =
					VGroup,
						Child, ColGroup(2),
							Child, Label2("Color"), Child, databuf.cf_color = Colorfield,
						End,
						Child, databuf.fs_radius = (Object*)NewObject(CL_Float->mcc_Class, NULL,
							MUIA_Float_Label, "Radius",
							MUIA_Float_ControlChar, 'r',
							MUIA_Float_Min, 0,
							MUIA_Float_LowerBound, TRUE,
							MUIA_Float_LowerInside, TRUE,
						End,
						Child, databuf.fs_angle = (Object*)NewObject(CL_Float->mcc_Class, NULL,
							MUIA_Float_Label, "Opening Angle",
							MUIA_Float_ControlChar, 'p',
							MUIA_Float_Min, 0,
							MUIA_Float_Max, 360,
							MUIA_Float_LowerBound, TRUE,
							MUIA_Float_UpperBound, TRUE,
						End,
						Child, ColGroup(2),
							Child, Label2("Soft spot %"),
							Child, databuf.ls_falloffradius = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
								MUIA_StringSlider_Min, 1,
								MUIA_StringSlider_Max, 100,
								MUIA_StringSlider_LowerBound, TRUE,
								MUIA_StringSlider_UpperBound, TRUE,
							End,
						End,
					End;
			}
			else
			{
				group =
					VGroup,
						Child, ColGroup(2),
							Child, Label2("Color"), Child, databuf.cf_color = Colorfield,
						End,
						Child, databuf.fs_radius = (Object*)NewObject(CL_Float->mcc_Class, NULL,
							MUIA_Float_Label, "Radius",
							MUIA_Float_ControlChar, 'r',
							MUIA_Float_Min, 0,
							MUIA_Float_LowerBound, TRUE,
							MUIA_Float_LowerInside, TRUE,
						End,
					End;
			}
			pages =
				RegisterGroup(LightSettingPages),
					Child, set,
					Child, VGroup,
						Child, HGroup,
							Child, group,
							Child, ColGroup(2),
								Child, KeyLabel2("Cast shadows",'c'), Child, databuf.cm_shadow = CheckMarkNew(FALSE,'c'),
								Child, KeyLabel2("Lens flares",'l'), Child, databuf.cm_lensflares = CheckMarkNew(FALSE,'l'),
								Child, KeyLabel2("Visible light",'v'), Child, databuf.cm_lightstar = CheckMarkNew(FALSE,'v'),
							End,
						End,
						Child, VGroup, GroupFrameT("Falloff"),
							Child, ColGroup(3),
								Child, databuf.cm_falloff = CheckMarkNew(FALSE,'a'), Child, KeyLabel2("Falloff from track distance",'a'), Child, HSpace(0),
							End,
							Child, databuf.fs_falloff = (Object*)NewObject(CL_Float->mcc_Class, NULL,
								MUIA_Float_Label, "Falloff",
								MUIA_Float_ControlChar, 'f',
								MUIA_Float_Min, 0,
								MUIA_Float_LowerBound, TRUE,
							End,
						End,
						Child, VGroup, GroupFrameT("Shadowmap"),
							Child, ColGroup(3),
								Child, databuf.cm_shadowmap = CheckMarkNew(FALSE,'e'), Child, KeyLabel2("Enable",'e'), Child, HSpace(0),
							End,
							Child, ColGroup(2),
								Child, KeyLabel2("Size",'i'),
								Child, databuf.cy_shadowmapsize = CycleObject,
									MUIA_CycleChain, TRUE,
									MUIA_ControlChar, 'i',
									MUIA_Cycle_Entries, shadowmap,
								End,
							End,
						End,
						Child, HGroup, GroupFrameT("Light files"),
							Child, databuf.b_load = KeyTextButton("Load",'l'), TAG_DONE, End,
							Child, databuf.b_save = KeyTextButton("Save",'s'), TAG_DONE, End,
						End,
					End,
					Child, databuf.gr_lensflares,
					Child, databuf.gr_lightstar,
				End;
			break;
		case SETTINGS_SETMESH:
			databuf.mesh = (MESH*)databuf.object;
			if(databuf.mesh->external == EXTERNAL_NONE)
			{
				pages =
					RegisterGroup(SettingPages),
						Child, set,
						Child, VGroup,
							Child, VSpace(0),
							Child, HGroup,
								Child, HSpace(0),
								Child, databuf.cm_nophong = CheckMarkNew(FALSE,'a'),
								Child, KeyLabel2("No phong shading", 'a'),
								Child, HSpace(0),
							End,
							Child, VSpace(0),
						End,
					End;
			}
			else
			{
				if((databuf.mesh->external == EXTERNAL_CHILD) ||
					(databuf.mesh->external == EXTERNAL_RSOB_CHILD))
				{
					pages = set;
				}
				else
				{
					pages =
						RegisterGroup(SettingPages),
							Child, set,
							Child, VGroup,
								Child, VSpace(0),
								Child, HGroup,
									Child, KeyLabel2("Object filename",'o'),
									Child, HGroup,
										Child, databuf.pf_file = PopaslObject,
											MUIA_CycleChain,TRUE,
											MUIA_Popstring_String, KeyString(NULL,256,'o'), End,
											MUIA_Popstring_Button, PopButton(MUII_PopFile),
											ASLFR_TitleText, "Please select an object...",
										End,
									End,
								End,
								Child, HGroup,
									Child, databuf.cm_apply = CheckMarkNew(FALSE,'a'),
									Child, KeyLabel2("Apply surface to childs", 'a'),
									Child, HSpace(0),
								End,
								Child, VSpace(0),
							End,
						End;
				}
			}
			break;
		case SETTINGS_SETCYLINDER:
			pages =
				RegisterGroup(SettingPages),
					Child, set,
					Child, VGroup,
						Child, VSpace(0),
						Child, ColGroup(4),
							Child, HSpace(0),
							Child, databuf.cm_closetop = CheckMarkNew(FALSE,'t'),
							Child, KeyLabel2("Close top", 't'),
							Child, HSpace(0),

							Child, HSpace(0),
							Child, databuf.cm_closebottom = CheckMarkNew(FALSE,'b'),
							Child, KeyLabel2("Close bottom", 'b'),
							Child, HSpace(0),
						End,
						Child, VSpace(0),
					End,
				End;
			break;
		case SETTINGS_SETCONE:
			pages =
				RegisterGroup(SettingPages),
					Child, set,
					Child, VGroup,
						Child, VSpace(0),
						Child, ColGroup(4),
							Child, HSpace(0),
							Child, databuf.cm_closebottom = CheckMarkNew(FALSE,'b'),
							Child, KeyLabel2("Close bottom", 'b'),
							Child, HSpace(0),
						End,
						Child, VSpace(0),
					End,
				End;
			break;
		case SETTINGS_SETCSG:
			databuf.csg = (CSG*)databuf.object;
			pages =
				RegisterGroup(SettingPages),
					Child, set,
					Child, VGroup,
						Child, VSpace(0),
						Child, ColGroup(4),
							Child, HSpace(0),
							Child, KeyLabel2("Operation",'o'),
							Child, databuf.cy_operation = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_ControlChar, 'o',
								MUIA_Cycle_Entries, csg,
							End,
							Child, HSpace(0),
						End,
						Child, VSpace(0),
					End,
				End;
			break;
		case SETTINGS_SETSOR:
			pages =
				RegisterGroup(SettingPages),
					Child, set,
					Child, VGroup,
						Child, VSpace(0),
						Child, ColGroup(4),
							Child, HSpace(0),
							Child, KeyLabel2("Close top", 't'),
							Child, databuf.cm_closetop = CheckMarkNew(FALSE,'t'),
							Child, HSpace(0),

							Child, HSpace(0),
							Child, KeyLabel2("Close bottom", 'b'),
							Child, databuf.cm_closebottom = CheckMarkNew(FALSE,'b'),
							Child, HSpace(0),

							Child, HSpace(0),
							Child, KeyLabel2("Calculate",'a'),
							Child, databuf.cy_sorcalc = CycleObject,
								MUIA_CycleChain, TRUE,
								MUIA_ControlChar, 'a',
								MUIA_Cycle_Entries, sor,
							End,
							Child, HSpace(0),
						End,
						Child, VSpace(0),
					End,
				End;
			break;
	}

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title,"Settings",
		MUIA_Window_ID,MAKE_ID('S','E','T','I'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, ColGroup(2),
				Child, Label("Name"),
				Child, databuf.name = StringObject,
					StringFrame,
					MUIA_CycleChain, TRUE,
					MUIA_String_MaxLen, 256,
				End,
			End,
			Child, pages,

			Child, HGroup, MUIA_Group_SameWidth, TRUE,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
			End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct Settings_Data*)INST_DATA(cl,obj);
		*data = databuf;
		data->undo = NULL;

		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

		data->originator = (Object *)GetTagData(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		data->draw_area = (Object *)GetTagData(MUIA_Settings_DrawArea,0,msg->ops_AttrList);
		data->display = (DISPLAY *)GetTagData(MUIA_Settings_Display,0,msg->ops_AttrList);
		data->track = data->object->track;
		data->obj = obj;
		if(data->track)
			SetAttrs(data->s_track, MUIA_String_Contents, data->track->GetName(), TAG_DONE);

		if(data->object->CSGAble())
			SetAttrs(data->cm_inverted, MUIA_Selected, data->object->flags & OBJECT_INVERTED, TAG_DONE);

		DoMethod(data->cm_poslocal, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 1, MUIM_Settings_PosLocal);
		DoMethod(data->cm_alignlocal, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 1, MUIM_Settings_AlignLocal);
		DoMethod(data->cm_sizelocal, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 1, MUIM_Settings_SizeLocal);

		// methods of track section
		DoMethod(data->pb_track,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_Settings_PopTrack);
		DoMethod(data->s_track,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
			obj,1,MUIM_Settings_StringTrack);

		// close methods
		DoMethod(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_Settings_Cancel);
		DoMethod(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			obj,1,MUIM_Settings_Cancel);

		// other methods
		DoMethod(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,obj,1,MUIM_Settings_Ok);

		switch(data->type)
		{
			case SETTINGS_SETCAMERA:
				data->camera = (CAMERA*)GetTagData(MUIA_Settings_Object,0,msg->ops_AttrList);
				break;
			case SETTINGS_SETPOINTLIGHT:
			case SETTINGS_SETSPOTLIGHT:
			case SETTINGS_SETDIRECTIONALLIGHT:
				data->light = (LIGHT*)GetTagData(MUIA_Settings_Object,0,msg->ops_AttrList);
				break;
			case SETTINGS_SETMESH:
				data->mesh = (MESH*)GetTagData(MUIA_Settings_Object,0,msg->ops_AttrList);
				break;
			case SETTINGS_SETCSG:
				data->csg = (CSG*)GetTagData(MUIA_Settings_Object,0,msg->ops_AttrList);
				break;
		}

		SetVector(&data->localpos, 0.f, 0.f, 0.f);
		SetVector(&data->localalign, 0.f, 0.f, 0.f);
		SetVector(&data->localsize, 1.f, 1.f, 1.f);

		data->flares = NULL;
		data->star = NULL;
		UpdateGadgets(data);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   change gadgets according to light data
 * INPUT:         data     settings data
 * OUTPUT:        -
 *************/
static void UpdateGadgets(struct Settings_Data *data)
{
	FLARE *flare, *newflare;
	int count;
	float f;

	switch(data->type)
	{
		case SETTINGS_SETCAMERA:
			if(!data->track)
				SetAttrs(data->cm_focustrack, MUIA_Disabled, TRUE, TAG_DONE);

			// set numeric fields
			SetAttrs(data->fs_fdist, MUIA_Float_Value, &data->camera->focaldist, TAG_DONE);
			SetAttrs(data->fs_apert, MUIA_Float_Value, &data->camera->aperture, TAG_DONE);
			f = atan(data->camera->hfov) * 2 * INV_PI_180;
			SetAttrs(data->fs_hfov, MUIA_Float_Value, &f, TAG_DONE);
			f = atan(data->camera->vfov) * 2 * INV_PI_180;
			SetAttrs(data->fs_vfov, MUIA_Float_Value, &f, TAG_DONE);
			if(data->camera->flags & OBJECT_CAMERA_VFOV)
				SetAttrs(data->fs_vfov, MUIA_Disabled, TRUE, TAG_DONE);

			// methods of focus section
			DoMethod(data->cm_focustrack,MUIM_Notify,MUIA_Selected,TRUE,
				data->fs_fdist,3,MUIM_Set,MUIA_Disabled,TRUE);
			DoMethod(data->cm_focustrack,MUIM_Notify,MUIA_Selected,FALSE,
				data->fs_fdist,3,MUIM_Set,MUIA_Disabled,FALSE);

			// set resolutions if an entry in the liestview is selected
			DoMethod(data->lv_lens,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,
				data->obj,1,MUIM_Settings_SetLens);

			// checkmark
			if(data->camera->flags & OBJECT_CAMERA_FOCUSTRACK)
			{
				SetAttrs(data->cm_focustrack, MUIA_Selected, TRUE, TAG_DONE);
				SetAttrs(data->fs_fdist, MUIA_Disabled, TRUE, TAG_DONE);
			}
			SetAttrs(data->cm_vfov,MUIA_Selected,data->camera->flags & OBJECT_CAMERA_VFOV, TAG_DONE);
			DoMethod(data->cm_vfov,MUIM_Notify,MUIA_Selected,TRUE,
				data->fs_vfov,3,MUIM_Set,MUIA_Disabled,TRUE);
			DoMethod(data->cm_vfov,MUIM_Notify,MUIA_Selected,FALSE,
				data->fs_vfov,3,MUIM_Set,MUIA_Disabled,FALSE);
//       SetAttrs(data->cm_fastdof,MUIA_Selected,data->camera->flags & OBJECT_CAMERA_FASTDOF, TAG_DONE);
			break;
		case SETTINGS_SETPOINTLIGHT:
		case SETTINGS_SETSPOTLIGHT:
		case SETTINGS_SETDIRECTIONALLIGHT:
			DoMethod(data->b_load, MUIM_Notify, MUIA_Pressed, FALSE,
				data->obj, 1, MUIM_Settings_LoadLight);
			DoMethod(data->b_save, MUIM_Notify, MUIA_Pressed, FALSE,
				data->obj, 1, MUIM_Settings_SaveLight);

			while(data->flares)
			{
				flare = (FLARE*)data->flares->GetNext();
				delete data->flares;
				data->flares = flare;
			}
			if(data->light->flares)
			{
				// duplicate flares
				count = 0;
				flare = data->light->flares;
				while(flare)
				{
					newflare = flare->Duplicate();
					if(!newflare)
						break;
					newflare->Append((SLIST**)&data->flares);
					flare = (FLARE*)flare->GetNext();
					count++;
				}

				SetAttrs(data->ls_flarenumber, MUIA_Slider_Max, count, TAG_DONE);

				if(data->light->flags & OBJECT_LIGHT_FLARES)
				{
					data->curflare = data->flares;
					data->curflare->selected = TRUE;
					UpdateFlareGadgets(data);
				}
			}

			if(data->star)
				delete data->star;
			if(data->light->star)
			{
				// duplicate star
				data->star = data->light->star->Duplicate();
				if(data->star)
				{
					if(data->light->flags & OBJECT_LIGHT_STAR)
						UpdateStarGadgets(data);
				}
			}

			// set color field
			SetColorField(data->cf_color, &data->light->color);

			// set numeric fields
			SetAttrs(data->fs_radius, MUIA_Float_Value, &data->light->r, TAG_DONE);
			SetAttrs(data->fs_falloff, MUIA_Float_Value, &data->light->falloff, TAG_DONE);
			if(!data->track)
				SetAttrs(data->cm_falloff, MUIA_Disabled, TRUE, TAG_DONE);
			if(data->light->flags & OBJECT_LIGHT_TRACKFALLOFF)
			{
				SetAttrs(data->cm_falloff, MUIA_Selected, TRUE, TAG_DONE);
				SetAttrs(data->fs_falloff, MUIA_Disabled, TRUE, TAG_DONE);
			}
			DoMethod(data->cm_falloff,MUIM_Notify,MUIA_Selected,TRUE,
				data->fs_falloff,3,MUIM_Set,MUIA_Disabled,TRUE);
			DoMethod(data->cm_falloff,MUIM_Notify,MUIA_Selected,FALSE,
				data->fs_falloff,3,MUIM_Set,MUIA_Disabled,FALSE);

			if(data->type == SETTINGS_SETSPOTLIGHT)
			{
				f = ((SPOT_LIGHT*)(data->light))->angle * INV_PI_180;
				SetAttrs(data->fs_angle, MUIA_Float_Value, &f, TAG_DONE);
				f = ((SPOT_LIGHT*)(data->light))->falloff_radius * 100.f;
				SetAttrs(data->ls_falloffradius, MUIA_StringSlider_Value, &f, TAG_DONE);
			}

			switch(data->light->shadowmapsize)
			{
				case 256:
					SetAttrs(data->cy_shadowmapsize, MUIA_Cycle_Active, 1, TAG_DONE);
					break;
				case 512:
					SetAttrs(data->cy_shadowmapsize, MUIA_Cycle_Active, 2, TAG_DONE);
					break;
				case 768:
					SetAttrs(data->cy_shadowmapsize, MUIA_Cycle_Active, 3, TAG_DONE);
					break;
				default:
					SetAttrs(data->cy_shadowmapsize, MUIA_Cycle_Active, 0, TAG_DONE);
					break;
			}

			// set checkmark
			SetAttrs(data->cm_shadow, MUIA_Selected, data->light->flags & OBJECT_LIGHT_SHADOW, TAG_DONE);
			if(data->light->flags & OBJECT_LIGHT_FLARES)
				SetAttrs(data->cm_lensflares, MUIA_Selected, TRUE, TAG_DONE);
			else
				SetAttrs(data->gr_lensflares, MUIA_Disabled, TRUE, TAG_DONE);
			DoMethod(data->cm_lensflares, MUIM_Notify, MUIA_Selected, TRUE,
				data->obj, 1, MUIM_Settings_EnableFlares);
			DoMethod(data->cm_lensflares, MUIM_Notify, MUIA_Selected, FALSE,
				data->gr_lensflares, 3, MUIM_Set, MUIA_Disabled, TRUE);

			if(data->light->flags & OBJECT_LIGHT_STAR)
				SetAttrs(data->cm_lightstar, MUIA_Selected, TRUE, TAG_DONE);
			else
				SetAttrs(data->gr_lightstar, MUIA_Disabled, TRUE, TAG_DONE);
			DoMethod(data->cm_lightstar, MUIM_Notify, MUIA_Selected, TRUE,
				data->obj, 1, MUIM_Settings_EnableStar);
			DoMethod(data->cm_lightstar, MUIM_Notify, MUIA_Selected, FALSE,
				data->gr_lightstar, 3, MUIM_Set, MUIA_Disabled, TRUE);

			SetAttrs(data->cm_shadowmap, MUIA_Selected, data->light->flags & OBJECT_LIGHT_SHADOWMAP, TAG_DONE);

			// open color window if someone click on colofields
			DoMethod(data->cf_color,MUIM_Notify,MUIA_Pressed,FALSE,
				data->obj,2,MUIM_Settings_OpenColor,data->cf_color);

			// lens flare data
			SetAttrs(data->flareview,
				MUIA_RequView_Display, data->display,
				MUIA_RequView_Flares, data->flares,
				TAG_DONE);
			// lens flare methods
			DoMethod(data->b_flareadd,MUIM_Notify,MUIA_Pressed,FALSE,
				data->obj,1,MUIM_Settings_AddFlare);
			DoMethod(data->b_flareremove,MUIM_Notify,MUIA_Pressed,FALSE,
				data->obj,1,MUIM_Settings_RemoveFlare);

			DoMethod(data->ls_flarenumber, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_FlareNumber);
			DoMethod(data->ls_flareradius, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_FlareUpdate);
			DoMethod(data->ls_flareradius, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_FlareUpdate);
			DoMethod(data->ls_flareposition, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_FlareUpdate);
			DoMethod(data->ls_flareposition, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_FlareUpdate);
			DoMethod(data->ls_flaretilt, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_FlareUpdate);
			DoMethod(data->ls_flaretilt, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_FlareUpdate);
			DoMethod(data->cy_flaretype, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_FlareUpdate);

			DoMethod(data->cf_flarecolor,MUIM_Notify,MUIA_Pressed,FALSE,
				data->obj,2,MUIM_Settings_OpenColor,data->cf_flarecolor);

			// star data
			SetAttrs(data->starview,
				MUIA_RequView_Display, data->display,
				MUIA_RequView_Star, data->star,
				TAG_DONE);

			DoMethod(data->ls_starradius, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->ls_starradius, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);

			DoMethod(data->ls_startilt, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->ls_startilt, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);

			DoMethod(data->fs_spikenoise, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->fs_spikeintensity, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);

			DoMethod(data->ls_haloradius, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->ls_haloradius, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);

			DoMethod(data->ls_innerhaloradius, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->ls_innerhaloradius, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);

			DoMethod(data->cm_starenable, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->cm_starrandom, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->cm_haloenable, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->ls_starspikes, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->ls_starrange, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);
			DoMethod(data->ls_starrange, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				data->obj, 1, MUIM_Settings_StarUpdate);

			DoMethod(data->cf_starcolor, MUIM_Notify, MUIA_Pressed, FALSE,
				data->obj, 2, MUIM_Settings_OpenColor, data->cf_starcolor);
			DoMethod(data->cf_ringcolor, MUIM_Notify, MUIA_Pressed, FALSE,
				data->obj, 2, MUIM_Settings_OpenColor, data->cf_ringcolor);
			break;
		case SETTINGS_SETMESH:
			if((data->mesh->external == EXTERNAL_ROOT) || (data->mesh->external == EXTERNAL_RSOB_ROOT))
			{
				SetAttrs(data->cm_apply, MUIA_Selected, data->mesh->flags & OBJECT_APPLYSURF, TAG_DONE);
				SetAttrs(data->pf_file, MUIA_String_Contents, data->mesh->file, TAG_DONE);
			}
			if(data->mesh->external == EXTERNAL_NONE)
				SetAttrs(data->cm_nophong, MUIA_Selected, data->mesh->flags & OBJECT_NOPHONG, TAG_DONE);
			break;
		case SETTINGS_SETCYLINDER:
			SetAttrs(data->cm_closetop, MUIA_Selected, !(data->object->flags & OBJECT_OPENTOP), TAG_DONE);
			SetAttrs(data->cm_closebottom, MUIA_Selected, !(data->object->flags & OBJECT_OPENBOTTOM), TAG_DONE);
			break;
		case SETTINGS_SETCONE:
			SetAttrs(data->cm_closebottom, MUIA_Selected, !(data->object->flags & OBJECT_OPENBOTTOM), TAG_DONE);
			break;
		case SETTINGS_SETCSG:
			SetAttrs(data->cy_operation, MUIA_Cycle_Active, data->csg->operation, TAG_DONE);
			break;
		case SETTINGS_SETSOR:
			SetAttrs(data->cm_closetop, MUIA_Selected, !(data->object->flags & OBJECT_OPENTOP), TAG_DONE);
			SetAttrs(data->cm_closebottom, MUIA_Selected, !(data->object->flags & OBJECT_OPENBOTTOM), TAG_DONE);
			SetAttrs(data->cy_sorcalc, MUIA_Cycle_Active, data->object->flags & OBJECT_SOR_ACCURATE ? 1 : 0, TAG_DONE);
			break;
	}
}

/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG mSet(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;
	VECTOR *v;
	char buf[31];

	tags=msg->ops_AttrList;
	do
	{
		tag=NextTagItem(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_Settings_Name:
					if (tag->ti_Data)
						SetAttrs(data->name,MUIA_String_Contents,(char*)tag->ti_Data, TAG_DONE);
					break;
				case MUIA_Settings_Pos:
					if (tag->ti_Data)
					{
						v = (VECTOR*)tag->ti_Data;
						data->pos = *v;
						Float2String(v->x,buf);
						SetAttrs(data->posx,MUIA_String_Contents,buf, TAG_DONE);
						Float2String(v->y,buf);
						SetAttrs(data->posy,MUIA_String_Contents,buf, TAG_DONE);
						Float2String(v->z,buf);
						SetAttrs(data->posz,MUIA_String_Contents,buf, TAG_DONE);
					}
					break;
				case MUIA_Settings_Align:
					if (tag->ti_Data)
					{
						v = (VECTOR*)tag->ti_Data;
						data->align = *v;
						Float2String(v->x,buf);
						SetAttrs(data->alignx,MUIA_String_Contents,buf, TAG_DONE);
						Float2String(v->y,buf);
						SetAttrs(data->aligny,MUIA_String_Contents,buf, TAG_DONE);
						Float2String(v->z,buf);
						SetAttrs(data->alignz,MUIA_String_Contents,buf, TAG_DONE);
					}
					break;
				case MUIA_Settings_Size:
					if (tag->ti_Data)
					{
						v = (VECTOR*)tag->ti_Data;
						data->size = *v;
						SetAttrs(data->sizex, MUIA_Float_Value, &v->x, TAG_DONE);
						SetAttrs(data->sizey, MUIA_Float_Value, &v->y, TAG_DONE);
						SetAttrs(data->sizez, MUIA_Float_Value, &v->z, TAG_DONE);
					}
					break;
				case MUIA_Settings_FlareNumber:
					SetAttrs(data->ls_flarenumber, MUIA_Slider_Level, tag->ti_Data, TAG_DONE);
					break;
				case MUIA_Settings_Undo:
					data->undo = (UNDO_TRANSFORM*)tag->ti_Data;
					break;
			}
		}
	}
	while(tag);

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	VECTOR *v;
	char *buf;

	switch (msg->opg_AttrID)
	{
		case MUIA_Settings_Name:
			GetAttr(MUIA_String_Contents,data->name,(ULONG*)msg->opg_Storage);
			break;
		case MUIA_Settings_PosLocal:
			GetAttr(MUIA_Selected, data->cm_poslocal, (ULONG*)(msg->opg_Storage));
			break;
		case MUIA_Settings_AlignLocal:
			GetAttr(MUIA_Selected, data->cm_alignlocal, (ULONG*)(msg->opg_Storage));
			break;
		case MUIA_Settings_SizeLocal:
			GetAttr(MUIA_Selected, data->cm_sizelocal, (ULONG*)(msg->opg_Storage));
			break;
		case MUIA_Settings_Pos:
			v = (VECTOR*)(msg->opg_Storage);
			GetAttr(MUIA_String_Contents,data->posx,(ULONG*)&buf);
			v->x = atof(buf);
			GetAttr(MUIA_String_Contents,data->posy,(ULONG*)&buf);
			v->y = atof(buf);
			GetAttr(MUIA_String_Contents,data->posz,(ULONG*)&buf);
			v->z = atof(buf);
			return TRUE;
		case MUIA_Settings_Align:
			v = (VECTOR*)(msg->opg_Storage);
			GetAttr(MUIA_String_Contents,data->alignx,(ULONG*)&buf);
			v->x = atof(buf);
			GetAttr(MUIA_String_Contents,data->aligny,(ULONG*)&buf);
			v->y = atof(buf);
			GetAttr(MUIA_String_Contents,data->alignz,(ULONG*)&buf);
			v->z = atof(buf);
			return TRUE;
		case MUIA_Settings_Size:
			v = (VECTOR*)(msg->opg_Storage);
			GetAttr(MUIA_Float_Value, data->sizex, (ULONG*)&v->x);
			GetAttr(MUIA_Float_Value, data->sizey, (ULONG*)&v->y);
			GetAttr(MUIA_Float_Value, data->sizez, (ULONG*)&v->z);
			return TRUE;
		case MUIA_Settings_Object:
			*(OBJECT**)(msg->opg_Storage) = data->object;
			return TRUE;
		case MUIA_Settings_Undo:
			*(UNDO_TRANSFORM**)(msg->opg_Storage) = data->undo;
			return TRUE;
	}

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   this function is called if the user clicks on the track
 *    popup button and opens a browser window
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG PopTrack(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data*)INST_DATA(cl,obj);
	Object *newobj;

	// create browser window
	newobj = (Object*)NewObject(CL_Browser->mcc_Class,NULL,
		MUIA_ChildWindow_Originator,obj,
		MUIA_Browser_Mode,BROWSER_ALL,
		MUIA_Browser_DrawArea, data->draw_area,
		MUIA_Browser_DontSelect, TRUE,
		TAG_DONE);
	if(newobj)
	{
		DoMethod(app,OM_ADDMEMBER,newobj);
		SetSleep(TRUE);
		SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
	}

	return 0;
}

/*************
 * DESCRIPTION:   this function is called everytime the user presses the return
 *    key in the track string gadget
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG StringTrack(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data*)INST_DATA(cl,obj);
	OBJECT *object;
	char *str;

	GetAttr(MUIA_String_Contents, data->s_track, (ULONG*)&str);

	if(strlen(str))
	{
		object = GetObjectByName(str);
		if(!object)
		{
			MUI_Request(app,NULL,0,NULL,"*Ok",errors[ERR_TRACKNOTFOUND]);
		}
		else
		{
			if(object == data->object)
				MUI_Request(app,NULL,0,NULL,"*Ok",errors[ERR_TRACKTOSELF]);
			else
			{
				switch(data->type)
				{
					case SETTINGS_SETCAMERA:
						SetAttrs(data->cm_focustrack, MUIA_Disabled, FALSE, TAG_DONE);
						data->track = object;
						break;
					case SETTINGS_SETSPOTLIGHT:
					case SETTINGS_SETPOINTLIGHT:
					case SETTINGS_SETDIRECTIONALLIGHT:
						SetAttrs(data->cm_falloff, MUIA_Disabled, FALSE, TAG_DONE);
						data->track = object;
						break;
					default:
						data->track = object;
						break;
				}
			}
		}
	}
	else
	{
		switch(data->type)
		{
			case SETTINGS_SETCAMERA:
				SetAttrs(data->cm_focustrack, MUIA_Disabled, TRUE, TAG_DONE);
				break;
			case SETTINGS_SETSPOTLIGHT:
			case SETTINGS_SETPOINTLIGHT:
			case SETTINGS_SETDIRECTIONALLIGHT:
				SetAttrs(data->cm_falloff, MUIA_Disabled, TRUE, TAG_DONE);
				break;
		}
		data->track = NULL;
	}

	return 0;
}

/*************
 * DESCRIPTION:   set lens parameters according to list view
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG SetLens(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data*)INST_DATA(cl,obj);
	char buffer[30];
	ULONG active;

	GetAttr(MUIA_List_Active,data->lv_lens,&active);

	Float2String((float)(lensvals[active][0]), buffer);
	SetAttrs(data->fs_hfov, MUIA_String_Contents, buffer, TAG_DONE);

	Float2String((float)(lensvals[active][1]), buffer);
	SetAttrs(data->fs_vfov, MUIA_String_Contents, buffer, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   is called when browser window is finished
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Browser_Finish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	Settings_Data *data = (Settings_Data *)INST_DATA(cl,obj);
	Object *win;
	OBJECT *object;

	win = msg->win;

	GetAttr(MUIA_Browser_ActiveObject, win, (ULONG*)&object);
	SetAttrs(win,MUIA_Window_Open,FALSE, TAG_DONE);

	if(object && msg->ok)
	{
		if(object == data->object)
			MUI_Request(app,NULL,0,NULL,"*Ok",errors[ERR_TRACKTOSELF]);
		else
		{
			data->track = object;
			SetAttrs(data->s_track, MUIA_String_Contents, object->GetName(), TAG_DONE);

			switch(data->type)
			{
				case SETTINGS_SETCAMERA:
					SetAttrs(data->cm_focustrack, MUIA_Disabled, FALSE, TAG_DONE);
					break;
				case SETTINGS_SETPOINTLIGHT:
				case SETTINGS_SETSPOTLIGHT:
				case SETTINGS_SETDIRECTIONALLIGHT:
					SetAttrs(data->cm_falloff, MUIA_Disabled, FALSE, TAG_DONE);
					break;
			}
		}
	}

	DoMethod(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);
	SetSleep(FALSE);

	return 0;
}

/*************
 * DESCRIPTION:   open color pop up window
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG OpenColor(struct IClass *cl,Object *obj,struct MUIP_Object *msg)
{
	Object *newobj;

	// create a color window
	newobj = (Object*)NewObject(CL_Color->mcc_Class,NULL,
		MUIA_ChildWindow_Originator,obj,
		MUIA_Colorfield,msg->obj,
		TAG_DONE);
	if(newobj)
	{
		DoMethod(app,OM_ADDMEMBER,newobj);
		SetSleep(TRUE);
		SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
	}
	return 0;
}

/*************
 * DESCRIPTION:   close color pop up window
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG CloseColor(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	Object *win;

	win = msg->win;
	SetAttrs(win,MUIA_Window_Open,FALSE, TAG_DONE);

	DoMethod(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);

	SetSleep(FALSE);

	return 0;
}

/*************
 * DESCRIPTION:   read data from flares section and change paramters of current flare
 * INPUT:         data     settings data
 * OUTPUT:        -
 *************/
static void UpdateFlare(struct Settings_Data *data)
{
	ULONG num;
	float v, tilt;
	FLARE *flare;

	// set color field
	GetColorField(data->cf_flarecolor, &data->curflare->color);

	GetAttr(MUIA_StringSlider_Value, data->ls_flareposition, (ULONG*)&v);
	data->curflare->pos = v*0.01;
	GetAttr(MUIA_StringSlider_Value ,data->ls_flareradius, (ULONG*)&v);
	data->curflare->radius = v*0.01;
	GetAttr(MUIA_StringSlider_Value, data->ls_flaretilt, (ULONG*)&v);
	tilt = v*PI_180;

	GetAttr(MUIA_Cycle_Active,data->cy_flaretype,&num);

	flare = data->flares;
	while(flare)
	{
		flare->tilt = tilt;
		if(num == 0)
		{
			flare->type = FLARE_DISC;
		}
		else
		{
			flare->type = FLARE_POLY;
			flare->edges = num+2;
		}
		flare = (FLARE*)flare->GetNext();
	}


	GetAttr(MUIA_Cycle_Active, data->cy_flarefunction, &num);
	data->curflare->func = num;
}

/*************
 * DESCRIPTION:   change flare gadgets according to 'curflare' data
 * INPUT:         data     settings data
 * OUTPUT:        -
 *************/
static void UpdateFlareGadgets(struct Settings_Data *data)
{
	ULONG v;
	float value;

	// set color field
	SetColorField(data->cf_flarecolor, &data->curflare->color);

	// set numeric fields
	value = data->curflare->radius*100.f;
	SetAttrs(data->ls_flareradius,
		MUIA_NoNotify, TRUE,
		MUIA_StringSlider_Value, &value,
		TAG_DONE);
	value = data->curflare->pos*100.f;
	SetAttrs(data->ls_flareposition,
		MUIA_NoNotify, TRUE,
		MUIA_StringSlider_Value, &value,
		TAG_DONE);
	value = data->curflare->tilt*INV_PI_180;
	SetAttrs(data->ls_flaretilt,
		MUIA_NoNotify, TRUE,
		MUIA_StringSlider_Value, &value,
		TAG_DONE);

	if(data->curflare->type == FLARE_DISC)
		v = 0;
	else
		v = data->curflare->edges - 2;
	SetAttrs(data->cy_flaretype,
		MUIA_NoNotify, TRUE,
		MUIA_Cycle_Active, v,
		TAG_DONE);

	SetAttrs(data->cy_flarefunction,
		MUIA_NoNotify, TRUE,
		MUIA_Cycle_Active, data->curflare->func,
		TAG_DONE);
}

/*************
 * DESCRIPTION:   read data from star section and change paramters of current star
 * INPUT:         data     settings data
 * OUTPUT:        -
 *************/
static void UpdateStar(struct Settings_Data *data)
{
	ULONG num;
	float v;
	char *s;

	GetColorField(data->cf_starcolor, &data->star->starcolor);
	GetColorField(data->cf_ringcolor, &data->star->ringcolor);

	GetAttr(MUIA_StringSlider_Value,data->ls_starradius,(ULONG*)&v);
	data->star->starradius = v*0.01;
	GetAttr(MUIA_StringSlider_Value,data->ls_startilt,(ULONG*)&v);
	data->star->tilt = v*PI_180;
	GetAttr(MUIA_StringSlider_Value,data->ls_haloradius,(ULONG*)&v);
	data->star->haloradius = v*0.01;
	GetAttr(MUIA_StringSlider_Value,data->ls_innerhaloradius,(ULONG*)&v);
	data->star->innerhaloradius = v*0.01;

	GetAttr(MUIA_Slider_Level,data->ls_starspikes,&num);
	if(data->star->spikes != num)
		data->star->SetSpikes(num);

	GetAttr(MUIA_String_Contents, data->fs_spikenoise, (ULONG*)&s);
	data->star->noise = atof(s);
	GetAttr(MUIA_String_Contents, data->fs_spikeintensity, (ULONG*)&s);
	data->star->brightness = atof(s);

	GetAttr(MUIA_StringSlider_Value,data->ls_starrange,(ULONG*)&v);
	if(data->starrange != v)
	{
		data->star->range = v*0.01;
		data->starrange = v;
		data->star->SetSpikes(data->star->spikes);
	}

	GetAttr(MUIA_Selected, data->cm_starenable, &num);
	if(num)
		data->star->flags |= STAR_ENABLE;
	else
		data->star->flags &= ~STAR_ENABLE;

	GetAttr(MUIA_Selected, data->cm_haloenable, &num);
	if(num)
		data->star->flags |= STAR_HALOENABLE;
	else
		data->star->flags &= ~STAR_HALOENABLE;

	GetAttr(MUIA_Selected, data->cm_starrandom, &num);
	if(num && !(data->star->flags & STAR_RANDOM))
	{
		data->star->flags |= STAR_RANDOM;
		data->star->SetSpikes(data->star->spikes);
	}
	if(!num && (data->star->flags & STAR_RANDOM))
	{
		data->star->flags &= ~STAR_RANDOM;
		data->star->SetSpikes(data->star->spikes);
	}
}

/*************
 * DESCRIPTION:   change star gadgets according to 'star' data
 * INPUT:         data     settings data
 * OUTPUT:        -
 *************/
static void UpdateStarGadgets(struct Settings_Data *data)
{
	float v;
	char buffer[32];

	SetColorField(data->cf_starcolor, &data->star->starcolor);
	SetColorField(data->cf_ringcolor, &data->star->ringcolor);

	// set numeric fields
	v = data->star->starradius*100.;
	SetAttrs(data->ls_starradius,
		MUIA_NoNotify, TRUE,
		MUIA_StringSlider_Value, &v,
		TAG_DONE);
	v = data->star->tilt*INV_PI_180;
	SetAttrs(data->ls_startilt,
		MUIA_NoNotify, TRUE,
		MUIA_StringSlider_Value, &v,
		TAG_DONE);
	v = data->star->haloradius*100.;
	SetAttrs(data->ls_haloradius,
		MUIA_NoNotify, TRUE,
		MUIA_StringSlider_Value, &v,
		TAG_DONE);
	v = data->star->innerhaloradius*100.;
	SetAttrs(data->ls_innerhaloradius,
		MUIA_NoNotify, TRUE,
		MUIA_StringSlider_Value, &v,
		TAG_DONE);
	v = data->star->range*100.;
	SetAttrs(data->ls_starrange,
		MUIA_NoNotify, TRUE,
		MUIA_StringSlider_Value, &v,
		TAG_DONE);

	SetAttrs(data->ls_starspikes,
		MUIA_NoNotify, TRUE,
		MUIA_Slider_Level, (ULONG)data->star->spikes,
		TAG_DONE);
	Float2String(data->star->noise, buffer);
	SetAttrs(data->fs_spikenoise, MUIA_String_Contents, buffer, TAG_DONE);
	Float2String(data->star->brightness, buffer);
	SetAttrs(data->fs_spikeintensity, MUIA_String_Contents, buffer, TAG_DONE);

	SetAttrs(data->cm_starenable,
		MUIA_NoNotify, TRUE,
		MUIA_Selected, data->star->flags & STAR_ENABLE,
		TAG_DONE);
	SetAttrs(data->cm_haloenable,
		MUIA_NoNotify, TRUE,
		MUIA_Selected, data->star->flags & STAR_HALOENABLE,
		TAG_DONE);
	SetAttrs(data->cm_starrandom,
		MUIA_NoNotify, TRUE,
		MUIA_Selected, data->star->flags & STAR_RANDOM,
		TAG_DONE);
}

/*************
 * DESCRIPTION:   ok button pressed -> accept new preferences
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	OBJECT *object;
	char *s;
	ULONG flags, num;
	FLARE *flare, *next;
	float f;

	// get the track object
	GetAttr(MUIA_String_Contents, data->s_track, (ULONG*)&s);

	if(strlen(s))
	{
		object = GetObjectByName(s);
		if(!object)
			MUI_Request(app,NULL,0,NULL,"*Ok",errors[ERR_TRACKNOTFOUND]);
		else
		{
			if(object == data->object)
				MUI_Request(app,NULL,0,NULL,"*Ok",errors[ERR_TRACKTOSELF]);
		}
		data->object->track = object;
		object->istracked = TRUE;
		data->object->UpdateTracking(&data->object->pos);
	}
	else
		data->object->track = NULL;

	if(data->object->CSGAble())
	{
		GetAttr(MUIA_Selected,data->cm_inverted,&flags);
		if(flags)
			data->object->flags |= OBJECT_INVERTED;
		else
			data->object->flags &= ~OBJECT_INVERTED;
	}

	switch(data->type)
	{
		case SETTINGS_SETCAMERA:
			// set numeric values
			GetAttr(MUIA_Float_Value, data->fs_fdist, (ULONG*)&data->camera->focaldist);
			GetAttr(MUIA_Float_Value, data->fs_apert, (ULONG*)&data->camera->aperture);
			GetAttr(MUIA_Float_Value, data->fs_hfov, (ULONG*)&f);
			data->camera->hfov = tan(f * .5 * PI_180);
			GetAttr(MUIA_Float_Value, data->fs_vfov, (ULONG*)&f);
			data->camera->vfov = tan(f * .5 * PI_180);

			// check marks
			GetAttr(MUIA_Selected,data->cm_focustrack,&flags);
			if(flags)
				data->camera->flags |= OBJECT_CAMERA_FOCUSTRACK;
			else
				data->camera->flags &= ~OBJECT_CAMERA_FOCUSTRACK;
			GetAttr(MUIA_Selected,data->cm_vfov,&flags);
			if(flags)
				data->camera->flags |= OBJECT_CAMERA_VFOV;
			else
				data->camera->flags &= ~OBJECT_CAMERA_VFOV;
/*       GetAttr(MUIA_Selected,data->cm_fastdof,&flags);
			if(flags)
				data->camera->flags |= OBJECT_CAMERA_FASTDOF;
			else
				data->camera->flags &= ~OBJECT_CAMERA_FASTDOF;*/

			break;
		case SETTINGS_SETPOINTLIGHT:
		case SETTINGS_SETSPOTLIGHT:
		case SETTINGS_SETDIRECTIONALLIGHT:
			// set numeric values
			GetAttr(MUIA_Float_Value, data->fs_radius, (ULONG*)&data->light->r);
			GetAttr(MUIA_Float_Value, data->fs_falloff, (ULONG*)&data->light->falloff);
			GetAttr(MUIA_Selected,data->cm_falloff,&flags);
			if(flags)
				data->light->flags |= OBJECT_LIGHT_TRACKFALLOFF;
			else
				data->light->flags &= ~OBJECT_LIGHT_TRACKFALLOFF;

			if(data->type == SETTINGS_SETSPOTLIGHT)
			{
				GetAttr(MUIA_Float_Value, data->fs_angle, (ULONG*)&f);
				((SPOT_LIGHT*)(data->light))->angle = f * PI_180;
				GetAttr(MUIA_StringSlider_Value, data->ls_falloffradius, (ULONG*)&f);
				((SPOT_LIGHT*)(data->light))->falloff_radius = f * 0.01f;
			}

			// set color
			GetColorField(data->cf_color,&data->light->color);

			// shadowmap
			GetAttr(MUIA_Selected,data->cm_shadowmap,&flags);
			if(flags)
				data->light->flags |= OBJECT_LIGHT_SHADOWMAP;
			else
				data->light->flags &= ~OBJECT_LIGHT_SHADOWMAP;
			GetAttr(MUIA_Cycle_Active, data->cy_shadowmapsize, &num);
			switch(num)
			{
				case 0:
					data->light->shadowmapsize = 128;
					break;
				case 1:
					data->light->shadowmapsize = 256;
					break;
				case 2:
					data->light->shadowmapsize = 512;
					break;
				case 3:
					data->light->shadowmapsize = 768;
					break;
			}

			// set shadow
			GetAttr(MUIA_Selected,data->cm_shadow,&flags);
			if(flags)
				data->light->flags |= OBJECT_LIGHT_SHADOW;
			else
				data->light->flags &= ~OBJECT_LIGHT_SHADOW;

			GetAttr(MUIA_Selected,data->cm_lensflares,&flags);
			if(flags)
				data->light->flags |= OBJECT_LIGHT_FLARES;
			else
				data->light->flags &= ~OBJECT_LIGHT_FLARES;

			GetAttr(MUIA_Selected,data->cm_lightstar,&flags);
			if(flags)
				data->light->flags |= OBJECT_LIGHT_STAR;
			else
				data->light->flags &= ~OBJECT_LIGHT_STAR;

			GetAttr(MUIA_Selected, data->cm_shadowmap, &flags);
			if(flags)
				data->light->flags |= OBJECT_LIGHT_SHADOWMAP;
			else
				data->light->flags &= ~OBJECT_LIGHT_SHADOWMAP;

			if(data->light->flags & OBJECT_LIGHT_FLARES)
				UpdateFlare(data);

			// remove old flares form light and copy changed flares to light
			flare = data->light->flares;
			while(flare)
			{
				next = (FLARE*)flare->GetNext();
				delete flare;
				flare = next;
			}
			data->light->flares = data->flares;
			flare = data->light->flares;
			while(flare)
			{
				flare->selected = FALSE;
				flare = (FLARE*)flare->GetNext();
			}

			if(data->light->star)
				delete data->light->star;
			data->light->star = data->star;
			break;
		case SETTINGS_SETMESH:
			if(data->mesh->external == EXTERNAL_ROOT)
			{
				GetAttr(MUIA_Selected, data->cm_apply, &flags);
				if(flags)
					data->mesh->flags |= OBJECT_APPLYSURF;
				else
					data->mesh->flags &= ~OBJECT_APPLYSURF;

				GetAttr(MUIA_String_Contents, data->pf_file, (ULONG*)&s);

				if(strlen(s))
					data->mesh->SetFileName(s);
				else
				{
					if(data->mesh->file)
						delete data->mesh->file;
					data->mesh->file = NULL;
				}
			}
			if(data->mesh->external == EXTERNAL_NONE)
			{
				GetAttr(MUIA_Selected, data->cm_nophong, &flags);
				if(flags)
					data->mesh->flags |= OBJECT_NOPHONG;
				else
					data->mesh->flags &= ~OBJECT_NOPHONG;
			}
			break;
		case SETTINGS_SETCYLINDER:
			GetAttr(MUIA_Selected, data->cm_closetop, &flags);
			if(!flags)
				data->object->flags |= OBJECT_OPENTOP;
			else
				data->object->flags &= ~OBJECT_OPENTOP;
			GetAttr(MUIA_Selected, data->cm_closebottom, &flags);
			if(!flags)
				data->object->flags |= OBJECT_OPENBOTTOM;
			else
				data->object->flags &= ~OBJECT_OPENBOTTOM;
			break;
		case SETTINGS_SETCONE:
			GetAttr(MUIA_Selected, data->cm_closebottom, &flags);
			if(!flags)
				data->object->flags |= OBJECT_OPENBOTTOM;
			else
				data->object->flags &= ~OBJECT_OPENBOTTOM;
			break;
		case SETTINGS_SETCSG:
			GetAttr(MUIA_Cycle_Active, data->cy_operation, &num);
			data->csg->operation = num;
			break;
		case SETTINGS_SETSOR:
			GetAttr(MUIA_Selected, data->cm_closetop, &flags);
			if(!flags)
				data->object->flags |= OBJECT_OPENTOP;
			else
				data->object->flags &= ~OBJECT_OPENTOP;
			GetAttr(MUIA_Selected, data->cm_closebottom, &flags);
			if(!flags)
				data->object->flags |= OBJECT_OPENBOTTOM;
			else
				data->object->flags &= ~OBJECT_OPENBOTTOM;
			GetAttr(MUIA_Cycle_Active, data->cy_sorcalc, &num);
			if(num)
				data->object->flags |= OBJECT_SOR_ACCURATE;
			else
				data->object->flags &= ~OBJECT_SOR_ACCURATE;
			break;
	}

	DoMethod(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_Settings_Finish, obj, TRUE);

	return 0;
}

/*************
 * DESCRIPTION:   cancel button pressed -> remove temporary flare list
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Cancel(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	FLARE *flare, *next;

	if((data->type == SETTINGS_SETPOINTLIGHT) ||
		(data->type == SETTINGS_SETSPOTLIGHT) ||
		(data->type == SETTINGS_SETDIRECTIONALLIGHT))
	{
		// remove flares
		flare = data->flares;
		while(flare)
		{
			next = (FLARE*)flare->GetNext();
			delete flare;
			flare = next;
		}
		delete data->star;
	}

	DoMethod(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_Settings_Finish, obj, FALSE);

	return 0;
}

/*************
 * DESCRIPTION:   This functions is called whenever the user enables the 'Lens flares'
 *    checkmark. It enables the lens flare settings page and add default lensflares if
 *    the light does'nt own any.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG EnableFlares(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	FLARE *flare;
	int count;

	// generate default flares when no flares are given
	if (!data->flares)
	{
		if(!data->light->SetDefaultFlares(&data->flares))
		{
			DisplayBeep(_screen(obj));
			return 0;
		}
	}

	// count amount of flares
	count = 0;
	flare = data->flares;
	while (flare)
	{
		count++;
		flare = (FLARE *)flare->GetNext();
	}

	data->curflare = data->flares;
	data->curflare->selected = TRUE;

	SetAttrs(data->ls_flarenumber,
		MUIA_Slider_Max, count,
		MUIA_Slider_Level, 0,
		TAG_DONE);

	SetAttrs(data->gr_lensflares, MUIA_Disabled, FALSE, TAG_DONE);
	SetAttrs(data->flareview, MUIA_RequView_Flares, data->flares, TAG_DONE);

	UpdateFlareGadgets(data);

	return NULL;
}

/*************
 * DESCRIPTION:   This functions is called whenever the user presses the 'Add' button
 *    in the 'Lens flares' page. It appends a lensflare to the lensflare list of the light
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG AddFlare(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	FLARE *flare;
	ULONG max = 0;

	flare = new FLARE;
	if(!flare)
	{
		DisplayBeep(_screen(obj));
		return 0;
	}

	flare->Append((SLIST**)&data->flares);

	GetAttr(MUIA_Slider_Max, data->ls_flarenumber, &max);
	max++;
	SetAttrs(data->ls_flarenumber, MUIA_Slider_Max, max, TAG_DONE);
	SetAttrs(data->ls_flarenumber, MUIA_Slider_Level, max, TAG_DONE);

	data->curflare->selected = FALSE;
	data->curflare = flare;

	return 0;
}

/*************
 * DESCRIPTION:   This functions is called whenever the user presses the 'Remove' button
 *    in the 'Lens flares' page. It removes the current selected lensflare form the lensflare list
 *    of the light (but only if at least one flare remains).
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG RemoveFlare(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	ULONG max = 0;

	GetAttr(MUIA_Slider_Max, data->ls_flarenumber, &max);

	// there must remain at least one flare
	if(max > 1)
	{
		data->curflare->Remove((SLIST**)&data->flares);
		max--;
		SetAttrs(data->ls_flarenumber, MUIA_Slider_Max, max, TAG_DONE);
	}
	else
		DisplayBeep(_screen(obj));

	return 0;
}

/*************
 * DESCRIPTION:   This functions is called whenever the user changes the flare number
 *    in the 'Lens flares' page. It updates the old lensflare and sets the gadget with the values.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG FlareNumber(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	ULONG cur, i;
	FLARE *flare;

	UpdateFlare(data);

	GetAttr(MUIA_Slider_Level, data->ls_flarenumber, &cur);

	i = 0;
	flare = data->flares;
	while(flare)
	{
		if(i == cur-1)
			break;
		i++;
		flare = (FLARE*)flare->GetNext();
	}
	if(flare != data->curflare)
	{
		data->curflare->selected = FALSE;
		flare->selected = TRUE;
		data->curflare = flare;
		UpdateFlareGadgets(data);

		MUI_Redraw(data->flareview, MADF_DRAWUPDATE);
	}

	return 0;
}

/*************
 * DESCRIPTION:   This functions is called whenever the user changes the contents of one of the gadgets
 *    in the 'Lens flares' page. It updates the lensflare and redraws the view area.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG FlareUpdate(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);

	UpdateFlare(data);

	MUI_Redraw(data->flareview, MADF_DRAWUPDATE);

	return 0;
}

/*************
 * DESCRIPTION:   This functions is called whenever the user enables the 'Visible light'
 *    checkmark. It enables the visible light settings page and add default star if
 *    the light does'nt own any.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG EnableStar(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);

	// generate a star when it's not given
	if(!data->star)
	{
		data->star = new STAR;
		if(!data->star)
			return 0;

		data->star->SetSpikes(4);
	}

	SetAttrs(data->starview, MUIA_RequView_Star, data->star, TAG_DONE);
	SetAttrs(data->gr_lightstar, MUIA_Disabled, FALSE, TAG_DONE);

	UpdateStarGadgets(data);

	return 0;
}

/*************
 * DESCRIPTION:   This functions is called whenever the user changes the contents of one of the gadgets
 *    in the 'Visible light' page. It updates the star and redraws the view area.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG StarUpdate(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);

	UpdateStar(data);

	MUI_Redraw(data->starview, MADF_DRAWUPDATE);

	return 0;
}

/*************
 * DESCRIPTION:   Load a light data file
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG LoadLight(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);

	if(!sciLoadLight(data->light))
		DisplayBeep(_screen(obj));
	else
		UpdateGadgets(data);

	return 0;
}

/*************
 * DESCRIPTION:   Save a light data file
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG SaveLight(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);

	if(!sciSaveLight(data->light))
		DisplayBeep(_screen(obj));

	return 0;
}

/*************
 * DESCRIPTION:   switches between local and global
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG PosLocal(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	ULONG poslocal;
	char buf[31], *s;
	VECTOR *from, *to;

	GetAttr(MUIA_Selected, data->cm_poslocal, &poslocal);

	if(poslocal)
	{
		to = &data->pos;
		from = &data->localpos;
	}
	else
	{
		to = &data->localpos;
		from = &data->pos;
	}
	GetAttr(MUIA_String_Contents, data->posx, (ULONG*)&s);
	to->x = atof(s);
	Float2String(from->x, buf);
	SetAttrs(data->posx, MUIA_String_Contents, buf, TAG_DONE);

	GetAttr(MUIA_String_Contents, data->posy, (ULONG*)&s);
	to->y = atof(s);
	Float2String(from->y, buf);
	SetAttrs(data->posy, MUIA_String_Contents, buf, TAG_DONE);

	GetAttr(MUIA_String_Contents, data->posz, (ULONG*)&s);
	to->z = atof(s);
	Float2String(from->z, buf);
	SetAttrs(data->posz, MUIA_String_Contents, buf, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   switches between local and global
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG AlignLocal(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	ULONG alignlocal;
	char buf[31], *s;
	VECTOR *from, *to;

	GetAttr(MUIA_Selected, data->cm_alignlocal, &alignlocal);

	if(alignlocal)
	{
		to = &data->align;
		from = &data->localalign;
	}
	else
	{
		to = &data->localalign;
		from = &data->align;
	}
	GetAttr(MUIA_String_Contents, data->alignx, (ULONG*)&s);
	to->x = atof(s);
	Float2String(from->x, buf);
	SetAttrs(data->alignx, MUIA_String_Contents, buf, TAG_DONE);

	GetAttr(MUIA_String_Contents, data->aligny, (ULONG*)&s);
	to->y = atof(s);
	Float2String(from->y, buf);
	SetAttrs(data->aligny, MUIA_String_Contents, buf, TAG_DONE);

	GetAttr(MUIA_String_Contents, data->alignz, (ULONG*)&s);
	to->z = atof(s);
	Float2String(from->z, buf);
	SetAttrs(data->alignz, MUIA_String_Contents, buf, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   switches between local and global
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG SizeLocal(struct IClass *cl,Object *obj,Msg msg)
{
	struct Settings_Data *data = (struct Settings_Data *)INST_DATA(cl,obj);
	ULONG sizelocal;
	char buf[31], *s;
	VECTOR *from, *to;

	GetAttr(MUIA_Selected, data->cm_sizelocal, &sizelocal);

	if(sizelocal)
	{
		to = &data->size;
		from = &data->localsize;
	}
	else
	{
		to = &data->localsize;
		from = &data->size;
	}
	GetAttr(MUIA_String_Contents, data->sizex, (ULONG*)&s);
	to->x = atof(s);
	Float2String(from->x, buf);
	SetAttrs(data->sizex, MUIA_String_Contents, buf, TAG_DONE);

	GetAttr(MUIA_String_Contents, data->sizey, (ULONG*)&s);
	to->x = atof(s);
	Float2String(from->z, buf);
	SetAttrs(data->sizez, MUIA_String_Contents, buf, TAG_DONE);

	GetAttr(MUIA_String_Contents, data->sizez, (ULONG*)&s);
	to->x = atof(s);
	Float2String(from->z, buf);
	SetAttrs(data->sizez, MUIA_String_Contents, buf, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG Settings_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case OM_SET:
			return(mSet(cl,obj,(struct opSet*)msg));
		case OM_GET:
			return(mGet(cl,obj,(struct opGet*)msg));
		case MUIM_Settings_Ok:
			return(Ok(cl,obj,msg));
		case MUIM_Settings_Cancel:
			return(Cancel(cl,obj,msg));
		case MUIM_Settings_PopTrack:
			return(PopTrack(cl,obj,msg));
		case MUIM_Settings_StringTrack:
			return(StringTrack(cl,obj,msg));
		case MUIM_Settings_SetLens:
			return(SetLens(cl,obj,msg));
		case MUIM_Settings_OpenColor:
			return(OpenColor(cl,obj,(struct MUIP_Object *)msg));
		case MUIM_Color_Finish:
			return(CloseColor(cl,obj,(struct MUIP_ChildWindow_Finish*)msg));
		case MUIM_Browser_Finish:
			return(Browser_Finish(cl,obj,(struct MUIP_ChildWindow_Finish *)msg));
		case MUIM_Settings_EnableFlares:
			return(EnableFlares(cl,obj,msg));
		case MUIM_Settings_AddFlare:
			return(AddFlare(cl,obj,msg));
		case MUIM_Settings_RemoveFlare:
			return(RemoveFlare(cl,obj,msg));
		case MUIM_Settings_FlareNumber:
			return(FlareNumber(cl,obj,msg));
		case MUIM_Settings_FlareUpdate:
			return(FlareUpdate(cl,obj,msg));
		case MUIM_Settings_EnableStar:
			return(EnableStar(cl,obj,msg));
		case MUIM_Settings_StarUpdate:
			return(StarUpdate(cl,obj,msg));
		case MUIM_Settings_LoadLight:
			return(LoadLight(cl,obj,msg));
		case MUIM_Settings_SaveLight:
			return(SaveLight(cl,obj,msg));
		case MUIM_Settings_PosLocal:
			return(PosLocal(cl,obj,msg));
		case MUIM_Settings_AlignLocal:
			return(AlignLocal(cl,obj,msg));
		case MUIM_Settings_SizeLocal:
			return(SizeLocal(cl,obj,msg));
	}
	return(DoSuperMethodA(cl,obj,msg));
}
