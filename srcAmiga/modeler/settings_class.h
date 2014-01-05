/***************
 * PROGRAM:       Modeler
 * NAME:          settings_class.h
 * DESCRIPTION:   definition for settings window class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    06.03.96 ah    initial release
 *    27.11.96 ah    added ls_innerhaloradius
 *    14.12.96 ah    added fs_spikewidth
 *    30.01.97 ah    added ls_startilt
 *    08.02.97 ah    added cm_nophong
 *    20.02.97 ah    added cm_falloff
 *    05.03.97 ah    added cm_shadowmap and cy_shadowmapsize
 ***************/

#ifndef SETTINGS_CLASS_H
#define SETTINGS_CLASS_H

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef CONE_H
#include "cone.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef UNDO_H
#include "undo.h"
#endif

struct Settings_Data
{
	UNDO_TRANSFORM *undo;

	Object *obj;

	Object *originator, *draw_area;
	Object *b_ok,*b_cancel;
	Object *name;
	Object *cm_poslocal, *cm_alignlocal, *cm_sizelocal;
	Object *posx,*posy,*posz;
	VECTOR pos, localpos;
	Object *alignx,*aligny,*alignz;
	VECTOR align, localalign;
	Object *sizex,*sizey,*sizez;
	VECTOR size, localsize;
	Object *cm_inverted;

	int type;
	OBJECT *object;
	OBJECT *track;

	// camera settings
	CAMERA *camera;
	Object *s_track, *pb_track, *fs_fdist, *cm_focustrack, *cm_fastdof;
	Object *fs_apert, *fs_hfov, *fs_vfov, *cm_vfov, *lv_lens;

	// light settings
	LIGHT *light;
	FLARE *curflare;
	FLARE *flares;
	DISPLAY *display;
	Object *gr_lensflares, *gr_lightstar;
	Object *cf_color;
	Object *fs_radius, *fs_falloff, *cm_falloff, *fs_angle, *ls_falloffradius;
	Object *cm_shadow, *cm_lensflares, *cm_lightstar;
	Object *cm_shadowmap, *cy_shadowmapsize;
	Object *b_load, *b_save;
	Object *ls_flarenumber;
	Object *cf_flarecolor;
	Object *ls_flareradius, *ls_flareposition;
	Object *cy_flarefunction;
	Object *cy_flaretype;
	Object *ls_flaretilt;
	Object *flareview;
	Object *b_flareadd, *b_flareremove;
	STAR *star;
	float starrange;
	Object *cm_starenable, *ls_starradius, *ls_startilt, *ls_starspikes;
	Object *cm_starrandom, *ls_starrange;
	Object *cm_haloenable, *ls_haloradius, *ls_innerhaloradius;
	Object *fs_spikenoise, *fs_spikeintensity;
	Object *cf_starcolor, *cf_ringcolor;
	Object *starview;

	// mesh settings
	MESH *mesh;
	Object *cm_nophong;
	Object *pf_file,*cm_apply;

	// cylinder settings
	Object *cm_closetop, *cm_closebottom;

	// csg settings
	CSG *csg;
	Object *cy_operation;

	// sor settings
	Object *cy_sorcalc;
};

enum
{
	SETTINGS_SETNONE,
	SETTINGS_SETCAMERA,
	SETTINGS_SETPOINTLIGHT,
	SETTINGS_SETSPOTLIGHT,
	SETTINGS_SETDIRECTIONALLIGHT,
	SETTINGS_SETMESH,
	SETTINGS_SETCYLINDER,
	SETTINGS_SETCONE,
	SETTINGS_SETCSG,
	SETTINGS_SETSOR
};

SAVEDS ASM ULONG Settings_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
