/***************
 * MODUL:         surface
 * NAME:          surface.h
 * DESCRIPTION:   definition of surface class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.02.95 ah    initial release
 *    15.03.95 ah    - reflect and transpar are now of
 *                   type COLOR
 *                   - texture is now a list
 *    22.03.95 ah    - added brushlist
 *    03.04.95 ah    added Insert() and FreeList()
 *    16.08.95 ah    added SURFACE.flags
 *       added foglength
 *    12.03.97 ah    added hypertextures
 ***************/

#ifndef SURFACE_H
#define SURFACE_H

class SURFACE;
class TEXTURE;
class HYPERTEXTURE;
class BRUSH;

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

class RSICONTEXT;

#define BRIGHT    1

class SURFACE : public LIST   // surface
{
	public:
		TEXTURE *texture;       // texturelist (NULL if none)
		HYPERTEXTURE *hypertexture;   // hypertexturelist (NULL if none)
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

		SURFACE();
		~SURFACE();
		void Insert(RSICONTEXT*);
		void AddBrush(BRUSH*);
		void AddTexture(TEXTURE*);
		void AddHyperTexture(HYPERTEXTURE *);
};

#endif /* SURFACE_H */
