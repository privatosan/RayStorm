/***************
 * MODUL:         vecmath
 * NAME:          vecmath.h
 * DESCRIPTION:   vecmath definitions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 ***************/

#ifndef VECMATH_H
#define VECMATH_H

#include <math.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifdef __cplusplus
#ifndef LIST_H
#include "list.h"
#endif
#endif

#define PI_180    0.0174532925199f
#define INV_PI_180      57.2957795128f
#ifndef PI
#define PI        3.141592653589f
#endif
#ifndef PI_2
#define PI_2      1.5707963267945f
#endif
#define TWO_PI_3  2.0943951023931954923084f
#define TWO_PI_43 4.1887902047863909846168f
#define INV_PI    0.318309886182f
#define INV_TWOPI 0.159154943091f

#define LNHALF    (-.69314718f)

#define EPSILON   ((float)1E-6f)     /* epsilon */
#define INFINITY  ((float)1E17f)     /* infinity */

#ifndef MIN
#define MIN(x,y)  (((x)<(y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)  (((x)>(y)) ? (x) : (y))
#endif

typedef struct                      /* vector 3d */
{
	float x, y, z;
} VECTOR;

typedef struct                      /* point 2d */
{
	float x, y;
} VECT2D;

extern const VECTOR minVector;
extern const VECTOR maxVector;

#ifdef __cplusplus
class MATRIX
{
	public:
		float m[16];

		void IdentityMatrix();
		void MultMat(MATRIX *ma,MATRIX *mb);
		void SetRotXMatrix(float theta);
		void SetRotYMatrix(float theta);
		void SetRotZMatrix(float theta);
		void SetTransMatrix(const VECTOR *d);
		void SetScaleMatrix(const VECTOR *s);
		void SetSRTMatrix(const VECTOR *,const VECTOR *,const VECTOR *);
		void SetSOTMatrix(const VECTOR *,const VECTOR *,const VECTOR *,const VECTOR *,const VECTOR *);
		void SetSOMatrix(const VECTOR *,const VECTOR *,const VECTOR *,const VECTOR *);
		void SetSTMatrix(const VECTOR *,const VECTOR *);
		void SetSMatrix(const VECTOR *);
		void SetTMatrix(const VECTOR *);
		void SetOTMatrix(const VECTOR *,const VECTOR *,const VECTOR *,const VECTOR *);
		void SetOMatrix(const VECTOR *,const VECTOR *,const VECTOR *);
		void SetRotMatrix(const VECTOR *);
		BOOL MultVectMat(VECTOR *);
		void MultInvDirMat(VECTOR*);
		void MultDirMat(VECTOR *);
		void MultNormalMat(VECTOR *);
		void MultMatAxis(VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		void MultMatOrient(VECTOR*, VECTOR*, VECTOR*);
		void InvMat(MATRIX *);
		void GenerateAxis(VECTOR*, VECTOR*, VECTOR*, VECTOR*);
};

#ifdef __MATRIX_STACK__
class STACK_ITEM : public DLIST
{
	public:
		MATRIX m, matrix;
};

class MATRIX_STACK
{
		STACK_ITEM *root;
	public:
		MATRIX matrix;

		MATRIX_STACK();
		~MATRIX_STACK();
		BOOL Push(MATRIX*);
		BOOL PushEnd(MATRIX*);
		void Pop();
		void Pop(MATRIX *m);
		void PopEnd(MATRIX*);
		void PopEnd();
		BOOL MultVectStack(VECTOR*);
		void MultInvDirStack(VECTOR*);
		void MultDirStack(VECTOR*);
		void GenerateAxis(VECTOR *, VECTOR *, VECTOR *, VECTOR *);
		MATRIX *GetMatrix() { return &matrix; };
};

class INVMATRIX_STACK
{
		STACK_ITEM *root;
	public:
		MATRIX matrix;

		INVMATRIX_STACK();
		~INVMATRIX_STACK();
		BOOL Push(MATRIX*);
		void Pop();
		BOOL MultVectStack(VECTOR*);
};
#endif
#endif

#ifdef LOWLEVEL
#ifdef __cplusplus
extern "C"
{
#endif
float dotp(const VECTOR *, const VECTOR *);
void VecSub(const VECTOR*, const VECTOR*, VECTOR *);
void VecAdd(const VECTOR*, const VECTOR*, VECTOR *);
void VecScale(const float, const VECTOR *, VECTOR *);
void VecComb(const float, const VECTOR *, const float, const VECTOR *, VECTOR *);
void VecAddScaled(const VECTOR *, const float, const VECTOR *, VECTOR *);
void VecSubScaled(const VECTOR *, const float, const VECTOR *, VECTOR *);
float Abs(const float);
void MinimizeVector(VECTOR*, const VECTOR*, const VECTOR*);
void MaximizeVector(VECTOR*, const VECTOR*, const VECTOR*);
#ifdef __cplusplus
};
#endif
#else
inline void VecCross(VECTOR*, VECTOR*, VECTOR*);
#define Abs(v) fabs(v)
#endif

float VecNormalize(VECTOR*);
void VecNormalizeNoRet(VECTOR*);
void VecCross(VECTOR*, VECTOR*, VECTOR*);
float VecNormCross(VECTOR*, VECTOR*, VECTOR*);

BOOL Refract(VECTOR*, float , const VECTOR*, VECTOR*, float);
void VecCoordSys(VECTOR*, VECTOR*, VECTOR*);
void CalcOrient(VECTOR*, VECTOR*, VECTOR*, VECTOR*);
void InvOrient(VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
void MakeBump(VECTOR*, VECTOR*, VECTOR*, float, float);
void InvMat3x3(float*, float*);

#ifdef __STORM__
float newatof(char *);
double newatan2(double sx, double cx);
#ifndef __PPC__
#define atof(s) newatof(s)
#endif // __PPC__
#endif // __STORM__

/*************
 * DESCRIPTION:   Normalize a vector
 * INPUT:         a        vector
 * OUTPUT:        original length
 *************/
#ifdef FAST_PPC_MATH
extern "C" float estinvsqrt(float);
extern "C" float estinv(float);
inline float VecNormalize(VECTOR *a)
{
	float d, d1;

	d = a->x*a->x + a->y*a->y + a->z*a->z;
	if (d < EPSILON)
		return 0.f;
	d1 = estinvsqrt(d);
	d = d1 * (1.5f - .5f * d * d1 * d1);
	a->x *= d;
	a->y *= d;
	a->z *= d;

	d1 = estinv(d);

	return d1 * (2.f - d * d1);
}

inline void VecNormalizeNoRet(VECTOR *a)
{
	float d, d1;

	d = a->x*a->x + a->y*a->y + a->z*a->z;
	if (d < EPSILON)
		return;
	d1 = estinvsqrt(d);
	d = d1 * (1.5f - .5f * d * d1 * d1);
	a->x *= d;
	a->y *= d;
	a->z *= d;
}
#else
#ifndef LOWLEVEL
inline float VecNormalize(VECTOR *a)
{
	float d, d1;

	d = (float)sqrt(a->x*a->x + a->y*a->y + a->z*a->z);
	if (d < EPSILON)
		return 0.f;
	d1 = d;
	d = 1.f/d;
	a->x *= d;
	a->y *= d;
	a->z *= d;

	return d1;
}

inline void VecNormalizeNoRet(VECTOR *a)
{
	float d;

	d = (float)sqrt(a->x*a->x + a->y*a->y + a->z*a->z);
	if (d < EPSILON)
		return;
	d = 1.f/d;
	a->x *= d;
	a->y *= d;
	a->z *= d;
}
#endif
#endif // FAST_PPC_MATH

/*************
 * DESCRIPTION:   Compute cross-product of a and b, place normalized result
 *                in r
 * INPUT:         a        first vector
 *                b        second vector
 *                r        result vector
 * OUTPUT:        length of result before normalization.
 *************/
#ifndef LOWLEVEL
inline float VecNormCross(VECTOR *a, VECTOR *b, VECTOR *r)
{
	VecCross(a, b, r);
	return VecNormalize(r);
}
#endif

/*************
 * DESCRIPTION:   Compute cross-product of a and b, place result in r
 * INPUT:         a        first vector
 *                b        second vector
 *                r        result vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void VecCross(VECTOR *a, VECTOR *b, VECTOR *r)
{
	r->x = (a->y * b->z) - (a->z * b->y);
	r->y = (a->z * b->x) - (a->x * b->z);
	r->z = (a->x * b->y) - (a->y * b->x);
}
#endif


/*************
 * FUNCTION:      SetVector
 * DESCRIPTION:   Sets the coordinates of a vector
 * INPUT:         v     vector
 *                x,y,z coordinates
 * OUTPUT:        -
 *************/
#ifdef __cplusplus
inline void SetVector(VECTOR *v, float x, float y, float z)
{
	v->x = x;
	v->y = y;
	v->z = z;
}
#endif

/*************
 * FUNCTION:      SetVect2D
 * DESCRIPTION:   Sets the coordinates of a 2D vector
 * INPUT:         v     vector
 *                x,y   coordinates
 * OUTPUT:        -
 *************/
#ifdef __cplusplus
inline void SetVect2D(VECT2D *v, float x, float y)
{
	v->x = x;
	v->y = y;
}
#endif

/*************
 * FUNCTION:      dotp
 * DESCRIPTION:   Calculate dotproduct
 * INPUT:         a        first vector
 *                b        second vector
 * OUTPUT:        dotproduct
 *************/
#ifndef LOWLEVEL
inline float dotp(const VECTOR *a, const VECTOR *b)
{
	return (a->x*b->x + a->y*b->y + a->z*b->z);
}
#endif

/*************
 * FUNCTION:      VecAbs
 * DESCRIPTION:   Calculate length of vector
 * INPUT:         a        vector
 * OUTPUT:        length of vector
 *************/
#ifdef __cplusplus
inline float VecAbs(const VECTOR *a)
{
	return ((float)sqrt(a->x*a->x + a->y*a->y + a->z*a->z));
}
#endif

/*************
 * FUNCTION:      VecSub
 * DESCRIPTION:   Subtract vectors
 * INPUT:         a        first vector
 *                b        second vector
 *                r        result vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void VecSub(const VECTOR *a, const VECTOR *b, VECTOR *r)
{
	r->x=a->x-b->x;
	r->y=a->y-b->y;
	r->z=a->z-b->z;
}
#endif

/*************
 * FUNCTION:      VecAdd
 * DESCRIPTION:   Add vectors
 * INPUT:         a        first vector
 *                b        second vector
 *                r        result vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void VecAdd(const VECTOR *a, const VECTOR *b, VECTOR *r)
{
	r->x=a->x+b->x;
	r->y=a->y+b->y;
	r->z=a->z+b->z;
}
#endif

/*************
 * FUNCTION:      VecScale
 * DESCRIPTION:   Scale vector
 * INPUT:         s        scale factor
 *                a        vector to scale
 *                r        result vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void VecScale(const float s, const VECTOR *a, VECTOR *r)
{
	r->x=s*a->x;
	r->y=s*a->y;
	r->z=s*a->z;
}
#endif

/*************
 * FUNCTION:      VecComb
 * DESCRIPTION:   Combine two vectors
 * INPUT:         s1       first scale factor
 *                v1       first vector
 *                s2       second scale factor
 *                v2       second vector
 *                r        result vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void VecComb(const float s1, const VECTOR *v1, const float s2,
	const VECTOR *v2, VECTOR *r)
{
	r->x = s1*v1->x + s2*v2->x;
	r->y = s1*v1->y + s2*v2->y;
	r->z = s1*v1->z + s2*v2->z;
}
#endif

/*************
 * FUNCTION:      VecAddScaled
 * DESCRIPTION:   Add vectors but scale one before
 * INPUT:         v1       first vector
 *                s        scale factor for second vector
 *                v2       second vector
 *                r        result vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void VecAddScaled(const VECTOR *v1, const float s, const VECTOR *v2,
	VECTOR *r)
{
	r->x = v1->x + s*v2->x;
	r->y = v1->y + s*v2->y;
	r->z = v1->z + s*v2->z;
}
#endif

/*************
 * FUNCTION:      VecSubScaled
 * DESCRIPTION:   Sub vectors and scale result
 * INPUT:         v1       first vector
 *                s        scale factor for result
 *                v2       second vector
 *                r        result vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void VecSubScaled(const VECTOR *v1, const float s, const VECTOR *v2,
	VECTOR *r)
{
	r->x = (v1->x - v2->x)*s;
	r->y = (v1->y - v2->y)*s;
	r->z = (v1->z - v2->z)*s;
}
#endif

/*************
 * FUNCTION:      MinimizeVector
 * DESCRIPTION:   Calculate minimum vector
 * INPUT:         result      minimum vector
 *                vector1     first vector
 *                vector2     second vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void MinimizeVector(VECTOR *result, const VECTOR *vector1, const VECTOR *vector2)
{
	result->x = MIN(vector1->x, vector2->x);
	result->y = MIN(vector1->y, vector2->y);
	result->z = MIN(vector1->z, vector2->z);
}
#endif

/*************
 * FUNCTION:      MaximizeVector
 * DESCRIPTION:   Calculate maximum vector
 * INPUT:         result      maximum vector
 *                vector1     first vector
 *                vector2     second vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
inline void MaximizeVector(VECTOR *result, const VECTOR *vector1, const VECTOR *vector2)
{
	result->x = MAX(vector1->x, vector2->x);
	result->y = MAX(vector1->y, vector2->y);
	result->z = MAX(vector1->z, vector2->z);
}
#endif

/*************
 * FUNCTION:      Orient2Angle
 * DESCRIPTION:   Calculates angle of orientation
 * INPUT:         angle       angle in degree
 *                orient_x    x orientation
 *                orient_y    y orientation
 *                orient_z    z orientation
 * OUTPUT:        none
 *************/
#if 0
/* based on the work of Ken Shoemake, 1993 */
/*** Order type constants, constructors, extractors ***/
	 /* There are 24 possible conventions, designated by:    */
	 /*     o EulAxI = axis used initially        */
	 /*     o EulPar = parity of axis permutation          */
	 /*     o EulRep = repetition of initial axis as last     */
	 /*     o EulFrm = frame from which axes are taken     */
	 /* Axes I,J,K will be a permutation of X,Y,Z.      */
	 /* Axis H will be either I or K, depending on EulRep.   */
	 /* Frame S takes axes from initial static frame.      */
	 /* If ord = (AxI=X, Par=Even, Rep=No, Frm=S), then       */
	 /* {a,b,c,ord} means Rz(c)Ry(b)Rx(a), where Rz(c)v       */
	 /* rotates v around Z by c radians.             */
#define EulFrmS        0
#define EulFrmR        1
#define EulFrm(ord)  ((unsigned)(ord)&1)
#define EulRepNo     0
#define EulRepYes    1
#define EulRep(ord)  (((unsigned)(ord)>>1)&1)
#define EulParEven   0
#define EulParOdd    1
#define EulPar(ord)  (((unsigned)(ord)>>2)&1)
#define EulSafe        "\000\001\002\000"
#define EulNext        "\001\002\000\001"
#define EulAxI(ord)  ((int)(EulSafe[(((unsigned)(ord)>>3)&3)]))
#define EulAxJ(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)==EulParOdd)]))
#define EulAxK(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)!=EulParOdd)]))
#define EulAxH(ord)  ((EulRep(ord)==EulRepNo)?EulAxK(ord):EulAxI(ord))
	 /* EulGetOrd unpacks all useful information about order simultaneously. */
#define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned o=ord;f=o&1;o>>=1;s=o&1;o>>=1;\
	 n=o&1;o>>=1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];h=s?k:i;}
	 /* EulOrd creates an order value between 0 and 23 from 4-tuple choices. */
#define EulOrd(i,p,r,f)    (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))
	 /* Static axes */
#define EulOrdXYZs    EulOrd(X,EulParEven,EulRepNo,EulFrmS)
#define EulOrdXYXs    EulOrd(X,EulParEven,EulRepYes,EulFrmS)
#define EulOrdXZYs    EulOrd(X,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdXZXs    EulOrd(X,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdYZXs    EulOrd(Y,EulParEven,EulRepNo,EulFrmS)
#define EulOrdYZYs    EulOrd(Y,EulParEven,EulRepYes,EulFrmS)
#define EulOrdYXZs    EulOrd(Y,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdYXYs    EulOrd(Y,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdZXYs    EulOrd(Z,EulParEven,EulRepNo,EulFrmS)
#define EulOrdZXZs    EulOrd(Z,EulParEven,EulRepYes,EulFrmS)
#define EulOrdZYXs    EulOrd(Z,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdZYZs    EulOrd(Z,EulParOdd,EulRepYes,EulFrmS)
	 /* Rotating axes */
#define EulOrdZYXr    EulOrd(X,EulParEven,EulRepNo,EulFrmR)
#define EulOrdXYXr    EulOrd(X,EulParEven,EulRepYes,EulFrmR)
#define EulOrdYZXr    EulOrd(X,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdXZXr    EulOrd(X,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdXZYr    EulOrd(Y,EulParEven,EulRepNo,EulFrmR)
#define EulOrdYZYr    EulOrd(Y,EulParEven,EulRepYes,EulFrmR)
#define EulOrdZXYr    EulOrd(Y,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdYXYr    EulOrd(Y,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdYXZr    EulOrd(Z,EulParEven,EulRepNo,EulFrmR)
#define EulOrdZXZr    EulOrd(Z,EulParEven,EulRepYes,EulFrmR)
#define EulOrdXYZr    EulOrd(Z,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdZYZr    EulOrd(Z,EulParOdd,EulRepYes,EulFrmR)

enum QuatPart {X, Y, Z, W};
typedef float HMatrix[4][4]; /* Right-handed, for column vectors */
#endif

#ifdef __cplusplus
inline void Orient2Angle(VECTOR *angle, const VECTOR *orient_x, const VECTOR *orient_y, const VECTOR *orient_z)
{
#if 0
/* Convert matrix to Euler angles (in radians). */
	int i,j,k,h,n,s,f;
	HMatrix M;

	M[0][0] = orient_x->x;
	M[0][1] = orient_x->y;
	M[0][2] = orient_x->z;
	M[1][0] = orient_y->x;
	M[1][1] = orient_y->y;
	M[1][2] = orient_y->z;
	M[2][0] = orient_z->x;
	M[2][1] = orient_z->y;
	M[2][2] = orient_z->z;

	EulGetOrd(EulOrdXYZr,i,j,k,h,n,s,f);
	if (s==EulRepYes)
	{
		double sy = sqrt(M[i][j]*M[i][j] + M[i][k]*M[i][k]);
		if (sy > 16*EPSILON)
		{
			angle->x = newatan2(M[i][j], M[i][k]);
			angle->y = newatan2(sy, M[i][i]);
			angle->z = newatan2(M[j][i], -M[k][i]);
		}
		else
		{
			angle->x = newatan2(-M[j][k], M[j][j]);
			angle->y = newatan2(sy, M[i][i]);
			angle->z = 0;
		}
	}
	else
	{
		double cy = sqrt(M[i][i]*M[i][i] + M[j][i]*M[j][i]);
		if (cy > 16*EPSILON)
		{
			angle->x = newatan2(M[k][j], M[k][k]);
			angle->y = newatan2(-M[k][i], cy);
			angle->z = newatan2(M[j][i], M[i][i]);
		}
		else
		{
			angle->x = newatan2(-M[j][k], M[j][j]);
			angle->y = newatan2(-M[k][i], cy);
			angle->z = 0;
		}
	}
	angle->x *= INV_PI_180;
	angle->y *= INV_PI_180;
	angle->z *= INV_PI_180;
	if (n == EulParOdd)
	{
		angle->x = -angle->x;
		angle->y = -angle->y;
		angle->z = -angle->z;
	}
	if (f==EulFrmR)
	{
		float t = angle->x;
		angle->x = angle->z;
		angle->z = t;
	}
#else
	angle->y = (float)asin(orient_x->z);
	if (cos(angle->y) != 0.)
	{
		angle->x = -(float)atan2(orient_y->z, orient_z->z)*INV_PI_180;
		angle->z = -(float)atan2(orient_x->y, orient_x->x)*INV_PI_180;
	}
	else
	{
		angle->x = -(float)atan2(orient_x->x, orient_y->y)*INV_PI_180;
		angle->z = 0.f;
	}
	angle->y *= INV_PI_180;

	if (angle->x < 180.0f)
		angle->x += 360.0f;
	if (angle->x > 180.0f)
		angle->x -= 360.0f;
	if (angle->y < 180.0f)
		angle->y += 360.0f;
	if (angle->y > 180.0f)
		angle->y -= 360.0f;
	if (angle->z < 180.0f)
		angle->z += 360.0f;
	if (angle->z > 180.0f)
		angle->z -= 360.0f;
#endif
}
#endif

/*************
 * FUNCTION:      equal
 * DESCRIPTION:   Test if two numbers are equal
 * INPUT:         a        first number
 *                b        second nubmer
 * OUTPUT:        TRUE if equal else FALSE
 *************/
#define equal(a, b) (fabs((a)-(b)) < EPSILON)

#endif // VECMATH_H
