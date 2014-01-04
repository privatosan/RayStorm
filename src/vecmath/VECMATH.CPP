/***************
 * MODUL:         vecmath
 * NAME:          vecmath.cpp
 * DESCRIPTION:   Miscellaneous vector routines
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 ***************/

#include <math.h>
#include <stdlib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef ERROR_H
#include "errors.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef LOWLEVEL
const VECTOR minVector = {-INFINITY, -INFINITY, -INFINITY};
const VECTOR maxVector = {INFINITY, INFINITY, INFINITY};
#endif

#ifdef __STORM__
/*************
 * DESCRIPTION:   replacement of maxon atan2
 * INPUT:
 * OUTPUT:
 *************/
double newatan2(double sx, double cx)
{
	if(sx > 0.f)
	{
		if(cx == 0.f)
			return PI_2;
		if(cx > 0.f)
			return atan(sx/cx);
		else
			return atan(sx/cx)+PI;
	}
	else
	{
		if(cx == 0.f)
			return -PI_2;
		if(cx > 0.f)
			return atan(sx/cx);
		else
			return atan(sx/cx)-PI;
	}
}
#endif

#ifdef __STORM__
/*************
 * DESCRIPTION:   replacement of storm atof
 * INPUT:
 * OUTPUT:
 *************/
float newatof(char *s)
{
	int pos;
	float num;
	int expo;
	BOOL negative,point;

	pos = 0;
	num = 0.f;
	expo = 0;
	negative = FALSE;
	point = FALSE;
	while(s[pos])
	{
		if(s[pos] >= '0' && s[pos] <= '9')
		{
			num *= 10.;
			num += (float)(s[pos] - '0');
			if(point)
				expo--;
			pos++;
			continue;
		}

		if(s[pos] == '-')
		{
			negative = TRUE;
			pos++;
			continue;
		}

		if(s[pos] == '.')
		{
			point = TRUE;
			pos++;
			continue;
		}

		if(s[pos] == 'e' || s[pos] == 'E')
		{
			expo += atoi(&s[pos+1]);
			break;
		}

		if(s[pos] == '+')
			pos++;
		else
			break;
	}
	if(negative)
		num = -num;
	num *= (float)pow(10.f,(float)expo);

	return num;
}
#endif

/*************
 * DESCRIPTION:   Given a vector, find two additional vectors such that all three
 *                are mutually perpendicular and uaxis X vaxis = vector.  The given
 *                vector need not be normalized. uaxis and vaxis are normalized.
 * INPUT:         vector         vector to find two vectors for
 *                uaxis          first found vector
 *                vaxis          second found vector
 * OUTPUT:        none
 *************/
void VecCoordSys(VECTOR *vector, VECTOR *uaxis, VECTOR *vaxis)
{
	uaxis->x = -vector->y;
	uaxis->y = vector->x;
	uaxis->z = 0.f;
	if (VecNormalize(uaxis) == 0.f)
	{
		uaxis->x = vector->z;
		uaxis->y = 0.f;
		uaxis->z = -vector->x;
		if (VecNormalize(uaxis) == 0.f)
			// Degenerate vector
			return;
	}
	VecNormCross(vector, uaxis, vaxis);
}

/*************
 * DESCRIPTION:   Calculate with three angles three vectors such that all are
 *                mutually perpendicular. The vectors are normalized
 * INPUT:         align       alignment
 *                orient      result vectors
 * OUTPUT:        none
 *************/
void CalcOrient(VECTOR *align, VECTOR *orient_x, VECTOR *orient_y, VECTOR *orient_z)
{
#if 0
/* Construct matrix from Euler angles (in radians). */
	double ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
	int i,j,k,h,n,s,f;
	HMatrix M;

	EulGetOrd(EulOrdXYZr,i,j,k,h,n,s,f);
	if (f==EulFrmR)
	{
		float t = align->x;
		align->x = align->z;
		align->z = t;
	}
	if (n==EulParOdd)
	{
		align->x = -align->x;
		align->y = -align->y;
		align->z = -align->z;
	}
	ti = align->x;
	tj = align->y;
	th = align->z;
	ci = cos(ti);
	cj = cos(tj);
	ch = cos(th);
	si = sin(ti);
	sj = sin(tj);
	sh = sin(th);
	cc = ci*ch;
	cs = ci*sh;
	sc = si*ch;
	ss = si*sh;
	if (s==EulRepYes)
	{
		M[i][i] = cj;    M[i][j] =  sj*si;    M[i][k] =  sj*ci;
		M[j][i] = sj*sh;  M[j][j] = -cj*ss+cc; M[j][k] = -cj*cs-sc;
		M[k][i] = -sj*ch; M[k][j] =  cj*sc+cs; M[k][k] =  cj*cc-ss;
	}
	else
	{
		M[i][i] = cj*ch; M[i][j] = sj*sc-cs; M[i][k] = sj*cc+ss;
		M[j][i] = cj*sh; M[j][j] = sj*ss+cc; M[j][k] = sj*cs-sc;
		M[k][i] = -sj;  M[k][j] = cj*si;    M[k][k] = cj*ci;
	}

	orient_x->x = M[0][0];
	orient_x->y = M[0][1];
	orient_x->z = M[0][2];
	orient_y->x = M[1][0];
	orient_y->y = M[1][1];
	orient_y->z = M[1][2];
	orient_z->x = M[2][0];
	orient_z->y = M[2][1];
	orient_z->z = M[2][2];
#else
	MATRIX matrix;

	matrix.SetRotMatrix(align);
	// x-orientation
	SetVector(orient_x, 1.f, 0.f, 0.f);
	matrix.MultVectMat(orient_x);

	// y-orientation
	SetVector(orient_y, 0.f, 1.f, 0.f);
	matrix.MultVectMat(orient_y);

	// z-orientation
	SetVector(orient_z, 0.f, 0.f, 1.f);
	matrix.MultVectMat(orient_z);
#endif
}

/*************
 * DESCRIPTION:   Invert orientation
 * INPUT:         ox,oy,oz    original orientation
 *                ox1,oy1,oz1 inverted orientation
 * OUTPUT:        none
 *************/
void InvOrient(VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *ox1, VECTOR *oy1, VECTOR *oz1)
{
	MATRIX m,m1;

	m.SetOMatrix(ox,oy,oz);
	m.InvMat(&m1);
	ox1->x = m1.m[5];
	ox1->y = m1.m[9];
	ox1->z = m1.m[13];
	oy1->x = m1.m[6];
	oy1->y = m1.m[10];
	oy1->z = m1.m[14];
	oz1->x = m1.m[7];
	oz1->y = m1.m[11];
	oz1->z = m1.m[15];
}

/*************
 * DESCRIPTION:   Calculate direction of refracted ray using Heckbert's formula.
 *                Returns TRUE if a total internal reflection occurs.
 * INPUT:         dir         direction vector
 *                index       index of refraction
 *                I
 *                N
 *                cos1        angle between ray and normal
 * OUTPUT:        TRUE if TIR occurs, else FALSE
 *************/
BOOL Refract(VECTOR *dir, float index, const VECTOR *I, VECTOR *N, float cos1)
{
	float cos2, k;
	VECTOR nrm;

	if (cos1 < 0.f)
	{
		// Hit the 'backside' of a surface -- flip the normal.
		nrm.x = -N->x;
		nrm.y = -N->y;
		nrm.z = -N->z;
		cos1 = -cos1;
	}
	else
	{
		nrm = *N;
	}

	cos2 = (float)(1.f - index*index*(1.f - cos1*cos1));
	if (cos2 < 0.f)
		return TRUE;      // Total internal reflection
	k =  (float)(-sqrt((double)cos2) + index * cos1);
	VecComb(index, I, k, &nrm, dir);
	return FALSE;
}

/*************
 * DESCRIPTION:   Modify given normal by "bumping" it.
 * INPUT:         norm        normal
 *                dpdu        delta u
 *                dpdv        delta v
 *                fu
 *                fv
 * OUTPUT:        none
 *************/
void MakeBump(VECTOR *norm, VECTOR *dpdu, VECTOR *dpdv, float fu, float fv)
{
	VECTOR tmp1, tmp2;

	VecCross(norm, dpdv, &tmp1);
	tmp1.x *= fu;
	tmp1.y *= fu;
	tmp1.z *= fu;
	VecCross(norm, dpdu, &tmp2);
	tmp2.x *= fv;
	tmp2.y *= fv;
	tmp2.z *= fv;
	VecSub(&tmp1, &tmp2, &tmp1);
	VecAdd(norm, &tmp1, norm);
	VecNormalizeNoRet(norm);
}

/*************
 * DESCRIPTION:   sets the matrix to identity matrix
 * INPUT:         none
 * OUTPUT:        none
 *************/
void MATRIX::IdentityMatrix()
{
	m[0] = m[5] = m[10] = m[15] = 1.f;
	m[1] = m[2] = m[3] = 0.f;
	m[4] = m[6] = m[7] = 0.f;
	m[8] = m[9] = m[11] = 0.f;
	m[12] = m[13] = m[14] = 0.f;
}

/*************
 * DESCRIPTION:   Multiply Matrix ma with mb
 * INPUT:         ma          matrix a
 *                mb          matrix b
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
void MATRIX::MultMat(MATRIX *ma,MATRIX *mb)
{
	int i;

	for(i=0; i<4; i++)
	{
		m[i] = (*ma->m) * mb->m[i] + ma->m[1] * mb->m[4+i] +
				 ma->m[2] * mb->m[8+i] + ma->m[3] * mb->m[12+i];
		m[4+i] = ma->m[4] * mb->m[i] + ma->m[5] * mb->m[4+i] +
					ma->m[6] * mb->m[8+i] + ma->m[7] * mb->m[12+i];
		m[8+i] = ma->m[8] * mb->m[i] + ma->m[9] * mb->m[4+i] +
					ma->m[10] * mb->m[8+i] + ma->m[11] * mb->m[12+i];
		m[12+i] = ma->m[12] * mb->m[i] + ma->m[13] * mb->m[4+i] +
					 ma->m[14] * mb->m[8+i] + ma->m[15] * mb->m[12+i];
	}
}
#endif

/*************
 * DESCRIPTION:   sets a rotation matrix (around x)
 * INPUT:         theta       rotation angle
 * OUTPUT:        none
 *************/
void MATRIX::SetRotXMatrix(float theta)
{
	float sintheta,costheta;
	float w;

	this->IdentityMatrix();
	if(theta==0)
		return;
	w = PI_180 * theta;
	sintheta = (float)sin(w);
	costheta = (float)cos(w);
	m[10] = costheta;
	m[11] = sintheta;
	m[14] = -sintheta;
	m[15] = costheta;
}

/*************
 * DESCRIPTION:   sets a rotation matrix (around y)
 * INPUT:         theta       rotation angle
 * OUTPUT:        none
 *************/
void MATRIX::SetRotYMatrix(float theta)
{
	float sintheta,costheta;
	float w;

	this->IdentityMatrix();
	if(theta==0)
		return;
	w=PI_180 * theta;
	sintheta = (float)sin(w);
	costheta = (float)cos(w);
	m[5] = costheta;
	m[7] = sintheta;
	m[13] = -sintheta;
	m[15] = costheta;
}

/*************
 * DESCRIPTION:   sets a rotation matrix (around z)
 * INPUT:         theta       rotation angle
 * OUTPUT:        none
 *************/
void MATRIX::SetRotZMatrix(float theta)
{
	float sintheta,costheta;
	float w;

	this->IdentityMatrix();
	if(theta==0)
		return;
	w=PI_180 * theta;
	sintheta = (float)sin(w);
	costheta = (float)cos(w);
	m[5] = costheta;
	m[6] = sintheta;
	m[9] = -sintheta;
	m[10] = costheta;
}
// } <- nedded by GoldEd

/*************
 * DESCRIPTION:   sets a translation matrix
 * INPUT:         d           translation values
 * OUTPUT:        none
 *************/
void MATRIX::SetTransMatrix(const VECTOR *d)
{
	this->IdentityMatrix();
	m[1] = d->x;
	m[2] = d->y;
	m[3] = d->z;
}

/*************
 * DESCRIPTION:   sets a scale matrix
 * INPUT:         s           scale factors
 * OUTPUT:        none
 *************/
void MATRIX::SetScaleMatrix(const VECTOR *s)
{
	this->IdentityMatrix();
	m[5] = s->x;
	m[10] = s->y;
	m[15] = s->z;
}

/*************
 * DESCRIPTION:   sets a scale rotate translate matrix
 * INPUT:         s           scale factors
 *                r           rotation angles
 *                d           translation values
 * OUTPUT:        none
 *************/
void MATRIX::SetSRTMatrix(const VECTOR *s, const VECTOR *r, const VECTOR *d)
{
	MATRIX m1,m2;
	MATRIX erg1,erg2;

	// Scaling
	m1.SetScaleMatrix(s);

	// x-Rotating
	m2.SetRotXMatrix(r->x);
	erg1.MultMat(&m1,&m2);

	// y-Rotating
	m1.SetRotYMatrix(r->y);
	erg2.MultMat(&erg1,&m1);

	// z-Rotating
	m1.SetRotZMatrix(r->z);
	erg1.MultMat(&erg2,&m1);

	// Translation
	m1.SetTransMatrix(d);
	this->MultMat(&erg1,&m1);
}

/*************
 * DESCRIPTION:   sets a scale orientation translate matrix
 * INPUT:         s           scale factors
 *                ox,oy,oz    orientation
 *                d           translation values
 * OUTPUT:        none
 *************/
void MATRIX::SetSOTMatrix(const VECTOR *s, const VECTOR *ox, const VECTOR *oy, const VECTOR *oz, const VECTOR *d)
{
	IdentityMatrix();
	m[ 1] = d->x;
	m[ 2] = d->y;
	m[ 3] = d->z;
	m[ 5] = ox->x * s->x;
	m[ 9] = ox->y * s->x;
	m[13] = ox->z * s->x;
	m[ 6] = oy->x * s->y;
	m[10] = oy->y * s->y;
	m[14] = oy->z * s->y;
	m[ 7] = oz->x * s->z;
	m[11] = oz->y * s->z;
	m[15] = oz->z * s->z;
}

/*************
 * DESCRIPTION:   sets a scale orientation matrix
 * INPUT:         s           scale factors
 *                ox,oy,oz    orientation
 * OUTPUT:        none
 *************/
void MATRIX::SetSOMatrix(const VECTOR *s, const VECTOR *ox, const VECTOR *oy, const VECTOR *oz)
{
	IdentityMatrix();
	m[ 5] = ox->x * s->x;
	m[ 9] = ox->y * s->x;
	m[13] = ox->z * s->x;
	m[ 6] = oy->x * s->y;
	m[10] = oy->y * s->y;
	m[14] = oy->z * s->y;
	m[ 7] = oz->x * s->z;
	m[11] = oz->y * s->z;
	m[15] = oz->z * s->z;
}

/*************
 * DESCRIPTION:   sets a scale translate matrix
 * INPUT:         s           scale factors
 *                d           translation values
 * OUTPUT:        none
 *************/
void MATRIX::SetSTMatrix(const VECTOR *s, const VECTOR *d)
{
	IdentityMatrix();
	m[ 1] = d->x;
	m[ 2] = d->y;
	m[ 3] = d->z;
	m[ 5] = s->x;
	m[10] = s->y;
	m[15] = s->z;
}

/*************
 * DESCRIPTION:   sets a scale matrix
 * INPUT:         s           scale factors
 * OUTPUT:        none
 *************/
void MATRIX::SetSMatrix(const VECTOR *s)
{
	IdentityMatrix();
	m[ 5] = s->x;
	m[10] = s->y;
	m[15] = s->z;
}

/*************
 * DESCRIPTION:   sets a translate matrix
 * INPUT:         d           translation values
 * OUTPUT:        none
 *************/
void MATRIX::SetTMatrix(const VECTOR *d)
{
	IdentityMatrix();
	m[ 1] = d->x;
	m[ 2] = d->y;
	m[ 3] = d->z;
}

/*************
 * DESCRIPTION:   sets an orientation translate matrix
 * INPUT:         ox,oy,oz    orientation
 *                d           translation values
 * OUTPUT:        none
 *************/
void MATRIX::SetOTMatrix(const VECTOR *ox, const VECTOR *oy, const VECTOR *oz, const VECTOR *d)
{
	IdentityMatrix();
	m[ 1] = d->x;
	m[ 2] = d->y;
	m[ 3] = d->z;
	m[ 5] = ox->x;
	m[ 9] = ox->y;
	m[13] = ox->z;
	m[ 6] = oy->x;
	m[10] = oy->y;
	m[14] = oy->z;
	m[ 7] = oz->x;
	m[11] = oz->y;
	m[15] = oz->z;
}

/*************
 * DESCRIPTION:   sets an orientation matrix
 * INPUT:         ox,oy,oz    orientation
 * OUTPUT:        none
 *************/
void MATRIX::SetOMatrix(const VECTOR *ox, const VECTOR *oy, const VECTOR *oz)
{
	IdentityMatrix();
	m[ 5] = ox->x;
	m[ 9] = ox->y;
	m[13] = ox->z;
	m[ 6] = oy->x;
	m[10] = oy->y;
	m[14] = oy->z;
	m[ 7] = oz->x;
	m[11] = oz->y;
	m[15] = oz->z;
}

/*************
 * DESCRIPTION:   sets a rotate matrix
 * INPUT:         r           rotation angles
 * OUTPUT:        none
 *************/
void MATRIX::SetRotMatrix(const VECTOR *r)
{
	MATRIX m1,m2;
	MATRIX erg1;

	// x-Rotating
	m1.SetRotXMatrix(r->x);

	// y-Rotating
	m2.SetRotYMatrix(r->y);
	erg1.MultMat(&m1,&m2);

	// z-Rotating
	m1.SetRotZMatrix(r->z);
	this->MultMat(&erg1,&m1);
}

/*************
 * DESCRIPTION:   Multiply vector v with m
 * INPUT:         v        vector
 * OUTPUT:        FALSE if failed else TRUE
 *************/
#ifndef LOWLEVEL
BOOL MATRIX::MultVectMat(VECTOR *v)
{
	float w,x,y,z;

	x = v->x;
	y = v->y;
	z = v->z;
	w = m[0] + x*m[4] + y*m[8] + z*m[12];
	if (fabs(w) < EPSILON)
		return FALSE;
	w = 1.f/w;

	v->x = (m[1] + x*m[5] + y*m[9] + z*m[13])*w;
	v->y = (m[2] + x*m[6] + y*m[10] + z*m[14])*w;
	v->z = (m[3] + x*m[7] + y*m[11] + z*m[15])*w;
	return TRUE;
}
#endif

/*************
 * DESCRIPTION:   Apply inverted transformation to a vector (translations have no effect)
 * INPUT:         dir   vector
 * OUTPUT:        -
 *************/
void MATRIX::MultInvDirMat(VECTOR *dir)
{
	float x,y,z;

	x = dir->x;
	y = dir->y;
	z = dir->z;

	dir->x = x*m[5] + y*m[6] + z*m[7];
	dir->y = x*m[9] + y*m[10] + z*m[11];
	dir->z = x*m[13] + y*m[14] + z*m[15];
}

/*************
 * DESCRIPTION:   Apply transformation to a vector (translations have no effect)
 * INPUT:         dir        vector
 * OUTPUT:        -
 *************/
void MATRIX::MultDirMat(VECTOR *dir)
{
	float x,y,z;

	x = dir->x;
	y = dir->y;
	z = dir->z;

	dir->x = x*m[5] + y*m[9] + z*m[13];
	dir->y = x*m[6] + y*m[10] + z*m[14];
	dir->z = x*m[7] + y*m[11] + z*m[15];
}

/*************
 * DESCRIPTION:   multiply by the transpose of the given
 *    matrix (which is the inverse of the 'desired' transformation).
 * INPUT:         norm     normal
 * OUTPUT:        -
 *************/
void MATRIX::MultNormalMat(VECTOR *norm)
{
	float x,y,z;

	x = norm->x;
	y = norm->y;
	z = norm->z;

	norm->x = x*m[5] + y*m[6] + z*m[7];
	norm->y = x*m[9] + y*m[10] + z*m[11];
	norm->z = x*m[13] + y*m[14] + z*m[15];

	VecNormalizeNoRet(norm);
}

/*************
 * DESCRIPTION:   Invert a 4*4 matrix.
 * INPUT:         r        result matrix
 * OUTPUT:        none
 *************/
void MATRIX::InvMat(MATRIX *r)
{
	float det;
	register int i,j;
	int i1,i2,i3,j1,j2,j3;
	BOOL negflag;

	negflag = FALSE;
	for (i=0; i<=3; i++)
	{
		i1 = ((i+1) & 3) << 2;
		i2 = ((i+2) & 3) << 2;
		i3 = ((i+3) & 3) << 2;
		for (j=0; j<=3; j++)
		{
			j1 = (j+1) & 3;
			j2 = (j+2) & 3;
			j3 = (j+3) & 3;
			r->m[j*4+i] = m[i1+j1] * (m[i2+j2] * m[i3+j3] - m[i2+j3] * m[i3+j2]) +
							  m[i1+j2] * (m[i2+j3] * m[i3+j1] - m[i2+j1] * m[i3+j3]) +
							  m[i1+j3] * (m[i2+j1] * m[i3+j2] - m[i2+j2] * m[i3+j1]);
			if (negflag)
				r->m[j*4+i] = - r->m[j*4+i];
			negflag = !negflag;
		}
		negflag = !negflag;
	}
	det = (*m) * (*r->m);
	for (i=1; i<=3; i++)
		det = det + m[i] * r->m[i*4];

	det = 1.f/det;
	for (i=0; i<16; i++)
		r->m[i] *= det;
}

/*************
 * DESCRIPTION:   Invert a 3*3 matrix.
 * INPUT:         a     matrix
 *                b     result matrix
 * OUTPUT:        none
 *************/
void InvMat3x3(float *a, float *b)
{
	float t;
	int i, j, i1;

	for (j=0; j<3; j++)              // Loop over cols of a from left to right, eliminating above and below diag
	{                                // Find largest pivot in column j among rows j..2
		i1 = j;                       // Row with largest pivot candidate
		for (i=j+1; i<3; i++)
		{
			if (fabs(a[i*3+j]) > fabs(a[i1*3+j]))
				 i1 = i;
		}

		t = a[i1*3];                  // Swap rows i1 and j in a and b to put pivot on diagonal
		a[i1*3] = a[j*3];
		a[j*3] = t;
		t = a[i1*3+1];
		a[i1*3+1] = a[j*3+1];
		a[j*3+1] = t;
		t = a[i1*3+2];
		a[i1*3+2] = a[j*3+2];
		a[j*3+2] = t;

		t = b[i1*3];
		b[i1*3] = b[j*3];
		b[j*3] = t;
		t = b[i1*3+1];
		b[i1*3+1] = b[j*3+1];
		b[j*3+1] = t;
		t = b[i1*3+2];
		b[i1*3+2] = b[j*3+2];
		b[j*3+2] = t;

		t = 1.f/a[j*3+j];
		b[j*3  ] *= t;                // Scale row j to have a unit diagonal
		b[j*3+1] *= t;
		b[j*3+2] *= t;
		a[j*3  ] *= t;
		a[j*3+1] *= t;
		a[j*3+2] *= t;

		for (i=0; i<3; i++)           // Eliminate off-diagonal elems in col j of a, doing identical ops to b
		{
			if (i!=j)
			{
				b[i*3  ] -= a[i*3+j] * b[j*3  ];
				b[i*3+1] -= a[i*3+j] * b[j*3+1];
				b[i*3+2] -= a[i*3+j] * b[j*3+2];
				a[i*3  ] -= a[i*3+j] * a[j*3  ];
				a[i*3+1] -= a[i*3+j] * a[j*3+1];
				a[i*3+2] -= a[i*3+j] * a[j*3+2];
			}
		}
	}
}

/*************
 * DESCRIPTION:   Multiplicate matrix with axis
 * INPUT:         ox,oy,oz    axis
 * OUTPUT:        -
 *************/
void MATRIX::MultMatOrient(VECTOR *ox, VECTOR *oy, VECTOR *oz)
{
	MATRIX matrix,m;

	m.SetOMatrix(ox, oy, oz);
	matrix.MultMat(this, &m);

	SetVector(ox, matrix.m[5], matrix.m[9], matrix.m[13]);
	SetVector(oy, matrix.m[6], matrix.m[10], matrix.m[14]);
	SetVector(oz, matrix.m[7], matrix.m[11], matrix.m[15]);
}

/*************
 * DESCRIPTION:   Multiplicate matrix with axis
 * INPUT:         pos,ox,oy,oz      axis
 * OUTPUT:        -
 *************/
void MATRIX::MultMatAxis(VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz)
{
	MATRIX matrix,m;

	m.SetOTMatrix(ox, oy, oz, pos);
	matrix.MultMat(this, &m);

	SetVector(pos, matrix.m[1], matrix.m[2], matrix.m[3]);
	SetVector(ox, matrix.m[5], matrix.m[9], matrix.m[13]);
	SetVector(oy, matrix.m[6], matrix.m[10], matrix.m[14]);
	SetVector(oz, matrix.m[7], matrix.m[11], matrix.m[15]);
}

/*************
 * DESCRIPTION:   Generate an axis from a matrix
 * INPUT:         ox,oy,oz    returned orientation
 *                p           returned position
 * OUTPUT:        -
 *************/
void MATRIX::GenerateAxis(VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *p)
{
	SetVector(p,  m[1], m[ 2], m[ 3]);
	SetVector(ox, m[5], m[ 9], m[13]);
	SetVector(oy, m[6], m[10], m[14]);
	SetVector(oz, m[7], m[11], m[15]);
}

#ifdef __MATRIX_STACK__
/*************
 * DESCRIPTION:   Constructor
 * INPUT:         -
 * OUTPUT:        -
 *************/
MATRIX_STACK::MATRIX_STACK()
{
	root = NULL;
	matrix.IdentityMatrix();
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         -
 * OUTPUT:        -
 *************/
MATRIX_STACK::~MATRIX_STACK()
{
	STACK_ITEM *si,*next;

	si = root;
	while(si)
	{
		next = (STACK_ITEM*)si->GetNext();
		delete si;
		si = next;
	}
	root = NULL;
}

/*************
 * DESCRIPTION:   Push a matrix
 * INPUT:         m     matrix
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL MATRIX_STACK::Push(MATRIX *m)
{
	STACK_ITEM *si;

	si = new STACK_ITEM;
	if(!si)
		return FALSE;
	si->m = *m;
	si->Insert((DLIST**)&root);

	si->matrix.MultMat(m, &matrix);
	matrix = si->matrix;

	return TRUE;
}

/*************
 * DESCRIPTION:   Push a matrix at the end of the stack
 * INPUT:         m     matrix
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL MATRIX_STACK::PushEnd(MATRIX *m)
{
	STACK_ITEM *si;
	MATRIX tmp;

	si = new STACK_ITEM;
	if(!si)
		return FALSE;
	si->m = *m;

	si->Append((DLIST**)&root);

	si->matrix = *m;
	tmp.MultMat(&matrix, m);
	matrix = tmp;

	return TRUE;
}

/*************
 * DESCRIPTION:   Pop a matrix
 * INPUT:         m     popped matrix
 * OUTPUT:        -
 *************/
void MATRIX_STACK::Pop(MATRIX *m)
{
	if(root)
	{
		if(m)
			*m = root->m;

		root->Remove((DLIST**)&root);
	}
	else
	{
		if(m)
			m->IdentityMatrix();
	}

	if(root)
		matrix = root->matrix;
	else
		matrix.IdentityMatrix();
}

/*************
 * DESCRIPTION:   Pop a matrix
 * INPUT:         -
 * OUTPUT:        -
 *************/
void MATRIX_STACK::Pop()
{
	if(root)
		root->Remove((DLIST**)&root);

	if(root)
		matrix = root->matrix;
	else
		matrix.IdentityMatrix();
}

/*************
 * DESCRIPTION:   Pop a matrix at the end of the stack
 * INPUT:         m     popped matrix
 * OUTPUT:        -
 *************/
void MATRIX_STACK::PopEnd(MATRIX *m)
{
	STACK_ITEM *cur,*next;
	MATRIX mat, mat1;

	if(!root)
		return;

	// skip to end
	if(root)
	{
		cur = root;
		next = (STACK_ITEM*)cur->GetNext();
		while(next)
		{
			cur = next;
			next = (STACK_ITEM*)cur->GetNext();
		}
		*m = cur->m;
		cur->Remove((DLIST**)&root);
	}
	else
	{
		*m = root->m;
		root->Remove((DLIST**)&root);
	}

	m->InvMat(&mat);
	mat1.MultMat(&matrix,&mat);
	matrix = mat1;
}

/*************
 * DESCRIPTION:   Pop a matrix at the end of the stack
 * INPUT:         -
 * OUTPUT:        -
 *************/
void MATRIX_STACK::PopEnd()
{
	STACK_ITEM *cur,*next;
	MATRIX m, mat, mat1;

	if(!root)
		return;

	// skip to end
	if(root)
	{
		cur = root;
		next = (STACK_ITEM*)cur->GetNext();
		while(next)
		{
			cur = next;
			next = (STACK_ITEM*)cur->GetNext();
		}
		m = cur->m;
		cur->Remove((DLIST**)&root);
	}
	else
	{
		m = root->m;
		root->Remove((DLIST**)&root);
	}

	m.InvMat(&mat);
	mat1.MultMat(&matrix,&mat);
	matrix = mat1;
}

/*************
 * DESCRIPTION:   Multiply vector with matrix stack
 * INPUT:         v     vector
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL MATRIX_STACK::MultVectStack(VECTOR *v)
{
	return matrix.MultVectMat(v);
}

/*************
 * DESCRIPTION:   Apply inverted transformation to a vector (translations have no effect)
 * INPUT:         dir   vector
 * OUTPUT:        -
 *************/
void MATRIX_STACK::MultInvDirStack(VECTOR *dir)
{
	matrix.MultInvDirMat(dir);
}

/*************
 * DESCRIPTION:   Apply transformation to a vector (translations have no effect)
 * INPUT:         dir   vector
 * OUTPUT:        -
 *************/
void MATRIX_STACK::MultDirStack(VECTOR *dir)
{
	matrix.MultDirMat(dir);
}

/*************
 * DESCRIPTION:   Generate an axis with the stack
 * INPUT:         ox,oy,oz    returned orientation
 *                p           returned position
 * OUTPUT:        -
 *************/
void MATRIX_STACK::GenerateAxis(VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *p)
{
	matrix.GenerateAxis(ox,oy,oz, p);
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         -
 * OUTPUT:        -
 *************/
INVMATRIX_STACK::INVMATRIX_STACK()
{
	root = NULL;
	matrix.IdentityMatrix();
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         -
 * OUTPUT:        -
 *************/
INVMATRIX_STACK::~INVMATRIX_STACK()
{
	STACK_ITEM *si,*next;

	si = root;
	while(si)
	{
		next = (STACK_ITEM*)si->GetNext();
		delete si;
		si = next;
	}
	root = NULL;
}

/*************
 * DESCRIPTION:   Push a matrix
 * INPUT:         m     matrix
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL INVMATRIX_STACK::Push(MATRIX *m)
{
	STACK_ITEM *si;
	MATRIX mat;

	si = new STACK_ITEM;
	if(!si)
		return FALSE;
	si->m = *m;
	si->Insert((DLIST**)&root);

	mat.MultMat(&matrix,m);
	matrix = mat;

	return TRUE;
}

/*************
 * DESCRIPTION:   Pop a matrix
 * INPUT:         -
 * OUTPUT:        -
 *************/
void INVMATRIX_STACK::Pop()
{
	STACK_ITEM *si;
	MATRIX mat;

	if(!root)
		return;

	root->Remove((DLIST**)&root);

	matrix.IdentityMatrix();
	si = root;
	while(si)
	{
		mat.MultMat(&si->m,&matrix);
		matrix = mat;
		si = (STACK_ITEM*)si->GetNext();
	}
}

/*************
 * DESCRIPTION:   Multiply vector with matrix stack
 * INPUT:         v     vector
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL INVMATRIX_STACK::MultVectStack(VECTOR *v)
{
	return matrix.MultVectMat(v);
}
#endif

