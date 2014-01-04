/***************
 * MODUL:         LWOB handler (Lighwave Object File Format)
 * NAME:          lwob.h
 * DESCRIPTION:   In this module are all the definitions to read LWOB-files
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    24.09.97 ah    initial release
 ***************/

#ifndef LWOB_H
#define LWOB_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#define ID_LWOB MAKE_ID('L','W','O','B')  // LightWave object
#define ID_PNTS MAKE_ID('P','N','T','S')  // points
#define ID_SRFS MAKE_ID('S','R','F','S')  // surface names
#define ID_POLS MAKE_ID('P','O','L','S')  // polygons
#define ID_SURF MAKE_ID('S','U','R','F')  // surface attributes
#define ID_COLR MAKE_ID('C','O','L','R')  // color
#define ID_FLAG MAKE_ID('F','L','A','G')  // flag
#define ID_LUMI MAKE_ID('L','U','M','I')  // luminosity
#define ID_DIFF MAKE_ID('D','I','F','F')  // diffuse
#define ID_SPEC MAKE_ID('S','P','E','C')  // specular
#define ID_REFL MAKE_ID('R','E','F','L')  // reflection
#define ID_TRAN MAKE_ID('T','R','A','N')  // transparency
#define ID_GLOS MAKE_ID('G','L','O','S')  // glossiness
#define ID_RIMG MAKE_ID('R','I','M','G')  // reflection image
#define ID_RSAN MAKE_ID('R','S','A','N')  // reflection map seam heading angle
#define ID_RIND MAKE_ID('R','I','N','D')  // refractive index
#define ID_EDGE MAKE_ID('E','D','G','E')  // edge transparency threshold
#define ID_SMAN MAKE_ID('S','M','A','N')  // smooth shaded angle
#define ID_CTEX MAKE_ID('C','T','E','X')  // color texture
#define ID_DTEX MAKE_ID('D','T','E','X')  // diffuse texture
#define ID_STEX MAKE_ID('S','T','E','X')  // specular texture
#define ID_RTEX MAKE_ID('R','T','E','X')  // reflection texture
#define ID_TTEX MAKE_ID('T','T','E','X')  // transparency texture
#define ID_BTEX MAKE_ID('B','T','E','X')  // bump texture
#define ID_TIMG MAKE_ID('T','I','M','G')  // texture mapping image
#define ID_TFLG MAKE_ID('T','F','L','G')  // texture flag
#define ID_TWRP MAKE_ID('T','W','R','P')  // texture wrap type
#define ID_TSIZ MAKE_ID('T','S','I','Z')  // texture size
#define ID_TCTR MAKE_ID('T','C','T','R')  // texture center
#define ID_TFAL MAKE_ID('T','F','A','L')  // texture falloff
#define ID_TVEL MAKE_ID('T','V','E','L')  // texture velocity
#define ID_TCLR MAKE_ID('T','C','L','R')  // texture color
#define ID_TVAL MAKE_ID('T','V','A','L')  // texture value
#define ID_TAMP MAKE_ID('T','A','M','P')  // texture amplitude (bump)
#define ID_TFRQ MAKE_ID('T','F','R','Q')  // texture noise frequencies
#define ID_TSP0 MAKE_ID('T','S','P','0')  // texture parameter 0
#define ID_TSP1 MAKE_ID('T','S','P','1')  // texture parameter 1
#define ID_TSP2 MAKE_ID('T','S','P','2')  // texture parameter 2

#endif // LWOB_H
