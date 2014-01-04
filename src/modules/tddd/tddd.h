/***************
 * MODUL:         TDDD handler (Imagine 3.0 Object File Format)
 * NAME:          tddd.h
 * DESCRIPTION:   In this module are all the definitions to read TDDD-files
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:         none
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.01.96 ah    initial release
 ***************/

#ifndef TDDD_H
#define TDDD_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#define ID_TDDD MAKE_ID('T','D','D','D')
#define ID_OBJ MAKE_ID('O','B','J',' ')
#define ID_DESC MAKE_ID('D','E','S','C')
#define ID_TOBJ MAKE_ID('T','O','B','J')

#define ID_NAME MAKE_ID('N','A','M','E')
#define ID_SHP2 MAKE_ID('S','H','P','2')
#define ID_POSI MAKE_ID('P','O','S','I')
#define ID_AXIS MAKE_ID('A','X','I','S')
#define ID_SIZE MAKE_ID('S','I','Z','E')
#define ID_BBOX MAKE_ID('B','B','O','X')
#define ID_STND MAKE_ID('S','T','N','D')
#define ID_STID MAKE_ID('S','T','I','D')
#define ID_STDT MAKE_ID('S','T','D','T')
#define ID_PNTS MAKE_ID('P','N','T','S')
#define ID_EDGE MAKE_ID('E','D','G','E')
#define ID_FACE MAKE_ID('F','A','C','E')
#define ID_PTH2 MAKE_ID('P','T','H','2')
#define ID_COLR MAKE_ID('C','O','L','R')
#define ID_REFL MAKE_ID('R','E','F','L')
#define ID_TRAN MAKE_ID('T','R','A','N')
#define ID_SPC1 MAKE_ID('S','P','C','1')
#define ID_CLST MAKE_ID('C','L','S','T')
#define ID_RLST MAKE_ID('R','L','S','T')
#define ID_TLST MAKE_ID('T','L','S','T')
#define ID_TXT3 MAKE_ID('T','X','T','3')
#define ID_BRS5 MAKE_ID('B','R','S','5')
#define ID_BRS4 MAKE_ID('B','R','S','4')
#define ID_FOGL MAKE_ID('F','O','G','L')
#define ID_PRP1 MAKE_ID('P','R','P','1')
#define ID_INT1 MAKE_ID('I','N','T','1')
#define ID_ANID MAKE_ID('A','N','I','D')
#define ID_FOR2 MAKE_ID('F','O','R','2')
#define ID_PART MAKE_ID('P','A','R','T')
#define ID_PTFN MAKE_ID('P','T','F','N')
#define ID_FGR2 MAKE_ID('F','G','R','2')
#define ID_BBSG MAKE_ID('B','B','S','G')
#define ID_SBSG MAKE_ID('S','B','S','G')
#define ID_EFLG MAKE_ID('E','F','L','G')

// This are defines for old imagine files
#define ID_SHAP MAKE_ID('S','H','A','P')
#define ID_TXT4 MAKE_ID('T','X','T','4')
#define ID_BRS3 MAKE_ID('B','R','S','3')

typedef LONG FRACT;

typedef struct
{
	UWORD begin;            // point numbers of first
	UWORD end;              // and second edge point
} EDGE;

// Edge flags
#define EDGE_SHARP   0x80
#define EDGE_QUICK   0x40

typedef struct
{
	UWORD p1;               // edge numbers of
	UWORD p2;               // the three edges of
	UWORD p3;               // the triangle
} FACE;

// 1/65536
#define FRACT2FLOAT 1.525878906e-5

// Imagine fixpoint Vector
typedef struct
{
	LONG x,y,z;
} VECTOR_I;

typedef struct
{
	UWORD Flags;            // texture flags:
									//   Bit  0       - apply to child objs
									//   Bits 1 thru 15 - reserved
	VECTOR_I pos;
	VECTOR_I x_axis;
	VECTOR_I y_axis;
	VECTOR_I z_axis;
	VECTOR_I size;          // local coordinates of texture axes.
	LONG  Params[16];       // texture parameters
	UBYTE PFlags[16];       // parameter flags for texture requester
									//   Bit 0 - alter red color in color chip
									//   Bit 1 - alter green color in color chip
									//   Bit 2 - alter blue color in color chip
									//   Bit 3 - alter parameter when resizing object
									//   Bits 4 thru 7 - reserved
	BYTE  SubGr[18];        // Subrgoup to restrict texture to
	BYTE  LockState[18];    // State name for "texture tacking"
	UBYTE Length;           // length of texture file name
} IMAGINE_TEXTURE;

typedef struct
{
	UWORD   Flags;          // brush type:
									//    0 - Color
									//    1 - Reflectivity
									//    2 - Filter
									//    3 - Altitude
									//    4 - Reflection
	UWORD   WFlags;         // brush wrapping flags:
									//    Bit 0 - wrap x
									//    Bit 1 - wrap z
									//    Bit 2 - apply to children
									//    Bit 3 - repeat brush
									//    Bit 4 - mirror with repeats
									//    Bit 5 - inverse video
									//    Bit 6 - Use genlock
	VECTOR_I pos;
	VECTOR_I x_axis;
	VECTOR_I y_axis;
	VECTOR_I z_axis;
	VECTOR_I size;
	UWORD FullScale;        // full scale value
	UWORD MaxSeq;           // highest number for sequenced brushes
	BYTE SubGr[18];         // Subrgoup to restrict brush to
	BYTE LockState[18];     // Brush lockstate
	UBYTE Length;           // length of brush file name
} IMAGINE_BRUSH;

typedef struct
{
	IMAGINE_BRUSH brush;
	BYTE pad[29];
	UBYTE Length;
} IMAGINE_BRUSH5;

typedef struct
{
	VECTOR_I x_axis;
	VECTOR_I y_axis;
	VECTOR_I z_axis;
} IMAGINE_AXIS;

typedef struct
{
	UBYTE dither;           // dithering factor (0-255)
	UBYTE hardness;         // hardness factor (0-255)
	UBYTE roughness;        // roughness factor (0-255)
	UBYTE shinyness;        // shinyness factor (0-255)
	UBYTE index;            // index of refraction - ir = (float)index / 100.0 + 1.0;
	UBYTE quickdraw;        // quickdraw type: 0=none, 1=bounding box, 2=quick edges
	UBYTE phong;            // Phong shading on/off
	UBYTE genlock;          // Genlock on/off
} IMAGINE_PROPS;

// defines for converting longword RGB to float RGB
// format of long: 00RRGGBB
// 00FFFFFF = 1. 1. 1.
#define CONVR(a) (((a)>>16)&0xff)
#define CONVG(a) (((a)>>8)&0xff)
#define CONVB(a) ((a)&0xff)

// for SHAP- and SHP2-chunk
#define SHAPE_SPHERE    0x0000
#define SHAPE_AXIS      0x0002
#define SHAPE_GROUND    0x0005
#define LAMP_BRIGHT     0x8000

// Brush flags
#define IM_MAP_COLOR          0x0000
#define IM_MAP_REFLECTIFITY   0x0001
#define IM_MAP_FILTER         0x0002
#define IM_MAP_ALTITUDE       0x0003
#define IM_MAP_REFLECTION     0x0004

#define IM_WRAP_X       0x0001
#define IM_WRAP_Y       0x0002
#define IM_WRAP_XY      0x0003

#define IM_WRAP_MASK    0x0003

#define IM_APPLY        0x0004
#define IM_REPEAT       0x0008
#define IM_MIRROR       0x0010
#define IM_INVERSE      0x0020
#define IM_GENLOCK      0x0040

#endif // TDDD_H
