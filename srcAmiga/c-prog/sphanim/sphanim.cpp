/***************
 * NAME:          sphanim.cpp
 * VERSION:       1.0 10.08.1995
 * DESCRIPTION:   Little anim of jumping balls
 *						this program has been compiled with Maxon C++ 3.0
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:         
 * HISTORY:       DATE		NAME	COMMENT
 *						10.08.95	ah		first release
 ***************/

#include <exec/memory.h>
#include <rexx/storage.h>
#include <rexx/rxslib.h>
#include <exec/types.h>

#include <pragma/exec_lib.h>
#include <pragma/rexxsyslib_lib.h>

#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <iostream.h>
#define c_Radius		1
#define g				15

struct Library *RexxSysBase = NULL;
static struct MsgPort *HostPort;
static struct MsgPort *ReplyPort = NULL;
static int msgs = 0;		// remaining messages

#define RED		0
#define GREEN	1
#define BLUE	2

typedef struct tSphere
{
	float x,y,z;
   float m;
	float vx,vy,vz;
	char color;
};

struct tSphere *SpList = NULL;

/*************
 * FUNCTION:		SendRexxMsg
 * VERSION:			1.0 08.05.1995
 * DESCRIPTION:	Sends a single command to Rexx host
 * INPUT:			HostPort		pointer to host message port
 *						SingleMsg	message to send
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.05.95	ah			first release
 *************/
void SendRexxMsg(struct MsgPort *HostPort, char *SingleMsg)
{
	struct RexxMsg *RexxMessage;

	while(RexxMessage = (struct RexxMsg*)GetMsg(ReplyPort))
	{
		// Remove Rexx message
		if(RexxMessage->rm_Result1)
		   cout << RexxMessage->rm_Args[0] << "-> Error: " << RexxMessage->rm_Result1 << "\n";
		ClearRexxMsg(RexxMessage,1);
		DeleteRexxMsg(RexxMessage);
		msgs--;
	}

	// Valid pointers given?
	if(HostPort && SingleMsg)
	{
		// Create a Rexx message
		RexxMessage = (struct RexxMsg *)CreateRexxMsg(ReplyPort,"",NULL);
		if(RexxMessage)
		{
			RexxMessage->rm_Args[0] = CreateArgstring(SingleMsg, strlen(SingleMsg));
			RexxMessage->rm_Action = RXFF_RESULT;

			// Send packet
			PutMsg(HostPort,(struct Message *)RexxMessage);
			msgs++;
			WaitPort(ReplyPort);
			RexxMessage = (struct RexxMsg*)GetMsg(ReplyPort);
			// Remove Rexx message
			if(RexxMessage->rm_Result1)
			   cout << RexxMessage->rm_Args[0] << "-> Error: " << RexxMessage->rm_Result1 << "\n";
			ClearRexxMsg(RexxMessage,1);
			DeleteRexxMsg(RexxMessage);
			msgs--;
		}
	}
}

/*************
 * FUNCTION:		InitObjects
 * VERSION:			1.0 08.05.1995
 * DESCRIPTION:	Initalize sphere positions and attributes
 * INPUT:			pics		amount of pics
 *						spheres	amount of spheres
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.05.95	ah			first release
 *************/
void InitObjects(int pics,int spheres)
{
	int i;

	for(i=0; i<spheres; i++)
	{
		SpList[i].x = float(rand())/RAND_MAX*50+35;
		SpList[i].y = float(rand())/RAND_MAX*50+10;
		SpList[i].z = -(float(rand())/RAND_MAX*50+150);
		SpList[i].m = 1;
		SpList[i].vx = 0;
		SpList[i].vy = float(rand())/RAND_MAX*4-2;
		SpList[i].vz = 40/pics;
		if(rand()<RAND_MAX/3)
		{
			SpList[i].color = RED;
		}
		else
		{
			if(rand()<RAND_MAX/2)
			{
				SpList[i].color = GREEN;
			}
			else
			{
				SpList[i].color = BLUE;
			}
		}
	}
}

/*************
 * FUNCTION:		MoveObjects
 * VERSION:			1.0 08.05.1995
 * DESCRIPTION:	Move spheres to new position
 * INPUT:			pics		amount of pics
 *						spheres	amount of spheres
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.05.95	ah			first release
 *************/
void MoveObjects(int pics,int spheres)
{
	int i;
	for(i=0; i<spheres; i++)
	{
		SpList[i].x += SpList[i].vx*(40/pics);
		SpList[i].y += SpList[i].vy*(40/pics);
		SpList[i].z += SpList[i].vz*(40/pics);
		SpList[i].vy -= g*.1;
		if(SpList[i].y <= c_Radius)
		{
			SpList[i].vy = -SpList[i].vy;
		}
	}
}

/*************
 * FUNCTION:		SaveObjects
 * VERSION:			1.0 08.05.1995
 * DESCRIPTION:	Send scene via ARexx to RayStorm 
 * INPUT:			pic		number of picture
 *						spheres	amount of spheres
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.05.95	ah			first release
 *************/
void SaveObjects(int pic,int spheres)
{
	int i;
	char buffer[256];

	sprintf(buffer, "SETCAMERA <90,35,0> <%f,35,%f> <0,1,0> 56.25 45", SpList[0].x, SpList[0].z);
	SendRexxMsg(HostPort, buffer);
	SendRexxMsg(HostPort, "POINTLIGHT <150,400,50> SHADOW");
	SendRexxMsg(HostPort, "NEWSURFACE CHECKER");
	SendRexxMsg(HostPort, "REFLECT [50,50,50]");
	SendRexxMsg(HostPort, "DIFFUSE [50,50,200]");
	SendRexxMsg(HostPort, "AMBIENT [50,50,200]");
	SendRexxMsg(HostPort, "IMTEXTURE /textures/checker.itx <0,1,0> <0,0,0> <10,10,10> 23 23 90");
	SendRexxMsg(HostPort, "PLANE CHECKER");
	SendRexxMsg(HostPort, "NEWSURFACE RED");
	SendRexxMsg(HostPort, "DIFFUSE [255,0,0]");
	SendRexxMsg(HostPort, "AMBIENT [255,0,0]");
	SendRexxMsg(HostPort, "NEWSURFACE GREEN");
	SendRexxMsg(HostPort, "DIFFUSE [0,255,0]");
	SendRexxMsg(HostPort, "AMBIENT [0,255,0]");
	SendRexxMsg(HostPort, "NEWSURFACE BLUE");
	SendRexxMsg(HostPort, "DIFFUSE [0,0,255]");
	SendRexxMsg(HostPort, "AMBIENT [0,0,255]");
	for(i=0; i<spheres; i++)
	{
		switch(SpList[i].color)
		{
			case RED:
				sprintf(buffer, "SPHERE RED <%f,%f,%f> 1", SpList[i].x,  SpList[i].y, SpList[i].z);
				SendRexxMsg(HostPort, buffer);
				break;
			case GREEN:
				sprintf(buffer, "SPHERE GREEN <%f,%f,%f> 1", SpList[i].x,  SpList[i].y, SpList[i].z);
				SendRexxMsg(HostPort, buffer);
				break;
			case BLUE:
				sprintf(buffer, "SPHERE BLUE <%f,%f,%f> 1", SpList[i].x,  SpList[i].y, SpList[i].z);
				SendRexxMsg(HostPort, buffer);
				break;
		}
	}
	SendRexxMsg(HostPort, "STARTRENDER");
	sprintf(buffer, "SAVEPIC pics/balls.%04d", pic);
	SendRexxMsg(HostPort, buffer);
	SendRexxMsg(HostPort, "CLEANUP");
}

/*************
 * FUNCTION:		cleanup
 * VERSION:			1.0 08.05.1995
 * DESCRIPTION:	close libs ...
 * INPUT:			none
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.05.95	ah			first release
 *************/
void cleanup()
{
	struct RexxMsg *RexxMessage;

	if(ReplyPort)
	{
		// free messages
		while(msgs)
		{
			// Wait for message
			WaitPort(ReplyPort);
			// get messages
			while(RexxMessage = (struct RexxMsg*)GetMsg(ReplyPort))
			{
				// Remove Rexx message
				ClearRexxMsg(RexxMessage,1);
				DeleteRexxMsg(RexxMessage);
				msgs--;
			}
		}
		// Free reply port signal bit
		FreeSignal(ReplyPort->mp_SigBit);
		// Free the replyport itself
		FreeMem(ReplyPort,sizeof(struct MsgPort));
	}
	if (RexxSysBase)
		CloseLibrary(RexxSysBase);
	if(SpList)
		delete [ ] SpList;
	exit(0);
}

/*************
 * FUNCTION:		Init
 * VERSION:			1.0 08.05.1995
 * DESCRIPTION:	Open libs, init ports
 * INPUT:			spheres	amount of spheres
 * OUTPUT:			none
 * HISTORY:       DATE		NAME		COMMENT
 *						08.05.95	ah			first release
 *************/
void Init(int spheres)
{
	// is raystrom already active ?
	HostPort = (struct MsgPort *)FindPort("RAYSTORM");
	if(!HostPort)
	{
		cout << "Raystorm is not running please start it\n";
		cleanup();
	}
	// Allocate a reply port
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
		}
	}

	// Open rexxsyslib library.
	RexxSysBase = OpenLibrary("rexxsyslib.library",0L);
	if (!RexxSysBase)
	{
		cout << "Can't open rexxsyslib.library\n";
		cleanup();
	}
	SpList = new tSphere[spheres];
	if(!SpList)
	{
		cout << "Not enough memory\n";
		cleanup();
	}
	srand(time(0));
}

void main(int argc, char *argv[])
{
	int i,pics;
	int spheres = 10;

	// Parse arguments
	if(argc != 2 && argc != 3)
	{
		cout << "Sphereanim V1.0 1995 by Andreas Heumann" << endl;
		cout << "Usage: sphanim picnum [spheres]" << endl;
		cout << "   picnum   - amount of pictures to create" << endl;
		cout << "   spheres  - amount of spheres" << endl;
		cleanup();
	}
	sscanf(argv[1],"%d",&pics);
	if(pics<2)
	{
		cout << "I need at least two pictures to create." << endl;
		cleanup();
	}
	if(argc == 3)
	{
		sscanf(argv[2],"%d",&spheres);
		if(pics<1)
		{
			cout << "I need at least one sphere." << endl;
			cleanup();
		}
	}

	Init(spheres);
	InitObjects(pics,spheres);

	for(i=0; i<pics; i++)
	{
		SendRexxMsg(HostPort, "SETSCREEN 160 128");
		SendRexxMsg(HostPort, "SETWORLD [0,0,0] [0,0,0]");
		MoveObjects(pics,spheres);
		SaveObjects(i+1,spheres);
	}

	cleanup();
} 