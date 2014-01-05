#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include <pragma/exec_lib.h>
#include <pragma/powerpc_lib.h>
#include <powerpc/powerpc.h>
#include "rmod:picload.h"
#include "rmod:pichand_lib.h"
class ACTOR;
class SURFACE;
class TRIANGLE;
class SPHERE;
class BOX;
class CYLINDER;
class CONE;
class PLANE;
class IM_TEXTURE;
class TEXTURE_OBJECT;
class BRUSH;
class BRUSH_OBJECT;
class CAMERA;
class LIGHT;
class FLARE;
class STAR;
class OBJECT;
typedef void rsiCONTEXT;
#include <vecmath.h>
#include "rmod:objhand_lib.h"

struct Library *PicHandBase;
struct Library *ObjHandBase;
extern struct Library *SysBase;
extern struct Library *PowerPCBase;

enum
{
	rsiTDone,
	rsiTMore,
	rsiT1,
	rsiT2,
	rsiT3,
	rsiT4,
	rsiT5
};

void vararg(va_list *list)
{
	BOOL done = FALSE;
	ULONG type;

	while(!done)
	{
		printf("%d\n", *list);
		type = *((ULONG*)(*list));
		if(type == rsiTDone)
		{
			done = TRUE;
			continue;
		}
		va_arg(*list,int);
		if(type == rsiTMore)
		{
			*list = *((va_list*)(*list));
			printf("%d\n", *list);
			continue;
		}
		printf("Tag: %d", type);
		type = *((ULONG*)(*list));
		va_arg(*list,int);
		printf(" Value: %d\n", type);
	}
	va_end(*list);
}

void varargfunc(int start, ...)
{
	va_list list;

	va_start(list, start);
	vararg(&list);
}

void varargtest()
{
	ULONG arg[8];

	arg[0] = rsiT2;
	arg[1] = 5;
	arg[2] = rsiT3;
	arg[3] = 6;
	arg[4] = rsiT4;
	arg[5] = 7;
	arg[6] = rsiTDone;
	arg[7] = 8;

	varargfunc(0, rsiT1, 2, rsiTMore, arg, rsiTDone);
}

void testpichand()
{
	char *errstr;
	PICTURE image;

	PicHandBase = OpenLibrary("mod:modules/picture/png", 2l);
	if(!PicHandBase)
	{
		printf("failed");
		return;
	}

	image.name = "mod:brushes/earth.png";
	image.caller = NULL;
	image.param = NULL;
	image.malloc = malloc;
	image.Progress = NULL;
	errstr = picRead(&image);
	if(errstr)
	{
		printf("failed: %s\n", errstr);
	}
	CloseLibrary(PicHandBase);

	printf("done it");
}

void testobjhand()
{
	ObjHandBase = OpenLibrary("rmod:tddd/stormppc/tddd", 2l);
	if(!ObjHandBase)
	{
		printf("failed");
		return;
	}

	if(!objInit())
	{
		printf("Init failed\n");
	}
	CloseLibrary(ObjHandBase);

	printf("done it");
}

typedef struct
{
	UWORD pad;
	float value;
} WORDALIGNED;

typedef struct
{
	ULONG pad;
	float value;
} LONGALIGNED;

void accesstest()
{
	clock_t start;
	WORDALIGNED wa;
	LONGALIGNED la;
	int i;
	float f;

	start = clock();
	for(i=0; i<5000000; i++)
	{
		la.value = 123.456;
		f = la.value;
	}
	printf("Longaligned: %d\n", clock()-start);

	start = clock();
	for(i=0; i<5000000; i++)
	{
		wa.value = 123.456;
		f = wa.value;
	}
	printf("Wordaligned: %d\n", clock()-start);

	start = clock();
	for(i=0; i<5000000; i++)
	{
		la.value = 123.456;
		f = la.value;
	}
	printf("Longaligned: %d\n", clock()-start);
}

class TEST
{
	public:
		float d;
		ULONG flags;
		VECTOR p[3], e[3];
		VECTOR vnorm[3];

		void scalebug();
};

void TEST::scalebug()
{
	VECTOR ptmp, anorm, norm;
	float k;

	anorm.x = Abs(ptmp.x);
	anorm.y = Abs(ptmp.y);
	anorm.z = Abs(ptmp.z);

	VecScale(k, &e[0], &e[0]);
	VecScale(k, &e[1], &e[1]);
	VecScale(k, &e[2], &e[2]);
}

void main()
{
	/*struct PPCArgs call;
	char name[256];
	ULONG sernum;*/

#ifdef MURX
	long args[3];

	args[0] = 13;
	args[1] = 2345;
	args[2] = 12345;
	args[3] = 123456;
	args[4] = 1234567;
	SPrintF ("%ld %ld %ld %ld %ld\n", args);
#endif
//   accesstest();
//   varargtest();
//   testobjhand();

#ifdef MURX
	KeyfileBase = OpenLibrary("s:RayStorm.key", 0l);
	if(!KeyfileBase)
		exit(1);

	PowerPCBase = OpenLibrary("powerpc.library", 0l);
	if(!PowerPCBase)
		exit(1);

//#pragma amicall(SysBase, 0x228, OpenLibrary(a1,d0))
	call.PP_Code = SysBase;
	call.PP_Offset = -0x228;
	call.PP_Regs[PPREG_A1] = (ULONG)"s:RayStorm.key";
	call.PP_Regs[PPREG_D0] = 0;
	call.PP_Stack = NULL;
	call.PP_StackSize = 0;
	Run68K(&call);
	printf("%d\n", call.PP_Regs[PPREG_D0]);

	// keyGetInfo
/*   call.PP_Code = KeyfileBase;
	call.PP_Offset = -0x1e;
	args[0] = (long) name;
	args[1] = (long) &sernum;
	call.PP_Stack = args;
	call.PP_StackSize = sizeof(long)*2;
	Run68K(&call);
	printf("%s %d\n", name, sernum);

	// keyDecodeKeyfile1
	call.PP_Code = KeyfileBase;
	call.PP_Offset = -0x24;
	call.PP_Stack = NULL;
	call.PP_StackSize = 0;
	Run68K(&call);
	printf("%d\n", call.PP_Regs[0]);

	// keyDecodeKeyfile2
	call.PP_Code = KeyfileBase;
	call.PP_Offset = -0x2a;
	call.PP_Stack = NULL;
	call.PP_StackSize = 0;
	Run68K(&call);
	printf("%d\n", call.PP_Regs[0]);*/

	//BOOL keyWritePicture(char*, PICTURE*, char *) = -0x30;
#endif

}
