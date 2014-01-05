/***************
 * PROGRAM:       Modeler
 * NAME:          materialitem_class.h
 * DESCRIPTION:   definitions for material subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    04.05.97 ah    initial release
 ***************/

#ifndef MATERIALITEM_CLASS_H
#define MATERIALITEM_CLASS_H

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef PREVIEW_H
#include "preview.h"
#endif

#ifndef SUBTASK_H
#include "subtask.h"
#endif

struct MaterialItem_Data
{
	Object *render, *text;
	ULONG width, height;
	SURFACE *surf;
	struct SubTask *st;
	struct PreviewData previewdata;
	Object *thumbnail;
	struct BitMap *thumbnailbitmap;
	ULONG seconds, micros;
};

#define PREVIEW_WIDTH 48
#define PREVIEW_HEIGHT 48

SAVEDS ASM ULONG MaterialItem_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
