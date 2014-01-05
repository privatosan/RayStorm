/***************
 * MODUL:         Rexx support
 * NAME:          rexx.h
 * VERSION:       0.1 08.03.1995
 * DESCRIPTION:   This modul contains all definitions for the Rexx module
 *	AUTHORS:			Andreas Heumann
 * BUGS:          none
 * TO DO:         all
 * HISTORY:       DATE		NAME	COMMENT
 *						98.03.95	ah		first release
 ***************/

#ifndef REXX_H
#define REXX_H

#include <exec/types.h>
#include <rexx/storage.h>

struct MsgPort *CreateRexxHost(STRPTR HostName);
void *DeleteRexxHost(struct MsgPort *RexxHost);
LONG SendRexxCommand(struct MsgPort *HostPort,STRPTR CommandString,
	STRPTR FileExtension,STRPTR HostName);
VOID FreeRexxCommand(struct RexxMsg *RexxMessage);
VOID ReplyRexxCommand(struct RexxMsg *RexxMessage,LONG Primary,LONG Secondary,STRPTR Result);
STRPTR GetRexxCommand(struct RexxMsg *RexxMessage);
STRPTR GetRexxArg(struct RexxMsg *RexxMessage);
LONG GetRexxResult1(struct RexxMsg *RexxMessage);
LONG GetRexxResult2(struct RexxMsg *RexxMessage);
struct RexxMsg *GetRexxMsg(struct MsgPort *RexxHost,LONG Wait);
ULONG SendRexxMsg(STRPTR HostName,STRPTR *MsgList,STRPTR SingleMsg,LONG GetResult);
VOID GetRexxString(STRPTR SourceString,STRPTR DestString);

#endif // REXX_H
