// general #defines

#ifndef TRUE
#define TRUE	1L
#define FALSE	0L
#endif
#ifndef	NULL
#define	NULL	0L
#endif

// generalized C/C++ types

// note: BYTE, WORD, and BOOL are defined in Windows header files.

typedef long LONG;
typedef unsigned long ULONG;
typedef short SHORT;
typedef unsigned short USHORT;
typedef	unsigned char	UBYTE;

typedef unsigned int UINT;
typedef	unsigned char	*STRPTR;
#if 1
typedef	void	*APTR;
#else
typedef	STRPTR	*APTR;
#endif

// list & node types/functions

// list & node structure

typedef struct MinNode {
	struct MinNode	*mln_Succ;
	struct MinNode	*mln_Pred;
}	MNODE;

typedef struct MinList {
	MNODE	*mlh_Head;
	MNODE	*mlh_Tail;
	MNODE	*mlh_TailPred;
}	MLIST;

#define	TXTF_RED	1	// unused
#define	TXTF_GRN	2	// unused
#define	TXTF_BLU	4	// unused
#define	TXTF_SCL	8
// TXTF_UNUSED is used to turn off the string gad display for unused data
#define	TXTF_UNUSED	16	// unused
// same as TXTF_UNUSED plus txtrdata is left justified
#define	TXTF_LUNUSED	32	// unused
// TXTF_AUTOSCALE is used to scale the texture's axis with respect to 
// the object's bounding box as follows:
// if set in pflags[0],  autoscale texture in x
// if set in pflags[1],  autoscale texture in y
// if set in pflags[2],  autoscale texture in z
#define	TXTF_AUTOSCALE	128	// unused

#define	Fix(X)		((LONG)((X)*65536.0))

#ifdef _MSC_VER		// Microsoft compiler version
#define	RAND15()	rand()
#define	SRAND15(X)	srand(X)
#else
#define	RAND15()	(rand()>>16)
#define	SRAND15(X)	(srand(X))
#endif

typedef	struct _vector {
	float	x;
	float	y;
	float	z;
}	IM_VECTOR;

typedef	struct _matrix {
	IM_VECTOR	I;
	IM_VECTOR	J;
	IM_VECTOR	K;
}	IM_MATRIX;

typedef	struct _axes	{
	IM_VECTOR	r;
	IM_MATRIX	m;
}	AXES;

typedef	struct _tform	{
	IM_VECTOR	r;
	IM_VECTOR	a;
	IM_VECTOR	b;
	IM_VECTOR	c;
	IM_VECTOR	s;
}	TFORM;

struct _coords {
	LONG	wx;
	LONG	wy;
	LONG	wz;
	LONG	px;
	LONG	py;
	LONG	pz;
	LONG	pyx;
	LONG	pyz;
};
typedef	struct _coords	COORDS;

#define	FSIZE	((LONG)sizeof(float))
#define	VSIZE	((LONG)sizeof(IM_VECTOR))
#define	MSIZE	((LONG)sizeof(IM_MATRIX))
#define	TSIZE	((LONG)sizeof(TFORM))
#define	CSIZE	((LONG)sizeof(COORDS))

typedef	struct _gfxdata {
	SHORT	width;
	SHORT	height;
	SHORT	xaspect;
	SHORT	yaspect;
	UBYTE	*r;
	UBYTE	*g;
	UBYTE	*b;
	UBYTE	*gen;

	MLIST	*objlist;

	void	*(*allocmem)(ULONG, ULONG);
	void	(*freemem)(void *, ULONG);
	void	(*copymem)(APTR, APTR, ULONG);
	void	(*notify)(const char *,...);

}	GFXDATA;

// Object flags (OBJECT.Flags) used in Imagine

#define	OBJF_PERSPVIEW	0x0001
#define	OBJF_FRONTVIEW	0x0002
#define	OBJF_RIGHTVIEW	0x0004
#define	OBJF_TOPVIEW	0x0008
#define	OBJF_ALLVIEWS	0x000f
#define	OBJF_PICKED	0x0010
#define	OBJF_SELECTED	0x0020
#define	OBJF_QUICKED	0x0040	// "quick" edge flag - for obj_eflags
#define	OBJF_TEMP	0x0080	// temporary flag - for objects
#define	OBJF_SHARP	0x0080

#define	OBJF_POINTS	0x0100
#define	OBJF_QUICK	0x0200
#define	OBJF_CUBED	0x0400
#define	OBJF_REBUILD	0x0800
#define	OBJF_BBOXED	0x1000

#define	OBJF_BLUE	0x0080

// Object grouping (joint) flags

#define	OBJGF_NORMAL	0
#define	OBJGF_HINGE	1
#define	OBJGF_BALL	2
#define	OBJGF_LEGAL3_0	0x0003

// SUBOBJ.sbj_shape #defines ... this should be in object.h
// objects
#define	SHAP_SPHERE	0
#define	SHAP_AXIS	2
#define	SHAP_FACETS	3	// not legal in TDDD files
#define	SHAP_GROUND	5
// internal use only
#define	SHAP_IGNORE	1	// not legal in TDDD files
#define	SHAP_PFACET	4	// not legal in TDDD files
#define	SHAPM_SUBTYPE	7	// used in 'hitobj()', 'spot()', etc.
// camera & "staging" light sources -- also used in staging file

// ... old (2.0 -- only used internally)
//	#define	SHAP_SPHERICAL	0x0100
//	#define	SHAP_CYLINDRICAL	0x0200
//	#define	SHAP_CONICAL	0x0400
//

#define	SHAP_SPHERICAL	0x0100		// unshaped, point light source
#define	SHAP_CONICAL	0x0200		// round, point light source
#define	SHAP_PYRAMID	0x0300		// rectangular, point light source
#define	SHAP_PLANAR		0x0500		// unshaped, parallel light source
#define	SHAP_CYLINDRICAL	0x0600	// round, parallel light source
#define	SHAP_BOX		0x0700		// rect., parallel light source

#define	SHAP_CAMERA	0x0800
// masks to identify (staging) lights or camera & (staging) lights
#define	SHAPM_LITES	0x0700
#define	SHAPM_CAMLITES	0x0f00
// "shape type" mask to ignore path flags & "reserved" flags
#define	SHAPM_TYPE	0x0fff
// paths
#define	SHAPF_PATH	0x1000
#define	SHAPF_CLOSED	0x2000
// flags used in raytrace code
#define	SHAPF_AGAIN	0x4000
#define	SHAPF_CHECKED	0x8000

// SUBOBJ.sbj_lamp #defines
#define	LITEF_POINTSRC		0x0001	// point source  -- one of these
#define	LITEF_PARALLEL		0x0002	// parallel rays -- must be set
#define	LITEF_ISLAMP		0x0003

#define	LITEF_NOSHAPE		0x0000	// no shape
#define	LITEF_ROUND			0x0004	// round shape
#define	LITEF_RECTANGULAR	0x0008	// rectangular shape
#define	LITEF_SHAPEMASK		0x000c	// mask for above

#define	LITEF_NOFLARE		0x0010	// flag for lens flare global F/X

#define	LITEF_NOFALLOFF		0x0000	// no falloff
#define	LITEF_DIMINISH		0x0020	// (1/R) diminishing
#define	LITEF_CONTROLLED	0x0040	// controlled falloff
#define	LITEF_RESERVED		0x0060	// reserved falloff type
#define	LITEF_FALLOFFMASK	0x0060	// mask for above

#define	LITEF_SHADOWS		0x0080	// casts shadows

#define	LITEF_BRIGHTOBJ		0x8000	// bright object

// forms "flags"
#define	FORM_VFORM	1
#define	FORM_TWOF	0
#define	FORM_ONEF	2
#define	FORM_ONES	4
#define	FORM_TYPE	6

// OBJECT macros ...

#define	OBJAXES(O)	((AXES *)&(O)->obj_object.sbj_r)
#define	OBJPOSN(O)	(&(O)->obj_object.sbj_r)
#define	OBJMTRX(O)	((IM_MATRIX *)&(O)->obj_object.sbj_a)

#define	PDAXES(PD)	((AXES *)&(PD)->r)
#define	PDPOSN(PD)	(&(PD)->r)
#define	PDMTRX(PD)	((IM_MATRIX *)&(PD)->a)

#define	ADAXES(AD)	((AXES *)&(AD)->r)
#define	ADPOSN(AD)	(&(AD)->r)
#define	ADMTRX(AD)	((IM_MATRIX *)&(AD)->a)

#define	BRAXES(BR)	((AXES *)&(BR)->tform.r)
#define	TXAXES(TX)	((AXES *)&(TX)->tform.r)

// Zero check macros - look a little nicer in "if" conditions

#define	ISZERO(EXPR)	((EXPR)==0)
#define	ISNULL(EXPR)	((EXPR)==0)
#define	ISNONZ(EXPR)	((EXPR)!=0)

// this disables the warning about "possible loss of data"
// generated whenever a double is stored in a float variable
// without an explicit type cast to float.
#ifdef _MSC_VER		// Microsoft compiler version
#pragma warning(disable:4244)
#endif

// structures used in rendering

typedef struct _rgbfp {
	float	r;
	float	g;
	float	b;
}	RGBFP;

typedef struct _ray {
	IM_VECTOR	m_base;			// ray origin
	IM_VECTOR	m_unit;			// ray direction
}	RAY;

typedef struct _patch {
	IM_VECTOR	ptc_pos;		// position - read only

	IM_VECTOR	ptc_nor;		// normal vector
	RGBFP	ptc_col;		// color    (RGB)
	RGBFP	ptc_ref;		// reflect  (RGB)
	RGBFP	ptc_tra;		// filter  (RGB)
	RGBFP	ptc_spc;		// specular (RGB)

	USHORT	ptc_shp;		// copy 'sbj_shape' & flags - read only
	USHORT	ptc_shd;		// flag - object can shadow itself - read only
	float	ptc_c1;			// 1st barycentric coord. - read only
	float	ptc_c2;			// 2nd barycentric coord. - read only
	RAY		*ptc_rayptr;	// - read only
	float	ptc_raydst;		// - read only

	float	ptc_foglen;
	float	ptc_shiny;		// new 3.1
	float	ptc_hard;
	float	ptc_index;
	float	ptc_bright;
	void	*ptc_txdata;	// points to 16 bytes for use by textures
	float	ptc_rough;		// new 3.3
	RGBFP	ptc_brlite;		// new 3.3

}	PATCH;

typedef	struct _subobj	{
	USHORT	sbj_shape;
	USHORT	sbj_lamp;
	struct _object	*sbj_parent;
	ULONG	sbj_number;
	IM_VECTOR	sbj_r;
	IM_VECTOR	sbj_a;
	IM_VECTOR	sbj_b;
	IM_VECTOR	sbj_c;
	IM_VECTOR	sbj_s;
}	SUBOBJ;

typedef	struct _ogfx {
	COORDS	*ogfx_points;
	COORDS	ogfx_parent;
	COORDS	ogfx_object;
	COORDS	ogfx_axisa;
	COORDS	ogfx_axisb;
	COORDS	ogfx_axisc;
	COORDS	ogfx_cube[8];
	COORDS	ogfx_texta;
	COORDS	ogfx_textb;
	COORDS	ogfx_textc;
}	OGFX;

#define	NUM_OCS	15

#define	NUM_IOBJ_PROPS	8

#define	IPRP_DITHER	0	// for old (pre 3.3) TDDD files (PRP1 chunk)
#define	IPRP_BRIGHT	0	// for 3.3+
#define	IPRP_HARD	1
#define	IPRP_ROUGH	2
#define	IPRP_SHINY	3
#define	IPRP_INDEX	4
#define	IPRP_QUICK	5
#define	IPRP_PHONG	6
#define	IPRP_GENLOCK	7

#define	IPRPF_PHONG	1
#define	IPRPF_PARTICLES	2

typedef struct _pthd {
	IM_VECTOR	r;
	IM_VECTOR	a;
	IM_VECTOR	b;
	IM_VECTOR	c;
	IM_VECTOR	s;
	USHORT	infrom;
	USHORT	outto;
	USHORT	flags;
	USHORT	extracnt;
}	PTHD;

#define	ASIZE	((LONG)sizeof(PTHD))

#define	PTHF_NEWPATH	0x01
#define	PTHF_CONNECTIN	0x02
#define	PTHF_CONNECTOUT	0x04
#define	PTHF_REVERSE	0x40
#define	PTHF_SHARP	0x80

typedef	struct _ford {
	SHORT	numc;
	SHORT	numf;
	SHORT	flags;
	SHORT	nums;
	IM_MATRIX	tform;
	IM_VECTOR	xlate;
	IM_VECTOR	*points;
	USHORT	*sections;
}	FORD;

typedef	struct _auxf {
	IM_MATRIX	tform;
	IM_VECTOR	xlate;
}	AUXF;

typedef	struct _subgrp	{
	MNODE	node;
	char	name[18];
	USHORT	count;
	USHORT	*list;
	USHORT	ptype;
	USHORT	pseed;
	float	psize;
	char	*pfname;
}	SUBGRP;

typedef struct _object
{
	MNODE	obj_node;
	struct _object	*obj_parent;
	MLIST	obj_children;
	MNODE	obj_link;
	SUBOBJ	obj_object;
	UBYTE	obj_props[NUM_IOBJ_PROPS];
	char	obj_name[18];
	USHORT	obj_flags;
	USHORT	obj_acount;
	USHORT	obj_pcount;
	USHORT	obj_ecount;
	USHORT	obj_fcount;
	PTHD	*obj_pathdata;
	IM_VECTOR	*obj_points;
	USHORT	*obj_edges;
	USHORT	*obj_faces;
	UBYTE	*obj_pflags;
	UBYTE	*obj_eflags;
	UBYTE	*obj_fflags;
	UBYTE	obj_diffuse[4];			// 4 bytes (0,R,G,B)
	UBYTE	obj_reflect[4];			// 4 bytes (0,R,G,B)
	UBYTE	obj_transmit[4];		// 4 bytes (0,R,G,B)
	UBYTE	obj_specular[4];		// 4 bytes (0,R,G,B)
	IM_VECTOR	obj_intensity;
	UBYTE	*obj_dlist;
	UBYTE	*obj_rlist;
	UBYTE	*obj_tlist;
	SUBOBJ	*obj_subjects;
	IM_VECTOR	*obj_phongs;

	MLIST	obj_txbrobj;
	MLIST	obj_txbrtree;
	UBYTE	obj_txbrpad[16];

	MLIST	obj_subgrps;
	float	obj_foglen;

	float	obj_bounds[6];
	OGFX	*obj_ogfx;
	struct _pface	*obj_pgfx;
	FORD	*obj_ford;
	IM_VECTOR	*obj_auxpoints;
	TFORM	*obj_auxtform;
	AUXF	*obj_auxf;
	PTHD	*obj_auxpthd1;
	PTHD	*obj_auxpthd2;

	UBYTE	obj_txbrpad2[32];

	struct _stgobj	*obj_stgobj;
	SHORT	obj_animpad;
	SHORT	obj_animflags;
	MLIST	obj_animdata;

	USHORT	obj_ptype;
	USHORT	obj_pseed;
	float	obj_psize;
	ULONG	obj_sbjcnt;

	MLIST	obj_statedata;

	USHORT	obj_grpflags;
	USHORT	obj_frzflags;

	UBYTE	obj_txbrpad3[4];

	IM_VECTOR	*obj_pts0;
	TFORM	*obj_axes0;
	IM_MATRIX	*obj_alt0;
	IM_VECTOR	*obj_phong0;

	char	*obj_pfname;

	USHORT	obj_quality;
	USHORT	obj_quality2;
	USHORT	obj_quality3;
	USHORT	obj_quality4;

	float	obj_ang1;
	float	obj_ang2;
	float	obj_ang3;
	float	obj_ang4;

	AXES	obj_jbase;

	char	obj_bbsg[18];
	char	obj_sbsg[18];

} OBJECT;

#define	MAXPOINTS	0x7fffL
#define	MAXEDGES	0x7fffL
#define	MAXFACES	0x7fffL
#define	MAXKNOTS	0x7fffL

#define	LINKOBJ(LINKPTR)	((OBJECT *)((UBYTE *)(LINKPTR)-24))

#define	OSIZE	((LONG)sizeof(OBJECT))

// #include "objstate.h"

// "particle" flags

	// type values - bits 0-3 of 'ptype's

#define	PTF_FCE		0x0000
#define	PTF_TET		0x0001
#define	PTF_PYR		0x0002
#define	PTF_OCT		0x0003
#define	PTF_CUB		0x0004
#define	PTF_BLK		0x0005
#define	PTF_DOD		0x0006
#define	PTF_SPH		0x0007
#define	PTF_RND		0x0008
#define	PTF_FILE	0x0009

#define	PTF_MASK	0x000f

	// size values - bits 8-11 of 'ptype's

#define	PSF_SMALL	0x0000
#define	PSF_LARGE	0x0100
#define	PSF_RANDOM	0x0200
#define	PSF_SPECIFY	0x0300

#define	PSF_MASK	0x0f00

	// centering values - bits 4-7 of 'ptype's

#define	PCF_INSC	0x0000
#define	PCF_CIRC	0x0010
#define	PCF_INTR	0x0020
#define	PCF_HOLO	0x0030

#define	PCF_MASK	0x00f0

	// alignment values - bits 12-15 of 'ptype's

#define	PAF_OBJ		0x0000
#define	PAF_FACE	0x1000
#define	PAF_RANDOM	0x2000

#define	PAF_MASK	0xf000

// lighting stuff ...

typedef	struct _litedata {
	RGBFP	intensity;	// light source intensities
	float	falloff;	// distance falloff factor
	TFORM	*tf;		// world coords of texture axes
	IM_VECTOR	basepoint;	// local (TXTR axis) coords
	IM_VECTOR	direction;	// local (TXTR axis) coords
	float	distance;	// distance to target
	IM_VECTOR	target;		// local (TXTR axis) coords

// ... more ...

	SUBOBJ	*lightsource;	// light source SUBOBJ
	PATCH	*spatch;	// pointer to surface patch
	float	rdotp;		// dotvec(viewint way, surf. normal)
	float	ldotp;		// dotvec(lighting way, surf. normal)
//	RAY	lray;			// lighting ray
}	LTDATA;
