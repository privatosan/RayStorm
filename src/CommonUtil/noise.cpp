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
		  -1.f,    0.604974f,   -0.937102f,    0.414115f,    0.576226f,  -0.0161593f,
	0.432334f,    0.103685f,    0.590539f,   0.0286412f,     0.46981f,    -0.84622f,
 -0.0734112f,   -0.304097f,    -0.40206f,   -0.210132f,   -0.919127f,    0.652033f,
	-0.83151f,   -0.183948f,   -0.671107f,    0.852476f,    0.043595f,   -0.404532f,
	 0.75494f,   -0.335653f,    0.618433f,    0.605707f,    0.708583f,   -0.477195f,
	0.899474f,    0.490623f,    0.221729f,   -0.400381f,   -0.853727f,   -0.932586f,
	0.659113f,    0.961303f,    0.325948f,   -0.750851f,    0.842466f,    0.734401f,
  -0.649866f,    0.394491f,   -0.466056f,   -0.434073f,    0.109026f,   0.0847028f,
  -0.738857f,    0.241505f,     0.16228f,    -0.71426f,   -0.883665f,   -0.150408f,
	-0.90396f,   -0.686549f,   -0.785214f,    0.488548f,   0.0246433f,    0.142473f,
  -0.602136f,    0.375845f, -0.00779736f,    0.498955f,   -0.268147f,    0.856382f,
  -0.386007f,   -0.596094f,   -0.867735f,   -0.570977f,   -0.914366f,     0.28896f,
	0.672206f,   -0.233783f,     0.94815f,    0.895262f,    0.343252f,   -0.173388f,
  -0.767971f,   -0.314748f,    0.824308f,   -0.342092f,    0.721431f,    -0.24004f,
	-0.63653f,    0.553277f,    0.376272f,    0.158984f,   -0.452659f,    0.396323f,
  -0.420676f,   -0.454154f,    0.122179f,    0.295857f,   0.0664225f,   -0.202075f,
  -0.724788f,    0.453513f,    0.224567f,   -0.908812f,    0.176349f,   -0.320516f,
  -0.697139f,    0.742702f,   -0.900786f,    0.471489f,   -0.133532f,    0.119127f,
  -0.889769f,    -0.23183f,   -0.669673f,   -0.046891f,   -0.803433f,   -0.966735f,
	0.475578f,   -0.652644f,   0.0112459f,   -0.730007f,    0.128283f,    0.145647f,
  -0.619318f,    0.272023f,    0.392966f,    0.646418f,  -0.0207675f,   -0.315908f,
	0.480797f,    0.535668f,   -0.250172f,    -0.83093f,   -0.653773f,   -0.443809f,
	0.119982f,   -0.897642f,     0.89453f,    0.165789f,    0.633875f,   -0.886839f,
	0.930877f,   -0.537194f,    0.587732f,    0.722011f,   -0.209461f,  -0.0424659f,
  -0.814267f,   -0.919432f,    0.280262f,    -0.66302f,   -0.558099f,   -0.537469f,
  -0.598779f,    0.929656f,   -0.170794f,   -0.537163f,    0.312581f,    0.959442f,
	0.722652f,    0.499931f,    0.175616f,   -0.534874f,   -0.685115f,    0.444999f,
	 0.17171f,    0.108202f,   -0.768704f,   -0.463828f,    0.254231f,    0.546014f,
	0.869474f,    0.875212f,   -0.944427f,    0.130724f,   -0.110185f,    0.312184f,
	-0.33138f,   -0.629206f,   0.0606546f,    0.722866f,  -0.0979477f,    0.821561f,
  0.0931258f,   -0.972808f,   0.0318151f,   -0.867033f,   -0.387228f,    0.280995f,
  -0.218189f,   -0.539178f,   -0.427359f,   -0.602075f,    0.311971f,    0.277974f,
	0.773159f,    0.592493f,  -0.0331884f,   -0.630854f,   -0.269947f,    0.339132f,
	0.581079f,    0.209461f,   -0.317433f,   -0.284993f,    0.181323f,    0.341634f,
	0.804959f,   -0.229572f,   -0.758907f,   -0.336721f,    0.605463f,   -0.991272f,
 -0.0188754f,   -0.300191f,    0.368307f,   -0.176135f,     -0.3832f,   -0.749569f,
	 0.62356f,   -0.573938f,    0.278309f,   -0.971313f,    0.839994f,   -0.830686f,
	0.439078f,     0.66128f,    0.694514f,   0.0565042f,     0.54342f,   -0.438804f,
 -0.0228428f,   -0.687068f,    0.857267f,    0.301991f,   -0.494255f,   -0.941039f,
	0.775509f,    0.410575f,   -0.362081f,   -0.671534f,   -0.348379f,    0.932433f,
	0.886442f,    0.868681f,   -0.225666f,   -0.062211f,  -0.0976425f,   -0.641444f,
  -0.848112f,    0.724697f,    0.473503f,    0.998749f,    0.174701f,    0.559625f,
  -0.029099f,   -0.337392f,   -0.958129f,   -0.659785f,    0.236042f,   -0.246937f,
	0.659449f,   -0.027512f,    0.821897f,   -0.226215f,   0.0181735f,    0.500481f,
  -0.420127f,   -0.427878f,    0.566186f
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
		t += (float)fabs(Noise(&tp, hashTable) * s);
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
		v += Noise(&p, hashTable) * (float)pow(f,-H);
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
		v += VLNoise(&p, hashTable) * (float)pow(f,-H);
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


