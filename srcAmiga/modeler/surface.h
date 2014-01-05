/***************
 * PROGRAM:       Modeler
 * NAME:          surface.h
 * DESCRIPTION:   definition of surface class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.01.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 *    03.05.97 ah    changed the way the surfaces are handled; they
 *       are now stored in a list and objects share one surface
 *       the surface name is now unique
 *    19.06.97 ah    added WriteObjectSurface()
 ***************/

#ifndef SURFACE_H
#define SURFACE_H

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef IFF_UTIL_H
#include "iff_util.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

#ifndef RSI_H
#include "rsi.h"
#endif

class SURFACE;

#ifdef __AMIGA__
#include "MUIdefs.h"
#endif

class BRUSH;
class TEXTURE;
class OBJECT;

#define SURF_DEFAULTNAME   "Default"

#define SURF_BRIGHT        0x01
#define SURF_EXTERNAL      0x02     // surface for an external object
#define SURF_WRITTEN       0x04     // surface is written to external object
#define SURF_DEFAULT       0x08     // this surface is the default one for each new created object

class SURFACE : public SLIST  // surface
{
	public:
		char *name;             // surfacename
		TEXTURE *texture;       // texturelist (NULL if none)
		BRUSH *brush;           // brushlist (NULL if none)
		ULONG flags;            // surface flags: BRIGHT
		COLOR ambient;          // ambient
		COLOR diffuse;          // diffuse
		COLOR specular;         // specular
		COLOR reflect;          // reflectivity
		COLOR transpar;         // transparency
		COLOR difftrans;        // Diffuse transmission 'curve'
		COLOR spectrans;        // Specular transmission 'curve'
		float refphong;         // reflected Phong coef
		float transphong;       // transmitted Phong coef
		float foglength;        // fog length
		float refrindex;        // index of refr
		float translucency;     // translucency
#ifdef __AMIGA__
		Object *win;            // the requester for this surface
#else
		CDialog *win;
#endif

		SURFACE();
		~SURFACE();
		void Add();
		void Remove();
		BOOL SetName(char *);
		char *GetName() { return name; }
		BOOL Write(struct IFFHandle *);
		BOOL Read(struct IFFHandle*, OBJECT*);
		void AddBrush(BRUSH *);
		void AddTexture(TEXTURE *);
		virtual rsiResult ToRSI(rsiCONTEXT*, void **,MATRIX_STACK*,OBJECT*);
		virtual SURFACE *Duplicate();
		char *Save(char*);
		char *Load(char*);
		void ConvertV100(OBJECT*);
		SURFACE *ConvertPreV200(OBJECT*);
		int GetSize();
		SURFACE *FindIdentical();
};

void FreeSurfaces();
SURFACE *GetSurfaceByName(char*);
char *ParseSurfaces(struct IFFHandle*);
BOOL WriteSurfaces(struct IFFHandle*);
BOOL ReadSurface(struct IFFHandle*, SURFACE**, OBJECT*);
BOOL WriteObjectSurface(struct IFFHandle*, OBJECT*);
void SurfaceResetWritten();

#endif /* SURFACE_H */
