/***************
 * PROGRAM:       Modeler
 * NAME:          animbar_class.h
 * DESCRIPTION:   definitions for animation bar subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.11.98 ah    initial release
 ***************/

#ifndef ANIMBAR_CLASS_H
#define ANIMBAR_CLASS_H

struct AnimBar_Data
{
	Object *b_first, *b_prev, *b_stop, *b_play, *b_next, *b_last;
	Object *sl_frame;
};

SAVEDS ASM ULONG AnimBar_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
