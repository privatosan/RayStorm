/*************************************************************************************
 *
 * Name:
 * terrain.cpp
 *
 * Purpose:
 *  Generates a landscape in grayscale, and render it with RayStorm
 *  
 * Author:
 *  Andreas Heumann
 *
 * History: 09.04.1995   first release (1.0)
 *
 *************************************************************************************/

#include <intuition/intuition.h>
#include <graphics/gfx.h>

#include <exec/memory.h>
#include <exec/types.h>
#include <rexx/storage.h>
#include <rexx/rxslib.h>

#include <pragma/exec_lib.h>
#include <pragma/rexxsyslib_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/graphics_lib.h>

#include "mcpp:work/octree/rexx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream.h>

#define VecSub(a,b,r) (r)->x=(a).x-(b).x,(r)->y=(a).y-(b).y,(r)->z=(a).z-(b).z
#define VecAdd(a,b,r) (r)->x=(a).x+(b).x,(r)->y=(a).y+(b).y,(r)->z=(a).z+(b).z
#define VecAddScaled(v1,s,v2,r)	(r).x = (v1).x + (s)*(v2).x, \
				 (r).y = (v1).y + (s)*(v2).y, \
				 (r).z = (v1).z + (s)*(v2).z
#define dotp(a, b)	(((a).x*(b).x)+((a).y*(b).y)+((a).z*(b).z))

/* uncomment to get debugging messages */
/*#define DEBUG*/
#define PI 3.1415

// default size
#define size 20
#define sph_size .4
#define sphnum 25

#define D 1
#define m 2
#define tmap .1
#define tsph .1
#define g .1

// ARexx command buffer
char buffer[256];

typedef struct VECTOR
{
	float x,y,z;
};

typedef struct SPHERE
{
	VECTOR pos;			// actual position
	VECTOR v;			// actual velocity
};

typedef struct SPRING
{
	float y,y1;			// actual and target y-position
	float vy;			// actual speed
	VECTOR norm;		// point normal
};

static SPHERE *sph = NULL;
static SPRING *map = NULL;
static WORD array[size][2];

struct Library *RexxSysBase = NULL;
struct Library *GfxBase = NULL;
struct Library *IntuitionBase = NULL;
struct RastPort *rport;

static struct Window *main_win = NULL;
static struct Screen *main_scr = NULL;
static struct MsgPort *HostPort;
static struct MsgPort *ReplyPort = NULL;
static int msgs = 0;		// remaining messages

// Prototypes
void cleanup();

/*******************************************************
 * Compute cross-product of a and b, place result in r
 *******************************************************/
void VecCross(VECTOR *a, VECTOR *b, VECTOR *r)
{
	r->x = (a->y * b->z) - (a->z * b->y);
	r->y = (a->z * b->x) - (a->x * b->z);
	r->z = (a->x * b->y) - (a->y * b->x);
}

/*******************************************************
 * Normalize vector
 *******************************************************/
float Normalize(VECTOR *a)
{
	float d, d1;

	d = sqrt(a->x*a->x + a->y*a->y + a->z*a->z);
	if (d < 1e-4)
		return 0.;
	d1 = d;
	d = 1/d;
	a->x *= d;
	a->y *= d;
	a->z *= d;

	return d1;
}

/*******************************************************
 * initialize spheres
 *******************************************************/
void initsph()
{
	int i;

	for(i=0; i<sphnum; i++)
	{
		sph[i].v.x = 0;
		sph[i].v.y = 0;
		sph[i].v.z = 0;
		sph[i].pos.x = float(rand() & 0x7fff) / 1730;
		sph[i].pos.y = 5;
		sph[i].pos.z = float(rand() & 0x7fff) / 1730;
	}
}

/*******************************************************
 * updates spheres
 *******************************************************/
void updatesph()
{
	int i,mapx,mapz;
	float dx,dz,y,k;
	float speed;
	VECTOR norm, dir,dir1;

	for(i=0; i<sphnum; i++)
	{
		mapx = int(floor(sph[i].pos.x));
		mapz = int(floor(sph[i].pos.z));
		dx = dz = - map[mapz*size+mapx].y;
		y = - dx;
		dx += map[mapz*size+mapx+1].y;
		dz += map[(mapz+1)*size+mapx].y;
		// y-position of map
		y += dx*(sph[i].pos.x - float(mapx)) + dz*(sph[i].pos.z - float(mapz));
		// collision with x-border
		if((sph[i].pos.x < 0) || (sph[i].pos.x > size-1))
			sph[i].v.x = -sph[i].v.x;
		// collision with z-border
		if((sph[i].pos.z < 0) || (sph[i].pos.z > size-1))
			sph[i].v.z = -sph[i].v.z;
		// Update position
		sph[i].pos.x += sph[i].v.x;
		sph[i].pos.y += sph[i].v.y;
		sph[i].pos.z += sph[i].v.z;
		// gravity
		sph[i].v.y -= g*tsph;
		// collision with map
		if(sph[i].pos.y < y+sph_size)
		{
			// speed of map
			map[mapz*size+mapx].vy = .3*sph[i].v.y;
			// map direction
			dir.x = 1;
			dir.y = dx;
			dir.z = 0;
			Normalize(&dir);
			dir1.x = 0;
			dir1.y = dz;
			dir1.z = 1;
			Normalize(&dir1);
			VecCross(&dir,&dir1,&norm);
			Normalize(&norm);
			// sphere direction
			dir.x = sph[i].v.x;
			dir.y = sph[i].v.y;
			dir.z = sph[i].v.z;
			speed = Normalize(&dir) * .7;
			// reflected direction (k = angle between norm and dir)
			k = -dotp(dir, norm);
			VecAddScaled(dir, 2.*k, norm, dir);
			Normalize(&dir);
			// calculate new speed
			sph[i].v.x = dir.x*speed;
			sph[i].v.y = dir.y*speed;
			sph[i].v.z = dir.z*speed;

			sph[i].pos.y = y+sph_size+.001;
		}
	}
}

/*******************************************************
 * updates landscape y-positions
 *******************************************************/
void updatemap()
{
	int i;

	for(i=size*size-1; i>=0; i--)
	{
		map[i].vy *= .95;
		map[i].vy += ((D*(map[i].y1-map[i].y))/m)*tmap;
		map[i].y += map[i].vy;
	}
}

/*******************************************************
 * displays spheres
 *******************************************************/
void displaysph(UBYTE pen)
{
	int i;

	SetAPen(rport,pen);
	for(i=0; i<sphnum; i++)
	{
		WritePixel(rport,SHORT(sph[i].pos.x*16),180-SHORT(sph[i].pos.y*16)-SHORT(sph[i].pos.z/8));
	}
}

/*******************************************************
 * displays landscape
 *******************************************************/
void displaymap()
{
	int x,z=0;

//	for(z=0; z<size; z++)
//	{
		for(x=0; x<size; x++)
		{
			array[x][0] = (x<<4);
			array[x][1] = 180-SHORT(map[x].y*16);
//			array[x][1] = 180-SHORT(map[z*size+x].y*16)-(z<<3);
		}
		Move(rport,array[0][0],array[0][1]);
		PolyDraw(rport, size-1, (WORD*)&array[1][0]);
//	}
/*	for(x=0; x<size; x++)
	{
		for(z=0; z<size; z++)
		{
			array[z][0] = (x<<4);
			array[z][1] = 180-SHORT(map[z*size+x].y)-(z<<3);
		}
		Move(rport,array[0][0],array[0][1]);
		PolyDraw(rport, size-1, (WORD*)&array[1][0]);
	}*/
}

/*******************************************************
 * test if IDCMP_CLOSEWINDOW is here
 *******************************************************/
BOOL handleIDCMP()
{
	struct IntuiMessage *message;
	ULONG imsgclass;
	USHORT imsgcode;
	BOOL done = FALSE;

	while ((!done) && (message = (struct IntuiMessage *)GetMsg(main_win->UserPort)))
	{
		imsgclass = message->Class;
		imsgcode = message->Code;
		ReplyMsg((struct Message *)message);
		
		switch (imsgclass)
		{
			case IDCMP_CLOSEWINDOW:
				done = TRUE;
				break;
			default:
				break;
		}
	}
	return done;
}

/*******************************************************
 * open libs, screen and window
 *******************************************************/
void init()
{
	int i;

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

	// Open intuition library.
	IntuitionBase = OpenLibrary("intuition.library",0L);
	if (!IntuitionBase)
	{
		cout << "Can't open intuition.library\n";
		cleanup();
	}
	// Open gfx library.
	GfxBase = OpenLibrary("graphics.library",0L);
	if (!GfxBase)
	{
		cout << "Can't open gfx.library\n";
		cleanup();
	}
	// Open rexxsyslib library.
	RexxSysBase = OpenLibrary("rexxsyslib.library",0L);
	if (!RexxSysBase)
	{
		cout << "Can't open rexxsyslib.library\n";
		cleanup();
	}

	// open screen
	main_scr = OpenScreenTags(NULL,
		SA_Width,		320,
		SA_Height,		256,
		SA_Depth,		1,
		SA_FullPalette,TRUE,
		SA_SysFont,		1L,
		SA_AutoScroll,	TRUE,
		SA_Pens,			GetScreenDrawInfo(LockPubScreen("Workbench"))->dri_Pens,
		SA_Title,		"Jump",
		TAG_DONE);

	// open window
	main_win = OpenWindowTags(NULL,
		WA_MinWidth,		100,
		WA_MinHeight,		100,
		WA_IDCMP,			IDCMP_CLOSEWINDOW,
		WA_Flags,			WFLG_CLOSEGADGET | WFLG_ACTIVATE | WFLG_DEPTHGADGET,
		WA_CustomScreen,	main_scr,
		WA_Title,			"Jump",
		TAG_END);
	if (!main_win)
	{
		cout << "Can't open window\n";
		cleanup();
	}
	rport = main_win->RPort;

	// initialize seed for random generator
	srand(time(0));

	// get memory for map
	map = (SPRING*)malloc(size*size*sizeof(SPRING));
	if(!map)
	{
		cout << "Can't get enough memory!" << "\n";
		cleanup();
	}
	for(i=size*size-1; i>=0; i--)
		map[i].norm.y = 1;
	// get memory for spheres
	sph = (SPHERE*)malloc(sphnum*sizeof(SPHERE));
	if(!sph)
	{
		cout << "Can't get enough memory!" << "\n";
		cleanup();
	}
}

/*******************************************************
 * close files, free mem and do all this cleanup stuff
 *******************************************************/
void cleanup()
{
	struct IntuiMessage *msg;
	struct Node *succ;
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

	// close mainwindow
	if (main_win)
	{
		Forbid();
		msg = (struct IntuiMessage*)main_win->UserPort->mp_MsgList.lh_Head;
		while((succ = msg->ExecMessage.mn_Node.ln_Succ) != NULL)
		{
			if(msg->IDCMPWindow == main_win)
			{
				Remove((struct Node*)msg);
				ReplyMsg((struct Message*)msg);
			}
			msg = (struct IntuiMessage*)succ;
		}
		ModifyIDCMP(main_win, 0L);
		Permit();
		CloseWindow(main_win);
	}
	// close screen
	if (main_scr)
	{
		CloseScreen(main_scr);
	}
	if(sph)
		free(sph);
	if(map)
		free(map);
	if (IntuitionBase)
		CloseLibrary(IntuitionBase);
	if (GfxBase)
		CloseLibrary(GfxBase);
	if (RexxSysBase)
		CloseLibrary(RexxSysBase);
	exit(0);
}

/*******************************************************
 * SendRexxMsg():
 *
 *	Sends a single command to Rexx host
 *******************************************************/
void SendRexxMsg(struct MsgPort *HostPort, char *SingleMsg)
{
	struct RexxMsg *RexxMessage;

	// free previous messages
	while(RexxMessage = (struct RexxMsg*)GetMsg(ReplyPort))
	{
		// Remove Rexx message
		ClearRexxMsg(RexxMessage,1);
		DeleteRexxMsg(RexxMessage);
		msgs--;
	}

	while(msgs > 200)
	{
		WaitPort(ReplyPort);
		while((RexxMessage = (struct RexxMsg*)GetMsg(ReplyPort)) && msgs > 200)
		{
			// Remove Rexx message
			ClearRexxMsg(RexxMessage,1);
			DeleteRexxMsg(RexxMessage);
			msgs--;
		}
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
		}
	}
}

/*******************************************************
 * generate landscape
 *******************************************************/
void generate(int x, int y, int xsize, int ysize)
{
	static unsigned char bittab[4][8] = {1,1,1,0,0,1,0,0, 1,1,1,1,0,1,0,1, 1,1,1,0,1,1,1,0, 1,1,1,1,1,1,1,1};
	int ptr;

	/* generate terrain */
	if(xsize>2)
	{
		if(!map[(x+xsize/2)+(y*size)].y1)
		{
			map[(x+xsize/2)+(y*size)].y1 =
				(map[x+(y*size)].y1+map[(x+xsize-1)+(y*size)].y1)/2+
				((rand()&0xFFFF)-0x7FFF)/(size/xsize);
		}
		if(!map[(x+xsize/2)+((y+ysize-1)*size)].y1)
		{
			map[(x+xsize/2)+((y+ysize-1)*size)].y1 =
				(map[x+((y+ysize-1)*size)].y1+
				 map[(x+xsize-1)+((y+ysize-1)*size)].y1)/2+
				((rand()&0xFFFF)-0x7FFF)/(size/xsize);
		}
	}
	if(ysize>2)
	{
		if(!map[x+((y+ysize/2)*size)].y1)
		{
			map[x+((y+ysize/2)*size)].y1 =
				(map[x+(y*size)].y1+map[x+((y+ysize-1)*size)].y1)/2+
				((rand()&0xFFFF)-0x7FFF)/(size/ysize);
		}
		if(!map[(x+xsize-1)+((y+ysize/2)*size)].y1)
		{
			map[(x+xsize-1)+((y+ysize/2)*size)].y1 =
				(map[(x+xsize-1)+(y*size)].y1+
				 map[(x+xsize-1)+((y+ysize-1)*size)].y1)/2+
				((rand()&0xFFFF)-0x7FFF)/(size/ysize);
		}
	}
	if(xsize>2 && ysize>2)
	{
		if(!map[(x+xsize/2)+((y+ysize/2)*size)].y1)
		{
			map[(x+xsize/2)+((y+ysize/2)*size)].y1 =
				(map[x+(y*size)].y1+map[(x+xsize-1)+(y*size)].y1+
				 map[(x+xsize-1)+((y+ysize-1)*size)].y1+
				 map[x+((y+ysize-1)*size)].y1)/4+
				((rand()&0xFFFF)-0x7FFF)/(size/xsize);
		}
	}

	/* End of recursion */
	if((xsize<=3) && (ysize<=3))
	{
		return;
	}

	/* start recursion */
	ptr = ((xsize&1)<<1) + (ysize&1);
	generate(x        ,y        ,xsize/2+bittab[ptr][0],ysize/2+bittab[ptr][1]);
	generate(x        ,y+ysize/2,xsize/2+bittab[ptr][2],ysize/2+bittab[ptr][3]);
	generate(x+xsize/2,y        ,xsize/2+bittab[ptr][4],ysize/2+bittab[ptr][5]);
	generate(x+xsize/2,y+ysize/2,xsize/2+bittab[ptr][6],ysize/2+bittab[ptr][7]);
}

/*******************************************************
 * initialize raystorm
 *******************************************************/
void initraystorm()
{
	SendRexxMsg(HostPort, "SETSCREEN 100 100");
	SendRexxMsg(HostPort, "SETWORLD 0 0 0 0 0 0");
	SendRexxMsg(HostPort, "SETCAMERA 10 30 -15 10 1 10 0 1 0 45 45");
}

/*******************************************************
 * initialize frame
 *******************************************************/
void initframe()
{
	SendRexxMsg(HostPort, "POINTLIGHT 10 30 -15 255 255 255");
	SendRexxMsg(HostPort, "NEWSURFACE LAND");
	SendRexxMsg(HostPort, "SPECULAR 255 255 255");
/*	SendRexxMsg(HostPort, "AMBIENT 25 25 64");
	SendRexxMsg(HostPort, "SPECULAR 150 150 255");
	SendRexxMsg(HostPort, "DIFFUSE 100 100 255");*/
	SendRexxMsg(HostPort, "NEWSURFACE RED");
	SendRexxMsg(HostPort, "DIFFUSE 255 0 0");
}

/*******************************************************
 * write landscape
 *******************************************************/
void writelandscape()
{
	int x,z;

	for(x=0; x<size-1; x++)
	{
		for(z=0; z<size-1; z++)
		{
			sprintf(buffer, "TRIANGLE LAND %d %2.2f %d %d %2.2f %d %d %2.2f %d",
				x, map[z*size+x].y, z,
				x+1, map[z*size+x+1].y, z, 
				x, map[(z+1)*size+x].y, z+1);
			SendRexxMsg(HostPort, buffer);
			sprintf(buffer, "TRIANGLE LAND %d %2.2f %d %d %2.2f %d %d %2.2f %d",
				x+1, map[z*size+x+1].y, z,
				x, map[(z+1)*size+x].y, z+1,
				x+1, map[(z+1)*size+x+1].y, z+1);
			SendRexxMsg(HostPort, buffer);
/*			sprintf(buffer, "TRIANGLE LAND %d %2.2f %d %d %2.2f %d %d %2.2f %d %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f",
				x, map[z*size+x].y1, z,
				x+1, map[z*size+x+1].y1, z, 
				x, map[(z+1)*size+x].y1, z+1,
				map[z*size+x].norm.x, map[z*size+x].norm.y, map[z*size+x].norm.z,
				map[z*size+x+1].norm.x, map[z*size+x+1].norm.y, map[z*size+x+1].norm.z,
				map[(z+1)*size+x].norm.x, map[(z+1)*size+x].norm.y, map[(z+1)*size+x].norm.z);
			SendRexxMsg(HostPort, buffer);
			sprintf(buffer, "TRIANGLE LAND %d %2.2f %d %d %2.2f %d %d %2.2f %d %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f",
				x+1, map[z*size+x+1].y1, z,
				x, map[(z+1)*size+x].y1, z+1,
				x+1, map[(z+1)*size+x+1].y1, z+1,
				map[z*size+x+1].norm.x, map[z*size+x+1].norm.y, map[z*size+x+1].norm.z,
				map[(z+1)*size+x].norm.x, map[(z+1)*size+x].norm.y, map[(z+1)*size+x].norm.z,
				map[(z+1)*size+x+1].norm.x, map[(z+1)*size+x+1].norm.y, map[(z+1)*size+x+1].norm.z);
			SendRexxMsg(HostPort, buffer);*/
		}
	}
}

/*******************************************************
 * write spheres
 *******************************************************/
void writespheres()
{
	int i;

	for(i=0; i<sphnum; i++)
	{
		sprintf(buffer, "SPHERE RED %2.2f %2.2f %2.2f %2.2f",
			sph[i].pos.x, sph[i].pos.y, sph[i].pos.z, sph_size);
		SendRexxMsg(HostPort, buffer);
	}
}

/*******************************************************
 * add normal to given normal at position x,y
 *******************************************************/
void addnorm(int pos, VECTOR *n)
{
	if(dotp(map[pos].norm, *n) > 0)
	{
		VecSub(map[pos].norm, *n, &map[pos].norm);
	}
	else
	{
		VecAdd(map[pos].norm, *n, &map[pos].norm);
	}
}

/*******************************************************
 * calculate landscape normals
 *******************************************************/
void calcnorms()
{
	int x,z;
	VECTOR e1,e2,n;

	for(x=0; x<size; x++)
	{
		for(z=0; z<size; z++)
		{
			map[z*size+x].norm.x = 0;
			map[z*size+x].norm.y = 0;
			map[z*size+x].norm.z = 0;
			// calculate normal of triangle
			if((z != size-1) && (x != size-1))
			{
				e1.x = 1;
				e1.y = map[z*size+x].y1 - map[z*size+x+1].y1;
				e1.z = 0;
				e2.x = 0;
				e2.y = map[z*size+x].y1 - map[(z+1)*size+x].y1;
				e2.z = 1;
				VecCross(&e1, &e2, &n);
				addnorm(z*size+x,&n);
			}
			if((z != size-1) && (x != 0))
			{
				e1.x = 0;
				e1.y = map[z*size+x].y1 - map[(z+1)*size+x].y1;
				e1.z = 1;
				e2.x = -1;
				e2.y = map[z*size+x].y1 - map[(z+1)*size+x-1].y1;
				e2.z = 1;
				VecCross(&e1, &e2, &n);
				addnorm(z*size+x,&n);
			}
			if((z != size-1) && (x != 0))
			{
				e1.x = -1;
				e1.y = map[z*size+x].y1 - map[(z+1)*size+x-1].y1;
				e1.z = 1;
				e2.x = -1;
				e2.y = map[z*size+x].y1 - map[z*size+x-1].y1;
				e2.z = 0;
				VecCross(&e1, &e2, &n);
				addnorm(z*size+x,&n);
			}
			if((z != 0) && (x != 0))
			{
				e1.x = -1;
				e1.y = map[z*size+x].y1 - map[z*size+x-1].y1;
				e1.z = 0;
				e2.x = 0;
				e2.y = map[z*size+x].y1 - map[(z-1)*size+x].y1;
				e2.z = -1;
				VecCross(&e1, &e2, &n);
				addnorm(z*size+x,&n);
			}
			if((z != 0) && (x != size-1))
			{
				e1.x = 0;
				e1.y = map[z*size+x].y1 - map[(z-1)*size+x].y1;
				e1.z = -1;
				e2.x = 1;
				e2.y = map[z*size+x].y1 - map[(z-1)*size+x+1].y1;
				e2.z = -1;
				VecCross(&e1, &e2, &n);
				addnorm(z*size+x,&n);
			}
			if((z != 0) && (x != size-1))
			{
				e1.x = 1;
				e1.y = map[z*size+x].y1 - map[(z-1)*size+x+1].y1;
				e1.z = -1;
				e2.x = 1;
				e2.y = map[z*size+x].y1 - map[z*size+x+1].y1;
				e2.z = 0;
				VecCross(&e1, &e2, &n);
				addnorm(z*size+x,&n);
			}
		}
	}
}

/*******************************************************
 * main function
 *******************************************************/
void main(int argc, char *argv[])
{
	float i=0;
	int j,pic=1;
	BOOL done = FALSE;

	init();

	map[(size/2)*(size+1)].y1 = 0xffff;
	map[0].y1 = 0;
	map[(size-1)].y1 = 0;
	map[(size-1)*size].y1 = 0;
	map[(size-1)+(size-1)*size].y1 = 0;
/*	map[0].y1 = rand() & 0xFFFF;
	map[(size-1)].y1 = rand() & 0xFFFF;
	map[(size-1)*size].y1 = rand() & 0xFFFF;
	map[(size-1)+(size-1)*size].y1 = rand() & 0xFFFF;*/

	generate(0,0,size,size);
	calcnorms();
	initsph();

	for(j=size*size-1; j>=0; j--)
	{
		map[j].y = -4;
		map[j].y1 *= 1e-4;
		map[j].y = map[j].y1;
		map[j].vy = 0;
	}

	while(!done)
	{
		displaymap();
		displaysph(1);
		updatemap();
		updatesph();
		cout << "Generating image" << pic << "\n";
		initraystorm();
		initframe();
		writelandscape();
		writespheres();
		SendRexxMsg(HostPort, "STARTRENDER QUICK");
		sprintf(buffer, "SAVEPIC pics/pic.%04d", pic);
		SendRexxMsg(HostPort, buffer);
		SendRexxMsg(HostPort, "CLEANUP");
		pic++;
		SetRast(rport,0);
		done = handleIDCMP();
	}


	cleanup();
}

