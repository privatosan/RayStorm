/***************
 * MODUL:         RayStorm
 * NAME:          raystorm.cpp
 * DESCRIPTION:   This module includes the userinterface to use RayStorm with
 *                the ARexx interface
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:         change building of octree (depth, memory usage)
 *                change handling of dithering
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    10.03.95 ah    added ARexx interface
 *    25.04.95 ah    added MUI-Interface
 *    17.05.95 ah    removed ARexx cmd "TEXTURE"
 *    09.07.95 ah    added ARexx cmds "BRUSHPATH", "TEXTUREPATH" and
 *                   "OBJECTPATH"
 *    08.08.95 ah    bugfix: PATH-commands made a mungwall-hit; fixed
 *    15.08.95 ah    added 'RANDJIT' for SETWORLD; 'SIZE' for POINTLIGHT and
 *       SPOTLIGHT; 'REPEAT' and 'MIRROR' for BRUSH
 *    16.08.95 ah    added backdrop picture; 'BACKDROP' for SETWORLD;
 *       'BRIGHT' for NEWSURFACE
 *    21.08.95 ah    added 'FOGLEN', 'FOGHEIGHT', 'FOGR', 'FOGG', 'FOGB'
 *       for 'SETWORLD'
 *    30.08.95 ah    added motion blur
 *    01.09.95 ah    added 'DISTRIB'
 *    10.09.95 ah    added SPECULAR for 'BRUSH'
 *    11.09.95 ah    added 'LEFT', 'TOP', 'RIGHT' and 'BOTTOM' for 'STARTRENDER'
 *    20.09.95 ah    added 'POSACTOR' and 'VIEWACTOR' for 'SETCAMERA'
 *    24.09.95 ah    changed error handling
 *    28.09.95 ah    added global reflection map
 *    12.10.95 ah    changed spotlight direction to look point;
 *       added 'LOOKP_ACTOR' to 'SPOTLIGHT';
 *       new format for vectors '<x,y,z>';
 *       new format for colors '[r,g,b]'
 *    19.10.95 ah    added parameter check for field rendering
 *    21.10.95 ah    added precalculated matrices for animations
 *    09.11.95 ah    added 'Time spend' and 'Time left'
 *    12.01.96 ah    added 'SOFTSHADOW' to 'DISTRIB'
 *    18.04.96 ah    added 'SURFACE' to 'LOADOBJ'
 *    11.05.96 ah    command 'QUIT' has disappeared, added it again
 *    17.06.96 ah    RayStorm now uses a library named 'raystorm.libraray'
 *       to do all the raytracing stuff. This program here provides a interface
 *       to use RayStorm with ARexx commands
 *    25.11.96 ah    added 'FLARE' and 'STAR'
 *    14.12.96 ah    added 'MINOBJECTS' to 'STARTRENDER';
 *       added 'SPIKEWIDTH' to 'STAR';
 *       added 'NOSTAR' and 'NOFLARES' to 'POINTLIGHT' and 'SPOTLIGHT'
 *    10.01.97 ah    added 'BOX' command
 *    12.01.97 ah    added 'CSG' command
 *    31.01.97 ah    added 'TILT' to 'STAR'
 *    25.02.97 ah    added estimated time to display
 *    05.03.97 ah    added 'SHADOWMAP' and 'SHADOWMAPSIZE' to 'POINTLIGHT' and 'SPOTLIGHT'
 *    12.03.97 ah    added 'HYPERTEXTURE'
 *    18.03.97 ah    added 'INVERTED' to 'CSG','BOX' and 'SPHERE'
 *    19.03.97 ah    added 'CYLINDER' command
 *    21.03.97 ah    added 'SOR' command
 *    23.03.97 ah    added 'CONE' command
 *    24.11.97 ah    added new arguments to 'STAR'
 *    05.02.98 ah    split in two files
 *    18.08.98 ah    added 'DIRECTIONALLIGHT'
 ***************/

// Add to have quantation possibility
//#define QUANT

// Add to get debugging messages
//#define DEBUG

#include <stdio.h>
#include <math.h>
#include <string.h>

/* MUI */
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <graphics/gfxmacros.h>

#ifdef __STORM__
#include <wbstartup.h>
#include <pragma/exec_lib.h>
#include <pragma/muimaster_lib.h>
#define REG(x) register __ ## x
#define ASM
#define SAVEDS
#include <pragma/dos_lib.h>
#include <clib/alib_protos.h>
#endif

#ifndef KEYFILE__LIB_H
#include "keyfile_library.h"
#endif

#ifndef INTERFACE_H
#include "interface.h"
#endif

#ifndef RSI_H
#include "rsi.h"
#endif

#ifdef QUANT
#ifndef QUANT_H
#include "quant.h"
#endif // QUANT_H
#endif // QUANT

#define __VERS__ "2.3"

UBYTE vers[] = "\0$VER: RayStorm "__VERS__" ("__DATE__")";

struct IntuitionBase *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *KeyfileBase = NULL;

#ifdef QUANT
static struct Screen *main_scr = NULL;
// screen colors
WORD colors = 32;
// Pointer to rows of pixelarray
static SMALL_COLOR **scrptr = NULL;
// colormap
static colorhist_item *colormap=NULL;
#endif

// MUI-application
typedef struct
{
	APTR  app;
	APTR  win;
	APTR  perc;
	APTR  log;
	APTR  cancel;
	APTR  tspend;
	APTR  testimated;
	APTR  tleft;
} APPLICATION;

static APPLICATION ObjApp = {NULL,NULL};

// ARexx-command portotypes
SAVEDS ASM ULONG f_triangle(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_newsurface(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_sphere(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_plane(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_box(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_cylinder(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_sor(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_cone(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_csg(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_pointlight(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_spotlight(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_directionallight(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_flare(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_star(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_loadobj(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_ambient(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_diffuse(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_foglen(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_specular(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_difftrans(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_spectrans(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_refexp(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_transexp(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_refrindex(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_reflect(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_transpar(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_transluc(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_imtexture(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_hypertexture(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_brush(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_setcamera(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_setscreen(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_setworld(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_savepic(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
#ifdef QUANT
SAVEDS ASM ULONG f_display(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
#endif
SAVEDS ASM ULONG f_cleanup(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_startrender(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_quit(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_antialias(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_wintofront(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_brushpath(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_texturepath(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_objectpath(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_alignment(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_newactor(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_position(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_size(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_distrib(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);
SAVEDS ASM ULONG f_geterrorstr(REG(a0) struct Hook *,REG(a2) Object *,REG(a1) ULONG *);

#ifdef QUANT
static struct Hook hooks[] =
#else
static struct Hook hooks[] =
#endif
{
	{{NULL, NULL},(HOOKFUNC)f_triangle,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_newsurface,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_sphere,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_plane,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_box,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_cylinder,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_sor,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_cone,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_csg,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_pointlight,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_spotlight,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_directionallight,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_flare,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_star,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_loadobj,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_ambient,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_diffuse,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_foglen,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_specular,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_difftrans,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_spectrans,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_refexp,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_transexp,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_refrindex,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_reflect,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_transpar,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_transluc,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_imtexture,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_hypertexture,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_brush,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_setcamera,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_setscreen,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_setworld,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_savepic,NULL,NULL},
#ifdef QUANT
	{{NULL, NULL},(HOOKFUNC)f_display,NULL,NULL},
#endif
	{{NULL, NULL},(HOOKFUNC)f_cleanup,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_startrender,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_quit,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_antialias,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_wintofront,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_brushpath,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_texturepath,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_objectpath,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_alignment,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_newactor,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_position,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_size,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_distrib,NULL,NULL},
	{{NULL, NULL},(HOOKFUNC)f_geterrorstr,NULL,NULL},
};

enum cmds
{
	CMD_TRIANGLE, CMD_NEWSURFACE, CMD_SPHERE, CMD_PLANE, CMD_BOX, CMD_CYLINDER, CMD_SOR,
	CMD_CONE, CMD_CSG, CMD_POINTLIGHT, CMD_SPOTLIGHT, CMD_DIRECTIONALLIGHT, CMD_FLARE, CMD_STAR, CMD_LOADOBJ,
	CMD_AMBIENT, CMD_DIFFUSE, CMD_FOGLEN, CMD_SPECULAR, CMD_DIFFTRANS, CMD_SPECTRANS, CMD_REFEXP,
	CMD_TRANSEXP, CMD_REFRINDEX, CMD_REFLECT, CMD_TRANSPAR, CMD_TRANSLUC, CMD_IMTEXTURE,
	CMD_HYPERTEXTURE, CMD_BRUSH, CMD_SETCAMERA, CMD_SETSCREEN, CMD_SETWORLD, CMD_SAVEPIC,
#ifdef QUANT
	CMD_DISPLAY,
#endif
	CMD_CLEANUP, CMD_STARTRENDER, CMD_QUIT, CMD_ANTIALIAS, CMD_WINTOFRONT,
	CMD_BRUSHPATH, CMD_TEXTUREPATH, CMD_OBJECTPATH,CMD_ALIGNMENT,
	CMD_NEWACTOR, CMD_POSITION, CMD_SIZE, CMD_DISTRIB, CMD_GETERRORSTR
};

static struct MUI_Command arexxcmds[] =
{
	{"TRIANGLE",      "SURF/A,P1/A,P2/A,P3/A,N1,N2,N3,ACTOR", 8, &hooks[CMD_TRIANGLE]},
	{"NEWSURFACE",    "NAME/A,BRIGHT/S", 2, &hooks[CMD_NEWSURFACE]},
	{"SPHERE",        "SURF/A,POS/A,RADIUS/A,ACTOR,FUZZY,INVERTED/S", 6, &hooks[CMD_SPHERE]},
	{"PLANE",         "SURF/A,POS,NORM,ACTOR", 4, &hooks[CMD_PLANE]},
	{"BOX",           "SURF/A,POS/A,LOWBOUNDS,HIGHBOUNDS,ALIGN,ACTOR,INVERTED/S", 7, &hooks[CMD_BOX]},
	{"CYLINDER",      "SURF/A,POS,SIZE,ALIGN,ACTOR,INVERTED/S,OPENTOP/S,OPENBOTTOM/S", 8, &hooks[CMD_CYLINDER]},
	{"SOR",           "SURF/A,POINTS/M/A,POS,SIZE,ALIGN,ACTOR,ACCURATE/S,INVERTED/S,OPENTOP/S,OPENBOTTOM/S", 10, &hooks[CMD_SOR]},
	{"CONE",          "SURF/A,POS,SIZE,ALIGN,ACTOR,INVERTED/S,OPENBOTTOM/S", 7, &hooks[CMD_CONE]},
	{"CSG",           "OPERATOR,INVERTED/S", 2, &hooks[CMD_CSG]},
	{"POINTLIGHT",    "POS/A,COLOR,SIZE,SHADOW/S,ACTOR,FALLOFF,NOSTAR/S,NOFLARES/S,SHADOWMAP/S,SHADOWMAPSIZE/N", 10, &hooks[CMD_POINTLIGHT]},
	{"SPOTLIGHT",     "POS/A,COLOR,LOOKPOINT,ANGLE,SIZE,SHADOW/S,ACTOR,LOOKP_ACTOR,FALLOFF,NOSTAR/S,NOFLARES/S,SHADOWMAP/S,SHADOWMAPSIZE/N,FALLOFFRADIUS", 14, &hooks[CMD_SPOTLIGHT]},
	{"DIRECTIONALLIGHT","POS/A,DIR,COLOR,SIZE,SHADOW/S,ACTOR,FALLOFF,NOSTAR/S,NOFLARES/S,SHADOWMAP/S,SHADOWMAPSIZE/N", 11, &hooks[CMD_DIRECTIONALLIGHT]},
	{"FLARE",         "POS/A,RADIUS/A,COLOR,TYPE,FUNCTION,EDGES/N,TILT", 7, &hooks[CMD_FLARE]},
	{"STAR",          "SPIKES/N,RADIUS,HALORADIUS,INNERHALORADIUS,RANGE,RANDOM_SEED/N,TILT,COLOR,RINGCOLOR,NOISE,BRIGHTNESS", 8, &hooks[CMD_STAR]},
	{"LOADOBJ",       "NAME/A,POS,ALIGN,SCALE,ACTOR,SURFACE", 6, &hooks[CMD_LOADOBJ]},
	{"AMBIENT",       "COLOR/A", 1, &hooks[CMD_AMBIENT]},
	{"DIFFUSE",       "COLOR/A", 1, &hooks[CMD_DIFFUSE]},
	{"FOGLEN",        "VALUE/A", 1, &hooks[CMD_FOGLEN]},
	{"SPECULAR",      "COLOR/A", 1, &hooks[CMD_SPECULAR]},
	{"DIFFTRANS",     "COLOR/A", 1, &hooks[CMD_DIFFTRANS]},
	{"SPECTRANS",     "COLOR/A", 1, &hooks[CMD_SPECTRANS]},
	{"REFEXP",        "VALUE/A", 1, &hooks[CMD_REFEXP]},
	{"TRANSEXP",      "VALUE/A", 1, &hooks[CMD_TRANSEXP]},
	{"REFRINDEX",     "VALUE/A", 1, &hooks[CMD_REFRINDEX]},
	{"REFLECT",       "COLOR/A", 1, &hooks[CMD_REFLECT]},
	{"TRANSPAR",      "COLOR/A", 1, &hooks[CMD_TRANSPAR]},
	{"TRANSLUC",      "VALUE/A", 1, &hooks[CMD_TRANSLUC]},
	{"IMTEXTURE",     "NAME/A,POS,ALIGN,SIZE,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16,ACTOR", 21, &hooks[CMD_IMTEXTURE]},
	{"HYPERTEXTURE",  "TYPE/A,POS,ALIGN,SIZE,P1,P2,P3,ACTOR", 8, &hooks[CMD_HYPERTEXTURE]},
	{"BRUSH",         "NAME/A,TYPE/A,WRAP/A,POS/A,ALIGN/A,SIZE/A,REPEAT/S,MIRROR/S,SOFT/S,ACTOR", 10, &hooks[CMD_BRUSH]},
	{"SETCAMERA",     "POS/A,VIEWPOINT,VIEWUP,FX,FY,FOCALDIST,APERTURE,POSACTOR,VIEWACTOR,FASTDOF/S", 10, &hooks[CMD_SETCAMERA]},
	{"SETSCREEN",     "RESX/N/A,RESY/N/A,COLORS/N", 3, &hooks[CMD_SETSCREEN]},
	{"SETWORLD",      "BACK/A,AMBIENT,RANDJIT/S,BACKDROP,FOGLEN,FOGHEIGHT,FOGCOLOR,REFLMAP", 8, &hooks[CMD_SETWORLD]},
	{"SAVEPIC",       "NAME/A,FORMAT", 2, &hooks[CMD_SAVEPIC]},
#ifdef QUANT
	{"DISPLAY",       "FLOYD/S", 1, &hooks[CMD_DISPLAY]},
#endif
	{"CLEANUP",       NULL, 0, &hooks[CMD_CLEANUP]},
	{"STARTRENDER",   "QUICK/S,DEPTH/N,FROM,TO,LEFT/N,TOP/N,RIGHT/N,BOTTOM/N,MINOBJECTS/N,HYPERSTEP", 10, &hooks[CMD_STARTRENDER]},
	{"QUIT",          NULL, 0, &hooks[CMD_QUIT]},
	{"ANTIALIAS",     "SAMPLES/N/A,WIDTH,CONTRAST", 3, &hooks[CMD_ANTIALIAS]},
	{"WINTOFRONT",    NULL, 0, &hooks[CMD_WINTOFRONT]},
	{"BRUSHPATH",     "NAME/A", 1, &hooks[CMD_BRUSHPATH]},
	{"TEXTUREPATH",   "NAME/A", 1, &hooks[CMD_TEXTUREPATH]},
	{"OBJECTPATH",    "NAME/A", 1, &hooks[CMD_OBJECTPATH]},
	{"ALIGNMENT",     "FROM/A,TO/A,ALIGN/A,TYPE", 4, &hooks[CMD_ALIGNMENT]},
	{"NEWACTOR",      "NAME/A,POS,ALIGN,SIZE", 4, &hooks[CMD_NEWACTOR]},
	{"POSITION",      "FROM/A,TO/A,POS/A,TYPE", 4, &hooks[CMD_POSITION]},
	{"SIZE",          "FROM/A,TO/A,SIZE/A,TYPE", 4, &hooks[CMD_SIZE]},
	{"DISTRIB",       "SAMPLES/N,SOFTSHADOW/N", 2, &hooks[CMD_DISTRIB]},
	{"GETERRORSTR",   "ERRNUM/N/A", 1, &hooks[CMD_GETERRORSTR]},
	{NULL,            NULL, 0, NULL},
};

// string buffer
static char wintitle[256];
extern char buffer[];
// Application errors
extern char *app_errors[];

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

/*************
 * DESCRIPTION:   open a error requester
 * INPUT:         none
 * OUTPUT:        none
 *************/
BOOL open_error_request(unsigned char *text)
{
	if(!IntuitionBase)
	{
		printf("%s\n",text);
	}
	else
	{
		LONG back;
		struct EasyStruct easyrequ =
		{
			sizeof(struct EasyStruct),
			0,
			&vers[7],
			text,
			(unsigned char*)"OK",
		};

		if(ObjApp.win)
			SetAttrs((Object *)ObjApp.win, MUIA_Window_Sleep, TRUE, TAG_DONE);
		back = EasyRequestArgs(NULL, &easyrequ, NULL, NULL);
		if(ObjApp.win)
			SetAttrs((Object *)ObjApp.win, MUIA_Window_Sleep, FALSE, TAG_DONE);
		return back;
	}
	return 0;
}

/*************
 * DESCRIPTION:   write to log
 * INPUT:         text     text to print to log
 * OUTPUT:        none
 *************/
void WriteLog(rsiCONTEXT *rc, char *text)
{
	ULONG entrie;

	GetAttr(MUIA_List_Entries, (Object*)ObjApp.log, &entrie);
	if(entrie>50)
	{
		DoMethod((Object *)ObjApp.log, MUIM_List_Remove, MUIV_List_Remove_First);
		entrie--;
	}
	DoMethod((Object *)ObjApp.log, MUIM_List_InsertSingle, text, MUIV_List_Insert_Bottom);
	DoMethod((Object *)ObjApp.log, MUIM_List_Jump, entrie);
}

/*************
 * DESCRIPTION:   update rendering status
 * INPUT:         percent     percent finished
 *                elapsed     elapsed seconds since start of rendering
 *                y           current line
 *                lines       amount of lines since last call
 *                line        pointer to last renderd line
 * OUTPUT:        none
 *************/
void UpdateStatus(rsiCONTEXT *rc, float percent, float elapsed, int y, int lines, rsiSMALL_COLOR *line)
{
	float end;

	SetAttrs((Object *)ObjApp.perc,MUIA_Gauge_Current,(ULONG)(percent*65535.f), TAG_DONE);

	sprintf(buffer, "Time spend: %02d:%02d:%02d",
		(int)floor(elapsed/3600.f),
		(int)floor(elapsed/60.f) % 60,
		(int)floor(elapsed) % 60);
	SetAttrs((Object *)ObjApp.tspend,MUIA_Text_Contents,(ULONG)buffer, TAG_DONE);

	if(percent > 0.f)
		end = elapsed / percent;
	else
		end = 0.f;

	sprintf(buffer, "Time estimated: %02d:%02d:%02d",
		(int)floor(end/3600.f),
		(int)floor(end/60.f) % 60,
		(int)floor(end) % 60);
	SetAttrs((Object *)ObjApp.testimated,MUIA_Text_Contents,(ULONG)buffer, TAG_DONE);

	end = (1.f - percent) * end;

	sprintf(buffer, "Time left: %02d:%02d:%02d",
		(int)floor(end/3600.f),
		(int)floor(end/60.f) % 60,
		(int)floor(end) % 60);
	SetAttrs((Object *)ObjApp.tleft,MUIA_Text_Contents,(ULONG)buffer, TAG_DONE);
}

/*************
 * DESCRIPTION:   check if cancel button is pressed
 * INPUT:         none
 * OUTPUT:        TRUE if pressed else FALSE
 *************/
BOOL CheckCancel(rsiCONTEXT *rc)
{
	ULONG pressed;

	GetAttr(MUIA_Selected, (Object*)ObjApp.cancel, &pressed);
	return pressed;
}

/*************
 * DESCRIPTION:   update window and input methods
 * INPUT:         none
 * OUTPUT:        none
 *************/
void Cooperate(rsiCONTEXT *rc)
{
	// Update window
	DoMethod((Object *)ObjApp.app,MUIM_Application_InputBuffered);
}

/*************
 * DESCRIPTION:   Initialize
 *                - open libs
 *                - create mui-interface
 * INPUT:         none
 * OUTPUT:        error number
 *************/
static ULONG Init()
{
	char name[32];
	ULONG sernum;

	// Open intuition library.
	IntuitionBase = (struct IntuitionBase*)OpenLibrary((unsigned char*)"intuition.library",37L);
	if (!IntuitionBase)
		return ERROR_INTUITIONLIB;

	// Open muimaster library.
	MUIMasterBase = OpenLibrary((unsigned char*)MUIMASTER_NAME,10);/*MUIMASTER_VMIN);*/
	if (!MUIMasterBase)
		return ERROR_MUIMASTERLIB;

	// Open keyfile library.
	KeyfileBase = OpenLibrary("s:RayStorm.key",0);
	if(KeyfileBase)
	{
		keyGetInfo(name, &sernum);

		sprintf(wintitle,"RayStorm Script 2.1 registered for %s; Serial No.: %d", name, sernum);
	}
	else
		strcpy(wintitle,"RayStorm Script 2.1 Demo");

	// Open Window
	ObjApp.app = ApplicationObject,
		MUIA_Application_Title,       "RayStorm Script",
		MUIA_Application_Version,     "$VER: RayStorm Script "__VERS__" ("__DATE2__")",
		MUIA_Application_Copyright,   "©1995-97 by Andreas Heumann, Mike Hesser",
		MUIA_Application_Author,      "Andreas Heumann, Mike Hesser",
		MUIA_Application_Description, "ARexx interface to RayStorm raytracer",
		MUIA_Application_Base,        "RAYSTORM",
		MUIA_Application_Commands,    arexxcmds,
		MUIA_Application_Window,      ObjApp.win = WindowObject,
			MUIA_Window_Title, wintitle,
			MUIA_Window_ID   , MAKE_ID('M','A','I','N'),
			WindowContents, VGroup,
				Child, VGroup,
					GroupFrameT("State"),
					Child, ObjApp.log = ListviewObject,
						MUIA_Listview_Input, FALSE,
						MUIA_Listview_List, ListObject,
							ReadListFrame,
							MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
							MUIA_List_DestructHook, MUIV_List_DestructHook_String,
						End,
					End,
					Child, ObjApp.perc = GaugeObject,
						GaugeFrame,
						MUIA_Weight, 20,
						MUIA_Gauge_Horiz, TRUE,
						MUIA_Gauge_Max, 65535,
					End,
					Child, ScaleObject, MUIA_Scale_Horiz, TRUE, End,
					Child, HGroup,
						Child, ObjApp.tspend = TextObject,
							MUIA_Text_PreParse, MUIX_L,
							MUIA_Text_Contents, "Time spend: 00:00:00",
						End,
						Child, ObjApp.testimated = TextObject,
							MUIA_Text_PreParse, MUIX_C,
							MUIA_Text_Contents, "Time spend: 00:00:00",
						End,
						Child, ObjApp.tleft = TextObject,
							MUIA_Text_PreParse, MUIX_R,
							MUIA_Text_Contents, "Time left: 00:00:00",
						End,
					End,
				End,
				Child, ObjApp.cancel = SimpleButton("_Cancel"),
			End,
		End,
	End;

	if (!ObjApp.app)
		return ERROR_WINDOW;

	SetAttrs((Object *)ObjApp.cancel,MUIA_Disabled,TRUE, TAG_DONE);
	DoMethod((Object *)ObjApp.win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,ObjApp.app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
	SetAttrs((Object *)ObjApp.win,MUIA_Window_Open,TRUE, TAG_DONE);
	return FALSE;
}

/*************
 * DESCRIPTION:   Cleanup
 *                - close libs
 *                - free mem
 *                - free octree
 * INPUT:         none
 * OUTPUT:        none
 *************/
void Cleanup()
{
#ifdef QUANT
	if(scrptr)
		delete [ ] scrptr;
	if(colormap)
		delete [ ] colormap;
	if(main_scr)
		CloseScreen(main_scr);
#endif

	if(MUIMasterBase)
	{
		// close window
		SetAttrs((Object *)ObjApp.win,MUIA_Window_Open,FALSE, TAG_DONE);
		if(ObjApp.app)
			MUI_DisposeObject((Object *)ObjApp.app);
		CloseLibrary(MUIMasterBase);
	}
	if (IntuitionBase)
		CloseLibrary((struct Library*)IntuitionBase);
	if(KeyfileBase)
		CloseLibrary(KeyfileBase);
}

/*************
 * DESCRIPTION:   Displays a rendered image
 * INPUT:         floyd    TRUE if floyd-dithering else FALSE
 * OUTPUT:        none
 *************/
#ifdef QUANT
static void display(const BOOL floyd)
{
	static struct RastPort *rport;
	int x,y,i;

	scrptr = new SMALL_COLOR*[camera->yres];
	if(!scrptr)
		Error(RL_PANIC, ERR_MEM);
	for(i=0; i<camera->yres; i++)
	{
		scrptr[i] = &scrarray[i*camera->xres];
	}

	colormap = new struct colorhist_item[colors];
	if(!colormap)
		Error(RL_PANIC, ERR_MEM);

	SetAttrs((Object *)ObjApp.text,MUIA_Text_Contents,"Quantizing", TAG_DONE);
	quant(camera->xres, camera->yres, colors, scrptr, colormap, floyd);
	SetAttrs((Object *)ObjApp.text,MUIA_Text_Contents,"Displaying", TAG_DONE);

	delete [ ] scrptr;
	scrptr = NULL;

	if(!main_scr)
	{
		// open screen
		main_scr = OpenScreenTags(NULL,
			SA_Width,      camera->xres,
			SA_Height,     camera->yres,
			SA_Depth,      5,
			SA_FullPalette,TRUE,
			SA_SysFont,    1L,
			SA_Pens,       GetScreenDrawInfo(LockPubScreen("Workbench"))->dri_Pens,
			SA_Title,      "Raystorm",
			TAG_DONE);
		if (!main_scr)
			Error(RL_PANIC, ERROR_SCREEN);
		rport = &main_scr->RastPort;
	}

	for(i=0; i<colors; i++)
	{
		SetRGB32(&main_scr->ViewPort,i,(ULONG)(colormap[i].color.r) << 24,
												 (ULONG)(colormap[i].color.g) << 24,
												 (ULONG)(colormap[i].color.b) << 24);
	}

	delete [ ] colormap;
	colormap = NULL;

	for(y=0; y<camera->yres; y++)
	{
		for(x=0; x<camera->xres; x++)
		{
			SetAPen(rport,(UBYTE)(scrarray[y*camera->xres+x].r));
			WritePixel(rport,x,y);
		}
	}
	SetAttrs((Object *)ObjApp.text,MUIA_Text_Contents,"Ready", TAG_DONE);
}
#endif

void main(void)
{
	char *err;
	ULONG error;
	BOOL done = FALSE;
	ULONG signals,id;

	err = InitInterface();
	if(err)
	{
		open_error_request(err);
		exit(1);
	}

	// Open window and libs
	error = Init();
	if(error)
	{
		open_error_request(app_errors[error-100]);
		Cleanup();
		exit(1);
	}

	while(!done)
	{
		id = DoMethod((Object *)ObjApp.app,MUIM_Application_Input,&signals);
		switch (id)
		{
			case MUIV_Application_ReturnID_Quit:
				done = TRUE;
				break;
		}
		if (!done && signals)
			Wait(signals);
	}
	Cleanup();
	CleanupInterface();
	exit(0);
}

#define FUNC(name) SAVEDS ASM ULONG f_ ## name (REG(a0) struct Hook *hook, \
					 REG(a2) Object *appl, \
					 REG(a1) ULONG *arg) \
{ \
	return name (arg); \
}

FUNC(triangle)
FUNC(newsurface)
FUNC(setcamera)
FUNC(setscreen)
FUNC(pointlight)
FUNC(spotlight)
FUNC(directionallight)
FUNC(flare)
FUNC(star)
FUNC(sphere)
FUNC(plane)
FUNC(box)
FUNC(cylinder)
FUNC(sor)
FUNC(cone)
FUNC(csg)
FUNC(loadobj)
FUNC(setworld)
FUNC(ambient)
FUNC(diffuse)
FUNC(foglen)
FUNC(specular)
FUNC(difftrans)
FUNC(spectrans)
FUNC(refexp)
FUNC(transexp)
FUNC(refrindex)
FUNC(reflect)
FUNC(transpar)
FUNC(transluc)
FUNC(imtexture)
FUNC(hypertexture)
FUNC(brush)
FUNC(savepic)
#ifdef QUANT
SAVEDS ASM ULONG f_display(REG(a0) struct Hook *hook,
					REG(a2) Object *appl,
					REG(a1) ULONG *arg)
{
	display((BOOL)arg[0]);

	return RETURN_OK;
}
#endif
FUNC(cleanup)
SAVEDS ASM ULONG f_startrender(REG(a0) struct Hook *hook,
					 REG(a2) Object *appl,
					 REG(a1) ULONG *arg)
{
	SetAttrs((Object *)ObjApp.perc,MUIA_Gauge_Current,0, TAG_DONE);
	SetAttrs((Object *)ObjApp.cancel,MUIA_Disabled,FALSE, TAG_DONE);

	return startrender(arg);
}

SAVEDS ASM ULONG f_quit(REG(a0) struct Hook *hook,
					REG(a2) Object *appl,
					REG(a1) ULONG *arg)
{
	DoMethod((Object*)ObjApp.app,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
	return rsiERR_NONE;
}

FUNC(antialias)

SAVEDS ASM ULONG f_wintofront(REG(a0) struct Hook *hook,
						REG(a2) Object *appl,
						REG(a1) ULONG *arg)
{
	// bring window to front
	DoMethod((Object *)ObjApp.win,MUIM_Window_ToFront,TRUE);
	return rsiERR_NONE;
}

FUNC(brushpath)
FUNC(texturepath)
FUNC(objectpath)
FUNC(newactor)
FUNC(position)
FUNC(alignment)
FUNC(size)
FUNC(distrib)

SAVEDS ASM ULONG f_geterrorstr(REG(a0) struct Hook *hook,
					 REG(a2) Object *appl,
					 REG(a1) ULONG *arg)
{
	geterrorstr(arg);

	SetAttrs((Object *)ObjApp.app, MUIA_Application_RexxString, buffer, TAG_DONE);

	return rsiERR_NONE;
}
