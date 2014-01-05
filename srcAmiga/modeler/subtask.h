/***************
 * PROGRAM:       Modeler
 * NAME:          subtask.h
 * DESCRIPTION:   definitions for subtasks
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    01.04.97 ah    initial release
 ***************/

#ifndef SUBTASK_H
#define SUBTASK_H

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef PREVIEW_H
#include "preview.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

enum
{
	STC_STARTUP,
	STC_SHUTDOWN,
	STC_START,
	STC_STOP,
	STC_RESUME,
	STC_CLEANUP
};

enum
{
	SUBTASK_PREVIEW
};

struct SubTaskMsg
{
	struct Message stm_Message;
	WORD           stm_Command;
	APTR           stm_Parameter;
	LONG           stm_Result;
};

struct SubTask
{
	struct Task      *st_Task;    /* sub task pointer */
#ifdef __PPC__
	struct MsgPortPPC *st_Port;   /* allocated by sub task */
	struct MsgPortPPC *st_Reply;   /* reply to main task */
#else
	struct MsgPort   *st_Port;    /* allocated by sub task */
	struct MsgPort    st_Reply;   /* reply to main task */
#endif
	APTR              st_Data;    /* more initial data to pass to the sub task */
	struct SubTaskMsg st_Message; /* Message buffer */
};

struct PreviewData
{
	PREVIEW *preview;
	OBJECT *object;
	Object *renderarea, *caller, *msghandler;
	SURFACE *surf;
	ULONG width,height;
};

struct RenderData
{
	char *err;
	rsiCONTEXT *rc;
	Object *caller, *msghandler;
};

extern "C" void PreviewSubTask();
extern "C" void RenderSubTask();

struct SubTask *CreateSubTask(void (*func)(),LONG, char*, ULONG, ULONG);
LONG SendSubTaskMsg(struct SubTask*, WORD, APTR);
void KillSubTask(struct SubTask*);
void ExitSubTask(struct SubTask*, struct SubTaskMsg*);
struct SubTask *InitSubTask();
#ifdef __MIXEDBINARY__
struct SubTask *CreateSubTaskPPC(ULONG, LONG, char*, ULONG, ULONG);
LONG SendSubTaskMsgPPC(struct SubTask*, WORD, APTR);
void KillSubTaskPPC(struct SubTask*);
void ExitSubTaskPPC(struct SubTask*, struct SubTaskMsg*);
#endif

#endif
