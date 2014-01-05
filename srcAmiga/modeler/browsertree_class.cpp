/***************
 * PROGRAM:       Modeler
 * NAME:          browsertree_class.cpp
 * DESCRIPTION:   class for browser tree
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.03.97 ah    initial release
 ***************/

#include <stdio.h>
#include <string.h>
#include <graphics/gfxbase.h>
#include <pragma/utility_lib.h>
#ifdef __PPC__
#include <clib/powerpc_protos.h>
#endif

extern struct GfxBase *GfxBase;

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef BROWSERTREE_CLASS_H
#include "browsertree_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

static ULONG palette16[] =
{
	0xaaaaaaaa,0xaaaaaaaa,0xaaaaaaaa,
	0xffffffff,0xffffffff,0xffffffff,
	0xecececec,0xecececec,0xecececec,
	0xdadadada,0xdadadada,0xdadadada,
	0xc8c8c8c8,0xc8c8c8c8,0xc8c8c8c8,
	0xb6b6b6b6,0xb6b6b6b6,0xb6b6b6b6,
	0xa3a3a3a3,0xa3a3a3a3,0xa3a3a3a3,
	0x91919191,0x91919191,0x91919191,
	0x7f7f7f7f,0x7f7f7f7f,0x7f7f7f7f,
	0x6d6d6d6d,0x6d6d6d6d,0x6d6d6d6d,
	0x5b5b5b5b,0x5b5b5b5b,0x5b5b5b5b,
	0x48484848,0x48484848,0x48484848,
	0x36363636,0x36363636,0x36363636,
	0x24242424,0x24242424,0x24242424,
	0x12121212,0x12121212,0x12121212,
	0x00000000,0x00000000,0x00000000
};

static UBYTE bodysphere[] =
{
	0x03,0x80,0x01,0x40,0x01,0xc0,0x02,0x00,
	0x04,0xa0,0x02,0xc0,0x0b,0x10,0x0c,0x00,
	0x16,0x70,0x0c,0x80,0x17,0x08,0x18,0x00,
	0x07,0x70,0x1d,0x88,0x16,0x00,0x18,0x00,
	0x26,0x78,0x2c,0x80,0x37,0x04,0x38,0x00,
	0x31,0xd4,0x2a,0xe8,0x33,0x04,0x3c,0x00,
	0x2e,0x70,0x3b,0x74,0x33,0x8c,0x3c,0x00,
	0x17,0x48,0x35,0x80,0x39,0xf8,0x3e,0x04,
	0x11,0xd0,0x1a,0xf0,0x1c,0xf0,0x1f,0x08,
	0x09,0xf0,0x1d,0x08,0x1e,0x00,0x1f,0xf8,
	0x04,0x80,0x0f,0x00,0x0f,0xf0,0x0f,0xf0,
	0x03,0xc0,0x03,0x80,0x03,0xc0,0x03,0xc0,
};

static UBYTE bodycone[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
	0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,
	0x00,0x80,0x01,0x80,0x00,0x40,0x01,0x00,
	0x01,0x80,0x00,0xc0,0x00,0x00,0x01,0x00,
	0x01,0x40,0x01,0x80,0x03,0x20,0x02,0x00,
	0x02,0x80,0x03,0xe0,0x01,0x00,0x02,0x00,
	0x07,0xf0,0x00,0x80,0x05,0x10,0x06,0x00,
	0x01,0xb0,0x00,0xd0,0x05,0x00,0x06,0x00,
	0x0b,0xd0,0x06,0xa8,0x0b,0x08,0x0c,0x00,
	0x0a,0xa8,0x06,0xd8,0x0b,0x00,0x0c,0x00,
	0x10,0x94,0x02,0xec,0x1b,0x04,0x1c,0x00,
	0x09,0x88,0x0a,0xf0,0x03,0x08,0x0c,0x00,
};

static UBYTE bodycylinder[] =
{
	0x01,0x80,0x02,0xe0,0x03,0x00,0x04,0x00,
	0x1b,0xc0,0x0a,0xf8,0x13,0x00,0x1c,0x00,
	0x21,0x84,0x3a,0xf8,0x33,0x04,0x3c,0x00,
	0x3b,0xc4,0x2a,0xf8,0x33,0x04,0x3c,0x00,
	0x2b,0x4c,0x3a,0x78,0x33,0x84,0x3c,0x00,
	0x3b,0x6c,0x2a,0x78,0x33,0x84,0x3c,0x00,
	0x3f,0x60,0x2a,0x74,0x33,0x8c,0x3c,0x00,
	0x2e,0x38,0x3b,0x3c,0x33,0xc4,0x3c,0x00,
	0x3f,0xf8,0x2a,0x7c,0x33,0x84,0x3c,0x00,
	0x2a,0x30,0x3b,0x34,0x33,0xcc,0x3c,0x00,
	0x1c,0x90,0x09,0x10,0x11,0xe8,0x1e,0x00,
	0x06,0x80,0x03,0x00,0x03,0xe0,0x04,0x00,
};

static UBYTE bodybox[] =
{
	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0xa0,
	0x02,0xe0,0x03,0xf0,0x03,0xf0,0x0c,0x08,
	0x17,0x98,0x07,0xe4,0x17,0xfc,0x18,0x00,
	0x10,0xe0,0x0b,0x04,0x1b,0xfc,0x1c,0x00,
	0x0f,0xe8,0x0c,0x04,0x0f,0xfc,0x0c,0x00,
	0x0d,0xfc,0x0e,0x00,0x0f,0xfc,0x0c,0x00,
	0x0d,0xf8,0x0c,0x04,0x0d,0xfc,0x0e,0x00,
	0x0e,0xa8,0x0d,0x54,0x0d,0xfc,0x0e,0x00,
	0x06,0x14,0x0f,0xe8,0x0d,0xfc,0x0e,0x00,
	0x04,0x84,0x05,0x7c,0x07,0xfc,0x06,0x00,
	0x00,0x00,0x03,0xe0,0x03,0xe0,0x02,0x00,
	0x02,0x00,0x01,0x00,0x03,0x00,0x02,0x80,
};

static UBYTE bodyplane[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x20,0x01,0xa0,0x01,0xe0,0x00,0x00,
	0x2f,0xa0,0x37,0xa0,0x38,0x40,0x00,0x00,
	0x85,0x48,0x05,0x78,0xfa,0x88,0x00,0x00,
	0x1b,0xe8,0x1b,0xf8,0x64,0x00,0x00,0x00,
	0x6f,0x54,0x6f,0xf8,0x50,0x00,0x00,0x00,
	0x1e,0x82,0x1f,0xfc,0x20,0x00,0x00,0x00,
	0x0d,0x02,0x1f,0xfc,0x00,0x01,0x00,0x00,
	0x0a,0x00,0x0f,0xf0,0x10,0x08,0x00,0x00,
	0x04,0xc0,0x0f,0x80,0x00,0x40,0x00,0x00,
	0x00,0x00,0x0a,0x00,0x0e,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

static UBYTE bodycamera[] =
{
	0x08,0x00,0x04,0xc0,0x04,0xc0,0x0b,0x00,
	0x18,0xa0,0x04,0xa0,0x04,0xe0,0x1b,0x00,
	0x0c,0xc0,0x14,0x80,0x04,0xc0,0x1b,0x00,
	0x02,0xa0,0x04,0x40,0x00,0x00,0x0f,0xe0,
	0x03,0x80,0x7c,0xc0,0x00,0x00,0x7f,0xd0,
	0x0f,0xe0,0x70,0x98,0x00,0xf8,0x7f,0x00,
	0x17,0xb6,0x68,0xfe,0x00,0xfe,0x7f,0x00,
	0x0f,0xde,0x70,0x8e,0x00,0xce,0x7f,0x30,
	0x47,0x8e,0x78,0xfe,0x00,0xce,0x7f,0x30,
	0x03,0xcc,0x7c,0xcc,0x00,0xfc,0x7f,0x32,
	0x05,0x08,0x1a,0x48,0x00,0x58,0x1f,0xb6,
	0x01,0x48,0x00,0x40,0x00,0x40,0x01,0xa8,
};

static UBYTE bodymesh[] =
{
	0x11,0x80,0x15,0x80,0xe8,0x00,0xfe,0x00,
	0x00,0x30,0x02,0xe8,0xfc,0x18,0xff,0x00,
	0x01,0xf1,0x06,0x7f,0xf8,0x80,0xff,0x00,
	0x14,0x5a,0x1b,0x5a,0xe0,0x25,0xff,0x80,
	0x22,0x90,0x3d,0x10,0xc0,0x6f,0xff,0x80,
	0x01,0x64,0x7e,0xe4,0x00,0x5b,0x7f,0x80,
	0x00,0x41,0x3f,0x81,0x00,0x3f,0x3f,0xc0,
	0x00,0x22,0x0f,0xc0,0x00,0x3e,0x0f,0xc0,
	0x00,0x0c,0x07,0xc0,0x00,0x1c,0x07,0xe0,
	0x00,0x14,0x01,0xe0,0x00,0x1c,0x01,0xe0,
	0x00,0x18,0x00,0x70,0x00,0x18,0x00,0x60,
	0x00,0x18,0x00,0x08,0x00,0x08,0x00,0x10,
};

static UBYTE bodypointlight[] =
{
	0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,
	0x04,0x90,0x00,0x80,0x04,0x10,0x00,0x00,
	0x00,0x00,0x00,0x80,0x02,0x20,0x00,0x00,
	0x00,0x80,0x01,0x40,0x00,0x00,0x00,0x00,
	0x15,0xd4,0x06,0x30,0x18,0x0c,0x00,0x00,
	0x00,0x80,0x01,0x40,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x80,0x02,0x20,0x00,0x00,
	0x04,0x90,0x00,0x80,0x04,0x10,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,
	0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

static UBYTE bodyspotlight[] =
{
	0x10,0x00,0x10,0x00,0x44,0x00,0x00,0x00,
	0x38,0x00,0x28,0x00,0x00,0x00,0x00,0x00,
	0xfe,0x00,0x44,0x00,0x82,0x00,0x00,0x00,
	0x30,0x00,0x28,0x00,0x04,0x00,0x00,0x00,
	0x12,0x00,0x16,0x00,0x49,0x00,0x00,0x00,
	0x11,0x80,0x02,0x80,0x14,0x40,0x00,0x00,
	0x00,0xd0,0x01,0x30,0x02,0x08,0x00,0x00,
	0x01,0x70,0x01,0x88,0x00,0x06,0x00,0x00,
	0x00,0x38,0x00,0xc6,0x00,0x01,0x00,0x00,
	0x00,0x1f,0x00,0x60,0x00,0x00,0x00,0x00,
	0x00,0x2f,0x00,0x30,0x00,0x00,0x00,0x00,
	0x00,0x17,0x00,0x18,0x00,0x00,0x00,0x00,
};

static UBYTE bodydirectionallight[] =
{
	0xea,0x00,0xf5,0x80,0xfd,0x60,0x7e,0xd8,
	0x9f,0xa6,0x6f,0xc9,0x93,0xf6,0x65,0xf9,
	0x1b,0x7e,0x06,0xbf,0x01,0xaf,0x00,0x57,
	0x18,0x00,0x0c,0x00,0x03,0x00,0x81,0xc0,
	0xe0,0x60,0x70,0x38,0x1c,0x0e,0x06,0x07,
	0x03,0x81,0x00,0xc0,0x00,0x30,0x00,0x18,
	0x06,0x00,0x03,0x80,0x00,0xe0,0x00,0x38,
	0x00,0x1e,0x80,0x07,0xe0,0x01,0x78,0x00,
	0x1c,0x00,0x07,0x00,0x01,0xc0,0x00,0x60,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


static UBYTE bodyunion[] =
{
	0x0e,0xe0,0x05,0x50,0x07,0x70,0x08,0x80,
	0x12,0x28,0x0b,0xb0,0x2c,0xc4,0x30,0x00,
	0x59,0x9c,0x32,0x20,0x5c,0xc2,0x60,0x00,
	0x1d,0xdc,0x76,0x62,0x58,0x80,0x60,0x00,
	0x99,0x9e,0xb2,0x20,0xdc,0xc1,0xe0,0x00,
	0xc7,0x75,0xab,0xba,0xcc,0xc1,0xf0,0x00,
	0xb9,0x9c,0xed,0xdd,0xce,0xe3,0xf0,0x00,
	0x5d,0xd2,0xd6,0x60,0xe7,0x7e,0xf8,0x81,
	0x47,0x74,0x6b,0xbc,0x73,0x3c,0x7c,0xc2,
	0x27,0x7c,0x74,0x42,0x78,0x80,0x7f,0xfe,
	0x12,0x20,0x3c,0xc0,0x3f,0xfc,0x3f,0xfc,
	0x0f,0xf0,0x0e,0xe0,0x0f,0xf0,0x0f,0xf0,
};

static UBYTE bodyinters[] =
{
	0x1e,0x78,0x1e,0x78,0x1e,0x78,0x1e,0x78,
	0x34,0xd4,0x34,0x54,0x35,0xd4,0x35,0x54,
	0x6a,0x2a,0x69,0xaa,0x6a,0x6a,0x6b,0x2a,
	0x55,0x36,0x56,0xf6,0x56,0x36,0x57,0x16,
	0xab,0x2b,0xac,0xcb,0xae,0x2b,0xaf,0x0b,
	0xd6,0x15,0xd5,0xd5,0xd6,0x35,0xd7,0x15,
	0xa9,0x6b,0xaf,0x4b,0xae,0xab,0xaf,0x0b,
	0xd1,0x95,0xd6,0x15,0xd7,0xd5,0xd7,0x35,
	0x6a,0xea,0x6b,0xea,0x6b,0xea,0x6b,0x2a,
	0x57,0x96,0x57,0x56,0x57,0x16,0x57,0xd6,
	0x2a,0xac,0x2b,0x2c,0x2b,0xac,0x2b,0xac,
	0x1e,0x78,0x1e,0x78,0x1e,0x78,0x1e,0x78,
};

static UBYTE bodydiff[] =
{
	0x0e,0x70,0x05,0x70,0x07,0x70,0x08,0x70,
	0x13,0xac,0x0b,0xac,0x2d,0xac,0x31,0xac,
	0x5b,0x56,0x33,0x56,0x5f,0x56,0x63,0x56,
	0x1e,0xaa,0x76,0xaa,0x5a,0xaa,0x62,0xaa,
	0x9d,0x55,0xb5,0x55,0xdd,0x55,0xe5,0x55,
	0xc6,0xab,0xae,0xab,0xce,0xab,0xf6,0xab,
	0xbd,0x55,0xed,0x55,0xcd,0x55,0xf5,0x55,
	0x5e,0xab,0xd6,0xab,0xe6,0xab,0xfe,0xab,
	0x47,0x56,0x6b,0x56,0x73,0x56,0x7f,0x56,
	0x26,0xaa,0x76,0xaa,0x7a,0xaa,0x7e,0xaa,
	0x13,0xdc,0x3d,0xdc,0x3f,0xdc,0x3f,0xdc,
	0x0f,0x70,0x0e,0x70,0x0f,0x70,0x0f,0x70,
};

static UBYTE bodysor[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0e,0xae,0x0f,0xfe,0x70,0x00,0x7f,0xfe,
	0xe4,0x66,0xb8,0x79,0xc0,0x7f,0xff,0x80,
	0x70,0x90,0x66,0xec,0x78,0xfc,0x7f,0x02,
	0x38,0x80,0x3c,0xf8,0x3f,0x04,0x3f,0xfc,
	0x1e,0x78,0x1f,0x80,0x1f,0xf8,0x1f,0xf8,
	0x09,0x70,0x1e,0x58,0x1f,0x98,0x1f,0xe0,
	0x2b,0x5c,0x32,0x68,0x23,0x84,0x3c,0x04,
	0x04,0x20,0x0c,0xf0,0x07,0x10,0x08,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

static UBYTE *bodies[] =
{
	bodysphere,
	bodycone,
	bodycylinder,
	bodybox,
	bodyplane,
	bodymesh,
	bodycamera,
	bodypointlight,
	bodyspotlight,
	bodydirectionallight,
	bodyunion,
	bodyinters,
	bodydiff,
	bodysor
};

#define IMAGE_HEIGHT 12
#define IMAGE_WIDTH  16

enum
{
	IMAGE_SPHERE,
	IMAGE_CONE,
	IMAGE_CYLINDER,
	IMAGE_BOX,
	IMAGE_PLANE,
	IMAGE_MESH,
	IMAGE_CAMERA,
	IMAGE_POINTLIGHT,
	IMAGE_SPOTLIGHT,
	IMAGE_DIRECTIONALLIGHT,
	IMAGE_UNION,
	IMAGE_INTERS,
	IMAGE_DIFF,
	IMAGE_SOR
};

// hooks
typedef struct
{
	OBJECT *obj;
	Object *thumbnail, *list;
} OBJECT_ITEM;

static SAVEDS ASM ULONG ListOpenFunc(REG(a1) MUIS_Listtree_TreeNode *tn)
{
	((OBJECT_ITEM*)tn->tn_User)->obj->flags |= OBJECT_NODEOPEN;
	return 0;
}

static SAVEDS ASM ULONG ListCloseFunc(REG(a1) MUIS_Listtree_TreeNode *tn)
{
	((OBJECT_ITEM*)tn->tn_User)->obj->flags &= ~OBJECT_NODEOPEN;
	return 0;
}

static SAVEDS ASM ULONG ListConstFunc(REG(a1) OBJECT_ITEM *oi)
{
	OBJECT_ITEM *noi;

	noi = new OBJECT_ITEM;
	if(!noi)
		return NULL;

	*noi = *oi;
	return (ULONG)noi;
}

static SAVEDS ASM ULONG ListDestFunc(REG(a1) OBJECT_ITEM *oi)
{
// There is a problem with deleting the images of the list. MUI
// causes some Enforcer hits here. So I remove this lines.
// Test it again when we have a new version of listtre class
//   if(oi->thumbnail)
//      DOMETHOD(2) oi->list, MUIM_List_DeleteImage, oi->thumbnail);
	delete oi;
	return 0;
}

static SAVEDS ASM ULONG ListDisplayFunc(REG(a1) MUIS_Listtree_TreeNode *tn, REG(a2) char **array)
{
	static char buffer[16];
	OBJECT_ITEM *oi;

	array[0] = NULL;

	oi = (OBJECT_ITEM*)(tn->tn_User);

	if(oi->thumbnail)
		sprintf(buffer, "\33O[%08lx]", oi->thumbnail);
	else
		buffer[0] = 0;

	array[1] = buffer;
	return 0;
}

static const struct Hook ListOpenHook = { { NULL,NULL },(HOOKFUNC)ListOpenFunc,NULL,NULL };
static const struct Hook ListCloseHook = { { NULL,NULL },(HOOKFUNC)ListCloseFunc,NULL,NULL };
static const struct Hook ListConstHook = { { NULL,NULL },(HOOKFUNC)ListConstFunc,NULL,NULL };
static const struct Hook ListDestHook = { { NULL,NULL },(HOOKFUNC)ListDestFunc,NULL,NULL };
static const struct Hook ListDisplayHook = { { NULL,NULL },(HOOKFUNC)ListDisplayFunc,NULL,NULL };

/*************
 * DESCRIPTION:   build a name for a treenode from the object name,
 *    according image and surface thumb nail image
 * INPUT:         oi          object item
 *                buffer      character buffer to write name to
 *                data        class data
 * OUTPUT:        -
 *************/
static void BuildName(OBJECT_ITEM *oi, char *buffer, struct BrowserTree_Data *data)
{
	Object *thumbnail;

	switch(oi->obj->GetType())
	{
		case OBJECT_CSG:
			switch(((CSG*)oi->obj)->operation)
			{
				case CSG_UNION:
					sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_UNION], oi->obj->GetName());
					break;
				case CSG_INTERSECTION:
					sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_INTERS], oi->obj->GetName());
					break;
				case CSG_DIFFERENCE:
					sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_DIFF], oi->obj->GetName());
					break;
			}
			break;
		case OBJECT_SPHERE:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_SPHERE], oi->obj->GetName());
			break;
		case OBJECT_CYLINDER:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_CYLINDER], oi->obj->GetName());
			break;
		case OBJECT_CONE:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_CONE], oi->obj->GetName());
			break;
		case OBJECT_BOX:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_BOX], oi->obj->GetName());
			break;
		case OBJECT_PLANE:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_PLANE], oi->obj->GetName());
			break;
		case OBJECT_MESH:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_MESH], oi->obj->GetName());
			break;
		case OBJECT_CAMERA:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_CAMERA], oi->obj->GetName());
			break;
		case OBJECT_POINTLIGHT:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_POINTLIGHT], oi->obj->GetName());
			break;
		case OBJECT_SPOTLIGHT:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_SPOTLIGHT], oi->obj->GetName());
			break;
		case OBJECT_DIRECTIONALLIGHT:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_DIRECTIONALLIGHT], oi->obj->GetName());
			break;
		case OBJECT_SOR:
			sprintf(buffer, "\33O[%08lx]%s", data->image[IMAGE_SOR], oi->obj->GetName());
			break;
		default:
			strcpy(buffer, oi->obj->GetName());
			break;
	}

// There is a problem with deleting the images of the list. MUI
// causes some Enforcer hits here. So I remove this lines.
// Test it again when we have a new version of listtre class
//   if(oi->thumbnail)
//   {
//      DOMETHOD(oi->list, MUIM_List_DeleteImage, oi->thumbnail);
//      oi->thumbnail = NULL;
//   }

	if(oi->obj->surf && (GfxBase->LibNode.lib_Version>=39))
	{
		thumbnail = sciGetMaterialThumbNail(oi->obj->surf);
		if(thumbnail)
			oi->thumbnail = (Object*)DOMETHOD(data->obj, MUIM_List_CreateImage, thumbnail, 0);
	}
}

/*************
 * DESCRIPTION:   this function is called by the 'GetObjects()'-Function and adds one
 *    object-item to the list
 * INPUT:         obj         object
 *                data        browser data
 * OUTPUT:        -
 *************/
static void AddFunction(OBJECT *obj, void *data)
{
	OBJECT_ITEM oi;
	struct MUIS_Listtree_TreeNode *created;
	char buffer[256];
	OBJECT *up, *prev;

	if(!obj)
	{
		if(!((BrowserTree_Data *)data)->samelevel)
		{
			((BrowserTree_Data *)data)->where =
				(struct MUIS_Listtree_TreeNode *)DOMETHOD(
					((struct BrowserTree_Data *)data)->obj, MUIM_Listtree_GetEntry,
					((struct BrowserTree_Data *)data)->where, MUIV_Listtree_GetEntry_Position_Parent,0);
		}
		else
			((BrowserTree_Data *)data)->samelevel = FALSE;
	}
	else
	{
		((BrowserTree_Data *)data)->samelevel = TRUE;

		switch(((struct BrowserTree_Data *)data)->mode)
		{
			case BROWSER_SELECTEDSURFACE:
				if(!obj->selected || !obj->surf)
					return;
				break;
			case BROWSER_CAMERAS:
				if(obj->GetType() != OBJECT_CAMERA)
					return;
				break;
		}

		oi.obj = obj;
		oi.list = ((struct BrowserTree_Data *)data)->obj;
		oi.thumbnail = NULL;
		BuildName(&oi, buffer, (struct BrowserTree_Data *)data);

		if(obj->GoDown())
		{
			created = (struct MUIS_Listtree_TreeNode*)DOMETHOD(((struct BrowserTree_Data *)data)->obj, MUIM_Listtree_Insert,
				buffer, &oi, ((struct BrowserTree_Data *)data)->where, MUIV_Listtree_Insert_PrevNode_Sorted, TNF_LIST);
			((struct BrowserTree_Data *)data)->where = created;
			if(obj->flags & OBJECT_NODEOPEN)
			{
				// there is a bug in browsertree class which leads to some strange
				// results if a item is open when parent items are closed, so test
				// if a parent item is open before we open this item
				prev = obj;
				while(prev->GetPrev())
					prev = (OBJECT*)prev->GetPrev();
				up = (OBJECT*)prev->GoUp();
				if(up)
				{
					if(up->flags & OBJECT_NODEOPEN)
					{
						DOMETHOD(((struct BrowserTree_Data *)data)->obj, MUIM_Listtree_Open, MUIV_Listtree_Open_ListNode_Root,
							(APTR)((struct BrowserTree_Data *)data)->where, 0);
					}
					else
						obj->flags &= ~OBJECT_NODEOPEN;
				}
				else
				{
					DOMETHOD(((struct BrowserTree_Data *)data)->obj, MUIM_Listtree_Open, MUIV_Listtree_Open_ListNode_Root,
						(APTR)((struct BrowserTree_Data *)data)->where, 0);
				}
			}
		}
		else
		{
			created = (struct MUIS_Listtree_TreeNode*)DOMETHOD(((struct BrowserTree_Data *)data)->obj, MUIM_Listtree_Insert,
				buffer, &oi, ((struct BrowserTree_Data *)data)->where, MUIV_Listtree_Insert_PrevNode_Sorted, TNF_OPEN);
		}

		if(obj->selected)
			SetAttrs(((struct BrowserTree_Data *)data)->obj, MUIA_Listtree_Active, created, TAG_DONE);
	}
}

/*************
 * DESCRIPTION:   do initialations for browser tree
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct BrowserTree_Data databuf, *data;

	obj = (Object *)DoSuperNew(cl,obj,
		InputListFrame,
		MUIA_ContextMenu, TRUE,
		MUIA_List_MinLineHeight, 18,//IMAGE_HEIGHT+2,
		MUIA_Listtree_DoubleClick, MUIV_Listtree_DoubleClick_Off,
		MUIA_Listtree_SortHook, MUIV_Listtree_SortHook_Tail,
		MUIA_Listtree_ConstructHook, &ListConstHook,
		MUIA_Listtree_DestructHook, &ListDestHook,
		MUIA_Listtree_OpenHook, &ListOpenHook,
		MUIA_Listtree_CloseHook, &ListCloseHook,
		MUIA_Listtree_DisplayHook, &ListDisplayHook,
		MUIA_Listtree_Format, "MIW=20 MAW=100 BAR,MIW=-1 MAW=-1",
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct BrowserTree_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->obj = obj;
		data->draw_area = NULL;
		data->redraw = FALSE;
		data->old_entry = -1;
		data->drop_flags = 0;
		data->mode = BROWSER_ALL;
		data->dontselect = FALSE;

		DOMETHOD(obj, MUIM_Notify, MUIA_Listtree_Active, MUIV_EveryTime,
			obj, 1, MUIM_BrowserTree_Select);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Setup(struct IClass *cl,Object *obj,Msg msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data *)INST_DATA(cl,obj);
	int i;

	if (!DOSUPERMETHODA(cl,obj,msg))
		return(FALSE);

	for(i=0; i<IMAGE_AMOUNT; i++)
	{
		data->bimage[i] =
			BodyChunk(bodies[i],palette16,IMAGE_WIDTH,IMAGE_HEIGHT,4),
				MUIA_Bitmap_Transparent, 0,
			End;
		if(data->bimage[i])
			data->image[i] = (Object*)DOMETHOD(obj, MUIM_List_CreateImage, data->bimage[i], 0);
		else
			data->image[i] = NULL;
	}

	data->contextmenu = NULL;

	return TRUE;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Cleanup(struct IClass *cl,Object *obj,Msg msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data *)INST_DATA(cl,obj);
	int i;

	for(i=0; i<IMAGE_AMOUNT; i++)
	{
		if(data->image[i])
			DOMETHOD(obj, MUIM_List_DeleteImage, data->image[i]);
		if(data->image[i])
			MUI_DisposeObject(data->bimage[i]);
	}

	if(data->contextmenu)
		MUI_DisposeObject(data->contextmenu);

	return DOSUPERMETHODA(cl,obj,msg);
}

/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Set(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	tags=msg->ops_AttrList;
	do
	{
		tag = WARPOS_PPC_FUNC(NextTagItem)(&tags);
		if(tag)
		{
			switch(tag->ti_Tag)
			{
				case MUIA_BrowserTree_DrawArea:
					data->draw_area = (Object*)tag->ti_Data;
					break;
				case MUIA_BrowserTree_Mode:
					data->mode = (int)tag->ti_Data;
					break;
				case MUIA_BrowserTree_DontSelect:
					data->dontselect = (BOOL)tag->ti_Data;
			}
		}
	}
	while(tag);

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data *)INST_DATA(cl,obj);
	struct MUIS_Listtree_TreeNode *tn;
	OBJECT_ITEM *oi;

	switch (msg->opg_AttrID)
	{
		case MUIA_BrowserTree_ActiveObject:
			// get entry from list
			*(OBJECT**)(msg->opg_Storage) = NULL;
			tn = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
				MUIV_Listtree_GetEntry_ListNode_Root, MUIV_Listtree_GetEntry_Position_Active,0);
			if(tn)
			{
				oi = (OBJECT_ITEM*)tn->tn_User;
				if(oi->obj->external != EXTERNAL_CHILD)
					*(OBJECT**)(msg->opg_Storage) = oi->obj;
			}
			return TRUE;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   select an object
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Select(struct IClass *cl,Object *obj,Msg msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);
	struct MUIS_Listtree_TreeNode *tn;
	OBJECT_ITEM *oi;
	DISPLAY *disp;

	if(!data->redraw && data->draw_area)
	{
		tn = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
			MUIV_Listtree_GetEntry_ListNode_Root, MUIV_Listtree_GetEntry_Position_Active,0);
		if(tn)
		{
			oi = (OBJECT_ITEM*)tn->tn_User;
			if(!data->dontselect && (oi->obj->external != EXTERNAL_CHILD))
			{
				DeselectAll();
				oi->obj->SelectObject();

				GetAttr(MUIA_Draw_DisplayPtr, data->draw_area, (ULONG*)&disp);
				data->redraw = TRUE;
				DOMETHOD(data->draw_area,MUIM_Draw_RedrawDirty);
				data->redraw = FALSE;
			}
		}
	}

	return 0;
}

/*************
 * DESCRIPTION:   rebuild the browser contents
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Build(struct IClass *cl,Object *obj,Msg msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);

	// disable redraw to avoid flickering of the display
	SetAttrs(obj, MUIA_Listtree_Quiet, TRUE, TAG_DONE);
	data->redraw = TRUE;

	DOMETHOD(obj, MUIM_Listtree_Remove, MUIV_Listtree_Remove_ListNode_Root, MUIV_Listtree_Remove_TreeNode_All);

	// add objects to list
	data->where = NULL;
	GetObjects(AddFunction, (void*)data, 0);

	// enable redraw again
	SetAttrs(obj, MUIA_Listtree_Quiet, FALSE, TAG_DONE);
	data->redraw = FALSE;

	return 0;
}

/*************
 * DESCRIPTION:   update one item
 * INPUT:         system
 * OUTPUT:        0
 *************/
static BOOL UpdateList(MUIS_Listtree_TreeNode *tn, OBJECT *object, Object *obj, struct BrowserTree_Data *data)
{
	MUIS_Listtree_TreeNode *down;
	OBJECT_ITEM *oi;
	char buffer[256];

	while(tn)
	{
		oi = (OBJECT_ITEM*)tn->tn_User;
		if(oi->obj == object)
		{
			BuildName(oi, buffer, data);
			DOMETHOD(obj, MUIM_Listtree_Rename, tn, buffer, 0);
			return TRUE;
		}
		else
		{
			if(tn->tn_Flags & TNF_LIST)
			{
				down = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
					tn, MUIV_Listtree_GetEntry_Position_Head,0);
				if(down)
				{
					if(UpdateList(down, object, obj, data))
						return TRUE;
				}
			}

			tn = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
				tn, MUIV_Listtree_GetEntry_Position_Next,0);
		}
	}
	return FALSE;
}

static ULONG Update(struct IClass *cl,Object *obj, MUIP_OBJECT *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);
	MUIS_Listtree_TreeNode *tn;

	tn = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
		MUIV_Listtree_GetEntry_ListNode_Root, MUIV_Listtree_GetEntry_Position_Head,0);

	UpdateList(tn, msg->obj, obj, data);

	return 0;
}

/*************
 * DESCRIPTION:   add one item to the end
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG AddObject(struct IClass *cl,Object *obj, MUIP_OBJECT *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);
	OBJECT_ITEM oi;
	char buffer[256];
	struct MUIS_Listtree_TreeNode *created;

	oi.obj = msg->obj;
	oi.list = data->obj;
	oi.thumbnail = NULL;
	BuildName(&oi, buffer, (struct BrowserTree_Data *)data);

	created = (struct MUIS_Listtree_TreeNode*)DOMETHOD(data->obj, MUIM_Listtree_Insert,
		buffer, &oi, MUIV_Listtree_Insert_ListNode_Root, MUIV_Listtree_Insert_PrevNode_Tail, TNF_OPEN);
	SetAttrs(data->obj, MUIA_Listtree_Active, created, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   Update surface thumb nail images of items with given surface
 * INPUT:         system
 * OUTPUT:        0
 *************/
static void UpdateThumbNailList(MUIS_Listtree_TreeNode *tn, SURFACE *surf, Object *obj, struct BrowserTree_Data *data)
{
	MUIS_Listtree_TreeNode *down;
	OBJECT_ITEM *oi;
	char buffer[256];

	while(tn)
	{
		oi = (OBJECT_ITEM*)tn->tn_User;
		if(oi->obj->surf == surf)
		{
			BuildName(oi, buffer, data);
			DOMETHOD(obj, MUIM_Listtree_Rename, tn, buffer, 0);
		}

		if(tn->tn_Flags & TNF_LIST)
		{
			down = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
				tn, MUIV_Listtree_GetEntry_Position_Head,0);
			if(down)
				UpdateThumbNailList(down, surf, obj, data);
		}

		tn = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
			tn, MUIV_Listtree_GetEntry_Position_Next,0);
	}
}

static ULONG UpdateThumbNail(struct IClass *cl,Object *obj, MUIP_Material_Material *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);
	MUIS_Listtree_TreeNode *tn;

	tn = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
		MUIV_Listtree_GetEntry_ListNode_Root, MUIV_Listtree_GetEntry_Position_Head,0);

	UpdateThumbNailList(tn, msg->surf, obj, data);

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG DragReport(struct IClass *cl,Object *obj, MUIP_DragReport *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);
	struct MUIS_Listtree_TestPos_Result r;
	struct MUIS_Listtree_TreeNode *tn;
	ULONG dummy;
	BOOL surf;

	DOMETHOD(obj, MUIM_Listtree_TestPos, msg->x, msg->y, &r);

	if((r.tpr_ListEntry != data->old_entry) || (r.tpr_Flags != data->drop_flags))
	{
		if(!msg->update)
			return(MUIV_DragReport_Refresh);

		data->old_entry = r.tpr_ListEntry;
		data->drop_flags = r.tpr_Flags;

		tn = (MUIS_Listtree_TreeNode *)r.tpr_TreeNode;
		if(tn)
		{
			surf = GetAttr(MUIA_MaterialItem_Surface, msg->obj, &dummy);
			/*** if entry to drop has a list ***/
			// test if dropped object is a surface
			if(surf)
				DOMETHOD(obj, MUIM_Listtree_SetDropMark, r.tpr_ListEntry, MUIV_Listtree_TestPos_Result_Flags_Onto);
			else
			{
				DOMETHOD(obj, MUIM_Listtree_SetDropMark, r.tpr_ListEntry, r.tpr_Flags);
			}
			data->drop = tn;
		}
		else
		{
			DOMETHOD(obj, MUIM_Listtree_SetDropMark, r.tpr_ListEntry, MUIV_Listtree_SetDropMark_Values_None);
			data->drop = NULL;
		}
	}

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG DragDrop(struct IClass *cl,Object *obj, MUIP_DragDrop *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);
	struct MUIS_Listtree_TreeNode *tn;
	OBJECT *object;
	int insert = INSERT_HORIZ;
	BOOL updateall=FALSE;
	SURFACE *surf;
	MATRIX m,m1,matrix;

	// test if dropped object is a surface
	if(GetAttr(MUIA_MaterialItem_Surface, msg->obj, (ULONG*)&surf))
	{
		object = ((OBJECT_ITEM*)data->drop->tn_User)->obj;
		if(object->surf)
		{
			if(object->surf != surf)
			{
				sciAssignSurface(surf, object);
				DOMETHOD(obj, MUIM_BrowserTree_Update, ((OBJECT_ITEM*)data->drop->tn_User)->obj);
			}
		}
	}
	else
	{
		tn = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
			MUIV_Listtree_GetEntry_ListNode_Root, MUIV_Listtree_GetEntry_Position_Active,0);

		if(tn && (((OBJECT_ITEM*)tn->tn_User)->obj != ((OBJECT_ITEM*)data->drop->tn_User)->obj))
		{
			switch(data->drop_flags)
			{
				case MUIV_Listtree_TestPos_Result_Flags_Above:
					if(data->drop)
					{
						data->drop = (struct MUIS_Listtree_TreeNode *)DOMETHOD(obj, MUIM_Listtree_GetEntry,
							data->drop, MUIV_Listtree_GetEntry_Position_Previous,0);
						if(data->drop)
						{
							DOMETHOD(obj, MUIM_Listtree_Move,
								MUIV_Listtree_Move_OldListNode_Root, tn,
								MUIV_Listtree_Move_NewListNode_Root, data->drop, 0);
						}
						else
						{
							updateall = TRUE;
/*                   DOMETHOD(obj, MUIM_Listtree_Move,
								MUIV_Listtree_Move_OldListNode_Root, tn,
								MUIV_Listtree_Move_NewListNode_Root, MUIV_Listtree_Move_NewTreeNode_Head, 0);*/
						}
					}
					break;

				case MUIV_Listtree_TestPos_Result_Flags_Below:
					if(data->drop)
						DOMETHOD(obj, MUIM_Listtree_Move,
							MUIV_Listtree_Move_OldListNode_Root, tn,
							MUIV_Listtree_Move_NewListNode_Root, data->drop, 0);
					break;

				case MUIV_Listtree_TestPos_Result_Flags_Onto:
					if(data->drop)
					{
						updateall = TRUE;
/*                DOMETHOD(obj, MUIM_Listtree_Move,
							MUIV_Listtree_Move_OldListNode_Root, tn,
							data->drop, MUIV_Listtree_Move_NewTreeNode_Head, 0);*/

						insert = INSERT_VERT;
					}
					break;

				case MUIV_Listtree_TestPos_Result_Flags_Sorted:
					DOMETHOD(obj, MUIM_Listtree_Move,
						MUIV_Listtree_Move_OldListNode_Root, tn,
						MUIV_Listtree_Move_NewListNode_Root, MUIV_Listtree_Move_NewTreeNode_Head, 0);
					break;
			}

			object = ((OBJECT_ITEM*)tn->tn_User)->obj;

			if(data->drop)
			{
				((OBJECT_ITEM*)data->drop->tn_User)->obj->GetObjectMatrix(&m1);
				m1.InvMat(&m);

				object->GetObjectMatrix(&m1);
				matrix.MultMat(&m1, &m);
				SetVector(&object->pos, matrix.m[1], matrix.m[2], matrix.m[3]);
				SetVector(&object->orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
				SetVector(&object->orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
				SetVector(&object->orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);

				object->DeChainObject();
				object->Insert(((OBJECT_ITEM*)data->drop->tn_User)->obj, insert);
			}
			else
			{
				object->GetObjectMatrix(&matrix);
				matrix.GenerateAxis(&object->orient_x, &object->orient_y, &object->orient_z, &object->pos);
				object->DeChainObject();
				object->Insert(NULL, insert);
			}

			if(updateall)
				sciBrowserBuild();
		}
	}

	return 0;
}

/*************
 * DESCRIPTION:   Called when MUI is about to build a popup menu. Look if on the specified
 *    position is an object, and build the right menu.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);
	struct MUIS_Listtree_TestPos_Result item;
	struct MUIS_Listtree_TreeNode *tn;
	struct NewMenu *menu;
	OBJECT *object;

	if(data->contextmenu)
		MUI_DisposeObject(data->contextmenu);

	if(DOMETHOD(obj, MUIM_Listtree_TestPos, msg->mx, msg->my, &item))
	{
		tn = (MUIS_Listtree_TreeNode*)item.tpr_TreeNode;
		if(tn)
		{
			SetAttrs(obj, MUIA_Listtree_Active, tn, TAG_DONE);
			object = ((OBJECT_ITEM*)tn->tn_User)->obj;
			if(object)
			{
				menu = object->GetPopUpList();
				if(menu && (object->external != EXTERNAL_CHILD))
				{
					menu->nm_Label = object->GetName();
					data->contextmenu = MUI_MakeObject(MUIO_MenustripNM, menu, 0);
					return (ULONG)data->contextmenu;
				}
			}
		}
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Called when an user selects a menu item from a popup menu.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg)
{
	struct BrowserTree_Data *data = (struct BrowserTree_Data*)INST_DATA(cl,obj);

	if(data->draw_area)
		DOMETHOD(data->draw_area, MUIM_ContextMenuChoice, msg->item);

	return 0;
}

/*************
 * DESCRIPTION:   Dispatcher of browser tree class
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG BrowserTree_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
		case OM_SET:
			return(Set(cl,obj,(struct opSet*)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case MUIM_Setup:
			return Setup(cl,obj,msg);
		case MUIM_Cleanup:
			return Cleanup(cl,obj,msg);
		case MUIM_BrowserTree_Select:
			return(Select(cl,obj,msg));
		case MUIM_BrowserTree_Build:
			return(Build(cl,obj,msg));
		case MUIM_BrowserTree_Clear:
			DOMETHOD(obj, MUIM_KillNotify, MUIA_Listtree_Active);
			DOMETHOD(obj, MUIM_Listtree_Remove, MUIV_Listtree_Remove_ListNode_Root,
				MUIV_Listtree_Remove_TreeNode_All);
			DOMETHOD(obj, MUIM_Notify, MUIA_Listtree_Active, MUIV_EveryTime,
				obj, 1, MUIM_BrowserTree_Select);
			return 0;
		case MUIM_BrowserTree_Update:
			return(Update(cl,obj,(MUIP_OBJECT*)msg));
		case MUIM_BrowserTree_AddObject:
			return(AddObject(cl,obj,(MUIP_OBJECT*)msg));
		case MUIM_BrowserTree_UpdateThumbNail:
			return(UpdateThumbNail(cl,obj,(MUIP_Material_Material*)msg));
		case MUIM_DragQuery:
			return MUIV_DragQuery_Accept;
		case MUIM_DragReport:
			return(DragReport(cl,obj,(MUIP_DragReport*)msg));
		case MUIM_DragDrop:
			return(DragDrop(cl,obj,(MUIP_DragDrop*)msg));
		case MUIM_ContextMenuBuild:
			return(ContextMenuBuild(cl,obj,(struct MUIP_ContextMenuBuild*)msg));
		case MUIM_ContextMenuChoice:
			return(ContextMenuChoice(cl,obj,(struct MUIP_ContextMenuChoice*)msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
