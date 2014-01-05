/***************
 * PROGRAM:       Modeler
 * NAME:          attrswindow_class.h
 * DESCRIPTION:   definitions for MUI
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 ***************/

#ifndef ATTRSWINDOW_CLASS_H
#define ATTRSWINDOW_CLASS_H

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef SUBTASK_H
#include "subtask.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

typedef struct
{
	IMAGINE_TEXTURE *texture;
	BOOL added;
	char *infotext[16];
	float params[16];
} ITEXTURE_ITEM;

typedef struct
{
	RAYSTORM_TEXTURE *texture;
	BOOL added;
	TEXTURE_INFO *tinfo;
	void *data;
	struct Library *TextureBase;
} RTEXTURE_ITEM;

typedef struct
{
	HYPER_TEXTURE *texture;
	BOOL added;
} HTEXTURE_ITEM;

struct AttrsWindow_Data
{
	struct SubTask *st;
	LONG sigbit;
	SURFACE *surf, *origsurf;
	struct PreviewData *previewdata;

	Object *b_ok, *b_load, *b_save, *b_cancel;
	Object *name, *renderarea;
	Object *pop;
	Object *cf_diff, *cf_spec, *cf_transpar, *cf_reflect;
	Object *cf_ambient, *cf_transm, *cf_specm;
	Object *plist, *fs_sexp, *fs_texp,  *fs_rind;
	Object *fs_flen, *fs_transluc, *cm_bright;

	Object *lv_brush, *s_brushname;
	Object *b_brushadd, *b_brushchange, *b_brushview, *b_brushremove;
	Object *gr_brush;
	Object *rb_brushtype,*rb_brushwrap;
	Object *cm_brushrepeat,*cm_brushmirror,*cm_brushsoft;

	ITEXTURE_ITEM *itextureentry;
	Object *lv_itexture;
	Object *b_itextureadd, *b_itexturechange, *b_itextureremove;
	Object *t_p[16];
	Object *fs_p[16];

	RTEXTURE_ITEM *rtextureentry;
	Object *lv_rtexture;
	Object *dialog, *t_rtxttitle;
	Object *g_rtxtdialog, *b_rtextureadd, *b_rtexturechange, *b_rtextureremove;

	HTEXTURE_ITEM *htextureentry;
	Object *lv_htexture;
	Object *b_htextureadd, *b_htexturechange, *b_htextureremove;

	Object *cm_henable, *cy_htype;
	Object *t_hp1, *t_hp2, *t_hp3;
	Object *fs_hp1, *fs_hp2, *fs_hp3;

	Object *originator;
	OBJECT *object;
};

SAVEDS ASM ULONG AttrsWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
