/***************
 * PROGRAM:       Modeler
 * NAME:          light.h
 * DESCRIPTION:   light class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.04.96 ah    initial release
 *    20.11.96 ah    added flag LIGHT_TRANSTRACK
 *    18.12.96 ah    added GetSize()
 *    20.02.97 ah    added LIGHT_TRACKFALLOFF
 *    06.03.97 ah    added LIGHT_SHADOWMAP and shadowmapsize
 ***************/

#ifndef LIGHT_H
#define LIGHT_H

#ifndef OBJECT_H
#include "object.h"
#endif

class FLARE;
class STAR;

// light
class LIGHT : public OBJECT
{
	public:
		float r;          // radius of light
		float falloff;    // distance where the brightness of the light is 0
		COLOR color;      // color of light
		FLARE *flares;    // pointer to lens flares
		STAR  *star;      // pointer to light burn
		ULONG shadowmapsize;// size of shadowmap

		LIGHT();
		~LIGHT();

		virtual void SetDrawColor(DISPLAY*);
		virtual BOOL WriteLight(struct IFFHandle *);
		virtual int GetSize();
#ifdef __AMIGA__
		virtual struct NewMenu *GetPopUpList();
#else
		virtual int GetResourceID();
#endif
		void DrawLensEffects(DISPLAY *, MATRIX_STACK *);
		BOOL DuplicateLensEffects(LIGHT*);
		BOOL SetDefaultFlares(FLARE **);
		char *Load(char*);
		char *Save(char*);
		virtual BOOL IsVisible(DISPLAY*);
};

class POINT_LIGHT : public LIGHT
{
	public:
		POINT_LIGHT();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_POINTLIGHT; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual void CalcBBox();
};

class SPOT_LIGHT : public LIGHT
{
	public:
		float angle;      // opening angle
		float falloff_radius;    // light intensity is full at falloff_radius*angle and zero at angle

		SPOT_LIGHT();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_SPOTLIGHT; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual void CalcBBox();
};

class DIRECTIONAL_LIGHT : public LIGHT
{
	public:
		DIRECTIONAL_LIGHT();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_DIRECTIONALLIGHT; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual void CalcBBox();
};


// light settings window

OBJECT *ParseLight(struct IFFHandle *, OBJECT *, int, ULONG);

#endif /* LIGHT_H */
