/***************
 * NAME:          interface.c
 * DESCRIPTION:   MUI application specific routines
 * AUTHORS:       Andreas Heumann
 * BUGS:          none
 * TO DO:         all
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    14.08.95 ah    initial release
 *    05.10.95 ah    added E-MAIL and INFO
 *    03.04.96 ah    added special version for both platforms
 *    11.11.96 ah    added 'show' and 'save' buttons
 *    23.09.97 ah    added CPU-type
 ***************/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#include <graphics/gfx.h>

#include "interface.h"

extern void print(Object *);
extern void info(Object *);
extern void price(Object *);
extern void show(Object *);
extern void save(Object *);

#ifndef __STORM__
Object *MUI_NewObject(char *classname, Tag tag1, ...)
{
	return(MUI_NewObjectA(classname, (struct TagItem *) &tag1));
}

Object *MUI_MakeObject(LONG type, ...)
{
	return(MUI_MakeObjectA(type, (ULONG *)(((ULONG)&type)+4)));
}
#endif

static char *currencies[] =
{
	"German Marks",
	"US Dollar",
	NULL
};

static char *forms[] =
{
	"Snail Mail",
	"E-Mail",
	NULL
};

static char *cpu[] =
{
	"68030+881",
	"68040",
	"68060",
	"PPC",
	NULL
};

/*************
 * FUNCTION:      CreateApp
 * DESCRIPTION:   Create MUI application
 * INPUT:         none
 * OUTPUT:        none
 *************/
struct ObjApp *CreateApp(void)
{
	struct ObjApp *obj;

	static const struct Hook printHook = { { NULL,NULL },(HOOKFUNC)print,NULL,NULL };
	static const struct Hook infoHook = { { NULL,NULL },(HOOKFUNC)info,NULL,NULL };
	static const struct Hook showHook = { { NULL,NULL },(HOOKFUNC)show,NULL,NULL };
	static const struct Hook saveHook = { { NULL,NULL },(HOOKFUNC)save,NULL,NULL };
	static const struct Hook priceHook = { { NULL,NULL },(HOOKFUNC)price,NULL,NULL };

	obj = (ObjApp*)AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR);
	if (!obj)
		return(NULL);

	obj->App = ApplicationObject,
		MUIA_Application_Author, "Andreas Heumann",
		MUIA_Application_Base, "REGISTER",
		MUIA_Application_Title, "RayStorm Registration",
		MUIA_Application_Version, "$VER: RayStorm Registration "__VERS__" ("__DATE__")",
		MUIA_Application_Copyright, "©1995,96 by Andreas Heumann",
		MUIA_Application_Description, "RayStorm registration request",
		MUIA_Application_Window,
			obj->WI_label_0 = WindowObject,
			MUIA_Window_Title, "RayStorm Registration",
			WindowContents, VGroup,
				Child, HGroup,
					GroupFrameT("Personal Data"),
					Child, VGroup,
						MUIA_Group_Columns, 2,
						Child, KeyLabel2("First name", 'f'),
						Child, obj->s_fname = StringObject,
							MUIA_Frame, MUIV_Frame_String,
							MUIA_ControlChar, 'f',
						End,
						Child, KeyLabel2("Last name", 'l'),
						Child, obj->s_lname = StringObject,
							MUIA_Frame, MUIV_Frame_String,
							MUIA_ControlChar, 'l',
						End,
						Child, KeyLabel2("Street", 's'),
						Child, obj->s_street = StringObject,
							MUIA_Frame, MUIV_Frame_String,
							MUIA_ControlChar, 's',
						End,
					End,
					Child, VGroup,
						MUIA_Group_Columns, 2,
						Child, KeyLabel2("ZIP, City", 'z'),
						Child, obj->s_city = StringObject,
							MUIA_Frame, MUIV_Frame_String,
							MUIA_ControlChar, 'z',
						End,
						Child, KeyLabel2("Country", 'o'),
						Child, obj->s_country = StringObject,
							MUIA_Frame, MUIV_Frame_String,
							MUIA_ControlChar, 'o',
						End,
						Child, KeyLabel2("E-Mail", 'e'),
						Child, obj->s_mail = StringObject,
							MUIA_Frame, MUIV_Frame_String,
							MUIA_ControlChar, 'e',
						End,
					End,
				End,
				Child, HGroup,
					Child, VGroup,
						GroupFrameT("Order"),
						Child, HGroup,
							Child, VGroup,
								Child, ColGroup(2),
									Child, KeyLabel2("Amiga", 'a'),
									Child, obj->cm_amiga = KeyCheckMark(TRUE,'a'),
									Child, KeyLabel2("PC", 'p'),
									Child, obj->cm_pc = KeyCheckMark(FALSE,'p'),
								End,
							End,
							Child, VGroup,
								Child, HGroup,
									Child, KeyLabel2("HD", 'h'),
									Child, obj->cm_hd = KeyCheckMark(FALSE,'h'),
									Child, KeyLabel2("CPU", 'u'),
									Child, obj->cy_cpu = CycleObject,
										MUIA_ControlChar, 'u',
										MUIA_Cycle_Entries, cpu,
									End,
								End,
								Child, VSpace(0),
							End,
						End,
						Child, HGroup,
							Child, KeyLabel2("Form", 'r'),
							Child, obj->cy_form = CycleObject,
								MUIA_ControlChar, 'r',
								MUIA_Cycle_Entries, forms,
							End,
						End,
					End,
					Child, VGroup,
						GroupFrameT("Payment"),
						MUIA_Group_Columns, 2,
						Child, KeyLabel2("Currency", 'y'),
						Child, obj->cy_cur = CycleObject,
							MUIA_ControlChar, 'y',
							MUIA_Cycle_Entries, currencies,
						End,
						Child, KeyLabel2("Price", NULL),
						Child, obj->tx_price = TextObject,
							TextFrame,
							InnerSpacing(2,1),
							MUIA_Background, MUII_TextBack,
							MUIA_Text_SetMax, FALSE,
							MUIA_Text_Contents, "DM 40.-",
						End,
						Child, VSpace(0),
					End,
				End,
				Child, HGroup,
					Child, obj->b_info = SimpleButton("_Info"),
					Child, obj->b_print = SimpleButton("_Print"),
					Child, obj->b_save = SimpleButton("_Save"),
					Child, obj->b_show = SimpleButton("_Show"),
					Child, obj->b_cancel = SimpleButton("_Cancel"),
				End,
			End,
		End,
	End;


	if (!obj->App)
	{
		FreeVec(obj);
		return(NULL);
	}

	// activate price hook if:
	// - mail form is changed
	// - platform is changed
	// - currency is changed
	DoMethod((Object*)obj->cy_form,
		MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		obj->cy_form,2,MUIM_CallHook, &priceHook);
	DoMethod((Object*)obj->cy_cur,
		MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
		obj->cy_cur,2,MUIM_CallHook, &priceHook);
	DoMethod((Object*)obj->cm_amiga,
		MUIM_Notify, MUIA_Pressed, MUIV_EveryTime,
		obj->cm_amiga,2,MUIM_CallHook, &priceHook);
	DoMethod((Object*)obj->cm_pc,
		MUIM_Notify, MUIA_Pressed, MUIV_EveryTime,
		obj->cm_pc,2,MUIM_CallHook, &priceHook);
	DoMethod((Object*)obj->b_print,
		MUIM_Notify, MUIA_Pressed, FALSE,
		obj->b_print,2,MUIM_CallHook, &printHook);

	// activate other checkmark (platform) if one checkmark deselected
	DoMethod((Object*)obj->cm_amiga,
		MUIM_Notify, MUIA_Selected, FALSE,
		obj->cm_pc,3,MUIM_Set, MUIA_Selected, TRUE);
	DoMethod((Object*)obj->cm_amiga,
		MUIM_Notify, MUIA_Selected, FALSE,
		obj->cm_hd,3,MUIM_Set, MUIA_Disabled, TRUE);
	DoMethod((Object*)obj->cm_amiga,
		MUIM_Notify, MUIA_Selected, FALSE,
		obj->cy_cpu,3,MUIM_Set, MUIA_Disabled, TRUE);
	DoMethod((Object*)obj->cm_amiga,
		MUIM_Notify, MUIA_Selected, TRUE,
		obj->cm_hd,3,MUIM_Set, MUIA_Disabled, FALSE);
	DoMethod((Object*)obj->cm_amiga,
		MUIM_Notify, MUIA_Selected, TRUE,
		obj->cy_cpu,3,MUIM_Set, MUIA_Disabled, FALSE);
	DoMethod((Object*)obj->cm_pc,
		MUIM_Notify, MUIA_Selected, FALSE,
		obj->cm_amiga,3,MUIM_Set, MUIA_Selected, TRUE);

	// call info hook if info button pressed
	DoMethod((Object*)obj->b_info,
		MUIM_Notify, MUIA_Pressed, FALSE,
		obj->b_info,2,MUIM_CallHook, &infoHook);

	DoMethod((Object*)obj->b_show,
		MUIM_Notify, MUIA_Pressed, FALSE,
		obj->b_show,2,MUIM_CallHook, &showHook);

	DoMethod((Object*)obj->b_save,
		MUIM_Notify, MUIA_Pressed, FALSE,
		obj->b_save,2,MUIM_CallHook, &saveHook);

	// quit if cancel button is pressed
	DoMethod((Object*)obj->b_cancel,
		MUIM_Notify, MUIA_Pressed, FALSE,
		obj->App,2,MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	// setup cycle chain
	DoMethod((Object*)obj->WI_label_0,
		MUIM_Window_SetCycleChain, obj->s_fname,
		obj->s_lname,
		obj->s_street,
		obj->s_city,
		obj->s_country,
		obj->s_mail,
		obj->cm_amiga,
		obj->cm_hd,
		obj->cm_pc,
		obj->cy_form,
		obj->cy_cur,
		obj->b_info,
		obj->b_print,
		obj->b_save,
		obj->b_show,
		obj->b_cancel,
		0
		);

	// quit if close button is pressed
	DoMethod((Object *)obj->WI_label_0,MUIM_Notify,MUIA_Window_CloseRequest,
		TRUE,obj->App,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

	// open window
	SetAttrs(obj->WI_label_0,MUIA_Window_Open,TRUE,TAG_DONE);

	return(obj);
}

/*************
 * FUNCTION:      DisposeApp
 * DESCRIPTION:   Close MUI appliction
 * INPUT:         none
 * OUTPUT:        none
 *************/
void DisposeApp(struct ObjApp * obj)
{
	MUI_DisposeObject((Object *)obj->App);
	FreeVec(obj);
}

