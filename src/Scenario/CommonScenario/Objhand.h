/***************
 * PROGRAM:       Modeler
 * NAME:          objhand.h
 * DESCRIPTION:   definitions for external object loader
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    20.03.96 ah    initial release
 ***************/

#ifndef OBJHAND_H
#define OBJHAND_H

typedef void ACTOR;
typedef void IM_TEXTURE;

#ifndef OBJLINK_H
#include "objlink.h"
#endif

#ifndef UNDO_H
#include "undo.h"
#endif

extern OBJLINK link;

BOOL InitObjHand(OBJECT*, int, UNDO_CREATE*);
void CleanupObjHand();

#endif /* OBJHAND_H */
