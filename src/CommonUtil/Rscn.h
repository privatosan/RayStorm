/***************
 * PROGRAM:       Modeler
 * NAME:          rscn.h
 * DESCRIPTION:   Definitions for RayStorm scene files
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    18.06.96 ah    initial release
 *    18.11.96 ah    added star and flare chunks
 *    27.11.96 ah    added ID_IHRD and removed ID_TIGH
 *    11.01.97 ah    added box chunks
 *    06.03.97 ah    added ID_SMSZ
 *    25.03.97 ah    added cylinder chunk
 *    25.03.97 ah    added cone chunk
 *    13.05.97 ah    added ID_SRFS
 ***************/

#ifndef RSCN_H
#define RSCN_H

#ifdef __AMIGA__
	#ifndef IFF_IFFPARSE_H
	#include <libraries/iffparse.h>
	#endif
#else
	#include "iffparse.h"
#endif

#define VERSION      210                     // current version of RSCN-file
#define RSOB_VERSION 200                     // current version of RSOB-file

// often used chunks
#define ID_NAME MAKE_ID('N','A','M','E')     // name
#define ID_COLR MAKE_ID('C','O','L','R')     // color
#define ID_FLGS MAKE_ID('F','L','G','S')     // flags
#define ID_TRCK MAKE_ID('T','R','C','K')     // track object
#define ID_RADI MAKE_ID('R','A','D','I')     // radius
#define ID_FLEN MAKE_ID('F','L','E','N')     // fog length

// main chunks
#define ID_RSCN MAKE_ID('R','S','C','N')     // format identifier (RayStorm scene)
#define ID_VERS MAKE_ID('V','E','R','S')     // version of the file

// general chunks
#define ID_GNRL MAKE_ID('G','N','R','L')     // general data
#define ID_RESO MAKE_ID('R','E','S','O')     // resolution
#define ID_PICT MAKE_ID('P','I','C','T')     // picture name
#define ID_PTYP MAKE_ID('P','T','Y','P')     // picture type
#define ID_DISP MAKE_ID('D','I','S','P')     // display mode (BBOX, WIRE, SOLID)
#define ID_VIEW MAKE_ID('V','I','E','W')     // v2.1 or higher: number of current view (before: viewmode (FRONT, RIGHT, ...))
#define ID_GRID MAKE_ID('G','R','I','D')     // grid size
#define ID_OBSV MAKE_ID('O','B','S','V')     // observer position
#define ID_FHGT MAKE_ID('F','H','G','T')     // fog height
#define ID_FCOL MAKE_ID('F','C','O','L')     // fog color
#define ID_FILD MAKE_ID('F','I','L','D')     // rendering field
#define ID_MOBJ MAKE_ID('M','O','B','J')     // minimum octree objects
#define ID_FILT MAKE_ID('F','I','L','T')     // filter flags (FILTER_???)
#define ID_HSAM MAKE_ID('H','S','A','M')     // hyper texture samples
#define ID_HMIN MAKE_ID('H','M','I','N')     // minimum hyper texture delta density
#define ID_HMAX MAKE_ID('H','M','A','X')     // maximum hyper texture delta density

// frame chunks
#define ID_FRAM MAKE_ID('F','R','A','M')     // frame data
#define ID_TIME MAKE_ID('T','I','M','E')     // time slice
#define ID_AMBT MAKE_ID('A','M','B','T')     // global ambient color
#define ID_ANTC MAKE_ID('A','N','T','C')     // antialiasing contrast
#define ID_ANTS MAKE_ID('A','N','T','S')     // antialiasing samples
#define ID_BCKC MAKE_ID('B','C','K','C')     // background color
#define ID_BCKP MAKE_ID('B','C','K','P')     // background picture path
#define ID_REFM MAKE_ID('R','E','F','M')     // global reflection path
#define ID_SOFT MAKE_ID('S','O','F','T')     // softshadow samples
#define ID_MBLR MAKE_ID('M','B','L','R')     // motion blur samples
#define ID_ODPT MAKE_ID('O','D','P','T')     // octree depth

// surfaces chunk
#define ID_SRFS MAKE_ID('S','R','F','S')     // surfaces data

// object chunks
#define ID_HIER MAKE_ID('H','I','E','R')     // start of new hierarchy level
#define ID_POSI MAKE_ID('P','O','S','I')     // position
#define ID_ALGN MAKE_ID('A','L','G','N')     // alignment
#define ID_SIZE MAKE_ID('S','I','Z','E')     // size

// camera chunks
#define ID_CAMR MAKE_ID('C','A','M','R')     // header of camera chunk
#define ID_FDST MAKE_ID('F','D','S','T')     // focal distance
#define ID_APER MAKE_ID('A','P','E','R')     // aperture
#define ID_FDOV MAKE_ID('F','D','O','V')     // horizontal and vertical field of view

// light chunks
#define ID_PLGT MAKE_ID('P','L','G','T')     // header of pointlight chunk
#define ID_DIST MAKE_ID('D','I','S','T')     // falloff distance
#define ID_SLGT MAKE_ID('S','L','G','T')     // header of spotlight chunk
#define ID_ANGL MAKE_ID('A','N','G','L')     // opening angle
#define ID_FALL MAKE_ID('F','A','L','L')     // light intensity if full a falloff_radius*angle and zero at > angle
#define ID_SMSZ MAKE_ID('S','M','S','Z')     // size of shadowmap
#define ID_DLGT MAKE_ID('D','L','G','T')     // header of directional light chunk

// light flare chunks
#define ID_FLAR MAKE_ID('F','L','A','R')     // header of flare chunk
#define ID_TYPE MAKE_ID('T','Y','P','E')     // type of flare (disc=0, poly=1)
#define ID_FUNC MAKE_ID('F','U','N','C')     // function of flare (plane, ring, ...)
#define ID_TILT MAKE_ID('T','I','L','T')     // tilt angle of lare if type is poly

// light star chunks
#define ID_STAR MAKE_ID('S','T','A','R')     // header of light star chunk
#define ID_SRAD MAKE_ID('S','R','A','D')     // star radius in percent of screen width
#define ID_SPIK MAKE_ID('S','P','I','K')     // amount of spikes
#define ID_HRAD MAKE_ID('H','R','A','D')     // halo radius in percent of spike radius
#define ID_IHRD MAKE_ID('I','H','R','D')     // inner halo radius in percent of halo radius
#define ID_RANG MAKE_ID('R','A','N','G')     // star range in percent of screen width
#define ID_INTE MAKE_ID('I','N','T','E')     // spike intensities
#define ID_RCOL MAKE_ID('R','C','O','L')     // ring color
#define ID_BRIG MAKE_ID('B','R','I','G')     // brightness
#define ID_NOIS MAKE_ID('N','O','I','S')     // noise

// plane chunks
#define ID_PLAN MAKE_ID('P','L','A','N')     // header of plane chunk

// sphere chunks
#define ID_SPHR MAKE_ID('S','P','H','R')     // header of sphere chunk

// box chunks
#define ID_BOX  MAKE_ID('B','O','X',' ')     // header of box chunk
#define ID_LBND MAKE_ID('L','B','N','D')     // low boundaries (left, front, bottom edge)
#define ID_HBND MAKE_ID('H','B','N','D')     // high boundaries (right, back, top edge)

// cylinder chunks
#define ID_CYLR MAKE_ID('C','Y','L','R')     // header of cylinder chunk

// cone chunks
#define ID_CONE MAKE_ID('C','O','N','E')     // header of cone chunk

// csg chunks
#define ID_CSG  MAKE_ID('C','S','G',' ')     // header of csg chunk
#define ID_OPER MAKE_ID('O','P','E','R')     // csg operator

// mesh chunks
#define ID_MESH MAKE_ID('M','E','S','H')     // header of mesh chunk
#define ID_PNTS MAKE_ID('P','N','T','S')     // points
#define ID_EDGE MAKE_ID('E','D','G','E')     // edges
#define ID_FACE MAKE_ID('F','A','C','E')     // faces

// sor chunks
#define ID_SOR  MAKE_ID('S','O','R',' ')     // header of sor chunk

// external object chunks
#define ID_EXTN MAKE_ID('E','X','T','N')     // header of external object
#define ID_FILE MAKE_ID('F','I','L','E')     // name of external object

// surface chunks
#define ID_SURF MAKE_ID('S','U','R','F')     // header of surface chunk
#define ID_AMBT MAKE_ID('A','M','B','T')     // ambient color
#define ID_DIFU MAKE_ID('D','I','F','U')     // diffuse color
#define ID_SPEC MAKE_ID('S','P','E','C')     // specular color
#define ID_REFL MAKE_ID('R','E','F','L')     // reflective color
#define ID_TRNS MAKE_ID('T','R','N','S')     // transparent color
#define ID_DIFT MAKE_ID('D','I','F','T')     // diffuse transmission
#define ID_SPCT MAKE_ID('S','P','C','T')     // specular transmission
#define ID_RPHG MAKE_ID('R','P','H','G')     // reflected phong coef
#define ID_TPHG MAKE_ID('T','P','H','G')     // transmitted phong coef
#define ID_IXOR MAKE_ID('I','X','O','R')     // index of refraction
#define ID_TNSL MAKE_ID('T','N','S','L')     // translucency

// brush chunks
#define ID_BRSH MAKE_ID('B','R','S','H')     // header of brush chunk
#define ID_WRAP MAKE_ID('W','R','A','P')     // wrap method
#define ID_TYPE MAKE_ID('T','Y','P','E')     // brush type

// Imagine texture chunks
#define ID_ITXT MAKE_ID('I','T','X','T')     // header of Imagine texture chunk
#define ID_PARM MAKE_ID('P','A','R','M')     // parameters

// RayStorm texture chunks
#define ID_RTXT MAKE_ID('R','T','X','T')     // header of RayStorm texture chunk

// Hyper texture chunks
#define ID_HTXT MAKE_ID('H','T','X','T')     // header of hyper texture chunk

// frame flags
#define RSCN_FRAME_QUICK         0x01
#define RSCN_FRAME_RANDJIT       0x02

// general chunk flags
#define RSCN_GENERAL_GRID        0x01
#define RSCN_GENERAL_GRIDSNAP    0x02
#define RSCN_GENERAL_VIEWCAMERA  0x04  // obsolete with v2.1
#define RSCN_GENERAL_VIEWFOUR    0x08
#define RSCN_GENERAL_SHOW        0x10
#define RSCN_GENERAL_ENABLEAREA  0x20
#define RSCN_GENERAL_ENABLEBACK  0x40
#define RSCN_GENERAL_ENABLEREFL  0x80

// csg operations
#define RSCN_CSG_UNION           0x00
#define RSCN_CSG_INTERSECTION    0x01
#define RSCN_CSG_DIFFERENCE      0x02

/* edge flags */
#define RSCN_EDGE_SHARP          0x0001

/* brush flags */
#define RSCN_BRUSH_MAP_COLOR        0x000000
#define RSCN_BRUSH_MAP_REFLECTIFITY 0x000001
#define RSCN_BRUSH_MAP_FILTER       0x000002
#define RSCN_BRUSH_MAP_ALTITUDE     0x000003
#define RSCN_BRUSH_MAP_SPECULAR     0x000004
#define RSCN_BRUSH_MAP_REFLECTION   0x000005
#define RSCN_BRUSH_WRAP_X           0x010000
#define RSCN_BRUSH_WRAP_Y           0x020000
#define RSCN_BRUSH_WRAP_XY          0x030000
#define RSCN_BRUSH_REPEAT           0x080000
#define RSCN_BRUSH_MIRROR           0x100000
#define RSCN_BRUSH_SOFTBRUSH        0x200000

#define RSCN_BRUSH_MAP_MASK         0x000007
#define RSCN_BRUSH_WRAP_MASK        0x030000

/* surface flags */
#define RSCN_SURFACE_BRIGHT        1

/* object flags */
#define RSCN_OBJECT_INVERTED           0x00001
#define RSCN_OBJECT_NODEOPEN           0x00002
#define RSCN_OBJECT_OPENTOP            0x00004
#define RSCN_OBJECT_OPENBOTTOM         0x00008
#define RSCN_OBJECT_MESH_NOPHONG       0x00010
#define RSCN_OBJECT_SOR_ACCURATE       0x00020
#define RSCN_OBJECT_EXTERNAL_APPLY     0x00040
#define RSCN_OBJECT_LIGHT_SHADOW       0x00080
#define RSCN_OBJECT_LIGHT_FLARES       0x00100
#define RSCN_OBJECT_LIGHT_STAR         0x00200
#define RSCN_OBJECT_LIGHT_SHADOWMAP    0x00400
#define RSCN_OBJECT_LIGHT_TRACKFALLOFF 0x00800
#define RSCN_OBJECT_CAMERA_VFOV        0x01000
#define RSCN_OBJECT_CAMERA_FOCUSTRACK  0x02000
#define RSCN_OBJECT_CAMERA_FASTDOF     0x04000

// star flags
#define RSCN_STAR_ENABLE               0x00001
#define RSCN_STAR_HALOENABLE           0x00002
#define RSCN_STAR_RANDOM               0x00004

//------------------------
// surface definition file
//------------------------
#define ID_RMAT MAKE_ID('R','M','A','T')     // format identifier (RayStorm material)

//----------------------
// ligth definition file
//----------------------
#define ID_RLIG MAKE_ID('R','L','I','G')     // format identifier (RayStorm light)

//------------------------
// object file
//------------------------
#define ID_RSOB MAKE_ID('R','S','O','B')     // format identifier (RayStorm object)

#endif
