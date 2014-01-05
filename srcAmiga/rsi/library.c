#include "exec/types.h"
#include "exec/resident.h"

typedef struct RayStormBase
{
	UBYTE	sb_Flags
	UBYTE	sb_pad
	ULONG	sb_SysLib
	ULONG	sb_DosLib
	ULONG	sb_SegList
};

/*
 * The first executable location.  This should return an error in case someone tried to
 * run you as a program (instead of loading you as a library).
 */
void start()
{
	return -1;
}

static struct Resident RomTag =
{
	RTC_MATCHWORD,						/* word to match on (ILLEGAL)	*/
	&RomTag,								/* pointer to the above	*/
	&EndCode,							/* address to continue scan	*/
	RTF_AUTOINIT,						/* various tag flags		*/
	VERSION,								/* release version number	*/
	NT_LIBRARY,							/* type of module (NT_XXXXXX)	*/
	0,										/* initialization priority */
	"raystorm.library",				/* pointer to node name	*/
	"RayStorm 1.0 (08.09.96)\n"	/* pointer to identification string */
    APTR  rt_Init;		/* pointer to init code	*/
};

static APTR InitTable[] =
{
	14,			// size of library base data space
	funcTable,	// pointer to function initializers
   DC.L   dataTable         ; pointer to data initializers
   DC.L   initRoutine       ; routine to run
};

int EndCode;

static APTR funcTable[] =
{
	//------ standard system routines
	Open,
	Close,
	Expunge,
	Null,

	//------ my libraries definitions
	_rsiSetCooperateCB,
	_rsiSetCheckCancelCB,
	_rsiSetLogCB,
	_rsiSetUpdateStatusCB,

	_rsiInit,
	_rsiExit,
	_rsiGetErrorMsg,
	_rsiGetUser,
	_rsiCleanup,

	// raytracing controling commands
	_rsiRender,
	_rsiSetOctreeDepth,
	_rsiSetRenderField,
	_rsiSetAntialias,
	_rsiSetDistribution,
	_rsiSetRandomJitter,
	_rsiSetWorld,
	_rsiSetCamera,
	_rsiSetScreen,
	_rsiSetBrushPath,
	_rsiSetTexturePath,
	_rsiSetObjectPath,
	_rsiSavePicture,

	// commands for creating objects
	_rsiCreatePointLight,
	_rsiSetPointLight,
	_rsiCreateSpotLight,
	_rsiSetSpotLight,
	_rsiCreateSphere,
	_rsiSetSphere,
	_rsiCreateTriangle,
	_rsiCreatePlane,
	_rsiSetPlane,
	_rsiLoadObject,

	// SURFACE relating commands
	_rsiCreateSurface,
	_rsiSetSurface,
	_rsiAddTexture,
	_rsiSetTexture,
	_rsiAddBrush,
	_rsiSetBrush,

	// ACTOR relating commands
	_rsiCreateActor,
	_rsiSetActor,
	_rsiPosition,
	_rsiAlignment,
	_rsiSize,

   //------ function table end marker
	-1
};