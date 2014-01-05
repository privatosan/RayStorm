/***************
 * PROGRAM:       Modeler
 * NAME:          typedefs.h
 * DESCRIPTION:   type definitions
 * AUTHORS:       Andreas Heumann
 * HISTORY:       
 *    DATE     NAME  COMMENT
 *    30.11.95 ah    initial release
 *    19.02.98 ah    added PPC definitions
 ***************/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <exec/types.h>

#ifdef __PPC__
#define REG(x)
#define ASM
#define SAVEDS
#if __STORM__ >= 39
#define WARPOS_PPC_FUNC(a) a ## PPC
#else
#define WARPOS_PPC_FUNC(a) PPC ## a
#endif
#else
#define REG(x) register __ ## x
#define ASM
#define SAVEDS
#define WARPOS_PPC_FUNC(a) a
#endif

#endif
