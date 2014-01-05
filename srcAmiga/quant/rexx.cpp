/***************
 * MODUL:         Rexx support
 * NAME:          rexx.cpp
 * VERSION:       0.1 08.03.1995
 * DESCRIPTION:   This modul contains all functions needed to build and
 *						delete a ARexx-port and to handle ARexx messages.
 *	AUTHORS:			Andreas Heumann
 * BUGS:          none
 * TO DO:         all
 * HISTORY:       DATE		NAME	COMMENT
 *						08.03.95	ah		first release
 ***************/

#include <string.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <rexx/storage.h>
#include <rexx/rxslib.h>

#include <pragma/exec_lib.h>
#include <pragma/rexxsyslib_lib.h>

#ifndef REXX_H
#include "rexx.h"
#endif

/*************
 * FUNCTION:		CreateRexxHost
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Creates a RexxHost (special MsgPort) with a given name.
 *						Returns NULL if port already exists.
 * INPUT:			HostName
 * OUTPUT:			RexxHost
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
struct MsgPort *CreateRexxHost(STRPTR HostName)
{
	struct MsgPort *RexxHost;

	/* Valid name given? */
	if(HostName && HostName[0])
	{
		/* Already present? */
		if(!FindPort((char *)HostName))
		{
			/* Allocate the port body. */
			RexxHost = (struct MsgPort *)AllocMem(sizeof(struct MsgPort),MEMF_PUBLIC | MEMF_CLEAR);
			if(RexxHost)
			{
				/* Allocate a signal bit. */
				if((RexxHost->mp_SigBit = AllocSignal(-1)) != -1)
				{
					/* Initialize the MsgPort node head. */
					RexxHost->mp_Node.ln_Type	= NT_MSGPORT;
					RexxHost->mp_Node.ln_Pri	= 0;

					/* Allocate memory for MsgPort name. */
					RexxHost->mp_Node.ln_Name = (char *)AllocMem(strlen((char *)HostName)+1,MEMF_PUBLIC);
					if(RexxHost->mp_Node.ln_Name)
					{
						/* Copy the name. */
						strcpy(RexxHost->mp_Node.ln_Name,(char *)HostName);

						/* Deal with the rest of the flags. */
						RexxHost->mp_Flags	= PA_SIGNAL;
						RexxHost->mp_SigTask	= FindTask(NULL);

						/* Finally add it to the public port list. */
						AddPort(RexxHost);

						/* And return it to the caller. */
						return(RexxHost);
					}
					FreeSignal(RexxHost->mp_SigBit);
				}
				FreeMem(RexxHost,sizeof(struct MsgPort));
			}
		}
	}
	return(NULL);
}

/*************
 * FUNCTION:		DeleteRexxHost
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Deletes a MsgPort as created by CreateRexxHost().
 *						Returns NULL, so user can do 'Host = DeleteRexxHost(Host);'
 * INPUT:			RexxHost
 * OUTPUT:			NULL
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
void *DeleteRexxHost(struct MsgPort *RexxHost)
{
	/* Valid host port given? */
	if(RexxHost)
	{
		/* Remove it from the public list. */
		RemPort(RexxHost);

		/* Free the name. */
		FreeMem(RexxHost->mp_Node.ln_Name,strlen(RexxHost->mp_Node.ln_Name)+1);

		/* Free the allocated signal bit. */
		FreeSignal(RexxHost->mp_SigBit);

		/* Free the body. */
		FreeMem(RexxHost,sizeof(struct MsgPort));
	}
	return(NULL);
}

/*************
 * FUNCTION:		SendRexxCommand
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Sends a command to the rexx server, requires pointers
 *						to the MsgPort of the calling Host and the command string.
 *						File extension and host name are optional and may be
 *						NULL
 * INPUT:			HostPort
 *						CommandString
 *						FileExtension
 *						HostName
 * OUTPUT:			TRUE: sucessfull, else false
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
LONG SendRexxCommand(struct MsgPort *HostPort,STRPTR CommandString,STRPTR FileExtension,STRPTR HostName)
{
	struct MsgPort	*RexxPort = (struct MsgPort *)FindPort(RXSDIR);
	struct RexxMsg	*HostMessage;

	/* Valid pointers given? */
	if(CommandString && HostPort && RexxPort)
	{
		/* No special host name given? Take the MsgPort name. */
		if(!HostName)
			HostName = (STRPTR)HostPort->mp_Node.ln_Name;

		/* No file name extension? Take the default. */
		if(!FileExtension)
			FileExtension = (STRPTR)"rexx";

		/* Create the message. */
		HostMessage = CreateRexxMsg((struct MsgPort *)HostPort,(char *)FileExtension,(char *)HostName);
		if(HostMessage)
		{
			/* Add the command. */
			HostMessage->rm_Args[0] = CreateArgstring((char *)CommandString,strlen((char *)CommandString));
			if(HostMessage->rm_Args[0])
			{
				/* This is a command, not a function. */
				HostMessage->rm_Action = RXCOMM;

				/* Release it... */
				PutMsg(RexxPort,(struct Message*)HostMessage);

				/* Successful action. */
				return(TRUE);
			}
			DeleteRexxMsg(HostMessage);
		}
	}
	return(FALSE);
}

/*************
 * FUNCTION:		FreeRexxCommand
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Frees the contents of a RexxMsg
 * INPUT:			RexxMessage
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
VOID FreeRexxCommand(struct RexxMsg *RexxMessage)
{
	/* Valid pointer given? */
	if(RexxMessage && RexxMessage->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
	{
		/* Remove argument. */
		if(RexxMessage->rm_Args[0])
			DeleteArgstring((char *)RexxMessage->rm_Args[0]);

		/* Free the message. */
		DeleteRexxMsg(RexxMessage);
	}
}

/*************
 * FUNCTION:		ReplyRexxCommand
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Sends a RexxMsg back to the rexx server, can include
 *						result codes
 * INPUT:			RexxMessage
 *						Primary			primary result
 *						Secondary		secondary result
 *						Result			result string
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
VOID ReplyRexxCommand(struct RexxMsg *RexxMessage,LONG Primary,LONG Secondary,STRPTR Result)
{
	/* Valid pointer given? */
	if(RexxMessage && RexxMessage->rm_Node.mn_Node.ln_Type == NT_MESSAGE)
	{
		/* No secondary result and results wanted? */
		if(Secondary == NULL && (RexxMessage->rm_Action & RXFF_RESULT))
		{
			/* Build result string... */
			if(Result)
				Secondary = (LONG)CreateArgstring((char *)Result,strlen((char *)Result));
		}

		/* Set both results... */
		RexxMessage->rm_Result1 = Primary;
		RexxMessage->rm_Result2 = Secondary;

		/* ...and reply the message. */
		ReplyMsg((struct Message *)RexxMessage);
	}
}

/*************
 * FUNCTION:		GetRexxCommand
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Returns a pointer to the command string if
 *						the RexxMsg is a command request
 * INPUT:			RexxMessage
 * OUTPUT:			command string
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
STRPTR GetRexxCommand(struct RexxMsg *RexxMessage)
{
	if(!RexxMessage || RexxMessage->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
		return(NULL);
	else
		return(RexxMessage->rm_Args[0]);
}

/*************
 * FUNCTION:		GetRexxArg
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Returns a pointer to the first RexxMsg argument
 * INPUT:			RexxMessage
 * OUTPUT:			first argument
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
STRPTR GetRexxArg(struct RexxMsg *RexxMessage)
{
	if(!RexxMessage)
		return(NULL);
	else
		return(RexxMessage->rm_Args[0]);
}

/*************
 * FUNCTION:		GetRexxResult1
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Returns the 1st RexxMsg result
 * INPUT:			RexxMessage
 * OUTPUT:			1st result
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
LONG GetRexxResult1(struct RexxMsg *RexxMessage)
{
	if(!RexxMessage)
		return(NULL);
	else
		return(RexxMessage->rm_Result1);
}

/*************
 * FUNCTION:		GetRexxResult2
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Returns the 2nd RexxMsg result
 * INPUT:			RexxMessage
 * OUTPUT:			2nd result
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
LONG GetRexxResult2(struct RexxMsg *RexxMessage)
{
	if(!RexxMessage)
		return(NULL);
	else
		return(RexxMessage->rm_Result2);
}

/*************
 * FUNCTION:		GetRexxMsg
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Picks up pending RexxMessages from a RexxHost and
 *						returns them to the caller. I desired, will wait
 *						for new messages to arrive if none is present yet
 * INPUT:			RexxHost			rexx host message port
 *						Wait				TRUE: wait for new messages
 * OUTPUT:			received RexxMessage
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
struct RexxMsg *GetRexxMsg(struct MsgPort *RexxHost,LONG Wait)
{
	struct RexxMsg *RexxMessage = NULL;

	/* Valid pointer given? */
	if(RexxHost)
	{
		/* Try to pick up a message. */
		RexxMessage = (struct RexxMsg *)GetMsg((struct MsgPort *)RexxHost);
		while(!RexxMessage)
		{
			/* No message available. Are we to wait? */
			if(Wait)
				WaitPort((struct MsgPort *)RexxHost);
			else
				break;
		}
	}
	/* Return the result (may be NULL). */
	return(RexxMessage);
}

/*************
 * FUNCTION:		SendRexxMsg
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Sends a single (or a list of) command(s) to Rexx host
 *						and returns the secondary result
 * INPUT:			HostName			name of host
 *						MsgList			messagelist (SingleMsg must be NULL)
 *						SingleMsg		single message (MsgList must be NULL)
 *						GetResult		TRUE: get results
 * OUTPUT:			secondary result
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
ULONG SendRexxMsg(STRPTR HostName,STRPTR *MsgList,STRPTR SingleMsg,LONG GetResult)
{
	struct RexxMsg *RexxMessage;
	struct MsgPort *HostPort,*ReplyPort;
	ULONG Result = 0;
	SHORT i;

	/* Valid pointers given? */
	if(HostName && (MsgList || SingleMsg))
	{
		/* Can we find the host? */
		HostPort = (struct MsgPort *)FindPort((char *)HostName);
		if(HostPort)
		{
			/* Allocate a reply port. */
			ReplyPort = (struct MsgPort *)AllocMem(sizeof(struct MsgPort),MEMF_PUBLIC | MEMF_CLEAR);
			if(ReplyPort)
			{
				ReplyPort->mp_SigBit = AllocSignal(-1);
				if(ReplyPort->mp_SigBit != -1)
				{
					ReplyPort->mp_Node.ln_Type	= NT_MSGPORT;
					ReplyPort->mp_Flags			= PA_SIGNAL;
					ReplyPort->mp_SigTask		= FindTask(NULL);

					ReplyPort->mp_MsgList.lh_Head = (struct Node *)&ReplyPort->mp_MsgList.lh_Tail;
					ReplyPort->mp_MsgList.lh_Tail = 0;
					ReplyPort->mp_MsgList.lh_TailPred = (struct Node *)&ReplyPort->mp_MsgList.lh_Head;

					/* Create a Rexx message. */
					RexxMessage = (struct RexxMsg *)CreateRexxMsg(ReplyPort,"",(char *)HostName);
					if(RexxMessage)
					{
						/* A list of arguments or only a single arg? */
						if(MsgList)
						{
							for(i=0 ; i<16 ; i++)
								RexxMessage->rm_Args[i] = MsgList[i];
						}
						else
							RexxMessage->rm_Args[0] = SingleMsg;

						/* Do we want result codes? */
						if(GetResult)
							RexxMessage->rm_Action = RXFF_RESULT;

						/* Send packet and wait for the reply. */
						PutMsg(HostPort,(struct Message *)RexxMessage);
						WaitPort(ReplyPort);

						/* Remember result. */
						if(GetResult && !RexxMessage->rm_Result1)
							Result = RexxMessage->rm_Result2;

						/* Remove Rexx message. */
						DeleteRexxMsg(RexxMessage);
					}
					/* Free reply port signal bit. */
					FreeSignal(ReplyPort->mp_SigBit);
				}
				/* Free the replyport itself. */
				FreeMem(ReplyPort,sizeof(struct MsgPort));
			}
		}
	}
	/* Return the result. */
	return(Result);
}

/*************
 * FUNCTION:		GetRexxString
 * VERSION:			0.1 08.03.1995
 * DESCRIPTION:	Copy the result string returned by SendRexxMsg to user
 *						buffer and/or remove the original string.
 * INPUT:			SourceString
 *						DestString
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.03.95	ah			first release
 *************/
VOID GetRexxString(STRPTR SourceString,STRPTR DestString)
{
	/* Valid pointer given? */
	if(SourceString)
	{
		/* Destination memory buffer given? */
		if(DestString)
			strcpy((char *)DestString,(char *)SourceString);

		/* Deallocate the original string. */
		DeleteArgstring((char *)SourceString);
	}
}

