/***************
 * NAME:          rexxlib.h
 * VERSION:			V1.0 24.05.96
 * DESCRIPTION:	definitions for rexxlib
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		24.05.96	ah		initial release
 ***************/

#ifndef REXXLIB_H
#define REXXLIB_H

enum
{
	ERR_NONE,			// no error
	ERR_PORT,			// can't find port
	ERR_REPLYPORT,		// can't create reply port
	ERR_REPLYSIGNAL,	// can't allocate signal for port
	ERR_REXXSYSLIB,	// can't open rexxsyslib.library
	ERR_NOINIT			// you haven't called Rexx_Init befor Rexx_SendMsg
};

#ifdef __cplusplus
extern "C"
{
#endif
int Rexx_Init(char *port);
int Rexx_SendMsg(char *SingleMsg, int *result);
void Rexx_Cleanup();
#ifdef __cplusplus
}
#endif

#endif