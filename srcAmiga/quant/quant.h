/***************
 * MODUL:         quant
 * NAME:          quant.h
 * VERSION:       1.0 04.05.1995
 * DESCRIPTION:   defintions for quantize modul
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:         all
 * HISTORY:       DATE		NAME	COMMENT
 *						02.02.95	ah		first release
 *						04.05.95	ah		bumped version to V1 (1.0)
 ***************/

#ifndef QUANT_H
#define QUANT_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

typedef unsigned char gray;
#define MAXMAXVAL 255

typedef gray pixval;

typedef struct
{
    pixval r, g, b;
} pixel;

//typedef struct box* box_vector;
struct box
{
	int ind;
	int colors;
	int sum;
};

// maximal number of colors
#define MAXCOLORS 32767

#define LARGE_NORM
#define REP_AVERAGE_COLORS

#define FS_SCALE 1024

#define HASH_SIZE 20023

#define GETR(p) ((p).r)
#define GETG(p) ((p).g)
#define GETB(p) ((p).b)

#define hashpixel(p) ((((long) GETR(p) * 33023 + (long) GETG(p) * 30013 + (long) GETB(p) * 27011 ) & 0x7fffffff ) % HASH_SIZE )

#define ASSIGN(p,red,grn,blu) (p).r = (red); (p).g = (grn); (p).b = (blu)
#define EQUAL(p,q) ((p).r == (q).r && (p).g == (q).g && (p).b == (q).b)

#define LUMIN(p) (0.299*GETR(p) + 0.587*GETG(p) + 0.114*GETB(p))

#define DEPTH(newp,p,oldmaxval,newmaxval) \
	ASSIGN((newp), \
	((int)GETR(p)*(newmaxval) + (oldmaxval)/2)/(oldmaxval), \
	((int)GETG(p)*(newmaxval) + (oldmaxval)/2)/(oldmaxval), \
	((int)GETB(p)*(newmaxval) + (oldmaxval)/2)/(oldmaxval))

typedef struct colorhist_item* colorhist_vector;
struct colorhist_item
{
	pixel color;
	int value;
};

typedef struct colorhist_list_item* colorhist_list;
typedef colorhist_list* colorhash_table;
struct colorhist_list_item
{
	struct colorhist_item ch;
	colorhist_list next;
};

void quant(int, int, int, pixel**, colorhist_item*, char);

#endif /* QUANT_H */
