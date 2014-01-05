/***************
 * PROGRAM:       Modeler
 * NAME:          subtask.cpp
 * DESCRIPTION:   some functions to easily spawn subtask
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.05.97 ah    initial release
 ***************/

#include <dos/dostags.h>
#include <exec/memory.h>

#include <pragma/dos_lib.h>
#include <pragma/exec_lib.h>
//#ifdef __PPC__
#include <pragma/powerpc_lib.h>
//#endif

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef SUBTASK_H
#include "subtask.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifdef __MIXEDBINARY__
#define LINES 16
#else
#define LINES 8
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   thread which does the rendering. This subtask waits
 *    for a STC_START message. Then it renders the scene
 * INPUT:         -
 * OUTPUT:        -
 *************/
#ifdef __PPC__
extern "ASM" void _RenderSubTaskPPC(struct SubTask *st)
#else
void RenderSubTask()
#endif
{
#ifndef __PPC__
	struct SubTask *st;
#endif
	BOOL done = FALSE, start = FALSE;
	struct SubTaskMsg *stm;
	struct RenderData *data;

#ifndef __PPC__
	st = InitSubTask();
#endif
	if(st)
	{
		/*
		** after the sub task is up and running, we go into
		** a loop and process the messages from the main task.
		*/
		while(!done)
		{
			while((stm = (struct SubTaskMsg *)WARPOS_PPC_FUNC(GetMsg)(st->st_Port)))
			{
				switch(stm->stm_Command)
				{
					case STC_SHUTDOWN:
						done = TRUE;
						break;
					case STC_START:
						data = (struct RenderData*)stm->stm_Parameter;
						start = TRUE;
						break;
/*               case STC_STOP:
						break;
					case STC_RESUME:
						break;*/
				}

				/*
				** If we received a shutdown message, we do not reply it
				** immediately. First, we need to free our resources.
				*/
				if(done)
					break;

				WARPOS_PPC_FUNC(ReplyMsg) ((struct Message *)stm);

				if(start)
				{
					data->err = sciRender(data->rc);
					DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 4, MUIM_MsgHandler_PushMessage, data->caller, 1, MUIM_SubTask_Finished);
					start = FALSE;
				}
			}
			/* We have nothing to do, just sit quietly and wait for something to happen */
			if(!done)
				WARPOS_PPC_FUNC(WaitPort)(st->st_Port);
		}
#ifdef __PPC__
		ExitSubTaskPPC(st, stm);
#else
		ExitSubTask(st, stm);
#endif
	}
}
#endif

/*************
 * DESCRIPTION:   thread which does the preview rendering. This subtask waits
 *    for a STC_START message with PreviewData as parameter. It then initializes
 *    the render area and the preview class. After it renderd one line it sends
 *    the render area a DrawUpdate message.
 * OLD Version: "and switches in stop state. The render
 *    area has to send a STC_RESUME message if it has renderd the line to screen."
 * INPUT:         -
 * OUTPUT:        -
 *************/
#ifdef __PPC__
void PreviewSubTaskPPC(struct SubTask *st)
#else
void PreviewSubTask()
#endif
{
#ifndef __PPC__
	struct SubTask *st;
#endif
	BOOL done = FALSE, working = FALSE, init = FALSE, stop = FALSE;
	int y;
	struct SubTaskMsg *stm;
	struct PreviewData *data;
	rsiSMALL_COLOR *line;

#ifndef __PPC__
	st = InitSubTask();
#endif
	if(st)
	{
		/*
		** after the sub task is up and running, we go into
		** a loop and process the messages from the main task.
		*/
		while(!done)
		{
			while((stm = (struct SubTaskMsg *)WARPOS_PPC_FUNC(GetMsg)(st->st_Port)))
			{
				switch(stm->stm_Command)
				{
					case STC_SHUTDOWN:
						// This is the shutdown message from KillSubTask().
						done = TRUE;
						working = FALSE;
						break;
					case STC_CLEANUP:
						working = FALSE;
						if(data->preview)
						{
#ifdef __MIXEDBINARY__
							PreviewDelete(data->preview);
#else
							delete data->preview;
#endif
							data->preview = NULL;
						}
						break;
					case STC_START:
						working = TRUE;
						stop = FALSE;
						init = FALSE;
						data = (struct PreviewData*)stm->stm_Parameter;
#ifdef __MIXEDBINARY__
						data->preview = PreviewCreate();
#else
						data->preview = new PREVIEW;
#endif
						if(!data->preview)
							done = TRUE;
						break;
					case STC_STOP:
						stop = TRUE;
						break;
					case STC_RESUME:
						stop = FALSE;
						break;
				}

				/*
				** If we received a shutdown message, we do not reply it
				** immediately. First, we need to free our resources.
				*/
				if(done)
					break;
				WARPOS_PPC_FUNC(ReplyMsg)((struct Message *)stm);
			}
			if(working && !stop)
			{
				if(!init)
				{
#ifdef __MIXEDBINARY__
					if(Init(data->preview, data->width, data->height, LINES, data->surf, data->object))
#else
					if(data->preview->Init(data->width, data->height, LINES, data->surf, data->object))
#endif
					{
						init = TRUE;
						DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 4, MUIM_MsgHandler_PushMessage, data->renderarea, 1, MUIM_Render_ResetColors);
						DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 6, MUIM_MsgHandler_PushMessage, data->renderarea, 3, MUIM_Set, MUIA_Render_SubTask, st);
#ifdef __MIXEDBINARY__
						DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 6, MUIM_MsgHandler_PushMessage, data->renderarea, 3, MUIM_Set, MUIA_Render_Data, GetLine(data->preview));
#else
						DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 6, MUIM_MsgHandler_PushMessage, data->renderarea, 3, MUIM_Set, MUIA_Render_Data, data->preview->GetLine());
#endif
						DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 6, MUIM_MsgHandler_PushMessage, data->renderarea, 3, MUIM_Set, MUIA_Render_Lines, LINES);
						y = 0;
					}
					else
					{
						working = FALSE;
					}
				}

#ifdef __MIXEDBINARY__
				RenderLines(data->preview, y);
				line = GetLine(data->preview);
#else
				data->preview->RenderLines(y);
				line = data->preview->GetLine();
#endif
				DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 6, MUIM_MsgHandler_PushMessage, data->renderarea, 3, MUIM_Set, MUIA_Render_Data, &line[y*data->width]);
				DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 5, MUIM_MsgHandler_PushMessage, data->renderarea, 2, MUIM_Render_DrawUpdate, y);

				y += LINES;
				if(y >= data->height)
				{
					working = FALSE;
					if(data->caller)
					{
						DOMETHOD(app, MUIM_Application_PushMethod, data->msghandler, 4, MUIM_MsgHandler_PushMessage, data->caller, 1, MUIM_SubTask_Finished);
					}
				}
			}
			else
			{
				/* We have nothing to do, just sit quietly and wait for something to happen */
				if(!done)
				{
					WARPOS_PPC_FUNC(WaitPort)(st->st_Port);
				}
			}
		}
#ifdef __PPC__
		ExitSubTaskPPC(st, stm);
#else
		ExitSubTask(st, stm);
#endif
		if(data->preview)
		{
#ifdef __MIXEDBINARY__
			PreviewDelete(data->preview);
#else
			delete data->preview;
#endif
		}
	}
}

/* the following functions are called from the main task */

#ifdef __PPC__
/*************
 * DESCRIPTION:   create a subtask
 * INPUT:         type     type of subtask
 *                sigbit   signal bit for reply port
 *                name     name of the task
 *                priority
 *                stack
 * OUTPUT:        subtask structure
 *************/
struct SubTask *CreateSubTaskPPC(ULONG type, LONG sigbit, char *name, ULONG priority, ULONG stack)
{
	struct SubTask *st;

	st = (struct SubTask*)AllocVec(sizeof(struct SubTask), MEMF_PUBLIC|MEMF_CLEAR);
	if(st)
	{
		st->st_Reply = CreateMsgPortPPC();
		if(!st->st_Reply)
		{
			FreeVec(st);
			return NULL;
		}
		st->st_Port = CreateMsgPortPPC();
		if(!st->st_Port)
		{
			DeleteMsgPortPPC(st->st_Reply);
			FreeVec(st);
			return NULL;
		}
		st->st_Task = (struct Task *)CreateTaskPPCTags(
			TASKATTR_CODE, PreviewSubTaskPPC,
			TASKATTR_NAME, name,
			//TASKATTR_PRI, priority,
			TASKATTR_R3, st,
			//TASKATTR_STACKSIZE, stack,
			TAG_DONE);
		if(!st->st_Task)
		{
			FreeVec(st);
			st = NULL;
		}
	}
	return st;
}

/*************
 * DESCRIPTION:   send a message to a subtask and wait for reply
 * INPUT:         st       subtask structure
 *                command
 *                params
 * OUTPUT:        result returned from subtask
 *************/
LONG SendSubTaskMsgPPC(struct SubTask *st, WORD command, APTR params)
{
	st->st_Message.stm_Message.mn_ReplyPort = (struct MsgPort*)st->st_Reply;
	st->st_Message.stm_Message.mn_Length    = sizeof(struct SubTaskMsg);
	st->st_Message.stm_Command              = command;
	st->st_Message.stm_Parameter            = params;
	st->st_Message.stm_Result               = 0;

	PutMsgPPC(st->st_Port, (struct Message *)&st->st_Message);
	WaitPortPPC(st->st_Reply);
	GetMsgPPC(st->st_Reply);

	return(st->st_Message.stm_Result);
}

/*************
 * DESCRIPTION:   send a shutdown message to a subtask and free subtask structure
 * INPUT:         st       subtask structure
 * OUTPUT:        -
 *************/
void KillSubTaskPPC(struct SubTask *st)
{
	SendSubTaskMsgPPC(st,STC_SHUTDOWN,st);
	if(st->st_Port)
		DeleteMsgPortPPC(st->st_Port);
	if(st->st_Reply)
		DeleteMsgPortPPC(st->st_Reply);
	FreeVec(st);
}

/* the following functions are called from the sub task */

/*************
 * DESCRIPTION:   save exit a subtask
 * INPUT:         st       subtask structure
 *                stm      SHUTDOWN message received from main task
 * OUTPUT:        -
 *************/
void ExitSubTaskPPC(struct SubTask *st,struct SubTaskMsg *stm)
{
	stm->stm_Result = FALSE;
	ReplyMsgPPC((struct Message *)stm);
}

#else // __PPC__

/*************
 * DESCRIPTION:   create a subtask
 * INPUT:         func     subtask function
 *                sigbit   signal bit for reply port
 *                name     name of the task
 *                priority
 *                stack
 * OUTPUT:        subtask structure
 *************/
struct SubTask *CreateSubTask(void (*func)(), LONG sigbit, char *name, ULONG priority, ULONG stack)
{
	struct SubTask *st;

	st = (struct SubTask*)AllocVec(sizeof(struct SubTask), MEMF_PUBLIC|MEMF_CLEAR);
	if(st)
	{
		st->st_Reply.mp_Node.ln_Name = NULL;
		st->st_Reply.mp_Node.ln_Pri = 0;
		st->st_Reply.mp_Node.ln_Type = NT_MSGPORT;
		st->st_Reply.mp_Flags = PA_SIGNAL;
		st->st_Reply.mp_SigBit = sigbit;
		st->st_Reply.mp_SigTask = (struct Task*)WARPOS_PPC_FUNC(FindTask)(NULL);
		NewList(&st->st_Reply.mp_MsgList);
		st->st_Task = (struct Task *)CreateNewProcTags(
			NP_Name, name,
			NP_Entry, func,
			NP_Priority, priority,
			NP_StackSize, stack,
			TAG_DONE);
		if(!st->st_Task)
		{
			FreeVec(st);
			st = NULL;
		}
		else
		{
			if(!SendSubTaskMsg(st, STC_STARTUP, st))
			{
				FreeVec(st);
				st = NULL;
			}
		}
	}
	return st;
}

/*************
 * DESCRIPTION:   send a message to a subtask and wait for reply
 * INPUT:         st       subtask structure
 *                command
 *                params
 * OUTPUT:        result returned from subtask
 *************/
LONG SendSubTaskMsg(struct SubTask *st, WORD command, APTR params)
{
	st->st_Message.stm_Message.mn_ReplyPort = (struct MsgPort*)&st->st_Reply;
	st->st_Message.stm_Message.mn_Length    = sizeof(struct SubTaskMsg);
	st->st_Message.stm_Command              = command;
	st->st_Message.stm_Parameter            = params;
	st->st_Message.stm_Result               = 0;

	PutMsg(command==STC_STARTUP ? &((struct Process *)st->st_Task)->pr_MsgPort : st->st_Port,(struct Message *)&st->st_Message);
	WaitPort(&st->st_Reply);
	GetMsg(&st->st_Reply);

	return(st->st_Message.stm_Result);
}

/*************
 * DESCRIPTION:   send a shutdown message to a subtask and free subtask structure
 * INPUT:         st       subtask structure
 * OUTPUT:        -
 *************/
void KillSubTask(struct SubTask *st)
{
	SendSubTaskMsg(st,STC_SHUTDOWN,st);
	FreeVec(st);
}

/* the following functions are called from the sub task */

/*************
 * DESCRIPTION:   save exit a subtask
 * INPUT:         st       subtask structure
 *                stm      SHUTDOWN message received from main task
 * OUTPUT:        -
 *************/
void ExitSubTask(struct SubTask *st,struct SubTaskMsg *stm)
{
	if(st->st_Port)
		DeleteMsgPort(st->st_Port);
	Forbid();
	stm->stm_Result = FALSE;
	WARPOS_PPC_FUNC(ReplyMsg)((struct Message *)stm);
}

/*************
 * DESCRIPTION:   initialize a subtask and reply INIT message
 * INPUT:         -
 * OUTPUT:        subtask structure received from main task
 *************/
struct SubTask *InitSubTask()
{
	struct Process *me = (struct Process*)FindTask(NULL);
	struct SubTask *st;
	struct SubTaskMsg *stm;
	BOOL done=FALSE;

	// Wait for our startup message and ignore all other messages

	while(!done)
	{
		WaitPort(&me->pr_MsgPort);
		while((stm = (struct SubTaskMsg *)GetMsg(&me->pr_MsgPort)))
		{
			if(stm->stm_Command == STC_STARTUP)
			{
				done = TRUE;
				break;
			}
		}
	}
	st = (struct SubTask *)stm->stm_Parameter;

	st->st_Port = CreateMsgPort();
	if(st->st_Port)
	{
		/*
		** Reply startup message, everything ok.
		** Note that if the initialization fails, the code falls
		** through and replies the startup message with a stm_Result
		** of 0 after a Forbid(). This tells the calling function that the
		** sub task failed to run.
		*/

		stm->stm_Result = TRUE;
		ReplyMsg((struct Message *)stm);
		return(st);
	}

	ExitSubTask(st,stm);
	return(NULL);
}
#endif // __PPC__
