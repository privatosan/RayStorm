/***************
 * PROGRAM:       Modeler
 * NAME:          ppc_mui_stubs.cpp
 * DESCRIPTION:   mui stubs for ppc
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    19.02.98 ah    initial release
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#define MUI_HOOK(name) SAVEDS ASM ULONG stub_ ## name (REG(a1) APTR msg, REG(a2) APTR object) \
{ \
	return name ## (msg, object); \
}

MUI_HOOK(animtree_ListDisplayFunc)

#define MUI_DISPATCH(name) SAVEDS ASM ULONG sub_ ## name (REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg) \
{ \
	return name ## (cl, obj, msg);
}

MUI_DISPATCH(AnimTree_Dispatcher)
