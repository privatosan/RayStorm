/***************
 * PROGRAM:       Modeler
 * NAME:          ppc_mui_stubs.h
 * DESCRIPTION:   mui stubs for ppc
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    19.02.98 ah    initial release
 ***************/

#ifndef PPC_MUI_STUBS_H
#define PPC_MUI_STUBS_H

#define MUI_HOOK_PROTO(name) SAVEDS ASM ULONG stub_ ## name (REG(a1) APTR, REG(a2) APTR);

MUI_HOOK_PROTO(animtree_ListDisplayFunc)

#define MUI_DISPATCH_PROTO(name) SAVEDS ASM ULONG sub_ ## name (REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

MUI_DISPATCH_PROTO(AnimTree_Dispatcher)

#endif // PPC_MUI_STUBS_H
