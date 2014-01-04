/***************
 * PROGRAM:       Modeler
 * NAME:          brush.h
 * DESCRIPTION:   definition of brush class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.06.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 *    03.06.97 ah    added BRUSH_OBJECT
 ***************/

#ifndef BRUSH_H
#define BRUSH_H

class SURFACE;
#ifndef OBJECT_H
#include "object.h"
#endif

// brush type
enum
{
	BRUSH_MAP_COLOR,
	BRUSH_MAP_REFLECTIVITY,
	BRUSH_MAP_FILTER,
	BRUSH_MAP_ALTITUDE,
	BRUSH_MAP_SPECULAR
};

// wrap method
enum
{
	BRUSH_WRAP_FLAT,
	BRUSH_WRAP_X,
	BRUSH_WRAP_Y,
	BRUSH_WRAP_XY
};

#define BRUSH_SOFT   0x01
#define BRUSH_REPEAT 0x02
#define BRUSH_MIRROR 0x04

class BRUSH : public SLIST
{
	public:
		char *file;             // file name

		ULONG type;             // brush type
		ULONG wrap;             // wrap type
		ULONG flags;            // various flags

		BRUSH();
		~BRUSH();
		BOOL Write(struct IFFHandle*);
		BOOL SetName(char*);
		char *GetName() { return file; };
		int GetSize();
		BRUSH *DupObj();
};

class BRUSH_OBJECT : public OBJECT
{
	public:
		BRUSH *brush;

		BRUSH_OBJECT();
		virtual void CalcBBox();
		virtual OBJECT *DupObj();
		virtual rsiResult ToRSI(rsiCONTEXT*, void*,OBJECT*,MATRIX_STACK*);
		virtual int GetType() { return OBJECT_BRUSH; }
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetSize();
		virtual BOOL SetName(char *name) { return brush->SetName(name); }
		virtual char *GetName() { return brush->GetName(); }
		virtual void SetDrawColor(DISPLAY*);
		virtual BOOL IsVisible(DISPLAY*);
#ifdef __AMIGA__
		virtual struct NewMenu *GetPopUpList();
#endif
};

BRUSH *ParseBrush(struct IFFHandle*, SURFACE*);
BRUSH_OBJECT *ParseBrushObject(struct IFFHandle*, OBJECT*);

#endif /* BRUSH_H */
