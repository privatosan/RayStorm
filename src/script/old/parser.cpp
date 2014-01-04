/***************
 * NAME:				parser.cpp
 * PROJECT:			RayStorm
 * VERSION:			0.1 19.03.1996
 * DESCRIPTION:	parses the RayStorm script language
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						19.03.1996	mh		first release (0.1)
 *					   03.02.1997	mh		rsiStarTilt
 ***************/

#include <math.h>
#include "parser.h"
#include "rsi.h"
#include "fileutil.h"
#include "itemlist.h"
#include "csgstack.h"

#ifndef PI
#define PI 3.141592654
#endif

extern "C"
{
int f_triangle(ULONG *);
int f_newsurface(ULONG *);
int f_sphere(ULONG *);
int f_plane(ULONG *);
int f_box(ULONG *);
int f_cylinder(ULONG *);
int f_sor(ULONG *);
int f_cone(ULONG *);
int f_csg(ULONG *);
int f_pointlight(ULONG *);
int f_spotlight(ULONG *);
int f_flare(ULONG *);
int f_star(ULONG *);
int f_flare(ULONG *);
int f_loadobj(ULONG *);
int f_ambient(ULONG *);
int f_diffuse(ULONG *);
int f_foglen(ULONG *);
int f_specular(ULONG *);
int f_difftrans(ULONG *);
int f_spectrans(ULONG *);
int f_refexp(ULONG *);
int f_transexp(ULONG *);
int f_refrindex(ULONG *);
int f_reflect(ULONG *);
int f_transpar(ULONG *);
int f_transluc(ULONG *);
int f_imtexture(ULONG *);
int f_hypertexture(ULONG *);
int f_brush(ULONG *);
int f_setcamera(ULONG *);
int f_setscreen(ULONG *);
int f_setworld(ULONG *);
int f_savepic(ULONG *);
int f_cleanup(ULONG *);
int f_startrender(ULONG *);
int f_antialias(ULONG *);
int f_brushpath(ULONG *);
int f_texturepath(ULONG *);
int f_picturepath(ULONG *);
int f_includepath(ULONG *);
int f_objectpath(ULONG *);
int f_alignment(ULONG *);
int f_newactor(ULONG *);
int f_position(ULONG *);
int f_size(ULONG *);
int f_distrib(ULONG *);
int f_include(ULONG *);
}

// structures
struct RAYCOMMAND
{
	char *name;
	int (*func)(ULONG *arg);
};

struct INCLUDE
{
	char name[256];
	FILE *hFile;
	int linenumber;
};

// maximum nesting of includes
#define MAXINCLUDE	10
#define ERROR_BASE	100

#define MAXARGS 30
#define KEYWORD(a,b) (a ? (!stricmp((char*)(a), b)) : FALSE)

enum errnos
{
	RETURN_OK,
	ERROR_MEM = ERROR_BASE,
	ERROR_SURFACE,
	ERROR_RESOLUTION,
	ERROR_BRUSHTYPE,
	ERROR_BRUSHMETHOD,
	ERROR_INTERPOL,
	ERROR_READ,
	ERROR_WRITE,
	ERROR_INTEXP,
	ERROR_FLOATEXP,
	ERROR_VECTOR,
	ERROR_COLOREXP,
	ERROR_STRINGEXP,
	ERROR_INVKEYW,
	ERROR_UNKNOWNCOM,
	ERROR_SYNTAX,
	ERROR_MAXINCLUDE,
	ERROR_NOLIGHT,
	ERROR_FLARETYPE,
	ERROR_FLAREFUNC,
	ERROR_CSG,
	ERROR_CSGOPERATOR,
	ERROR_HYPERTYPE,
   ERROR_VECTOR2D
};

// Application errors
static char *app_errors[] =
{
	"Not enough memory",
	"Missing surface name",
	"Wrong screen resolution",
	"Unknown brush mapping type",
	"Unknown brush wrapping method",
	"Unknown interpolation method",
	"Can't open file",
	"Can't write file",
	"Integer expected",
	"Float expected",
	"Vector expected",
	"Color expected",
	"String expected",
	"Invalid identifier",
	"Unknown command",
	"Syntax error",
	"Maximum nesting reached",
	"Unknown flare type",
	"Unknown flare function",
	"Only one object for a CSG object has been defined",
	"Unknown CSG operator",
   "Unknown hyper texture type",
   "Invalid 2d-vector definition"
};

// all commands RayStorm knows
static RAYCOMMAND commands[] =
{
	{"TRIANGLE",		f_triangle},
	{"NEWSURFACE",		f_newsurface},
	{"SPHERE",			f_sphere},
	{"PLANE",			f_plane},
	{"BOX",				f_box},
	{"CYLINDER",		f_cylinder},
	{"SOR",				f_sor},
	{"CONE",				f_cone},
	{"CSG",				f_csg},		
	{"POINTLIGHT",		f_pointlight},
	{"SPOTLIGHT",		f_spotlight},
	{"LOADOBJ",			f_loadobj},
	{"AMBIENT",			f_ambient},
	{"DIFFUSE",			f_diffuse},
	{"FOGLEN",			f_foglen},
	{"SPECULAR",		f_specular},
	{"DIFFTRANS",		f_difftrans},
	{"SPECTRANS",		f_spectrans},
	{"REFEXP",			f_refexp},
	{"TRANSEXP",		f_transexp},
	{"REFRINDEX",		f_refrindex},
	{"REFLECT",			f_reflect},
	{"TRANSPAR",		f_transpar},
	{"TRANSLUC",		f_transluc},
	{"IMTEXTURE",		f_imtexture},
	{"HYPERTEXTURE",	f_hypertexture},
	{"BRUSH",			f_brush},
	{"SETCAMERA",		f_setcamera},
	{"SETSCREEN",		f_setscreen},
	{"SETWORLD",		f_setworld},
	{"SAVEPIC",			f_savepic},
	{"CLEANUP",			f_cleanup},
	{"STARTRENDER",	f_startrender},
	{"ANTIALIAS",		f_antialias},
	{"BRUSHPATH",		f_brushpath},
	{"TEXTUREPATH",	f_texturepath},
	{"OBJECTPATH",		f_objectpath},
	{"PICTUREPATH", 	f_picturepath},
	{"INCLUDEPATH", 	f_includepath},
	{"ALIGNMENT",		f_alignment},
	{"NEWACTOR",		f_newactor},
	{"POSITION",		f_position},
	{"SIZE",				f_size},
	{"DISTRIB",			f_distrib},
	{"INCLUDE",			f_include},
	{"STAR",				f_star},
	{"FLARE",			f_flare},
	{NULL, 0L},
};

// global variables
static int		include;				// current include nesting
static INCLUDE	Includefile[MAXINCLUDE];
static char		szIncludePath[256];
static char		szPicturePath[256];
static CRenderOutput *pDisplay;
static ITEMLIST *surf = NULL;
static ITEMLIST *actor = NULL;
static int		xRes=160, yRes=128;
		 rsiResult err;
static rsiVECTOR 	v;
static char 	buf[256];
static void 	*obj;
static void		*light = NULL;
rsiSMALL_COLOR	*scrarray = NULL;

// amount of objects needed for CSG
static int insideCSG = 0;

static void Cooperate()
{
	pDisplay->Cooperate();
}

static void Log(char *text)
{
	pDisplay->Log(text);
}

static BOOL CheckCancel()
{
	return pDisplay->CheckCancel();
}

static void __cdecl UpdateStatus(float percent, float time, int, int, rsiSMALL_COLOR *)
{
	pDisplay->UpdateStatus(percent, time);
}

BOOL GetPictureInfo(UBYTE **p, int *xR, int *yR)
{
	*p = (UBYTE*)scrarray;
	*xR = xRes;
	*yR = yRes;

	return (*p != NULL);
}

void ParserCleanup()
{
	if (scrarray)
		delete [ ] scrarray;
}

void open_error_request(unsigned char *err)
{
	MessageBox(NULL, (char *)err, "Oh Oh ! - Error", MB_OK | MB_TASKMODAL);
}

void GetError(char *buffer, int no)
{
	if (no >= ERROR_BASE)
		strcpy(buffer, app_errors[no - ERROR_BASE]);
	else
		rsiGetErrorMsg(buffer, no);
}


/*************
 * FUNCTION:		read_vector
 * DESCRIPTION:	read vector from string
 * INPUT:			v		pointer to vector
 *						s		vector string; format '<x,y,z>'
 * OUTPUT:			TRUE if ok else FALSE
 *************/
BOOL read_vector(rsiVECTOR *v, char *s)
{
	if (s)
	{
		if (s[0] != '<')	
			return FALSE;
		if (s[strlen(s)-1] != '>')
			return FALSE;
		v->x = float(atof(strtok(&s[1],",")));
		v->y = float(atof(strtok(NULL,",")));
		v->z = float(atof(strtok(NULL,">")));

		return TRUE;
	}
	else
		return FALSE;
}

/*************
 * FUNCTION:      read_vector2d
 * DESCRIPTION:   read 2d-vector from string
 * INPUT:         v     pointer to vector
 *                s     vector string; format '<x,y>'
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL read_vector2d(rsiVECTOR2D *v, char *s)
{
	char buf[256];
	
	if (s)
	{
		strcpy(buf, s);
		
	   if(buf[0] != '<')
			return FALSE;
  		if(buf[strlen(buf)-1] != '>')
			return FALSE;
		v->x = float(atof(strtok(&buf[1],",")));
		v->y = float(atof(strtok(NULL,">")));
		return TRUE;
	}
	else
		return FALSE;
}

/*************
 * FUNCTION:		read_color
 * DESCRIPTION:	read color from string
 * INPUT:			c		pointer to color
 *						s		color string; format '[r,g,b]'
 * OUTPUT:			TRUE if ok else FALSE
 *************/
BOOL read_color(rsiCOLOR *c, char *s)
{
	if (s)
	{
		if (s[0] != '<')
			return FALSE;
		if (s[strlen(s)-1] != '>')
			return FALSE;
		c->r = float(atol(strtok(&s[1],",")))/255;
		c->g = float(atol(strtok(NULL,",")))/255;
		c->b = float(atol(strtok(NULL,"<")))/255;

		return TRUE;
	}
	return FALSE;
}

int f_triangle(ULONG *arg)
{
	void *surf, *actor = NULL;
	int i;
	rsiVECTOR v[3];
	rsiVECTOR n[3];

	if (!arg[0])
		return rsiERR_SURFACE;
		
	surf = FindSurfListItem((char*)arg[0]);
	if (arg[7])
	{
		actor = FindActorListItem((char*)arg[7]);
	}
	if (!surf)
		return rsiERR_SURFACE;

	for(i=0; i<3; i++)
	{
		if (!read_vector(&v[i],(char*)arg[i+1]))
			return ERROR_VECTOR;
	}

	if (arg[4])
	{
		for(i=0; i<3; i++)
		{
			if (arg[i+4])
			{
				if (!read_vector(&n[i],(char*)arg[i+4]))
					return ERROR_VECTOR;
			}
		}
		return rsiCreateTriangle(surf, &v[0], &v[1], &v[2],
			rsiTTriangleActor, actor,
			rsiTTriangleNorm1, &n[0],
			rsiTTriangleNorm2, &n[1],
			rsiTTriangleNorm3, &n[2], rsiTDone);
	}
	else
		return rsiCreateTriangle(surf, &v[0], &v[1], &v[2],
			rsiTTriangleActor, actor, rsiTDone);
}

int f_newsurface(ULONG *arg)
{
	int flags = 0;
	rsiResult err;
	void *surface;

	if (KEYWORD(arg[1], "BRIGHT"))
		flags |= rsiFSurfBright;

	err = rsiCreateSurface(&surface, rsiTSurfFlags, flags, rsiTDone);
	if (err)
		return err;

	if (!arg[0])
		return ERROR_SURFACE;
		
	surf = AddSurfList((char*)arg[0], surface);
	if (!surf)
		return ERROR_MEM;

	return rsiERR_NONE;
}

int f_setcamera(ULONG *arg)
{
	rsiVECTOR pos,vec,vup;
	void *actor;
	float fnum;
	rsiResult err;

	if (!read_vector(&pos, (char*)arg[0]))
		return ERROR_VECTOR;

	if (arg[1])
	{
		if (!read_vector(&vec, (char*)arg[1]))
			return ERROR_VECTOR;
	}

	if (arg[2])
	{
		if (!read_vector(&vup,(char*)arg[2]))
			return ERROR_VECTOR;
	}

	if (arg[1] && arg[2])
	{
		err = rsiSetCamera(&pos,rsiTCameraLook,&vec,rsiTCameraViewUp,&vup,rsiTDone);
		if (err)
			return err;
	}
	if (arg[1] && !arg[2])
	{
		err = rsiSetCamera(&pos,rsiTCameraLook,&vec,rsiTDone);
		if (err)
			return err;
	}
	if (!arg[1] && arg[2])
	{
		err = rsiSetCamera(&pos, rsiTCameraViewUp, &vup, rsiTDone);
		if (err)
			return err;
	}

	if (arg[3])
	{
		fnum = (float)atof((char*)arg[3]);
		err = rsiSetCamera(&pos, rsiTCameraHFov, fnum, rsiTDone);
		if (err)
			return err;
	}

	if (arg[4])
	{
		fnum = (float)atof((char*)arg[4]);
		err = rsiSetCamera(&pos, rsiTCameraVFov, fnum, rsiTDone);
		if (err)
			return err;
	}

	if (arg[5])
	{
		fnum = (float)atof((char*)arg[5]);
		err = rsiSetCamera(&pos, rsiTCameraFocalDist, fnum, rsiTDone);
		if (err)
			return err;
	}

	if (arg[6])
	{
		fnum = (float)atof((char*)arg[6]);
		err = rsiSetCamera(&pos, rsiTCameraAperture, fnum, rsiTDone);
		if (err)
			return err;
	}

	if (arg[7])
	{
		actor = FindActorListItem((char*)arg[7]);
		if (!actor)
			return rsiERR_ACTOR;
		err = rsiSetCamera(&pos, rsiTCameraPosActor, actor, rsiTDone);
		if (err)
			return err;
	}

	if (arg[8])
	{
		actor = FindActorListItem((char*)arg[8]);
		if (!actor)
			return rsiERR_ACTOR;
		err = rsiSetCamera(&pos,rsiTCameraLookActor,actor,rsiTDone);
		if (err)
			return err;
	}

	return rsiERR_NONE;
}

int f_setscreen(ULONG *arg)
{
	if (!arg[0] || !arg[1])
		return ERROR_INTEXP;

	xRes = atoi((char *)arg[0]);
	yRes = atoi((char *)arg[1]);
	
	return rsiSetScreen(xRes, yRes);
}

int f_pointlight(ULONG *arg)
{
	rsiVECTOR vec;
	rsiCOLOR color;
	float fnum;
	void *actor;
	rsiResult err;
	ULONG flags = 0;

	err = rsiCreatePointLight(&light);
	if (err)
		return err;

	if (!read_vector(&vec, (char*)arg[0]))
		return ERROR_VECTOR;
	err = rsiSetPointLight(light, rsiTLightPos, &vec, rsiTDone);
	if (err)
		return err;

	if (arg[1])
	{
		if (!read_color(&color,(char*)arg[1]))
			return ERROR_COLOREXP;
		err = rsiSetPointLight(light, rsiTLightColor, &color,rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		fnum = (float)atof((char*)arg[2]);
		err = rsiSetPointLight(light, rsiTLightRadius, fnum,rsiTDone);
		if (err)
			return err;
	}

	if (KEYWORD(arg[3], "SHADOW"))
		flags |= rsiFLightShadow;

	if (arg[4])
	{
		actor = FindActorListItem((char*)arg[6]);
		if (!actor)
			return rsiERR_ACTOR;

		err = rsiSetPointLight(light, rsiTLightActor, actor,rsiTDone);
		if (err)
			return err;
	}

	if (arg[5])
	{
		fnum = (float)atof((char*)arg[5]);
		err = rsiSetPointLight(light, rsiTLightFallOff, fnum,rsiTDone);
		if (err)
			return err;
	}

	if (!KEYWORD(arg[6], "NOSTAR"))
		flags |= rsiFLightStar;

	if (!KEYWORD(arg[7], "NOFLARES"))
		flags |= rsiFLightFlares;

	err = rsiSetPointLight(light, rsiTLightFlags, flags, rsiTDone);
	if (err)
		return err;
		
	return rsiERR_NONE;
}

int f_spotlight(ULONG *arg)
{
	rsiVECTOR vec;
	rsiCOLOR color;
	float fnum;
	void *actor;
	rsiResult err;
	ULONG flags = 0;

	err = rsiCreateSpotLight(&light);
	if (err)
		return err;

	if (!read_vector(&vec,(char*)arg[0]))
		return ERROR_VECTOR;
	err = rsiSetSpotLight(light,rsiTLightPos,&vec,rsiTDone);
	if (err)
		return err;

	if (arg[1])
	{
		if (!read_color(&color,(char*)arg[1]))
			return ERROR_COLOREXP;
		err = rsiSetSpotLight(light,rsiTLightColor,&color,rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		if (!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetSpotLight(light,rsiTLightLookPoint,&vec,rsiTDone);
		if (err)
			return err;
	}

	if (arg[3])
	{
		fnum = (float)atof((char*)arg[3]);
		err = rsiSetSpotLight(light, rsiTLightAngle,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[4])
	{
		fnum = (float)atof((char*)arg[4]);
		err = rsiSetSpotLight(light, rsiTLightRadius, fnum, rsiTDone);
		if (err)
			return err;
	}

	if (KEYWORD(arg[5], "SHADOW"))
		flags |= rsiFLightShadow;

	if (arg[6])
	{
		actor = FindActorListItem((char*)arg[6]);
		if (!actor)
			return rsiERR_ACTOR;

		err = rsiSetSpotLight(light, rsiTLightActor, actor,rsiTDone);
		if (err)
			return err;
	}

	if (arg[7])
	{
		actor = FindActorListItem((char*)arg[7]);
		if (!actor)
			return rsiERR_ACTOR;

		err = rsiSetSpotLight(light, rsiTLightLookPActor, actor,rsiTDone);
		if (err)
			return err;
	}

	if (arg[8])
	{
		fnum = (float)atof((char*)arg[8]);
		err = rsiSetSpotLight(light, rsiTLightFallOff, fnum,rsiTDone);
		if (err)
			return err;
	}

	if (!KEYWORD(arg[9], "NOSTAR"))
		flags |= rsiFLightStar;

	if (!KEYWORD(arg[10], "NOFLARES"))
		flags |= rsiFLightFlares;

	err = rsiSetSpotLight(light, rsiTLightFlags, flags, rsiTDone);
	if (err)
		return err;

	return rsiERR_NONE;
}

int f_sphere(ULONG *arg)
{
	rsiVECTOR vec;
	float fnum;
	void *sphere,*surf,*actor;
	rsiResult err;

	if (!arg[0])
		return ERROR_SURFACE;
		
	surf = FindSurfListItem((char*)arg[0]);
	if (!surf)
		return rsiERR_SURFACE;

	err = rsiCreateSphere(&sphere,surf);
	if (err)
		return err;

	if (!read_vector(&vec,(char*)arg[1]))
		return ERROR_VECTOR;
	err = rsiSetSphere(sphere,rsiTSpherePos,&vec,rsiTDone);
	if (err)
		return err;

	if (arg[2])
	{
		fnum = (float)atof((char*)arg[2]);
		err = rsiSetSphere(sphere,rsiTSphereRadius,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[3])
	{
		actor = FindActorListItem((char*)arg[3]);
		if (!actor)
			return rsiERR_ACTOR;

		err = rsiSetSphere(sphere, rsiTSphereActor, actor, rsiTDone);
		if (err)
			return err;
	}

	if(insideCSG > 0)
	{
		SetCSGObject(sphere);
		insideCSG--;
	}

	return rsiERR_NONE;
}

int f_plane(ULONG *arg)
{
	rsiVECTOR vec;
	void *plane,*surf,*actor;
	rsiResult err;

	if (!arg[0])
		return ERROR_SURFACE;
		
	surf = FindSurfListItem((char*)arg[0]);
	if (!surf)
		return rsiERR_SURFACE;

	err = rsiCreatePlane(&plane,surf);
	if (err)
		return err;

	if (arg[1])
	{
		if (!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetPlane(plane, rsiTPlanePos, &vec, rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		if (!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetPlane(plane, rsiTPlaneNorm, &vec, rsiTDone);
		if (err)
			return err;
	}

	if (arg[3])
	{
		actor = FindActorListItem((char*)arg[3]);
		if (!actor)
			return rsiERR_ACTOR;

		err = rsiSetPlane(plane,rsiTPlaneActor,actor,rsiTDone);
		if (err)
			return err;
	}

	return rsiERR_NONE;
}

int f_box(ULONG *arg)
{
	rsiVECTOR vec;
	void *box,*surf,*actor;
	rsiResult err;

	if (!arg[0])
		return ERROR_SURFACE;
		
	surf = FindSurfListItem((char*)arg[0]);
	if(!surf)
		return rsiERR_SURFACE;

	err = rsiCreateBox(&box,surf);
	if(err)
		return err;

	if(arg[1])
	{
		if(!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetBox(box,rsiTBoxPos,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[2])
	{
		if(!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetBox(box,rsiTBoxLowBounds,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[3])
	{
		if(!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetBox(box,rsiTBoxHighBounds,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[4])
	{
		if(!read_vector(&vec,(char*)arg[4]))
			return ERROR_VECTOR;
		err = rsiSetBox(box,rsiTBoxAlign,&vec,rsiTDone);
		if(err)
			return err;
	}

	if(arg[5])
	{
		actor = FindActorListItem((char*)arg[5]);
		if(!actor)
			return rsiERR_ACTOR;

		err = rsiSetBox(box,rsiTBoxActor,actor,rsiTDone);
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

int f_cylinder(ULONG *arg)
{
   rsiVECTOR vec;
   void *cylinder,*surf,*actor;
   rsiResult err;
   int flags=0;

	if (!arg[0])
		return ERROR_SURFACE;

   surf = FindSurfListItem((char*)arg[0]);
   if(!surf)
      return rsiERR_SURFACE;

   err = rsiCreateCylinder(&cylinder,surf);
   if(err)
      return err;

   if(arg[1])
   {
      if(!read_vector(&vec,(char*)arg[1]))
         return ERROR_VECTOR;
      err = rsiSetCylinder(cylinder,rsiTCylinderPos, &vec, rsiTDone);
      if(err)
         return err;
   }

   if(arg[2])
   {
      if(!read_vector(&vec,(char*)arg[2]))
         return ERROR_VECTOR;
      err = rsiSetCylinder(cylinder, rsiTCylinderSize, &vec, rsiTDone);
      if(err)
         return err;
   }

   if(arg[3])
   {
      if(!read_vector(&vec,(char*)arg[3]))
         return ERROR_VECTOR;
      err = rsiSetCylinder(cylinder,rsiTCylinderAlign,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[4])
   {
      actor = FindActorListItem((char*)arg[4]);
      if(!actor)
         return rsiERR_ACTOR;

      err = rsiSetCylinder(cylinder,rsiTCylinderActor,actor,rsiTDone);
      if(err)
         return err;
   }

   if(arg[5])
      flags |= rsiFCylinderInverted;

   if(arg[6])
      flags |= rsiFCylinderOpenTop;

   if(arg[7])
      flags |= rsiFCylinderOpenBottom;

   err = rsiSetCylinder(cylinder, rsiTCylinderFlags, flags, rsiTDone);
   if(err)
      return err;

   if(insideCSG > 0)
   {
      SetCSGObject(cylinder);
      insideCSG--;
   }

   return rsiERR_NONE;
}

int f_sor(ULONG *arg)
{
   rsiVECTOR vec;
   void *sor,*surf,*actor;
   rsiResult err;
   int flags=0;
   int number;
   char **item;
   rsiVECTOR2D *p, dummy;

	if (!arg[0])
		return ERROR_SURFACE;

   surf = FindSurfListItem((char*)arg[0]);
   if(!surf)
      return rsiERR_SURFACE;

   // count points
   number = 0;
	while (read_vector2d(&dummy, (char*)arg[number + 1]))
		number++;

   p = new rsiVECTOR2D[number];
   if(!p)
      return rsiERR_MEM;

   // read points
   number = 0;
   while(arg[number + 1])
   {
      if(!read_vector2d(&p[number], (char*)arg[number + 1]))
      {
         delete p;
         return ERROR_VECTOR2D;
      }
      number++;
   }

   err = rsiCreateSOR(&sor,surf,number,p);
   delete p;
   if(err)
      return err;

	number++;
   if(arg[number])
   {
      if(!read_vector(&vec,(char*)arg[2]))
         return ERROR_VECTOR;
      err = rsiSetSOR(sor,rsiTSORPos,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[number + 1])
   {
      if(!read_vector(&vec,(char*)arg[3]))
         return ERROR_VECTOR;
      err = rsiSetSOR(sor,rsiTSORSize,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[number + 2])
   {
      if(!read_vector(&vec,(char*)arg[4]))
         return ERROR_VECTOR;
      err = rsiSetSOR(sor,rsiTSORAlign,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[number + 3])
   {
      actor = FindActorListItem((char*)arg[5]);
      if(!actor)
         return rsiERR_ACTOR;

      err = rsiSetSOR(sor,rsiTSORActor,actor,rsiTDone);
      if(err)
         return err;
   }

   if(arg[number + 6])
      flags |= rsiFSORSturm;

   if(arg[number + 7])
      flags |= rsiFSORInverted;

   if(arg[number + 8])
      flags |= rsiFSOROpenTop;

   if(arg[number + 9])
      flags |= rsiFSOROpenBottom;

   err = rsiSetSOR(sor, rsiTSORFlags, flags, rsiTDone);
   if(err)
      return err;

   if(insideCSG > 0)
   {
      SetCSGObject(sor);
      insideCSG--;
   }

   return rsiERR_NONE;
}

int f_cone(ULONG *arg)
{
   rsiVECTOR vec;
   void *cone,*surf,*actor;
   rsiResult err;
   int flags=0;

	if (!arg[0])
		return ERROR_SURFACE;

   surf = FindSurfListItem((char*)arg[0]);
   if(!surf)
      return rsiERR_SURFACE;

   err = rsiCreateCone(&cone,surf);
   if(err)
      return err;

   if(arg[1])
   {
      if(!read_vector(&vec,(char*)arg[1]))
         return ERROR_VECTOR;
      err = rsiSetCone(cone,rsiTConePos,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[2])
   {
      if(!read_vector(&vec,(char*)arg[2]))
         return ERROR_VECTOR;
      err = rsiSetCone(cone,rsiTConeSize,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[3])
   {
      if(!read_vector(&vec,(char*)arg[3]))
         return ERROR_VECTOR;
      err = rsiSetCone(cone,rsiTConeAlign,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[4])
   {
      actor = FindActorListItem((char*)arg[4]);
      if(!actor)
         return rsiERR_ACTOR;

      err = rsiSetCone(cone,rsiTConeActor,actor,rsiTDone);
      if(err)
         return err;
   }

   if(arg[5])
      flags |= rsiFConeInverted;

   if(arg[6])
      flags |= rsiFConeOpenBottom;

   err = rsiSetCone(cone, rsiTConeFlags, flags, rsiTDone);
   if(err)
      return err;

   if(insideCSG > 0)
   {
      SetCSGObject(cone);
      insideCSG--;
   }

   return rsiERR_NONE;
}

int f_csg(ULONG *arg)
{
	int operation;
	void *csg;
	rsiResult err;

	err = rsiCreateCSG(&csg);
	if(err)
		return err;

	if(arg[0])
	{
		if(KEYWORD(arg[0], "UNION"))
			operation = rsiFCSGUnion;
		else
			if(KEYWORD(arg[0], "INTERSECTION"))
				operation = rsiFCSGIntersection;
			else
				if(KEYWORD(arg[0], "DIFFERENCE"))
					operation = rsiFCSGDifference;
				else
					return ERROR_CSGOPERATOR;

		err = rsiSetCSG(csg, rsiTCSGOperator, operation, rsiTDone);
		if(err)
			return err;
	}

	if(insideCSG > 0)
	{
		SetCSGObject(csg);
		insideCSG--;
	}

	insideCSG += 2;
	if(!AddCSGItem(csg))
		return ERROR_MEM;

	return rsiERR_NONE;
}

int f_loadobj(ULONG *arg)
{
	rsiVECTOR pos = {0.,0.,0.}, align = {0.,0.,0.}, scale = {1.,1.,1.};
	void *surf=NULL,*actor=NULL;

	if (arg[1])
	{
		if (!read_vector(&pos,(char*)arg[1]))
			return ERROR_VECTOR;
	}
	if (arg[2])
	{
		if (!read_vector(&align,(char*)arg[2]))
			return ERROR_VECTOR;
	}
	if (arg[3])
	{
		if (!read_vector(&scale,(char*)arg[3]))
			return ERROR_VECTOR;
	}
	if (arg[4])
	{
		actor = FindActorListItem((char*)arg[4]);
		if (!actor)
			return rsiERR_ACTOR;
	}
	if (arg[5])
	{
		surf = FindSurfListItem((char*)arg[5]);
		if (!surf)
			return rsiERR_SURFACE;
	}

	return rsiLoadObject((char*)arg[0],
		rsiTObjPos,&pos,
		rsiTObjAlign,&align,
		rsiTObjScale,&scale,
		rsiTObjActor,actor,
		rsiTObjSurface,surf,
		rsiTDone);
}

int f_setworld(ULONG *arg)
{
	rsiCOLOR color;
	float fnum;
	rsiResult err;
	void *dmy=NULL;

	if (arg[0])
	{
		if (!read_color(&color,(char*)arg[0]))
			return ERROR_COLOREXP;
		err = rsiSetWorld(dmy,rsiTWorldBackground,&color,rsiTDone);
		if (err)
			return err;
	}

	if (arg[1])
	{
		if (!read_color(&color,(char*)arg[1]))
			return ERROR_COLOREXP;
		err = rsiSetWorld(dmy,rsiTWorldAmbient,&color,rsiTDone);
		if (err)
			return err;
	}

	if (KEYWORD(arg[2], "RANDJIT"))
	{
		err = rsiSetRandomJitter(KEYWORD(arg[2], "RANDJIT"));
		if (err)
			return err;
	}

	if (arg[3])
	{
		err = rsiSetWorld(dmy,rsiTWorldBackdropPic,(char*)arg[3],rsiTDone);
		if (err)
			return err;
	}

	if (arg[4])
	{
		fnum = (float)atof((char*)arg[4]);
		err = rsiSetWorld(dmy,rsiTWorldFogLength,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[5])
	{
		fnum = (float)atof((char*)arg[5]);
		err = rsiSetWorld(dmy,rsiTWorldFogHeight,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[6])
	{
		if (!read_color(&color,(char*)arg[6]))
			return ERROR_COLOREXP;
		err = rsiSetWorld(dmy,rsiTWorldFogColor,&color,rsiTDone);
		if (err)
			return err;
	}

	if (arg[7])
	{
		err = rsiSetWorld(dmy,rsiTWorldReflectionMap,(char*)arg[7],rsiTDone);
		if (err)
			return err;
	}

	return rsiERR_NONE;
}

int f_ambient(ULONG *arg)
{
	rsiCOLOR color;

	if (!surf)
		return rsiERR_SURFACE;

	if (!read_color(&color,(char*)arg[0]))
		return ERROR_COLOREXP;

	return rsiSetSurface(surf->item,rsiTSurfAmbient,&color,rsiTDone);
}

int f_diffuse(ULONG *arg)
{
	rsiCOLOR color;

	if (!surf)
		return rsiERR_SURFACE;

	if (!read_color(&color,(char*)arg[0]))
		return ERROR_COLOREXP;

	return rsiSetSurface(surf->item,rsiTSurfDiffuse,&color,rsiTDone);
}

int f_foglen(ULONG *arg)
{
	float fnum;

	if (!surf)
		return rsiERR_SURFACE;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(surf->item,rsiTSurfFoglen,fnum,rsiTDone);
}

int f_specular(ULONG *arg)
{
	rsiCOLOR color;

	if (!surf)
		return rsiERR_SURFACE;

	if (!read_color(&color,(char*)arg[0]))
		return ERROR_COLOREXP;

	return rsiSetSurface(surf->item,rsiTSurfSpecular,&color,rsiTDone);
}

int f_difftrans(ULONG *arg)
{
	rsiCOLOR color;

	if (!surf)
		return rsiERR_SURFACE;

	if (!read_color(&color,(char*)arg[0]))
		return ERROR_COLOREXP;

	return rsiSetSurface(surf->item,rsiTSurfDifftrans,&color,rsiTDone);
}

int f_spectrans(ULONG *arg)
{
	rsiCOLOR color;

	if (!surf)
		return rsiERR_SURFACE;

	if (!read_color(&color,(char*)arg[0]))
		return ERROR_COLOREXP;

	return rsiSetSurface(surf->item,rsiTSurfSpectrans,&color,rsiTDone);
}

int f_refexp(ULONG *arg)
{
	float fnum;

	if (!surf)
		return rsiERR_SURFACE;

	if (!arg[0])
		return ERROR_FLOATEXP;
		
	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(surf->item,rsiTSurfRefexp,fnum,rsiTDone);
}

int f_transexp(ULONG *arg)
{
	float fnum;

	if (!surf)
		return rsiERR_SURFACE;

	if (!arg[0])
		return ERROR_FLOATEXP;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(surf->item,rsiTSurfTransexp,fnum,rsiTDone);
}

int f_refrindex(ULONG *arg)
{
	float fnum;

	if (!surf)
		return rsiERR_SURFACE;

	if (!arg[0])
		return ERROR_FLOATEXP;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(surf->item,rsiTSurfRefrindex,fnum,rsiTDone);
}

int f_reflect(ULONG *arg)
{
	rsiCOLOR color;

	if (!surf)
		return rsiERR_SURFACE;

	if (!read_color(&color,(char*)arg[0]))
		return ERROR_COLOREXP;

	return rsiSetSurface(surf->item,rsiTSurfReflect,&color,rsiTDone);
}

int f_transpar(ULONG *arg)
{
	rsiCOLOR color;

	if (!surf)
		return rsiERR_SURFACE;

	if (!read_color(&color,(char*)arg[0]))
		return ERROR_COLOREXP;

	return rsiSetSurface(surf->item,rsiTSurfTranspar,&color,rsiTDone);
}

int f_transluc(ULONG *arg)
{
	float fnum;

	if (!surf)
		return rsiERR_SURFACE;

	if (!arg[0])
		return ERROR_FLOATEXP;

	fnum = (float)atof((char*)arg[0]);

	return rsiSetSurface(surf->item,rsiTSurfTransluc,fnum,rsiTDone);
}

int f_imtexture(ULONG *arg)
{
	void *texture, *actor;
	rsiVECTOR vec;
	float fnum;
	rsiResult err;

	if (!surf)
		return rsiERR_SURFACE;

	err = rsiAddTexture(&texture,(char*)arg[0],surf->item);
	if (err)
		return err;

	if (arg[1])
	{
		if (!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetTexture(texture,rsiTTexturePos,&vec,rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		if (!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetTexture(texture,rsiTTextureAlign,&vec,rsiTDone);
		if (err)
			return err;
	}

	if (arg[3])
	{
		if (!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetTexture(texture,rsiTTextureSize,&vec,rsiTDone);
		if (err)
			return err;
	}

	if (arg[4])
	{
		actor = FindActorListItem((char*)arg[20]);
		if (!actor)
			return rsiERR_ACTOR;
		err = rsiSetTexture(texture,rsiTTextureActor,actor,rsiTDone);
		if (err)
			return err;
	}
	
	if (arg[5])
	{
		fnum = (float)atof((char*)arg[5]);
		err = rsiSetTexture(texture,rsiTTexturePara1,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[6])
	{
		fnum = (float)atof((char*)arg[6]);
		err = rsiSetTexture(texture,rsiTTexturePara2,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[7])
	{
		fnum = (float)atof((char*)arg[7]);
		err = rsiSetTexture(texture,rsiTTexturePara3,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[8])
	{
		fnum = (float)atof((char*)arg[8]);
		err = rsiSetTexture(texture,rsiTTexturePara4,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[9])
	{
		fnum = (float)atof((char*)arg[9]);
		err = rsiSetTexture(texture,rsiTTexturePara5,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[10])
	{
		fnum = (float)atof((char*)arg[10]);
		err = rsiSetTexture(texture,rsiTTexturePara6,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[11])
	{
		fnum = (float)atof((char*)arg[11]);
		err = rsiSetTexture(texture,rsiTTexturePara7,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[12])
	{
		fnum = (float)atof((char*)arg[12]);
		err = rsiSetTexture(texture,rsiTTexturePara8,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[13])
	{
		fnum = (float)atof((char*)arg[13]);
		err = rsiSetTexture(texture,rsiTTexturePara9,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[14])
	{
		fnum = (float)atof((char*)arg[14]);
		err = rsiSetTexture(texture,rsiTTexturePara10,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[15])
	{
		fnum = (float)atof((char*)arg[15]);
		err = rsiSetTexture(texture,rsiTTexturePara11,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[16])
	{
		fnum = (float)atof((char*)arg[16]);
		err = rsiSetTexture(texture,rsiTTexturePara12,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[17])
	{
		fnum = (float)atof((char*)arg[17]);
		err = rsiSetTexture(texture,rsiTTexturePara13,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[18])
	{
		fnum = (float)atof((char*)arg[18]);
		err = rsiSetTexture(texture,rsiTTexturePara14,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[19])
	{
		fnum = (float)atof((char*)arg[19]);
		err = rsiSetTexture(texture,rsiTTexturePara15,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[20])
	{
		fnum = (float)atof((char*)arg[20]);
		err = rsiSetTexture(texture,rsiTTexturePara16,fnum,rsiTDone);
		if (err)
			return err;
	}
	return rsiERR_NONE;
}

int f_hypertexture(ULONG *arg)
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

   err = rsiAddHyperTexture(&texture,type,surf->item);
   if(err)
      return err;

   if(arg[1])
   {
      if(!read_vector(&vec,(char*)arg[1]))
         return ERROR_VECTOR;
      err = rsiSetHyperTexture(texture,rsiTHyperPos,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[2])
   {
      if(!read_vector(&vec,(char*)arg[2]))
         return ERROR_VECTOR;
      err = rsiSetHyperTexture(texture,rsiTHyperAlign,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[3])
   {
      if(!read_vector(&vec,(char*)arg[3]))
         return ERROR_VECTOR;
      err = rsiSetHyperTexture(texture,rsiTHyperSize,&vec,rsiTDone);
      if(err)
         return err;
   }

   if(arg[4])
   {
      fnum = (float)atof((char*)arg[4]);
      err = rsiSetHyperTexture(texture,rsiTHyperP1,fnum,rsiTDone);
      if(err)
         return err;
   }

   if(arg[5])
   {
      fnum = (float)atof((char*)arg[5]);
      err = rsiSetHyperTexture(texture,rsiTHyperP2,fnum,rsiTDone);
      if(err)
         return err;
   }

   if(arg[6])
   {
      fnum = (float)atof((char*)arg[6]);
      err = rsiSetHyperTexture(texture,rsiTHyperP3,fnum,rsiTDone);
      if(err)
         return err;
   }

   if(arg[7])
   {
      actor = FindActorListItem((char*)arg[7]);
      if(!actor)
         return rsiERR_ACTOR;
      err = rsiSetHyperTexture(texture,rsiTHyperActor,actor,rsiTDone);
      if(err)
         return err;
   }

   return rsiERR_NONE;
}

int f_brush(ULONG *arg)
{
	void *brush,*actor;
	rsiVECTOR vec;
	int flags = 0;
	rsiResult err;

	if (!surf)
		return rsiERR_SURFACE;

	err = rsiAddBrush(&brush,((char*)arg[0]),surf->item);
	if (err)
		return err;

	if (KEYWORD((char*)arg[1], "COLOR"))
		flags = rsiFBrushColor;
	else
		if (KEYWORD((char*)arg[1], "REFLECT"))
			flags = rsiFBrushReflectivity;
		else
			if (KEYWORD((char*)arg[1], "FILTER"))
				flags = rsiFBrushFilter;
			else
				if (KEYWORD((char*)arg[1], "ALTITUDE"))
					flags = rsiFBrushAltitude;
				else
					if (KEYWORD((char*)arg[1], "SPECULAR"))
						flags = rsiFBrushSpecular;
					else
						return ERROR_BRUSHTYPE;

	if (KEYWORD((char*)arg[2], "WRAPX"))
		flags |= rsiFBrushWrapX;
	else
		if (KEYWORD((char*)arg[2], "WRAPY"))
			flags |= rsiFBrushWrapY;
		else
			if (KEYWORD((char*)arg[2], "WRAPXY"))
				flags |= rsiFBrushWrapXY;
			else
				if (!KEYWORD((char*)arg[2], "FLAT"))
					return ERROR_BRUSHMETHOD;

	if (KEYWORD(arg[6], "REPEAT"))
		flags |= rsiFBrushRepeat;
	if (KEYWORD(arg[7], "MIRROR"))
		flags |= rsiFBrushMirror;
	if (KEYWORD(arg[8], "SOFT"))
		flags |= rsiFBrushSoft;

	err = rsiSetBrush(brush,rsiTBrushFlags,flags,rsiTDone);
	if (err)
		return err;

	if (!read_vector(&vec,(char*)arg[3]))
		return ERROR_VECTOR;
	err = rsiSetBrush(brush,rsiTBrushPos,&vec,rsiTDone);
	if (err)
		return err;

	if (!read_vector(&vec,(char*)arg[4]))
		return ERROR_VECTOR;
	err = rsiSetBrush(brush,rsiTBrushAlign,&vec,rsiTDone);
	if (err)
		return err;

	if (!read_vector(&vec,(char*)arg[5]))
		return ERROR_VECTOR;
	err = rsiSetBrush(brush,rsiTBrushSize,&vec,rsiTDone);
	if (err)
		return err;

	if (arg[9])
	{
		actor = FindActorListItem((char*)arg[9]);
		if (!actor)
			return rsiERR_ACTOR;
		err = rsiSetBrush(brush,rsiTBrushActor,actor,rsiTDone);
		if (err)
			return err;
	}

	return rsiERR_NONE;
}

int f_savepic(ULONG *arg)
{
	rsiResult err;

	if (!arg[0])
		return ERROR_STRINGEXP;
		
	if (arg[1])
		err = rsiSavePicture((char*)arg[0],rsiTPicType,(char*)arg[1],rsiTDone);
	else
		err = rsiSavePicture((char*)arg[0],rsiTDone);
	if (err)
		return err;

	return rsiERR_NONE;
}


int f_cleanup(ULONG *arg)
{
	rsiResult err;

	Log("Cleanup");

	err = rsiCleanup();
	if (err)
		return err;

	DestroyLists();		// free surface and actor lists
	surf = NULL;
	actor = NULL;

	return rsiERR_NONE;
}

int f_startrender(ULONG *arg)
{
	rsiResult err;
	int flags = 0;
	float begin, end;
	int left,top,right,bottom;
	void *dmy = NULL;
	BOOL res;
	rsiSMALL_COLOR *scr;

	if(insideCSG > 0)
		return ERROR_CSG;

	if (KEYWORD(arg[0], "QUICK"))
		flags |= rsiFRenderQuick;

	if (arg[1])
	{
		err = rsiSetOctreeDepth(atoi((char*)arg[1]));
		if (err)
			return err;
	}

	if (arg[2])
		begin = (float)atof((char*)arg[2]);
	else
		begin = 0.;

	if (arg[3])
		end = (float)atof((char*)arg[3]);
	else
		end = 0.;

	if (arg[4])
		left = atoi((char *)arg[4]);
	else
		left = rsiFDefault;

	if (arg[5])
		top = atoi((char *)arg[5]);
	else
		top = rsiFDefault;

	if (arg[6])
		right = atoi((char *)arg[6]);
	else
		right = rsiFDefault;

	if (arg[7])
		bottom = atoi((char *)arg[7]);
	else
		bottom = rsiFDefault;

	if (arg[8])
	{
		err = rsiSetWorld(dmy, rsiTWorldMinObjects, atoi((char *)arg[8]), rsiTDone);
		if(err)
			return err;
	}

	err = rsiSetRenderField(left,top,right,bottom);
	if (err)
		return err;

	if (scrarray)
		delete [ ] scrarray;

	err = rsiRender((UBYTE**)&scr,rsiTRenderFlags,flags,rsiTRenderTimeBegin,begin,rsiTRenderTimeEnd,end,rsiTDone);
	
	if (err)
		return err;
	
	scrarray = new rsiSMALL_COLOR[xRes*yRes];
	if(!scrarray)
		return ERROR_MEM;

	memcpy(scrarray, scr, sizeof(rsiSMALL_COLOR)*xRes*yRes);
	return err;
}

int f_antialias(ULONG *arg)
{
	rsiCOLOR color;
	float fnum;
	rsiResult err;

	if (arg[1])
	{
		fnum = (float)atof((char*)arg[1]);
		err = rsiSetAntialias(atoi((char *)arg[0]),rsiTAntiFilter,fnum,rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		if (!read_color(&color,(char*)arg[2]))
			return ERROR_COLOREXP;
		err = rsiSetAntialias(atoi((char *)arg[0]),rsiTAntiContrast,&color,rsiTDone);
		if (err)
			return err;
	}

	return rsiSetAntialias(atoi((char *)arg[0]),rsiTDone);
}


int f_brushpath(ULONG *arg)
{
	if (!arg[0])
		return ERROR_STRINGEXP;

	return rsiSetBrushPath((char*)arg[0]);
}

int f_texturepath(ULONG *arg)
{
	if (!arg[0])
		return ERROR_STRINGEXP;

	return rsiSetTexturePath((char*)arg[0]);
}

int f_objectpath(ULONG *arg)
{
	if (!arg[0])
		return ERROR_STRINGEXP;

	return rsiSetObjectPath((char*)arg[0]);
}

int f_includepath(ULONG *arg)
{
	if (!arg[0])
		return ERROR_STRINGEXP;

	strcpy(szIncludePath, (char *)arg[0]);
	
	return RETURN_OK;
}

int f_picturepath(ULONG *arg)
{
	if (!arg[0])
		return ERROR_STRINGEXP;

	strcpy(szPicturePath, (char *)arg[0]);
	
	return RETURN_OK;
}


int f_newactor(ULONG *arg)
{
	rsiVECTOR vec;
	void *act;
	rsiResult err;

	err = rsiCreateActor(&act);
	if (err)
		return err;

	if (!arg[0])
		return ERROR_STRINGEXP;

	actor = AddActorList((char*)arg[0],act);
	if (!actor)
		return rsiERR_MEM;

	if (arg[1])
	{
		if (!read_vector(&vec,(char*)arg[1]))
			return ERROR_VECTOR;
		err = rsiSetActor(act,rsiTActorPos,&vec,rsiTDone);
		if (err)
			return err;
	}
	if (arg[2])
	{
		if (!read_vector(&vec,(char*)arg[2]))
			return ERROR_VECTOR;
		err = rsiSetActor(act,rsiTActorAlign,&vec,rsiTDone);
		if (err)
			return err;
	}
	if (arg[3])
	{
		if (!read_vector(&vec,(char*)arg[3]))
			return ERROR_VECTOR;
		err = rsiSetActor(act,rsiTActorSize,&vec,rsiTDone);
		if (err)
			return err;
	}
	return rsiERR_NONE;
}

int f_position(ULONG *arg)
{
	rsiVECTOR vec;
	float begin,end;
	int flags = rsiFActionLinear;

	if (!actor)
		return rsiERR_ACTOR;

	begin = end = 0;

	if (arg[0])
		begin = (float)atof((char*)arg[0]);

	if (arg[1])
		end = (float)atof((char*)arg[1]);

	if (!read_vector(&vec,(char*)arg[2]))
		return ERROR_VECTOR;

	if (arg[3])
	{
		if (!stricmp((char*)arg[3], "LINEAR"))
			flags |= rsiFActionLinear;
		else
			if (!stricmp((char*)arg[3], "SPLINE"))
				flags |= rsiFActionSpline;
			else
				return ERROR_INTERPOL;
	}

	return rsiPosition(actor->item,begin,end,&vec,rsiTPosFlags,flags,rsiTDone);
}

int f_alignment(ULONG *arg)
{
	rsiVECTOR vec;
	float begin,end;
	int flags = rsiFActionLinear;

	if (!actor)
		return rsiERR_ACTOR;

	begin = end = 0;

	if (arg[0])
		begin = (float)atof((char*)arg[0]);

	if (arg[1])
	end = (float)atof((char*)arg[1]);

	if (!read_vector(&vec,(char*)arg[2]))
		return ERROR_VECTOR;

	if (arg[3])
	{
		if (!stricmp((char*)arg[3], "LINEAR"))
			flags |= rsiFActionLinear;
		else
			if (!stricmp((char*)arg[3], "SPLINE"))
				flags |= rsiFActionSpline;
			else
				return ERROR_INTERPOL;
	}

	return rsiAlignment(actor->item,begin,end,&vec,rsiTAlignFlags,flags,rsiTDone);
}

int f_size(ULONG *arg)
{
	rsiVECTOR vec;
	float begin,end;
	int flags = rsiFActionLinear;

	if (!actor)
		return rsiERR_ACTOR;

	begin = end = 0;

	if (arg[0])
		begin = (float)atof((char*)arg[0]);

	if (arg[1])
		end = (float)atof((char*)arg[1]);

	if (!read_vector(&vec,(char*)arg[2]))
		return ERROR_VECTOR;

	if (arg[3])
	{
		if (!stricmp((char*)arg[3], "LINEAR"))
			flags |= rsiFActionLinear;
		else
			if (!stricmp((char*)arg[3], "SPLINE"))
				flags |= rsiFActionSpline;
			else
				return ERROR_INTERPOL;
	}

	return rsiSize(actor->item,begin,end,&vec,rsiTSizeFlags,flags,rsiTDone);
}

int f_distrib(ULONG *arg)
{
	int s1 = 1, s2 = 1;

	if (arg[0])
		s1 = atoi((char *)arg[0]);

	if (arg[1])
		s2 = atoi((char *)arg[1]);

	return rsiSetDistribution(s1, s2);
}

int f_include(ULONG *arg)
{
	char buf[256];

	include++;
	if (include == MAXINCLUDE)
		return ERROR_MAXINCLUDE;
	if (!arg[0])
		return ERROR_SYNTAX;
		
	strcpy(Includefile[include].name, (char *)arg[0]);
	Includefile[include].linenumber = 0;
	if (Includefile[include].name[0])
	{
		//ExpandPath(szIncludePath, Includefile[include].name, buf);
		if (!Includefile[include].name[0])
		{
			// cannot find file
			return ERROR_READ;
		}
	}
	Includefile[include].hFile = fopen(buf, "rt");
	if (!Includefile[include].hFile)
		return ERROR_READ;

	return RETURN_OK;
}


int f_star(ULONG *arg)
{
	rsiResult err;
	void *star;
	float f, range, diff;
	float *intensities;
	int i,j,spikes, randseed, step, h;
	BOOL update;

	update = FALSE;
	
	if (!light)
		return ERROR_NOLIGHT;

	err = rsiCreateStar(light, &star);
	if (err)
		return err;

	if (arg[0])
	{
		spikes = (int)atoi((char*)arg[0]);
		update = TRUE;
	}
	else
		spikes = 4;

	err = rsiSetStar(star, rsiTStarSpikes, spikes, rsiTDone);
	if (err)
		return err;
		
	if (arg[1])
	{
		f = (float)atof((char*)arg[1])/100;
		err = rsiSetStar(star, rsiTStarRadius, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		f = (float)atof((char*)arg[2])/100;
		err = rsiSetStar(star, rsiTStarHaloRadius, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[3])
	{
		f = (float)atof((char*)arg[3])/100;
		err = rsiSetStar(star, rsiTStarInnerHaloRadius, f, rsiTDone);
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
		randseed = (int)atoi((char*)arg[5]);
		update = TRUE;
	}
	else
		randseed = 0;

	if (arg[6])
	{
		f = (float)atof((char*)arg[6]);
		err = rsiSetStar(star, rsiTStarSpikeWidth, f, rsiTDone);
		if (err)
			return err;
	}
	
	if (arg[7])
	{
		f = (float)atof((char*)arg[7]);
		err = rsiSetStar(star, rsiTStarTilt, f, rsiTDone);
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
		err = rsiSetStar(star, rsiTStarIntensities, intensities, rsiTDone);
		delete intensities;
		if (err)
			return err;
	}
	
	return rsiERR_NONE;
}

int f_flare(ULONG *arg)
{
	rsiResult err;
	rsiCOLOR color;
	void *flare;
	float f;
	int i, flags;

	if (!light)
		return ERROR_NOLIGHT;

	err = rsiCreateFlare(light, &flare);
	if (err)
		return err;

	if (arg[0])
	{
		f = (float)atof((char*)arg[0])/100;
		err = rsiSetFlare(flare, rsiTFlarePos, f, rsiTDone);
		if (err)
			return err;
	}
	if (arg[1])
	{
		f = (float)atof((char*)arg[1])/100;
		err = rsiSetFlare(flare, rsiTFlareRadius, f, rsiTDone);
		if (err)
			return err;
	}

	if (arg[2])
	{
		if (!read_color(&color, (char*)arg[2]))
			return ERROR_COLOREXP;

		err = rsiSetFlare(flare, rsiTFlareColor, &color, rsiTDone);
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
	err = rsiSetFlare(flare, rsiTFlareType, flags, rsiTDone);
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
					return ERROR_FLAREFUNC;
	}
	err = rsiSetFlare(flare, rsiTFlareFunc, flags, rsiTDone);
	if (err)
		return err;

	if (arg[5])
	{
		i = (int)atoi((char*)arg[5]);
		err = rsiSetFlare(flare, rsiTFlareEdges, i, rsiTDone);
		if (err)
			return err;
	}

	if (arg[6])
	{

		f = (float)atof((char*)arg[6])*PI/180;
		err = rsiSetFlare(flare, rsiTFlareTilt, f, rsiTDone);
		if (err)
			return err;
	}

	return rsiERR_NONE;
}

// Scan dismantels the given string <buf> into its components.
// The components are divided either by <,> or <">
// the results are saved in arg
int Scan(char *line, char *buf, ULONG *arg)
{
	int i = 0, argi, len;
	char *end, *pos;
	pos = line;
	strcpy(buf, "");
	if (!pos)
		return 0;

	argi = 0;

	while (pos[i] && (iscntrl(pos[i]) || pos[i] == ' '))	// ignore spaces
		i++;
	if (!pos[i] || pos[i] == '\n' || (pos[i] == '/' && pos[i + 1] == '/'))	
		return 1;								// return if empty line or comment

	// read as long as there are arguments
	while (!(!pos[i] || pos[i] == '\n' || (pos[i] == '/' && pos[i + 1] == '/')))
	{
		while (pos[i] && (iscntrl(pos[i]) || pos[i] == ' '))	// ignore spaces
			i++;

		if (pos[i] == ',' && argi > 0)
		{
			i++;
			while (pos[i] && (iscntrl(pos[i]) || pos[i] == ' '))	// ignore spaces
				i++;
		}
		
		if (pos[i] == '<')
			end = strpbrk(&pos[i+1], ">\n") + 1;
		else
			if (pos[i] == '"')
				end = strpbrk(&pos[++i], "\"\n") + 1;
			else
		    	end = strpbrk(&pos[i], ", \"\n");

		if (end)
			len = int(end - &pos[i]);
		else
			len = strlen(&pos[i]);

		if (argi == 0)
		{
			strncpy(buf, &pos[i], len);
			buf[len] = '\0';
		}
		else
		{
			if (len > 0)
			{
				arg[argi - 1] = (ULONG)(new char[len + 1]);
				strncpy((char *)arg[argi - 1], &pos[i], len);
				((char *)(arg[argi - 1]))[len] = 0;
				if (pos[i + len - 1] == '"')
					((char *)(arg[argi - 1]))[len - 1] = 0;  // that's SOOO dirty!!!
			}
		}
		i+= len;
		argi++;
	}
	return 0;
}

void GetLine(char **prog, char *line, int *linenumber)
{
	int nLength;
	char *end;
	
	// are we including a file ?
	if (include)
	{
		if (feof(Includefile[include].hFile))
		{
			fclose(Includefile[include].hFile);
			include--;
			(*linenumber)++;
		}
	}
	if (include)
	{
		fgets(line, 256, Includefile[include].hFile);
		Includefile[include].linenumber++;
	}
	else
	{
		end = strpbrk(*prog, "\n\r\0");
		if (end)
			nLength = int(end - *prog);
		else
			nLength = strlen(*prog);
		memcpy(line, *prog, nLength);
		*prog+= nLength;
		while (**prog == '\n' || **prog == '\r')
		{
			if (**prog == '\n')
				(*linenumber)++;
			(*prog)++;
		}
		line[nLength] = '\0'; 
	}
}


DWORD WINAPI Execute(LPDWORD data)
{
	int i, linenumber;
	char line[256], buf[256], logbuf[100], *prog;
	BOOL error = FALSE;

	obj = NULL;
	light = NULL;

	ULONG arg[MAXARGS];

	for (i = 0; i < MAXARGS; i++)
		arg[i] = NULL;

	prog = ((THREAD_DATA *)data)->szProg;
	pDisplay = ((THREAD_DATA *)data)->pDisplay;
	strcpy(szIncludePath, "");
	strcpy(szPicturePath, "");

	err = rsiSetLogCB(Log);
	if (err)
	{
		rsiGetErrorMsg(buf,err);
		Log(buf);
	}

	err = rsiSetUpdateStatusCB(UpdateStatus);
	if (err)
	{
		rsiGetErrorMsg(buf,err);
		Log(buf);
	}

	err = rsiSetCheckCancelCB(CheckCancel);
	if (err)
	{
		rsiGetErrorMsg(buf,err);
		Log(buf);
	}

	err = rsiSetCooperateCB(Cooperate);
	if (err)
	{
		rsiGetErrorMsg(buf,err);
		Log(buf);
	}

	linenumber = 0;
	include = 0;

	while ((*prog || include > 0) && !error)
	{ 
		GetLine(&prog, line, &linenumber);
		// read command first
		if (!Scan(line, buf, arg))
		{
			i = 0;
			// find command in list	
  			while (commands[i].name && stricmp(buf, commands[i].name))
  		 		i++;
				
			// call corresponding function with "arg" as argument
			if (commands[i].func)
				error = commands[i].func(arg);
			else
				error = ERROR_UNKNOWNCOM;
				
			for (i = 0; i < MAXARGS; i++)
			{
				if (arg[i])
					delete (ULONG *)arg[i];
				arg[i] = NULL;
			}

			if (error)
			{
				if (include)
				{
					sprintf(logbuf, "In include file: \"%s\"", Includefile[include].name);
					pDisplay->Log(logbuf);
					linenumber = Includefile[include].linenumber;
				}
				sprintf(logbuf, "Error executing command in line %i", linenumber);
				pDisplay->Log(logbuf);
				pDisplay->Log(line);
				GetError(logbuf, error);
				pDisplay->Log(logbuf);
			}
		}
	}
	if (error)
	{
		pDisplay->Log("Error occurred");
		pDisplay->Log("Cancel execution");
		// close open include files
		while (include > 0)
		{
			if (Includefile[include].hFile)
				fclose(Includefile[include].hFile);
			include--;
		}
	}
	else
	{
		if (pDisplay->CheckCancel())
			pDisplay->Log("Execution canceled");
		else
			pDisplay->Log("Execution successful");
	}

	return TRUE;
}


