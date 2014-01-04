/***************
 * MODUL:         polysolv
 * NAME:          polysolv.h
 * DESCRIPTION:   polynom solv definitions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    31.03.97 ah    initial release
 ***************/

#ifndef POLYSOLV_H
#define POLYSOLV_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

int SolveCubic(double*, double*);
int SolveQuadratic(double*, double*);
int PolySolve(int, double*, double*);

#endif // POLYSOLV_H
