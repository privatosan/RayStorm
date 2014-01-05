/***************
 * NAME:          rexxlib.c
 * VERSION:			V1.0 24.05.96
 * DESCRIPTION:	Utility functions to send ARexx commands
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		24.05.96	ah		initial release
 ***************/

#include <string.h>

#include <exec/memory.h>
#include <rexx/storage.h>
#include <rexx/rxslib.h>
#include <exec/types.h>

#include <pragma/exec_lib.h>
#include <pragma/rexxsyslib_lib.h>

#include "rexxlib.h"

struct Library *RexxSysBase = NULL;
static struct MsgPort *HostPort;
static struct MsgPort *ReplyPort = NULL;

/*************
 * FUNCTION:		Rexx_Cleanup
 * DESCRIPTION:	close libs, free Port ...
 * INPUT:			none
 * OUTPUT:			none
 *************/
void Rexx_Cleanup()
{
	if(ReplyPort)
	{
		// Free reply port signal bit
		FreeSignal(ReplyPort->mp_SigBit);
		// Free the replyport itself
		FreeMem(ReplyPort,sizeof(struct MsgPort));
	}
	if (RexxSysBase)
		CloseLibrary(RexxSysBase);
}

/*************
 * FUNCTION:		Rexx_SendMsg
 * DESCRIPTION:	Sends a single command to Rexx host
 * INPUT:			SingleMsg	message to send
 *						result		result of command
 * OUTPUT:			error number
 *************/
int Rexx_SendMsg(char *SingleMsg, int *result)
{
	struct RexxMsg *RexxMessage;

	// Valid pointers given?
	if( HostPort && SingleMsg && ReplyPort )
	{
		// Create a Rexx message
		RexxMessage = (struct RexxMsg *)CreateRexxMsg(ReplyPort,"",NULL);
		if(RexxMessage)
		{
			RexxMessage->rm_Args[0] = CreateArgstring(SingleMsg, strlen(SingleMsg));
			RexxMessage->rm_Action = RXFF_RESULT;

			// Send packet
			PutMsg(HostPort,(struct Message *)RexxMessage);
			WaitPort(ReplyPort);
			RexxMessage = (struct RexxMsg*)GetMsg(ReplyPort);

			// Remove Rexx message
			*result = RexxMessage->rm_Result1;

			ClearRexxMsg(RexxMessage,1);
			DeleteRexxMsg(RexxMessage);
		}
	}
	else
		return ERR_NOINIT;
	
	return NULL;
}

/*************
 * FUNCTION:		Rexx_Init
 * DESCRIPTION:	Open libs, init ports
 * INPUT:			port		name of port to send messages to
 * OUTPUT:			error number
 *************/
int Rexx_Init(char *port)
{
	// is raystrom already active ?
	HostPort = (struct MsgPort *)FindPort(port);
	if(!HostPort)
		return ERR_PORT;

	// Allocate a reply port
	ReplyPort = (struct MsgPort *)AllocMem(sizeof(struct MsgPort),MEMF_PUBLIC | MEMF_CLEAR);
	if( !ReplyPort )
		return ERR_REPLYPORT;

	ReplyPort->mp_SigBit = AllocSignal(-1);
	if(ReplyPort->mp_SigBit == -1)
		return ERR_REPLYSIGNAL;

	ReplyPort->mp_Node.ln_Type	= NT_MSGPORT;
	ReplyPort->mp_Flags			= PA_SIGNAL;
	ReplyPort->mp_SigTask		= FindTask(NULL);

	ReplyPort->mp_MsgList.lh_Head = (struct Node *)&ReplyPort->mp_MsgList.lh_Tail;
	ReplyPort->mp_MsgList.lh_Tail = 0;
	ReplyPort->mp_MsgList.lh_TailPred = (struct Node *)&ReplyPort->mp_MsgList.lh_Head;

	// Open rexxsyslib library.
	RexxSysBase = OpenLibrary("rexxsyslib.library",0L);
	if (!RexxSysBase)
		return ERR_REXXSYSLIB;

	return ERR_NONE;
}
