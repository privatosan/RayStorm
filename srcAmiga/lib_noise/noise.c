/***************
 * MODUL:         noise
 * NAME:          noise.c
 * DESCRIPTION:   Functions for textures
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.06.95 ah    initial release
 *    24.07.95 ah    new version of noise, turbulence
 *    26.08.95 ah    added DNoise()
 *    27.01.97 ah    added VfBm()
 *    11.10.97 ah    optimized Noise() for more speed
 ***************/

#include <stdlib.h>
#include <math.h>

#ifndef TYPEDEFS_H
#include <typedefs.h>
#endif

#ifndef VECMATH_H
#include <vecmath.h>
#endif

#define MINX      -1000000
#define MINY      MINX
#define MINZ      MINX

#define SCURVE(a) ((a)*(a)*(3.f-2.f*(a)))
#define REALSCALE (3.051757813e-5f)  // 2.0/65536.0
#define NREALSCALE (4.8828125e-4f)   // 2.0/4096.0
#define Hash3d(a,b,c) hashTable[hashTable[hashTable[(a) & 0xfff] ^ ((b) & 0xfff)] ^ ((c) & 0xfff)]
#define Hash(a,b,c) (xtab[(xtab[(xtab[(a) & 0xff] ^ (b)) & 0xff] ^ (c)) & 0xff] & 0xff)

#define MAXSIZE   500

#define INCRSUM(result, m, s, x, y, z) \
	incrsum_tmp = &RTable[m]; \
	result ((s)*(incrsum_tmp[0]*0.5f \
		+ incrsum_tmp[1]*(x) \
		+ incrsum_tmp[2]*(y) \
		+ incrsum_tmp[3]*(z)));

static float RTable[267] =
{
			-1,    0.604974,   -0.937102,    0.414115,    0.576226,  -0.0161593,
	0.432334,    0.103685,    0.590539,   0.0286412,     0.46981,    -0.84622,
 -0.0734112,   -0.304097,    -0.40206,   -0.210132,   -0.919127,    0.652033,
	-0.83151,   -0.183948,   -0.671107,    0.852476,    0.043595,   -0.404532,
	 0.75494,   -0.335653,    0.618433,    0.605707,    0.708583,   -0.477195,
	0.899474,    0.490623,    0.221729,   -0.400381,   -0.853727,   -0.932586,
	0.659113,    0.961303,    0.325948,   -0.750851,    0.842466,    0.734401,
  -0.649866,    0.394491,   -0.466056,   -0.434073,    0.109026,   0.0847028,
  -0.738857,    0.241505,     0.16228,    -0.71426,   -0.883665,   -0.150408,
	-0.90396,   -0.686549,   -0.785214,    0.488548,   0.0246433,    0.142473,
  -0.602136,    0.375845, -0.00779736,    0.498955,   -0.268147,    0.856382,
  -0.386007,   -0.596094,   -0.867735,   -0.570977,   -0.914366,     0.28896,
	0.672206,   -0.233783,     0.94815,    0.895262,    0.343252,   -0.173388,
  -0.767971,   -0.314748,    0.824308,   -0.342092,    0.721431,    -0.24004,
	-0.63653,    0.553277,    0.376272,    0.158984,   -0.452659,    0.396323,
  -0.420676,   -0.454154,    0.122179,    0.295857,   0.0664225,   -0.202075,
  -0.724788,    0.453513,    0.224567,   -0.908812,    0.176349,   -0.320516,
  -0.697139,    0.742702,   -0.900786,    0.471489,   -0.133532,    0.119127,
  -0.889769,    -0.23183,   -0.669673,   -0.046891,   -0.803433,   -0.966735,
	0.475578,   -0.652644,   0.0112459,   -0.730007,    0.128283,    0.145647,
  -0.619318,    0.272023,    0.392966,    0.646418,  -0.0207675,   -0.315908,
	0.480797,    0.535668,   -0.250172,    -0.83093,   -0.653773,   -0.443809,
	0.119982,   -0.897642,     0.89453,    0.165789,    0.633875,   -0.886839,
	0.930877,   -0.537194,    0.587732,    0.722011,   -0.209461,  -0.0424659,
  -0.814267,   -0.919432,    0.280262,    -0.66302,   -0.558099,   -0.537469,
  -0.598779,    0.929656,   -0.170794,   -0.537163,    0.312581,    0.959442,
	0.722652,    0.499931,    0.175616,   -0.534874,   -0.685115,    0.444999,
	 0.17171,    0.108202,   -0.768704,   -0.463828,    0.254231,    0.546014,
	0.869474,    0.875212,   -0.944427,    0.130724,   -0.110185,    0.312184,
	-0.33138,   -0.629206,   0.0606546,    0.722866,  -0.0979477,    0.821561,
  0.0931258,   -0.972808,   0.0318151,   -0.867033,   -0.387228,    0.280995,
  -0.218189,   -0.539178,   -0.427359,   -0.602075,    0.311971,    0.277974,
	0.773159,    0.592493,  -0.0331884,   -0.630854,   -0.269947,    0.339132,
	0.581079,    0.209461,   -0.317433,   -0.284993,    0.181323,    0.341634,
	0.804959,   -0.229572,   -0.758907,   -0.336721,    0.605463,   -0.991272,
 -0.0188754,   -0.300191,    0.368307,   -0.176135,     -0.3832,   -0.749569,
	 0.62356,   -0.573938,    0.278309,   -0.971313,    0.839994,   -0.830686,
	0.439078,     0.66128,    0.694514,   0.0565042,     0.54342,   -0.438804,
 -0.0228428,   -0.687068,    0.857267,    0.301991,   -0.494255,   -0.941039,
	0.775509,    0.410575,   -0.362081,   -0.671534,   -0.348379,    0.932433,
	0.886442,    0.868681,   -0.225666,   -0.062211,  -0.0976425,   -0.641444,
  -0.848112,    0.724697,    0.473503,    0.998749,    0.174701,    0.559625,
  -0.029099,   -0.337392,   -0.958129,   -0.659785,    0.236042,   -0.246937,
	0.659449,   -0.027512,    0.821897,   -0.226215,   0.0181735,    0.500481,
  -0.420127,   -0.427878,    0.566186
};

#ifdef __cplusplus
extern "C"
{
#endif

/*************
 * DESCRIPTION:   Get memory for noise table and initialize
 * INPUT:         none
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL InitNoise(short **hashTable)
{
	int i, j, temp;
	short *hT;

	hT = (short*)malloc(4096*sizeof(short));
	if(!hT)
		return FALSE;

	for (i=0; i<4096; i++)
		hT[i] = i;

	srand(1);
	for (i=4095;i>0; i--)
	{
		j = (int)(rand()%4096);
		temp = hT[i];
		hT[i] = hT[j];
		hT[j] = temp;
	}

	*hashTable = hT;

	return TRUE;
}

/*************
 * DESCRIPTION:   Free memory for noise table
 * INPUT:         none
 * OUTPUT:        none
 *************/
void CleanupNoise(short *hashTable)
{
	if(hashTable)
		free(hashTable);
}

/*************
 * DESCRIPTION:   Noise function
 * INPUT:         v        noise position
 * OUTPUT:        noise
 *************/
float Noise(VECTOR *v, short *hashTable)
{
	int ix, iy, iz, jx, jy, jz;
	float x, y, z;
	float sx, sy, sz, tx, ty, tz;
	float sum,sum1,sum2, sumj,sumj1,sumj2, res;
	short m;
	float t,s,ts1,ts2;
	float *incrsum_tmp;

	/* ensures the values are positive. */
	x = v->x - MINX;
	y = v->y - MINY;
	z = v->z - MINZ;

	/* its equivalent integer lattice point. */
	ix = (int)floor(x);
	iy = (int)floor(y);
	iz = (int)floor(z);
	jx = ix + 1;
	jy = iy + 1;
	jz = iz + 1;

	sum = x - ix;
	sumj = x - jx;
	sx = SCURVE(sum);
	sum1 = y - iy;
	sumj1 = y - jy;
	sy = SCURVE(sum1);
	sum2 = z - iz;
	sumj2 = z - jz;
	sz = SCURVE(sum2);

	/* the complement values of sx,sy,sz */
	tx = 1.f - sx;
	ty = 1.f - sy;
	tz = 1.f - sz;

	t = ty*tz;
	s = sx*sy;
	ts1 = ty*sz;
	ts2 = tx*sy;

	m = Hash3d( ix, iy, iz ) & 0xFF;
	INCRSUM(res= , m, (tx*t),   sum,  sum1,  sum2);

	m = Hash3d( jx, iy, iz ) & 0xFF;
	INCRSUM(res+=, m, (sx*t),   sumj, sum1,  sum2);

	m = Hash3d( ix, jy, iz ) & 0xFF;
	INCRSUM(res+=, m, (ts2*tz), sum,  sumj1, sum2);

	m = Hash3d( jx, jy, iz ) & 0xFF;
	INCRSUM(res+=, m, (s*tz),   sumj, sumj1, sum2);

	m = Hash3d( ix, iy, jz ) & 0xFF;
	INCRSUM(res+=, m, (tx*ts1), sum,  sum1,  sumj2);

	m = Hash3d( jx, iy, jz ) & 0xFF;
	INCRSUM(res+=, m, (sx*ts1), sumj, sum1,  sumj2);

	m = Hash3d( ix, jy, jz ) & 0xFF;
	INCRSUM(res+=, m, (ts2*sz), sum,  sumj1, sumj2);

	m = Hash3d( jx, jy, jz ) & 0xFF;
	INCRSUM(res+=, m, (s*sz),   sumj, sumj1, sumj2);

	return res;
}

/*************
 * DESCRIPTION:   Vector-valued noise function
 * INPUT:         v        noise position
 *                r        result vector
 * OUTPUT:        none
 *************/
void DNoise(VECTOR *v, VECTOR *r, short *hashTable)
{
	int ix, iy, iz, jx, jy, jz;
	float x, y, z;
	float px, py, pz, s;
	float sx, sy, sz, tx, ty, tz;
	short m;
	float tytz,sxsy,tysz,txsy;
	float *incrsum_tmp;

	/* ensures the values are positive. */
	x = v->x - MINX;
	y = v->y - MINY;
	z = v->z - MINZ;

	/* its equivalent integer lattice point. */
	ix = (int)floor(x);
	iy = (int)floor(y);
	iz = (int)floor(z);
	jx = ix + 1;
	jy = iy + 1;
	jz = iz + 1;

	sx = SCURVE(x - ix);
	sy = SCURVE(y - iy);
	sz = SCURVE(z - iz);

	/* the complement values of sx,sy,sz */
	tx = 1.f - sx;
	ty = 1.f - sy;
	tz = 1.f - sz;

	tytz = ty*tz;
	sxsy = sx*sy;
	tysz = ty*sz;
	txsy = tx*sy;

	/*
	 *  interpolate!
	 */
	m = Hash3d( ix, iy, iz ) & 0xFF;
	px = x-ix;  py = y-iy;  pz = z-iz;
	s = tx*tytz;
	INCRSUM(r->x=, m,  s,px,py,pz);
	INCRSUM(r->y=, m+4,s,px,py,pz);
	INCRSUM(r->z=, m+8,s,px,py,pz);

	m = Hash3d( jx, iy, iz ) & 0xFF;
	px = x-jx;
	s = sx*tytz;
	INCRSUM(r->x+=, m,  s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( jx, jy, iz ) & 0xFF;
	py = y-jy;
	s = sxsy*tz;
	INCRSUM(r->x+=, m,  s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( ix, jy, iz ) & 0xFF;
	px = x-ix;
	s = txsy*tz;
	INCRSUM(r->x+=, m,  s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( ix, jy, jz ) & 0xFF;
	pz = z-jz;
	s = txsy*sz;
	INCRSUM(r->x+=, m,  s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( jx, jy, jz ) & 0xFF;
	px = x-jx;
	s = sxsy*sz;
	INCRSUM(r->x+=, m,  s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( jx, iy, jz ) & 0xFF;
	py = y-iy;
	s = sx*tysz;
	INCRSUM(r->x+=, m,  s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( ix, iy, jz ) & 0xFF;
	px = x-ix;
	s = tx*tysz;
	INCRSUM(r->x+=, m,  s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);
}

/*************
 * DESCRIPTION:   Noise function
 * INPUT:         v        noise position
 * OUTPUT:        noise
 *************/
float VLNoise(VECTOR *v, short *hashTable)
{
	VECTOR offset, r;

	offset.x = v->x + 0.5f;        /* misregister domain */
	offset.y = v->y + 0.5f;
	offset.z = v->z + 0.5f;

	DNoise( &offset, &r, hashTable);   /* get a random vector */

	/* ``point'' is the domain; distort domain by adding ``offset'' */
	r.x += v->x;
	r.y += v->y;
	r.z += v->z;

	return Noise(&r, hashTable);         /* distorted-domain noise */

}

/*************
 * DESCRIPTION:   Turbulence function
 * INPUT:         v        noise position
 *                octaves  turbulence octaves
 * OUTPUT:        turbulence
 *************/
float Turbulence(VECTOR *v, int octaves, short *hashTable)
{
	float s, t;
	VECTOR tp;

	s = 1.f;
	t = 0.f;
	tp = *v;

	while(octaves--)
	{
		t += fabs(Noise(&tp, hashTable) * s);
		s *= 0.5f;
		tp.x += tp.x;
		tp.y += tp.y;
		tp.z += tp.z;
	}

	if(t>1.f)
		t = 1.f;
	if(t<0.f)
		t = 0.f;
	return t;
}

/*************
 * DESCRIPTION:   Procedural fBm
 * INPUT:         v           position
 *                H           fractal increment parameter
 *                lacunarity  gap between successive frequencies
 *                octaves     number of frequencies in the fBm
 * OUTPUT:
 *************/
float fBm(VECTOR *vec, float H, float lacunarity, int octaves, short *hashTable)
{
	float v,f;
	int i;
	VECTOR p;

	v = 0.f;
	f = 1.f;
	p = *vec;
	for(i=0; i<octaves; i++)
	{
		v += Noise(&p, hashTable) * pow(f,-H);
		p.x *= lacunarity;
		p.y *= lacunarity;
		p.z *= lacunarity;
		f *= lacunarity;
	}
	return v;
}

/*************
 * DESCRIPTION:   Vector-based procedural fBm
 * INPUT:         v           position
 *                H           fractal increment parameter
 *                lacunarity  gap between successive frequencies
 *                octaves     number of frequencies in the fBm
 *                r           result vector
 * OUTPUT:        -
 *************/
void VfBm(VECTOR *vec, float omega, float lambda, int octaves, VECTOR *r, short *hashTable)
{
	float l,o;
	int i;
	VECTOR p, res;

	r->x = 0.f;
	r->y = 0.f;
	r->z = 0.f;
	l = 1.f;
	o = 1.f;
	p = *vec;
	for(i=0; i<octaves; i++)
	{
		p.x *= l;
		p.y *= l;
		p.z *= l;
		DNoise(&p, &res, hashTable);
		r->x += res.x * o;
		r->y += res.y * o;
		r->z += res.z * o;
		l *= lambda;
		o *= omega;
	}
}

/*************
 * DESCRIPTION:   like procedural fBm but uses VLNoise as base function
 * INPUT:         v           position
 *                H           fractal increment parameter
 *                lacunarity  gap between successive frequencies
 *                octaves     number of frequencies in the fBm
 * OUTPUT:
 *************/
float VLfBm(VECTOR *vec, float H, float lacunarity, int octaves, short *hashTable)
{
	float v,f;
	int i;
	VECTOR p;

	v = 0.f;
	f = 1.f;
	p = *vec;
	for(i=0; i<octaves; i++)
	{
		v += VLNoise(&p, hashTable) * pow(f,-H);
		p.x *= lacunarity;
		p.y *= lacunarity;
		p.z *= lacunarity;
		f *= lacunarity;
	}
	return v;
}

#ifdef __cplusplus
};
#endif


