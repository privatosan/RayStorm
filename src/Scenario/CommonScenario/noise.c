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

static unsigned short xtab[256] =
{
	0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
	0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
	0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
	0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
	0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
	0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
	0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
	0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
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
BOOL InitNoise(float **RTable, short **hashTable)
{
	int i, j, temp;
	float *RT;
	short *hT;

	hT = (short*)malloc(4096*sizeof(short));
	if(!hT)
		return FALSE;
	RT = (float*)malloc(sizeof(float)*MAXSIZE);
	if(!RT)
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

	for (i=0; i<MAXSIZE; i++)
		RT[i] = xtab[i%256]*REALSCALE - 1.f;

	*RTable = RT;
	*hashTable = hT;

	return TRUE;
}

/*************
 * DESCRIPTION:   Free memory for noise table
 * INPUT:         none
 * OUTPUT:        none
 *************/
void CleanupNoise(float *RTable, short *hashTable)
{
	if(hashTable)
		free(hashTable);
	if(RTable)
		free(RTable);
}

/*************
 * DESCRIPTION:   Noise function
 * INPUT:         v        noise position
 * OUTPUT:        noise
 *************/
float Noise(VECTOR *v, float *RTable, short *hashTable)
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
void DNoise(VECTOR *v, VECTOR *r, float *RTable, short *hashTable)
{
	int ix, iy, iz, jx, jy, jz;
	float x, y, z;
	float px, py, pz, s;
	float sx, sy, sz, tx, ty, tz;
	short m;
	float *incrsum_tmp;

	/* ensures the values are positive. */
	x = v->x - MINX;
	y = v->y - MINY;
	z = v->z - MINZ;

	/* its equivalent integer lattice point. */
	ix = (int)floor(x);
	iy = (int)floor(y);
	iz = (int)floor(z);
	jx = ix+1;
	jy = iy + 1;
	jz = iz + 1;

	sx = SCURVE(x - ix);
	sy = SCURVE(y - iy);
	sz = SCURVE(z - iz);

	/* the complement values of sx,sy,sz */
	tx = 1.f - sx;
	ty = 1.f - sy;
	tz = 1.f - sz;

	/*
	 *  interpolate!
	 */
	m = Hash3d( ix, iy, iz ) & 0xFF;
	px = x-ix;  py = y-iy;  pz = z-iz;
	s = tx*ty*tz;
	INCRSUM(r->x=, m,s,px,py,pz);
	INCRSUM(r->y=, m+4,s,px,py,pz);
	INCRSUM(r->z=, m+8,s,px,py,pz);

	m = Hash3d( jx, iy, iz ) & 0xFF;
	px = x-jx;
	s = sx*ty*tz;
	INCRSUM(r->x+=, m,s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( jx, jy, iz ) & 0xFF;
	py = y-jy;
	s = sx*sy*tz;
	INCRSUM(r->x+=, m,s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( ix, jy, iz ) & 0xFF;
	px = x-ix;
	s = tx*sy*tz;
	INCRSUM(r->x+=, m,s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( ix, jy, jz ) & 0xFF;
	pz = z-jz;
	s = tx*sy*sz;
	INCRSUM(r->x+=, m,s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( jx, jy, jz ) & 0xFF;
	px = x-jx;
	s = sx*sy*sz;
	INCRSUM(r->x+=, m,s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( jx, iy, jz ) & 0xFF;
	py = y-iy;
	s = sx*ty*sz;
	INCRSUM(r->x+=, m,s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);

	m = Hash3d( ix, iy, jz ) & 0xFF;
	px = x-ix;
	s = tx*ty*sz;
	INCRSUM(r->x+=, m,s,px,py,pz);
	INCRSUM(r->y+=, m+4,s,px,py,pz);
	INCRSUM(r->z+=, m+8,s,px,py,pz);
}

/*************
 * DESCRIPTION:   Turbulence function
 * INPUT:         v        noise position
 *                octaves  turbulence octaves
 * OUTPUT:        turbulence
 *************/
float Turbulence(VECTOR *v, int octaves, float *RTable, short *hashTable)
{
	float s, t;
	VECTOR tp;

	s = 1.f;
	t = 0.f;
	tp = *v;

	while(octaves--)
	{
		t += fabs(Noise(&tp, RTable, hashTable) * s);
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
#ifdef __MAXON__
float fBm(VECTOR *vec, float H_, float lacunarity_, int octaves, float *RTable, short *hashTable)
{
	float H = H_;
	float lacunarity = lacunarity_;
#else
float fBm(VECTOR *vec, float H, float lacunarity, int octaves, float *RTable, short *hashTable)
{
#endif
	float v,f;
	int i;
	VECTOR p;

	v = 0.f;
	f = 1.f;
	p = *vec;
	for(i=0; i<octaves; i++)
	{
		v += Noise(&p, RTable, hashTable) * pow(f,-H);
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
#ifdef __MAXON__
void VfBm(VECTOR *vec, float omega_, float lambda_, int octaves, VECTOR *r, float *RTable, short *hashTable)
{
	float omega = omega_;
	float lambda = lambda_;
#else
void VfBm(VECTOR *vec, float omega, float lambda, int octaves, VECTOR *r, float *RTable, short *hashTable)
{
#endif
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
		DNoise(&p, &res, RTable, hashTable);
		r->x += res.x * o;
		r->y += res.y * o;
		r->z += res.z * o;
		l *= lambda;
		o *= omega;
	}
}

#ifdef __cplusplus
};
#endif
