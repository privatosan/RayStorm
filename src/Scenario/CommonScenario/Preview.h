/***************
 * PROGRAM:       Modeler
 * NAME:          preview.h
 * DESCRIPTION:   definition of preview class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    03.11.96 ah    initial release
 *    16.11.96 ah    added texture support (1.1)
 ***************/

#ifndef PREVIEW_H
#define PREVIEW_H

#ifdef __AMIGA__
#include <dos/dos.h>
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef RTEXTURE_H
#include "rtexture.h"
#endif

// The PATCH structure contains all the (ray) hit point info.
// This structure is passed to a texture function each time a
// ray hits the object with a texture on it.
typedef struct
{
	VECTOR   ptc_pos;    // global hit point - read only
	VECTOR   ptc_nor;    // surface normal (must be normalized)
	COLOR    ptc_col;    // surface color (R,G,B) at hit point
	COLOR    ptc_ref;    // surface color (R,G,B) at hit point
	COLOR    ptc_tra;    // surface color (R,G,B) at hit point
	COLOR    ptc_spc;    // surface color (R,G,B) at hit point - read only
	UWORD    ptc_shp;    // copy of SHAP flags - read only - see TDDD.DOC
	UWORD    ptc_shd;    // flag - obj can shadow itself - read only
	float    ptc_pc0;
	float    ptc_pc1;
	VECTOR   *ptc_ray;   // position/exit direction of camera ray - read only
	float    raydist;
	float    foglen;     // surface foglength
} IM_PATCH;

// The IM_TTABLE{} structure is used as a communication link between
// Imagine and the run-time loadable texture modules.  This structure is
// initialized and passed back to imagine by the entry module
// (texture_init()).
typedef struct
{
	LONG     id;            // version number identifier
	void     (*init)();     // reserved - curently unused
	void     (*cleanup)();  // reserved - curently unused
#ifdef __AMIGA__
	void     (*work)(float*, IM_PATCH*, VECTOR*, float*);
#else
#ifdef __WATCOMC__
	void  __cdecl (*work)(float*, IM_PATCH*, VECTOR*, float*);
#else
	void (*work)(float*, IM_PATCH*, VECTOR*, float*);
#endif
#endif
									// hook to the texture algorithm (the "guts")
	BYTE     **infotext;    // pointer to text fields for requester
	UBYTE    *infoflags;    // pointer to data field flags for requester
	float    *params;       // pointer to data values for requester
	float    *tform;        // pointer to texture axis geometry info
} IM_TTABLE;

typedef struct
{
	VECTOR pos;
	VECTOR orient_x, orient_y, orient_z;
	VECTOR size;
} TFORM;

class PREVIEW_TEXTURE;

class PREVIEW
{
	private:
		int xres, yres;
		int lines;
		VECTOR light;
		PREVIEW_TEXTURE *texture_root;
	public:
		SURFACE *surf;
		COLOR ambient;
		COLOR diffuse; 
		COLOR specular;
		COLOR reflect;
		COLOR transpar;
		COLOR difftrans;
		COLOR spectrans;
		float refphong;
		float transphong;
		float foglength;
		float refrindex;
		float translucency;

		void CalcPixel(int, int, VECTOR*);

	public:
		rsiSMALL_COLOR *line;

		PREVIEW();
		~PREVIEW();
		BOOL Init(int, int, int, SURFACE *, OBJECT*);
		void RenderLines(int);
		rsiSMALL_COLOR *GetLine() { return line; }
};

#ifdef __MIXEDBINARY__
PREVIEW *PreviewCreate();
void PreviewDelete(PREVIEW*);
BOOL Init(PREVIEW*, int, int, int, SURFACE *, OBJECT*);
void RenderLines(PREVIEW*, int);
rsiSMALL_COLOR *GetLine(PREVIEW*);
#endif

class PREVIEW_TEXTURE : public SLIST
{
	public:
		VECTOR size;
		VECTOR orient_x,orient_y,orient_z;
		VECTOR pos;
		MATRIX m;

		virtual ~PREVIEW_TEXTURE() { }
		virtual void Work(PREVIEW *, VECTOR *, VECTOR *) = 0;
		virtual BOOL Load(TEXTURE*) = 0;
};

class IPREVIEW_TEXTURE : public PREVIEW_TEXTURE
{
	public:
		IM_PATCH patch;      // patch-structure
		TFORM form;
		float params[16];
#ifdef __AMIGA__
		BPTR     seglist;    // segment list of Imagine texture file
#else
		HINSTANCE hInst;
#endif /* __AMIGA__ */
		IM_TTABLE *ttable;   // pointer to ttable-structure

		IPREVIEW_TEXTURE();
		virtual ~IPREVIEW_TEXTURE();
		void Work(PREVIEW *, VECTOR *, VECTOR *);
		BOOL Load(TEXTURE*);
};

class RPREVIEW_TEXTURE : public PREVIEW_TEXTURE
{
	public:
#ifdef __AMIGA__
		struct Library *Base;
#else // __AMIGA__
		HINSTANCE hLib;
#endif // __AMIGA__
		RAYSTORM_TEXTURE *texture;
		TEXTURE_INFO *tinfo;

		RPREVIEW_TEXTURE();
		virtual ~RPREVIEW_TEXTURE();
		void Work(PREVIEW *, VECTOR *, VECTOR *);
		BOOL Load(TEXTURE*);
};

#ifdef __MIXEDBINARY__
void PPCTextureCleanup(void (*cleanup)(_tinfo *tinfo), TEXTURE_INFO*, struct Library*);
extern "C" void PPCRTextureWork(TEXTURE_INFO *tinfo, void *data, TEXTURE_PATCH *ptch, VECTOR *relpos);
TEXTURE_INFO *stub_texture_init(char*, struct Library**);
TEXTURE_TYPES GetType(TEXTURE*);
TEXTURE_OBJECT *GetNext(TEXTURE_OBJECT*);
TEXTURE *GetNext(TEXTURE*);
void ITextureCleanup(IM_TTABLE *ttable);
IM_TTABLE *ITextureInit(IM_TTABLE* (*texture_init)(LONG arg0));
void ITextureWork(IM_TTABLE *ttable, float *a, IM_PATCH *b, VECTOR *c, float *d);
#endif


#endif /* PREVIEW_H */
