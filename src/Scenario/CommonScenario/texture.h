/***************
 * PROGRAM:       Modeler
 * NAME:          texture.h
 * DESCRIPTION:   definition of texture class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    03.07.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 *    09.06.97 ah    added TEXTURE_OBJECT
 *    11.05.98 ah    added HYPER_TEXTURE
 ***************/

#ifndef TEXTURE_H
#define TEXTURE_H

class SURFACE;
#ifndef OBJECT_H
#include "object.h"
#endif

enum TEXTURE_TYPES
{
	TEXTURE_IMAGINE,
	TEXTURE_RAYSTORM,
	TEXTURE_HYPER
};

class TEXTURE : public SLIST
{
	public:
		char *name;             // texture name

		TEXTURE();
		virtual ~TEXTURE();
		BOOL SetName(char*);
		char *GetName() { return name; };
		int GetSize();

		virtual BOOL Write(struct IFFHandle*) = 0;
		virtual TEXTURE *DupObj() = 0;
		virtual TEXTURE_TYPES GetType() = 0;
		virtual rsiResult ToRSI(rsiCONTEXT*, void**, void*) = 0;
};

class IMAGINE_TEXTURE : public TEXTURE
{
	public:
		float params[16];

		BOOL Write(struct IFFHandle*);
		TEXTURE *DupObj();
		TEXTURE_TYPES GetType() { return TEXTURE_IMAGINE; };
		virtual rsiResult ToRSI(rsiCONTEXT*, void**, void*);
};

class RAYSTORM_TEXTURE : public TEXTURE
{
	public:
		void *data;
		ULONG datasize;

		RAYSTORM_TEXTURE();
		~RAYSTORM_TEXTURE();
		BOOL Write(struct IFFHandle*);
		TEXTURE *DupObj();
		TEXTURE_TYPES GetType() { return TEXTURE_RAYSTORM; };
		virtual rsiResult ToRSI(rsiCONTEXT*, void**, void*);
};

class HYPER_TEXTURE : public TEXTURE
{
	public:
		void *data;
		ULONG datasize;

		HYPER_TEXTURE();
		~HYPER_TEXTURE();
		BOOL Write(struct IFFHandle*);
		TEXTURE *DupObj();
		TEXTURE_TYPES GetType() { return TEXTURE_HYPER; };
		virtual rsiResult ToRSI(rsiCONTEXT*, void**, void*);
};

class TEXTURE_OBJECT : public OBJECT
{
	public:
		TEXTURE *texture;

		TEXTURE_OBJECT();
		virtual void CalcBBox();
		virtual OBJECT *DupObj();
		virtual rsiResult ToRSI(rsiCONTEXT*, void*,OBJECT*,MATRIX_STACK*);
		virtual int GetType() { return OBJECT_TEXTURE; }
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetSize();
		virtual BOOL SetName(char *name) { return texture->SetName(name); }
		virtual char *GetName() { return texture->GetName(); }
		virtual void SetDrawColor(DISPLAY*);
		virtual BOOL IsVisible(DISPLAY*);
#ifdef __AMIGA__
		virtual struct NewMenu *GetPopUpList();
#endif
};

TEXTURE *ParseHyperTexture(struct IFFHandle *, SURFACE *);
TEXTURE *ParseRayStormTexture(struct IFFHandle *, SURFACE *);
TEXTURE *ParseImagineTexture(struct IFFHandle *, SURFACE *);
TEXTURE_OBJECT *ParseTextureObject(struct IFFHandle *, OBJECT *);

#endif /* TEXTURE_H */
