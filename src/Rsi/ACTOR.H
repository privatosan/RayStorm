/***************
 * MODUL:         actor
 * NAME:          actor.h
 * DESCRIPTION:   This file includes the memberfunctions of the actor-class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    23.08.95 ah    initial release
 *    27.08.95 ah    added TIME, ACTION, POS, ALIGNMENT, DIMENSION
 *    28.08.95 ah    added FindScaleAction(), FindRotateAction(), FindTransAction(),
 *       Animate(), AnimateAll(), matrix for ACTOR
 *    29.08.95 ah    added FindAction(), FindActionBefor()
 *    02.09.95 ah    added ACTOR::time
 *    21.10.95 ah    added precalculated matrices
 *    11.01.97 ah    added rotmatrix
 *    06.07.97 ah    added invmatrix
 ***************/

#ifndef ACTOR_H
#define ACTOR_H

#ifndef LIST_H
#include "list.h"
#endif

#ifndef VOXEL_H
#include "voxel.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

class RSICONTEXT;

// Time code
typedef struct
{
	float begin,end;     // begin and end time code
} TIME;

#define ACTION_LINEAR   1     // linear interpolation
#define ACTION_SPLINE   2     // spline interpolation

class ACTION : public SLIST    // action
{
	public:
		TIME time;              // time code
		UWORD flags;            // flags

		ACTION();
		ACTION* FindAction(const float );
		ACTION* FindActionBefore(const float );
};

class POS : public ACTION  // position
{
	public:
		VECTOR pos;             // end range position

		POS();
		void FreeAll();
};

class ALIGNMENT : public ACTION  // alignment
{
	public:
		VECTOR align;           // end range alignment

		ALIGNMENT();
		void FreeAll();
};

class DIMENSION : public ACTION     // size
{
	public:
		VECTOR size;            // end range size

		DIMENSION();
		void FreeAll();
};

class PRE_MATRIX : public SLIST   // precalculated matrix
{
	public:
		TIME time;              // matrix time
		MATRIX matrix;          // matrix
		MATRIX invmatrix;       // inverted matrix
		MATRIX rotmatrix;       // rotation matrix

		void FindMatrix(MATRIX **, MATRIX **, MATRIX **, const TIME *time);
		void FreeAll();
};

class ACTOR : public SLIST     // actor
{
	public:
		PRE_MATRIX *pre_mat;    // precalculated matrices
		POS *pos;               // positions
		ALIGNMENT *align;       // alignments
		DIMENSION *size;        // sizes
		VECTOR axis_pos;        // axis position,
		VECTOR axis_align;      //      alignment,
		VECTOR axis_size;       //      size
		VECTOR act_pos;         // current position,
		VECTOR act_align;       //        alignment,
		VECTOR act_size;        //        size
		MATRIX *matrix;         // animation matrix
		MATRIX *invmatrix;      // invertet animation matrix
		MATRIX *rotmatrix;      // rotation matrix
		TIME time;              // current time intervall

		ACTOR();
		~ACTOR();
		void Insert(RSICONTEXT*);
		void CalcVoxel(VOXEL *, const WORLD *);
		void AddPos(POS *pos);
		void AddAlign(ALIGNMENT *align);
		void AddSize(DIMENSION *size);
		void AddPreMatrix(PRE_MATRIX *);
		void Animate(const TIME *);
		void FindSize(VECTOR *, const float);
		void FindAlignment(VECTOR *, const float);
		void FindPosition(VECTOR *, const float);
		void CalcMatrix(MATRIX *, MATRIX *, MATRIX *, const float);
	private:
		void MaxVoxel(VECTOR *, VOXEL *);
};

float CalcTime(int, const WORLD *);
BOOL PreCalcMatrices(RSICONTEXT*, const TIME *);

#endif /* ACTOR_H */
