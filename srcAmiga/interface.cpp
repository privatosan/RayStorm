/***************
 * MODUL:         RayStorm
 * NAME:          interface.cpp
 * DESCRIPTION:   Interface functions to raystorm.library
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * BUGS:          none
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    05.02.98 ah    initial release
 *    18.08.98 ah    added directional light
 ***************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <clib/exec_protos.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef RSI_H
#include "rsi.h"
#endif

#ifndef RSI_LIB_H
#include "rsi_lib.h"
#endif

#ifndef ITEMLIST_H
#include "itemlist.h"
#endif

#ifndef CSGSTACK_H
#include "csgstack.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef RAYSTORM_H
#include "raystorm.h"
#endif

#ifndef INTERFACE_H
#include "interface.h"
#endif

#ifndef PI
#define PI 3.141592654
#endif
#define KEYWORD(a,b) (!stricmp((char*)(a), b))

struct Library *RayStormBase = NULL;

// Application errors
char *app_errors[] =
{
	"Wrong screen resolution",
	"Not enough memory",
	"Limitations of demo version reached",
	"Unknown brush mapping type",
	"Unknown brush wrapping method",
	"Invalid time intervall",
	"Unknown interpolation method",
	"No picture renderd",
	"Can't open screen",
#ifdef __PPC__
	"Can't open raystormppc.library V2 or higher",
#else
	"Can't open raystorm.library V2 or higher",
#endif
	"Can't open intuition.library",
	"Can't open window",
	"Can't open muimaster.library V10 or higher",
	"Invalid vector definition",
	"Invalid color definition",
	"Invalid region definition",
	"No light defined",
	"Unknown flare type",
	"Unknown flare function",
	"Only one object for a CSG object has been defined",
	"Unknown CSG operator",
	"Unknown hyper texture type",
	"Invalid 2d-vector definition",
#ifdef __PPC__
	"Failed to initialize raystormppc.library"
#else
	"Failed to initialize raystorm.library"
#endif
};

// string buffer
char buffer[256];

static ITEMLIST *surf = NULL;
static ITEMLIST *actor = NULL;
static void *light = NULL;
rsiCONTEXT *rc = NULL;

// amount of objects needed for CSG
static int insideCSG = 0;

#ifdef __PPC__
#ifdef LIB_ARG
#define CTXT RayStormBase, rc
#else
#define CTXT rc
#endif
#define CTXT rc
#define SAVEDS __saveds
#else
#define CTXT rc
#define SAVEDS
#endif

/*************
 * DESCRIPTION:   write to log
 * INPUT:         text     text to print to log
 * OUTPUT:        none
 *************/
static void SAVEDS WriteLogInterface(rsiCONTEXT *rc, char *text)
{
	WriteLog(rc, text);
}

/*************
 * DESCRIPTION:   update rendering status
 * INPUT:         percent     percent finished
 *                elapsed     elapsed seconds since start of rendering
 *                y           current line
 *                lines       amount of lines since last call
 *                line        pointer to last renderd line
 * OUTPUT:        none
 *************/
static void SAVEDS UpdateStatusInterface(rsiCONTEXT *rc, float percent, float elapsed, int y, int lines, rsiSMALL_COLOR *line)
{
	UpdateStatus(rc, percent, elapsed, y, lines, line);
}

/*************
 * DESCRIPTION:   check if cancel button is pressed
 * INPUT:         none
 * OUTPUT:        TRUE if pressed else FALSE
 *************/
static BOOL SAVEDS CheckCancelInterface(rsiCONTEXT *rc)
{
	return CheckCancel(rc);
}

/*************
 * DESCRIPTION:   update window and input methods
 * INPUT:         none
 * OUTPUT:        none
 *************/
static void SAVEDS CooperateInterface(rsiCONTEXT *rc)
{
	Cooperate(rc);
}

/*************
 * DESCRIPTION:   InitInterface
 *                open library, create context, set callbacks
 * INPUT:         none
 * OUTPUT:        error string
 *************/
char *InitInterface()
{
	rsiResult err;

#ifdef __AMIGA__
	RayStormBase =  OpenLibrary(RAYSTORMNAME,2L);
	if (!RayStormBase)
		return app_errors[ERROR_RAYSTORMLIB-100];
#endif // __AMIGA__

#if defined(__PPC__) && defined(LIB_ARG)
	err = rsiInit(RayStormBase);
#else
	err = rsiInit();
#endif
	if(err)
		return app_errors[ERROR_INITIALIZE-100];

#if defined(__PPC__) && defined(LIB_ARG)
	err = rsiCreateContext(RayStormBase, &rc);
#else
	err = rsiCreateContext(&rc);
#endif
	if(err)
		return app_errors[ERROR_INITIALIZE-100];

	err = rsiSetLogCB(CTXT, WriteLogInterface);
	if(err)
	{
		rsiGetErrorMsg(CTXT, buffer,err);
		return buffer;
	}
	err = rsiSetUpdateStatusCB(CTXT, UpdateStatusInterface);
	if(err)
	{
		rsiGetErrorMsg(CTXT, buffer,err);
		return buffer;
	}
	err = rsiSetCheckCancelCB(CTXT, CheckCancelInterface);
	if(err)
	{
		rsiGetErrorMsg(CTXT, buffer,err);
		return buffer;
	}
	err = rsiSetCooperateCB(CTXT, CooperateInterface);
	if(err)
	{
		rsiGetErrorMsg(CTXT, buffer,err);
		return buffer;
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Cleanup
 *                close libs, reset callbacks
 * INPUT:         none
 * OUTPUT:        none
 *************/
void CleanupInterface()
{
#ifdef __AMIGA__
	if (RayStormBase)
#endif // __AMIGA__
	{
		if(rc)
		{
			rsiSetLogCB(CTXT, NULL);
			rsiSetUpdateStatusCB(CTXT, NULL);
			rsiSetCheckCancelCB(CTXT, NULL);
			rsiSetCooperateCB(CTXT, NULL);
			rsiFreeContext(CTXT);
		}
#if defined(__PPC__) && defined(LIB_ARG)
		rsiExit(RayStormBase);
#else
		rsiExit();
#endif
#ifdef __AMIGA__
		CloseLibrary(RayStormBase);
#endif // __AMIGA__
	}
}

/*************
 * DESCRIPTION:   read vector from string
 * INPUT:         v     pointer to vector
 *                s     vector string; format '<x,y,z>'
 * OUTPUT:        TRUE if ok else FALSE
 *************/
static BOOL read_vector(rsiVECTOR *v, char *s)
{
	if(s[0] != '<')
		return FALSE;
	if(s[strlen(s)-1] != '>')
		return FALSE;
	v->x = float(atof(strtok(&s[1],",")));
	v->y = float(atof(strtok(NULL,",")));
	v->z = float(atof(strtok(NULL,">")));

	return TRUE;
}

/*************
 * DESCRIPTION:   read 2d-vector from string
 * INPUT:         v     pointer to vector
 *                s     vector string; format '<x,y>'
 * OUTPUT:        TRUE if ok else FALSE
 *************/
static BOOL read_vector2d(rsiVECTOR2D *v, char *s)
{
	if(s[0] != '<')
		return FALSE;
	if(s[strlen(s)-1] != '>')
		return FALSE;
	v->x = float(atof(strtok(&s[1],",")));
	v->y = float(atof(strtok(NULL,">")));

	return TRUE;
}

/*************
 * DESCRIPTION:   read color from string
 * INPUT:         c     pointer to color
 *                s     color string; format '[r,g,b]'
 * OUTPUT:        TRUE if ok else FALSE
 *************/
static BOOL read_color(rsiCOLOR *c, char *s)
{
	if(s[0] != '[')
		return FALSE;
	if(s[strlen(s)-1] != ']')
		return FALSE;
	c->r = float(atol(strtok(&s[1],",")))/255;
	c->g = float(atol(strtok(NULL,",")))/255;
	c->b = float(atol(strtok(NULL,"]")))/255;

	return TRUE;
}

ULONG triangle(ULONG *arg)
{
	void *surf,*actor=NULL;
	int i;
	rsiVECTOR v[3];
	rsiVECTOR n[3];

	surf = FindSurfListItem((char*)arg[0]);
	if(!surf)
		return rsiERR_SURFACE;

	for(i=0; i<3; i++)
	{
		if(!read_vector(&v[i],(char*)arg[i+1]))
			return ERROR_VECTOR;
	}

	if(arg[7])
	{
		actor = FindActorListItem((char*)arg[7]);
		if(!actor)
			return rsiERR_ACTOR;
	}

	if(arg[4])
	{
		for(i=0; i<3; i++)
		{
			if(arg[i+4])
			{
				if(!read_vector(&n[i],(char*)arg[i+4]))
					return ERROR_VECTOR;
			}
		}
		return rsiCreateTriangle(CTXT, surf, &v[0], &v[1], &v[2],
			rsiTTriangleActor, actor,
			rsiTTriangleNorm1, &n[0],
			rsiTTriangleNorm2, &n[1],
			rsiTTriangleNorm3, &n[2], rsiTDone);
	}
	else
		return rsiCreateTriangle(CTXT, surf, &v[0], &v[1], &v[2],
			rsiTTriangleActor, actor, rsiTDone);
}

ULONG newsurface(ULONG *arg)
{
	int flags = 0;
	rsiResult err;
	void *surface;

	if(arg[1])
		flags |= rsiFSurfBright;

	err = rsiCreateSurface(CTXT, &surface, rsiTSurfFlags, flags, rsiTDone);
	if(err)
		return err;

	surf = AddSurfList((char*)arg[0],surface);
	if(!surf)
		return ERROR_MEM;

	return rsiERR_NONE;
}

ULONG setcamera(ULONG *arg)
{
	rsiVECTOR pos,vec,vup;
	void *actor;
	float fnum;
	rsiResult err;

	if(!read_vector(&pos,(char*)arg[0]))
		return ERROR_VECTOR;

	err = rsiSetCamera(CTXT,
		rsiTCameraPos, &pos,
		rsiTDone);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
	}

	if(arg[2])
	{
		if(!read_vector(&vup,(char*)arg[2]))
			return ERROR_VECTOR;
	}

	if(arg[1] && arg[2])
	{
		err = rsiSetCamera(CTXT,
			rsiTCameraLook,&vec,
			rsiTCameraViewUp,&vup,
			rsiTDone);
		if(err)
			return err;
	}
	if(arg[1] && !arg[2])
	{
		err = rsiSetCamera(CTXT, rsiTCameraLook,&vec,rsiTDone);
		if(err)
			return err;
	}
	if(!arg[1] && arg[2])
	{
		err = rsiSetCamera(CTXT, rsiTCameraViewUp,&vup,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		fnum = (float)atof((char*)arg[3]);
		err = rsiSetCamera(CTXT, rsiTCameraHFov,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		fnum = (float)atof((char*)arg[4]);
		err = rsiSetCamera(CTXT, rsiTCameraVFov,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
	{
		fnum = (float)atof((char*)arg[5]);
		err = rsiSetCamera(CTXT, rsiTCameraFocalDist,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[6])
	{
		fnum = (float)atof((char*)arg[6]);
		err = rsiSetCamera(CTXT, rsiTCameraAperture,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[7])
	{
		actor = FindActorListItem((char*)arg[7]);
		if(!actor)
			return rsiERR_ACTOR;
		err = rsiSetCamera(CTXT, rsiTCameraPosActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[8])
	{
		actor = FindActorListItem((char*)arg[8]);
		if(!actor)
			return rsiERR_ACTOR;
		err = rsiSetCamera(CTXT, rsiTCameraLookActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[9])
	{
		err = rsiSetCamera(CTXT, rsiTCameraFlags, rsiFCameraFastDOF, rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG setscreen(ULONG *arg)
{
#ifdef QUANT
	colors = *(LONG*)arg[2];
#endif

	return rsiSetScreen(CTXT, (int)*(LONG*)arg[0],(int)*(LONG*)arg[1]);
}

ULONG pointlight(ULONG *arg)
{
	rsiVECTOR vec;
	rsiCOLOR color;
	float fnum;
	void *actor;
	rsiResult err;
	ULONG flags = 0;

	err = rsiCreatePointLight(CTXT, &light);
	if(err)
		return err;

	if(!read_vector(&vec,(char*)arg[0]))
		return ERROR_VECTOR;
	err = rsiSetPointLight(CTXT, light,rsiTLightPos,&vec,rsiTDone);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_color(&color,(char*)arg[1]))
			return ERROR_COLOR;
		err = rsiSetPointLight(CTXT, light,rsiTLightColor,&color,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		fnum = (float)atof((char*)arg[2]);
		err = rsiSetPointLight(CTXT, light,rsiTLightRadius,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
		flags |= rsiFLightShadow;

	if(arg[4])
	{
		actor = FindActorListItem((char*)arg[4]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetPointLight(CTXT, light,rsiTLightActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
	{
		fnum = (float)atof((char*)arg[5]);
		err = rsiSetPointLight(CTXT, light,rsiTLightFallOff,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(!arg[6])
		flags |= rsiFLightStar;

	if(!arg[7])
		flags |= rsiFLightFlares;

	if(arg[8])
		flags |= rsiFLightShadowMap;

	err = rsiSetPointLight(CTXT, light,rsiTLightFlags,flags,rsiTDone);
	if(err)
		return err;

	if(arg[9])
	{
		err = rsiSetPointLight(CTXT, light,rsiTLightShadowMapSize, (int)*(LONG*)arg[9], rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG spotlight(ULONG *arg)
{
	rsiVECTOR vec;
	rsiCOLOR color;
	float fnum;
	void *actor;
	rsiResult err;
	ULONG flags = 0;

	err = rsiCreateSpotLight(CTXT, &light);
	if(err)
		return err;

	if(!read_vector(&vec,(char*)arg[0]))
		return ERROR_VECTOR;
	err = rsiSetSpotLight(CTXT, light,rsiTLightPos,&vec,rsiTDone);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_color(&color,(char*)arg[1]))
			return ERROR_COLOR;
		err = rsiSetSpotLight(CTXT, light,rsiTLightColor,&color,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetSpotLight(CTXT, light,rsiTLightLookPoint,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		fnum = (float)atof((char*)arg[3]);
		err = rsiSetSpotLight(CTXT, light,rsiTLightAngle,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		fnum = (float)atof((char*)arg[4]);
		err = rsiSetSpotLight(CTXT, light,rsiTLightRadius,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
		flags |= rsiFLightShadow;

	if(arg[6])
	{
		actor = FindActorListItem((char*)arg[6]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetSpotLight(CTXT, light,rsiTLightActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[7])
	{
		actor = FindActorListItem((char*)arg[7]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetSpotLight(CTXT, light,rsiTLightLookPActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[8])
	{
		fnum = (float)atof((char*)arg[8]);
		err = rsiSetSpotLight(CTXT, light,rsiTLightFallOff,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(!arg[9])
		flags |= rsiFLightStar;

	if(!arg[10])
		flags |= rsiFLightFlares;

	if(arg[11])
		flags |= rsiFLightShadowMap;

	err = rsiSetSpotLight(CTXT, light,rsiTLightFlags,flags,rsiTDone);
	if(err)
		return err;

	if(arg[12])
	{
		err = rsiSetSpotLight(CTXT, light,rsiTLightShadowMapSize, (int)*(LONG*)arg[12], rsiTDone);
		if(err)
			return err;
	}

	if(arg[13])
	{
		fnum = (float)atof((char*)arg[13]);
		err = rsiSetSpotLight(CTXT, light,rsiTLightFallOffRadius,fnum,rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG directionallight(ULONG *arg)
{
	rsiVECTOR vec;
	rsiCOLOR color;
	float fnum;
	void *actor;
	rsiResult err;
	ULONG flags = 0;

	err = rsiCreateDirectionalLight(CTXT, &light);
	if(err)
		return err;

	if(!read_vector(&vec,(char*)arg[0]))
		return ERROR_VECTOR;
	err = rsiSetDirectionalLight(CTXT, light,rsiTLightPos,&vec,rsiTDone);
	if(err)
		return err;

	if(!read_vector(&vec,(char*)arg[1]))
		return ERROR_VECTOR;
	err = rsiSetDirectionalLight(CTXT, light,rsiTLightDir,&vec,rsiTDone);
	if(err)
		return err;

	if(arg[2])
	{
		if(!read_color(&color,(char*)arg[2]))
			return ERROR_COLOR;
		err = rsiSetDirectionalLight(CTXT, light,rsiTLightColor,&color,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		fnum = (float)atof((char*)arg[3]);
		err = rsiSetDirectionalLight(CTXT, light,rsiTLightRadius,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
		flags |= rsiFLightShadow;

	if(arg[5])
	{
		actor = FindActorListItem((char*)arg[5]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetDirectionalLight(CTXT, light,rsiTLightActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[6])
	{
		fnum = (float)atof((char*)arg[6]);
		err = rsiSetDirectionalLight(CTXT, light,rsiTLightFallOff,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(!arg[7])
		flags |= rsiFLightStar;

	if(!arg[8])
		flags |= rsiFLightFlares;

	if(arg[9])
		flags |= rsiFLightShadowMap;

	err = rsiSetDirectionalLight(CTXT, light,rsiTLightFlags,flags,rsiTDone);
	if(err)
		return err;

	if(arg[10])
	{
		err = rsiSetDirectionalLight(CTXT, light,rsiTLightShadowMapSize, (int)*(LONG*)arg[10], rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG flare(ULONG *arg)
{
	rsiResult err;
	rsiCOLOR color;
	void *flare;
	float f;
	int flags;

	if (!light)
		return ERROR_NOLIGHT;

	err = rsiCreateFlare(CTXT, light, &flare);
	if (err)
		return err;

	if (arg[0])
	{
		f = (float)atof((char*)arg[0])/100;
		err = rsiSetFlare(CTXT, flare, rsiTFlarePos, f, rsiTDone);
		if (err)
			return err;
	}
	if (arg[1])
	{
		f = (float)atof((char*)arg[1])/100;
		err = rsiSetFlare(CTXT, flare, rsiTFlareRadius, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		if (!read_color(&color, (char*)arg[2]))
		return ERROR_COLOR;

		err = rsiSetFlare(CTXT, flare, rsiTFlareColor, &color, rsiTDone);
		if (err)
			return err;
	}
	flags = 0;

	if (arg[3])
	{
		if (KEYWORD(arg[3], "DISC"))
			flags |= rsiFFlareDisc;
		else
			if (KEYWORD(arg[3], "POLYGON"))
				flags |= rsiFFlarePoly;
			else
				return ERROR_FLARETYPE;
	}
	err = rsiSetFlare(CTXT, flare, rsiTFlareType, flags, rsiTDone);
	if (err)
		return err;

	flags = 0;

	if (arg[4])
	{
		if (KEYWORD(arg[4], "PLAIN"))
			flags |= rsiFFlarePlain;
		else
			if (KEYWORD(arg[4], "RING"))
				flags |= rsiFFlareExp;
			else
				if (KEYWORD(arg[4], "RADIAL"))
					flags |= rsiFFlareRadial;
				else
					if (KEYWORD(arg[4], "EXP1"))
						flags |= rsiFFlareExp1;
					else
						if (KEYWORD(arg[4], "EXP2"))
							flags |= rsiFFlareExp2;
						else
							return ERROR_FLAREFUNC;
	}
	err = rsiSetFlare(CTXT, flare, rsiTFlareFunc, flags, rsiTDone);
	if (err)
		return err;


	if (arg[5])
	{
		err = rsiSetFlare(CTXT, flare, rsiTFlareEdges, (int)*(LONG*)arg[5], rsiTDone);
		if (err)
			return err;
	}

	if (arg[6])
	{

		f = (float)atof((char*)arg[6])*PI/180;
		err = rsiSetFlare(CTXT, flare, rsiTFlareTilt, f, rsiTDone);
		if (err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG star(ULONG *arg)
{
	rsiResult err;
	void *star;
	float f, range, diff;
	float *intensities;
	int i,j,spikes, randseed, step, h;
	rsiCOLOR color;
	BOOL update;

	update = FALSE;

	if (!light)
		return ERROR_NOLIGHT;

	err = rsiCreateStar(CTXT, light, &star);
	if (err)
		return err;

	if (arg[0])
	{
		spikes = (int)*(LONG*)arg[0];
		update = TRUE;
	}
	else
		spikes = 4;

	err = rsiSetStar(CTXT, star, rsiTStarSpikes, spikes, rsiTDone);
	if (err)
		return err;

	if (arg[1])
	{
		f = (float)atof((char*)arg[1])/100;
		err = rsiSetStar(CTXT, star, rsiTStarRadius, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		f = (float)atof((char*)arg[2])/100;
		err = rsiSetStar(CTXT, star, rsiTStarHaloRadius, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[3])
	{
		f = (float)atof((char*)arg[3])/100;
		err = rsiSetStar(CTXT, star, rsiTStarInnerHaloRadius, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[4])
	{
		range = (float)atof((char*)arg[4])/100;
		update = TRUE;
	}
	else
		range = 0;

	if (arg[5])
	{
		randseed = *(ULONG*)arg[5];
		update = TRUE;
	}
	else
		randseed = 0;

	if (arg[6])
	{
		f = (float)atof((char*)arg[7]);
		err = rsiSetStar(CTXT, star, rsiTStarTilt, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[7])
	{
		if (!read_color(&color, (char*)arg[7]))
			return ERROR_COLOR;

		err = rsiSetStar(CTXT, star, rsiTStarColor, &color, rsiTDone);
		if (err)
			return err;
	}

	if (arg[8])
	{
		if (!read_color(&color, (char*)arg[8]))
			return ERROR_COLOR;

		err = rsiSetStar(CTXT, star, rsiTStarRingColor, &color, rsiTDone);
		if (err)
			return err;
	}

	if (arg[9])
	{
		f = (float)atof((char*)arg[9]);
		err = rsiSetStar(CTXT, star, rsiTStarNoise, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[10])
	{
		f = (float)atof((char*)arg[100]);
		err = rsiSetStar(CTXT, star, rsiTStarBrightness, f, rsiTDone);
		if (err)
			return err;
	}

	if (update)
	{
		intensities = new float[spikes];
		if (!intensities)
			return ERROR_MEM;

		// calculate no. of sub-spike lengths
		// i.e. a star with 8 spikes has two lengths. One for the four main directions
		// and the other length for the spikes between them
		h = int(log(float(spikes))/log(2.));
		if (h > 2)
			diff = range/(h - 2);
		else
		{
			h++;
			diff = range;
		}

		// generate the spikes
		step = 1;
		for (j = 1; j < h; j++)
		{
			for (i = 0; i < spikes; i += step)
			{
				if (j == 1)
					intensities[i] = 1. - range;
				else
					intensities[i] += diff;
			}
			step += step;
		}

		// apply random length
		if (randseed > 0)
		{
			srand(randseed);
			for (i = 0; i < spikes; i++)
			{
				intensities[i]+= (float(rand())/RAND_MAX) * range - range;
				if (intensities[i] < 0.)
					intensities[i] = 0.;
					if (intensities[i] > 1.)
					intensities[i] = 1.;
			}
		}
		err = rsiSetStar(CTXT, star, rsiTStarIntensities, intensities, rsiTDone);
		delete intensities;
		if (err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG sphere(ULONG *arg)
{
	rsiVECTOR vec;
	float fnum;
	void *sphere,*surf,*actor;
	rsiResult err;

	surf = FindSurfListItem((char*)arg[0]);
	if(!surf)
		return rsiERR_SURFACE;

	err = rsiCreateSphere(CTXT, &sphere,surf);
	if(err)
		return err;

	if(!read_vector(&vec,(char*)arg[1]))
		return ERROR_VECTOR;
	err = rsiSetSphere(CTXT, sphere,rsiTSpherePos,&vec,rsiTDone);
	if(err)
		return err;

	fnum = (float)atof((char*)arg[2]);
	err = rsiSetSphere(CTXT, sphere,rsiTSphereRadius,fnum,rsiTDone);
	if(err)
		return err;

	if(arg[3])
	{
		actor = FindActorListItem((char*)arg[3]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetSphere(CTXT, sphere,rsiTSphereActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		fnum = (float)atof((char*)arg[4]);
		err = rsiSetSphere(CTXT, sphere,rsiTSphereFuzzy,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
	{
		err = rsiSetSphere(CTXT, sphere,rsiTSphereFlags,rsiFSphereInverted,rsiTDone);
		if(err)
			return err;
	}

	if(insideCSG > 0)
	{
		SetCSGObject(sphere);
		insideCSG--;
	}

	return rsiERR_NONE;
}

ULONG plane(ULONG *arg)
{
	rsiVECTOR vec;
	void *plane,*surf,*actor;
	rsiResult err;

	surf = FindSurfListItem((char*)arg[0]);
	if(!surf)
		return rsiERR_SURFACE;

	err = rsiCreatePlane(CTXT, &plane,surf);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetPlane(CTXT, plane,rsiTPlanePos,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetPlane(CTXT, plane,rsiTPlaneNorm,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		actor = FindActorListItem((char*)arg[3]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetPlane(CTXT, plane,rsiTPlaneActor,actor,rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG box(ULONG *arg)
{
	rsiVECTOR vec;
	void *box,*surf,*actor;
	rsiResult err;

	surf = FindSurfListItem((char*)arg[0]);
	if(!surf)
		return rsiERR_SURFACE;

	err = rsiCreateBox(CTXT, &box,surf);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetBox(CTXT, box,rsiTBoxPos,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetBox(CTXT, box,rsiTBoxLowBounds,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		if(!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetBox(CTXT, box,rsiTBoxHighBounds,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		if(!read_vector(&vec,(char*)arg[4]))
			return ERROR_VECTOR;
		err = rsiSetBox(CTXT, box,rsiTBoxAlign,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
	{
		actor = FindActorListItem((char*)arg[5]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetBox(CTXT, box,rsiTBoxActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[6])
	{
		err = rsiSetBox(CTXT, box,rsiTBoxFlags,rsiFBoxInverted,rsiTDone);
		if(err)
			return err;
	}

	if(insideCSG > 0)
	{
		SetCSGObject(box);
		insideCSG--;
	}

	return rsiERR_NONE;
}

ULONG cylinder(ULONG *arg)
{
	rsiVECTOR vec;
	void *cylinder,*surf,*actor;
	rsiResult err;
	int flags=0;

	surf = FindSurfListItem((char*)arg[0]);
	if(!surf)
		return rsiERR_SURFACE;

	err = rsiCreateCylinder(CTXT, &cylinder,surf);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetCylinder(CTXT, cylinder,rsiTCylinderPos, &vec, rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetCylinder(CTXT, cylinder, rsiTCylinderSize, &vec, rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		if(!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetCylinder(CTXT, cylinder,rsiTCylinderAlign,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		actor = FindActorListItem((char*)arg[4]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetCylinder(CTXT, cylinder,rsiTCylinderActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
		flags |= rsiFCylinderInverted;

	if(arg[6])
		flags |= rsiFCylinderOpenTop;

	if(arg[7])
		flags |= rsiFCylinderOpenBottom;

	err = rsiSetCylinder(CTXT, cylinder, rsiTCylinderFlags, flags, rsiTDone);
	if(err)
		return err;

	if(insideCSG > 0)
	{
		SetCSGObject(cylinder);
		insideCSG--;
	}

	return rsiERR_NONE;
}

ULONG sor(ULONG *arg)
{
	rsiVECTOR vec;
	void *sor,*surf,*actor;
	rsiResult err;
	int flags=0;
	int number;
	char **item;
	rsiVECTOR2D *p;

	surf = FindSurfListItem((char*)arg[0]);
	if(!surf)
		return rsiERR_SURFACE;

	// count points
	number = 0;
	item = (char**)arg[1];
	while(item[number])
		number++;

	p = new rsiVECTOR2D[number];
	if(!p)
		return rsiERR_MEM;

	// read points
	number = 0;
	item = (char**)arg[1];
	while(item[number])
	{
		if(!read_vector2d(&p[number], item[number]))
		{
			delete p;
			return ERROR_VECTOR2D;
		}
		number++;
	}

	err = rsiCreateSOR(CTXT, &sor,surf,number,p);
	delete p;
	if(err)
		return err;

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetSOR(CTXT, sor,rsiTSORPos,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		if(!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetSOR(CTXT, sor,rsiTSORSize,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		if(!read_vector(&vec,(char*)arg[4]))
			return ERROR_VECTOR;
		err = rsiSetSOR(CTXT, sor,rsiTSORAlign,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
	{
		actor = FindActorListItem((char*)arg[5]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetSOR(CTXT, sor,rsiTSORActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[6])
		flags |= rsiFSORSturm;

	if(arg[7])
		flags |= rsiFSORInverted;

	if(arg[8])
		flags |= rsiFSOROpenTop;

	if(arg[9])
		flags |= rsiFSOROpenBottom;

	err = rsiSetSOR(CTXT, sor, rsiTSORFlags, flags, rsiTDone);
	if(err)
		return err;

	if(insideCSG > 0)
	{
		SetCSGObject(sor);
		insideCSG--;
	}

	return rsiERR_NONE;
}

ULONG cone(ULONG *arg)
{
	rsiVECTOR vec;
	void *cone,*surf,*actor;
	rsiResult err;
	int flags=0;

	surf = FindSurfListItem((char*)arg[0]);
	if(!surf)
		return rsiERR_SURFACE;

	err = rsiCreateCone(CTXT, &cone,surf);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetCone(CTXT, cone,rsiTConePos,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetCone(CTXT, cone,rsiTConeSize,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		if(!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetCone(CTXT, cone,rsiTConeAlign,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		actor = FindActorListItem((char*)arg[4]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetCone(CTXT, cone,rsiTConeActor,actor,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
		flags |= rsiFConeInverted;

	if(arg[6])
		flags |= rsiFConeOpenBottom;

	err = rsiSetCone(CTXT, cone, rsiTConeFlags, flags, rsiTDone);
	if(err)
		return err;

	if(insideCSG > 0)
	{
		SetCSGObject(cone);
		insideCSG--;
	}

	return rsiERR_NONE;
}

ULONG csg(ULONG *arg)
{
	int operation;
	void *csg;
	rsiResult err;

	err = rsiCreateCSG(CTXT, &csg);
	if(err)
		return err;

	if(arg[0])
	{
		if(KEYWORD((char*)arg[0], "UNION"))
			operation = rsiFCSGUnion;
		else
			if(KEYWORD((char*)arg[0], "INTERSECTION"))
				operation = rsiFCSGIntersection;
			else
				if(KEYWORD((char*)arg[0], "DIFFERENCE"))
					operation = rsiFCSGDifference;
				else
					return ERROR_CSGOPERATOR;

		err = rsiSetCSG(CTXT, csg, rsiTCSGOperator, operation, rsiTDone);
		if(err)
			return err;
	}

	if(arg[1])
	{
		err = rsiSetCSG(CTXT, csg, rsiTCSGFlags, rsiFCSGInverted, rsiTDone);
		if(err)
			return err;
	}

	if(insideCSG > 0)
	{
		SetCSGObject(csg);
		insideCSG--;
	}

	if(!AddCSGItem(csg))
		return ERROR_MEM;
	insideCSG += 2;

	return rsiERR_NONE;
}

ULONG loadobj(ULONG *arg)
{
	rsiVECTOR pos = {0.,0.,0.}, align = {0.,0.,0.}, scale = {1.,1.,1.};
	void *surf=NULL,*actor=NULL;

	if(arg[1])
	{
		if(!read_vector(&pos,(char*)arg[1]))
			return ERROR_VECTOR;
	}
	if(arg[2])
	{
		if(!read_vector(&align,(char*)arg[2]))
			return ERROR_VECTOR;
	}
	if(arg[3])
	{
		if(!read_vector(&scale,(char*)arg[3]))
			return ERROR_VECTOR;
	}
	if(arg[4])
	{
		actor = FindActorListItem((char*)arg[4]);
		if(!actor)
			return rsiERR_ACTOR;
	}
	if(arg[5])
	{
		surf = FindSurfListItem((char*)arg[5]);
		if(!surf)
			return rsiERR_SURFACE;
	}

	return rsiLoadObject(CTXT, (char*)arg[0],
		rsiTObjPos,&pos,
		rsiTObjAlign,&align,
		rsiTObjScale,&scale,
		rsiTObjActor,actor,
		rsiTObjSurface,surf,
		rsiTDone);
}

ULONG setworld(ULONG *arg)
{
	rsiCOLOR color;
	float fnum;
	rsiResult err;

	if(arg[0])
	{
		if(!read_color(&color,(char*)arg[0]))
			return ERROR_COLOR;
		err = rsiSetWorld(CTXT, rsiTWorldBackground,&color,rsiTDone);
		if(err)
			return err;
	}

	if(arg[1])
	{
		if(!read_color(&color,(char*)arg[1]))
			return ERROR_COLOR;
		err = rsiSetWorld(CTXT, rsiTWorldAmbient,&color,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		err = rsiSetRandomJitter(CTXT, TRUE);
		if(err)
			return err;
	}

	if(arg[3])
	{
		err = rsiSetWorld(CTXT, rsiTWorldBackdropPic,(char*)arg[3],rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		fnum = (float)atof((char*)arg[4]);
		err = rsiSetWorld(CTXT, rsiTWorldFogLength,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
	{
		fnum = (float)atof((char*)arg[5]);
		err = rsiSetWorld(CTXT, rsiTWorldFogHeight,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[6])
	{
		if(!read_color(&color,(char*)arg[6]))
			return ERROR_COLOR;
		err = rsiSetWorld(CTXT, rsiTWorldFogColor,&color,rsiTDone);
		if(err)
			return err;
	}

	if(arg[7])
	{
		err = rsiSetWorld(CTXT, rsiTWorldReflectionMap,(char*)arg[7],rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG ambient(ULONG *arg)
{
	rsiCOLOR color;

	if(!surf)
		return rsiERR_SURFACE;

	if(!read_color(&color,(char*)arg[0]))
		return ERROR_COLOR;

	return rsiSetSurface(CTXT, surf->item,rsiTSurfAmbient,&color,rsiTDone);
}

ULONG diffuse(ULONG *arg)
{
	rsiCOLOR color;

	if(!surf)
		return rsiERR_SURFACE;

	if(!read_color(&color,(char*)arg[0]))
		return ERROR_COLOR;

	return rsiSetSurface(CTXT, surf->item,rsiTSurfDiffuse,&color,rsiTDone);
}

ULONG foglen(ULONG *arg)
{
	float fnum;

	if(!surf)
		return rsiERR_SURFACE;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(CTXT, surf->item,rsiTSurfFoglen,fnum,rsiTDone);
}

ULONG specular(ULONG *arg)
{
	rsiCOLOR color;

	if(!surf)
		return rsiERR_SURFACE;

	if(!read_color(&color,(char*)arg[0]))
		return ERROR_COLOR;

	return rsiSetSurface(CTXT, surf->item,rsiTSurfSpecular,&color,rsiTDone);
}

ULONG difftrans(ULONG *arg)
{
	rsiCOLOR color;

	if(!surf)
		return rsiERR_SURFACE;

	if(!read_color(&color,(char*)arg[0]))
		return ERROR_COLOR;

	return rsiSetSurface(CTXT, surf->item,rsiTSurfDifftrans,&color,rsiTDone);
}

ULONG spectrans(ULONG *arg)
{
	rsiCOLOR color;

	if(!surf)
		return rsiERR_SURFACE;

	if(!read_color(&color,(char*)arg[0]))
		return ERROR_COLOR;

	return rsiSetSurface(CTXT, surf->item,rsiTSurfSpectrans,&color,rsiTDone);
}

ULONG refexp(ULONG *arg)
{
	float fnum;

	if(!surf)
		return rsiERR_SURFACE;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(CTXT, surf->item,rsiTSurfRefexp,fnum,rsiTDone);
}

ULONG transexp(ULONG *arg)
{
	float fnum;

	if(!surf)
		return rsiERR_SURFACE;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(CTXT, surf->item,rsiTSurfTransexp,fnum,rsiTDone);
}

ULONG refrindex(ULONG *arg)
{
	float fnum;

	if(!surf)
		return rsiERR_SURFACE;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(CTXT, surf->item,rsiTSurfRefrindex,fnum,rsiTDone);
}

ULONG reflect(ULONG *arg)
{
	rsiCOLOR color;

	if(!surf)
		return rsiERR_SURFACE;

	if(!read_color(&color,(char*)arg[0]))
		return ERROR_COLOR;

	return rsiSetSurface(CTXT, surf->item,rsiTSurfReflect,&color,rsiTDone);
}

ULONG transpar(ULONG *arg)
{
	rsiCOLOR color;

	if(!surf)
		return rsiERR_SURFACE;

	if(!read_color(&color,(char*)arg[0]))
		return ERROR_COLOR;

	return rsiSetSurface(CTXT, surf->item,rsiTSurfTranspar,&color,rsiTDone);
}

ULONG transluc(ULONG *arg)
{
	float fnum;

	if(!surf)
		return rsiERR_SURFACE;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(CTXT, surf->item,rsiTSurfTransluc,fnum,rsiTDone);
}

ULONG imtexture(ULONG *arg)
{
	void *texture, *actor;
	rsiVECTOR vec;
	float fnum;
	rsiResult err;
	int i;

	if(!surf)
		return rsiERR_SURFACE;

	err = rsiAddImagineTexture(CTXT, &texture,(char*)arg[0],surf->item);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetTexture(CTXT, texture,rsiTTexturePos,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetTexture(CTXT, texture,rsiTTextureAlign,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		if(!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetTexture(CTXT, texture,rsiTTextureSize,&vec,rsiTDone);
		if(err)
			return err;
	}

	for(i=0; i<16; i++)
	{
		if(arg[i+4])
		{
			fnum = (float)atof((char*)arg[i+4]);
			err = rsiSetTexture(CTXT, texture, rsiTTexturePara1+i, fnum, rsiTDone);
			if(err)
				return err;
		}
	}

	if(arg[20])
	{
		actor = FindActorListItem((char*)arg[20]);
		if(!actor)
			return rsiERR_ACTOR;
		err = rsiSetTexture(CTXT, texture,rsiTTextureActor,actor,rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG hypertexture(ULONG *arg)
{
	void *texture, *actor;
	rsiVECTOR vec;
	rsiResult err;
	float fnum;
	int type;

	if(!surf)
		return rsiERR_SURFACE;

	if(KEYWORD((char*)arg[0], "EXPLOSION"))
		type = rsiFHyperExplosion;
	else
		return ERROR_HYPERTYPE;

	err = rsiAddHyperTexture(CTXT, &texture,type,surf->item);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetHyperTexture(CTXT, texture,rsiTHyperPos,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetHyperTexture(CTXT, texture,rsiTHyperAlign,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		if(!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetHyperTexture(CTXT, texture,rsiTHyperSize,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		fnum = (float)atof((char*)arg[4]);
		err = rsiSetHyperTexture(CTXT, texture,rsiTHyperP1,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
	{
		fnum = (float)atof((char*)arg[5]);
		err = rsiSetHyperTexture(CTXT, texture,rsiTHyperP2,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[6])
	{
		fnum = (float)atof((char*)arg[6]);
		err = rsiSetHyperTexture(CTXT, texture,rsiTHyperP3,fnum,rsiTDone);
		if(err)
			return err;
	}

	if(arg[7])
	{
		actor = FindActorListItem((char*)arg[7]);
		if(!actor)
			return rsiERR_ACTOR;
		err = rsiSetHyperTexture(CTXT, texture,rsiTHyperActor,actor,rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG brush(ULONG *arg)
{
	void *brush,*actor;
	rsiVECTOR vec;
	int flags = 0;
	rsiResult err;

	if(!surf)
		return rsiERR_SURFACE;

	err = rsiAddBrush(CTXT, &brush,((char*)arg[0]),surf->item);
	if(err)
		return err;

	if(KEYWORD((char*)arg[1], "COLOR"))
		flags = rsiFBrushColor;
	else
		if(KEYWORD((char*)arg[1], "REFLECT"))
			flags = rsiFBrushReflectivity;
		else
			if(KEYWORD((char*)arg[1], "FILTER"))
				flags = rsiFBrushFilter;
			else
				if(KEYWORD((char*)arg[1], "ALTITUDE"))
					flags = rsiFBrushAltitude;
				else
					if(KEYWORD((char*)arg[1], "SPECULAR"))
						flags = rsiFBrushSpecular;
					else
						return ERROR_BRUSHTYPE;

	if(KEYWORD((char*)arg[2], "WRAPX"))
		flags |= rsiFBrushWrapX;
	else
		if(KEYWORD((char*)arg[2], "WRAPY"))
			flags |= rsiFBrushWrapY;
		else
			if(KEYWORD((char*)arg[2], "WRAPXY"))
				flags |= rsiFBrushWrapXY;
			else
				if(!KEYWORD((char*)arg[2], "FLAT"))
					return ERROR_BRUSHMETHOD;

	if(arg[6])
		flags |= rsiFBrushRepeat;
	if(arg[7])
		flags |= rsiFBrushMirror;
	if(arg[8])
		flags |= rsiFBrushSoft;

	err = rsiSetBrush(CTXT, brush,rsiTBrushFlags,flags,rsiTDone);
	if(err)
		return err;

	if(!read_vector(&vec,(char*)arg[3]))
		return ERROR_VECTOR;
	err = rsiSetBrush(CTXT, brush,rsiTBrushPos,&vec,rsiTDone);
	if(err)
		return err;

	if(!read_vector(&vec,(char*)arg[4]))
		return ERROR_VECTOR;
	err = rsiSetBrush(CTXT, brush,rsiTBrushAlign,&vec,rsiTDone);
	if(err)
		return err;

	if(!read_vector(&vec,(char*)arg[5]))
		return ERROR_VECTOR;
	err = rsiSetBrush(CTXT, brush,rsiTBrushSize,&vec,rsiTDone);
	if(err)
		return err;

	if(arg[9])
	{
		actor = FindActorListItem((char*)arg[9]);
		if(!actor)
			return rsiERR_ACTOR;
		err = rsiSetBrush(CTXT, brush,rsiTBrushActor,actor,rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG savepic(ULONG *arg)
{
	rsiResult err;

	if(arg[1])
		err = rsiSavePicture(CTXT, (char*)arg[0],rsiTPicType,(char*)arg[1],rsiTDone);
	else
		err = rsiSavePicture(CTXT, (char*)arg[0],rsiTDone);
	if(err)
		return err;

// SetAttrs((Object *)ObjApp.app,MUIA_Application_RexxString,(ULONG)ImageError(), TAG_DONE);

	return rsiERR_NONE;
}

#ifdef QUANT
ULONG display(ULONG *arg)
{
	display((BOOL)arg[0]);

	return RETURN_OK;
}
#endif

ULONG cleanup(ULONG *arg)
{
	rsiResult err;

	WriteLog(rc, "Cleanup");

	err = rsiCleanup(CTXT);
	if(err)
		return err;

#ifdef QUANT
	if(scrptr)
		delete [ ] scrptr;
	if(colormap)
		delete [ ] colormap;
	if (main_scr)
		CloseScreen(main_scr);
#endif

	DestroyLists();      // free surface and actor lists
	DestroyCSGStack();
	surf = NULL;
	actor = NULL;
	insideCSG = 0;

	return rsiERR_NONE;
}

ULONG startrender(ULONG *arg)
{
	rsiResult err;
	int flags = 0;
	float begin, end, fnum;
	int left,top,right,bottom;
	UBYTE *scr;

	if(insideCSG > 0)
		return ERROR_CSG;

	if(arg[0])
		flags |= rsiFRenderQuick;

	if(arg[1])
	{
		err = rsiSetOctreeDepth(CTXT, (WORD)(*(ULONG*)arg[1]));
		if(err)
			return err;
	}

	if(arg[2])
		begin = (float)atof((char*)arg[2]);
	else
		begin = 0.;

	if(arg[3])
		end = (float)atof((char*)arg[3]);
	else
		end = 0.;

	if(arg[4])
		left = (int)(*(ULONG*)arg[4]);
	else
		left = rsiFDefault;

	if(arg[5])
		top = (int)(*(ULONG*)arg[5]);
	else
		top = rsiFDefault;

	if(arg[6])
		right = (int)(*(ULONG*)arg[6]);
	else
		right = rsiFDefault;

	if(arg[7])
		bottom = (int)(*(ULONG*)arg[7]);
	else
		bottom = rsiFDefault;

	if(arg[8])
	{
		err = rsiSetWorld(CTXT, rsiTWorldMinObjects, *(ULONG*)arg[8], rsiTDone);
		if(err)
			return err;
	}

	if(arg[9])
	{
		fnum = (float)atof((char*)arg[9]);
		err = rsiSetWorld(CTXT, rsiTWorldHyperStep, fnum, rsiTDone);
		if(err)
			return err;
	}

	err = rsiSetRenderField(CTXT, left,top,right,bottom);
	if(err)
		return err;

	return rsiRender(CTXT, &scr,rsiTRenderFlags,flags,rsiTRenderTimeBegin,begin,rsiTRenderTimeEnd,end,rsiTDone);
}

ULONG antialias(ULONG *arg)
{
	rsiCOLOR color;
	float fnum;
	rsiResult err;

	err = rsiSetAntialias(CTXT,
		rsiTAntiSamples, *((int*)arg[0]),
		rsiTDone);
	if(err)
		return err;

	if(arg[1])
	{
		fnum = (float)atof((char*)arg[1]);
		err = rsiSetAntialias(CTXT,
			rsiTAntiFilter,fnum,
			rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_color(&color,(char*)arg[2]))
			return ERROR_COLOR;
		err = rsiSetAntialias(CTXT,
			rsiTAntiContrast,&color,
			rsiTDone);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}

ULONG brushpath(ULONG *arg)
{
	return rsiSetBrushPath(CTXT, (char*)arg[0]);
}

ULONG texturepath(ULONG *arg)
{
	return rsiSetTexturePath(CTXT, (char*)arg[0]);
}

ULONG objectpath(ULONG *arg)
{
	return rsiSetObjectPath(CTXT, (char*)arg[0]);
}

ULONG newactor(ULONG *arg)
{
	rsiVECTOR vec;
	void *act;
	rsiResult err;

	err = rsiCreateActor(CTXT, &act);
	if(err)
		return err;

	actor = AddActorList((char*)arg[0],act);
	if(!actor)
		return rsiERR_MEM;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetActor(CTXT, act,rsiTActorPos,&vec,rsiTDone);
		if(err)
			return err;
	}
	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetActor(CTXT, act,rsiTActorAlign,&vec,rsiTDone);
		if(err)
			return err;
	}
	if(arg[3])
	{
		if(!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetActor(CTXT, act,rsiTActorSize,&vec,rsiTDone);
		if(err)
			return err;
	}
	return rsiERR_NONE;
}

ULONG position(ULONG *arg)
{
	rsiVECTOR vec;
	float begin,end;
	int flags = rsiFActionLinear;

	if(!actor)
		return rsiERR_ACTOR;

	begin = (float)atof((char*)arg[0]);
	end = (float)atof((char*)arg[1]);

	if(!read_vector(&vec,(char*)arg[2]))
		return ERROR_VECTOR;

	if(arg[3])
	{
		if(KEYWORD((char*)arg[3], "LINEAR"))
			flags |= rsiFActionLinear;
		else
			if(KEYWORD((char*)arg[3], "SPLINE"))
				flags |= rsiFActionSpline;
			else
				return ERROR_INTERPOL;
	}

	return rsiPosition(CTXT, actor->item,begin,end,&vec,/*rsiTPosFlags,flags,*/rsiTDone);
}

ULONG alignment(ULONG *arg)
{
	rsiVECTOR vec;
	float begin,end;
	int flags = rsiFActionLinear;

	if(!actor)
		return rsiERR_ACTOR;

	begin = (float)atof((char*)arg[0]);
	end = (float)atof((char*)arg[1]);

	if(!read_vector(&vec,(char*)arg[2]))
		return ERROR_VECTOR;

	if(arg[3])
	{
		if(KEYWORD((char*)arg[3], "LINEAR"))
			flags |= rsiFActionLinear;
		else
			if(KEYWORD((char*)arg[3], "SPLINE"))
				flags |= rsiFActionSpline;
			else
				return ERROR_INTERPOL;
	}


	return rsiAlignment(CTXT, actor->item,begin,end,&vec,/*rsiTAlignFlags,flags,*/rsiTDone);
}

ULONG size(ULONG *arg)
{
	rsiVECTOR vec;
	float begin,end;
	int flags = rsiFActionLinear;

	if(!actor)
		return rsiERR_ACTOR;

	begin = (float)atof((char*)arg[0]);
	end = (float)atof((char*)arg[1]);

	if(!read_vector(&vec,(char*)arg[2]))
		return ERROR_VECTOR;

	if(arg[3])
	{
		if(KEYWORD((char*)arg[3], "LINEAR"))
			flags |= rsiFActionLinear;
		else
			if(KEYWORD((char*)arg[3], "SPLINE"))
				flags |= rsiFActionSpline;
			else
				return ERROR_INTERPOL;
	}

	return rsiSize(CTXT, actor->item,begin,end,&vec,/*rsiTSizeFlags,flags,*/rsiTDone);
}

ULONG distrib(ULONG *arg)
{
	int motion,soft;

	if(arg[0])
		motion = *(int*)(arg[0]);
	else
		motion = 1;
	if(arg[1])
		soft = *(int*)(arg[1]);
	else
		soft = 1;
	return rsiSetDistribution(CTXT, motion, soft);
}

ULONG geterrorstr(ULONG *arg)
{
	rsiResult err;

	if((*(int*)arg[0]) >= 100)
		strcpy(buffer, app_errors[(*(int*)arg[0])-100]);
	else
	{
		err = rsiGetErrorMsg(CTXT, buffer, *(int*)arg[0]);
		if(err)
			return err;
	}

	return rsiERR_NONE;
}
