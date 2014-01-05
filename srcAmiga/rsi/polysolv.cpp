/***************
 * MODUL:         polysolv
 * NAME:          polysolv.cpp
 * DESCRIPTION:   Some functions to solve polynomial equations
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    31.03.97 ah    initial release
 ***************/

#include <math.h>
#include <stdlib.h>

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#define COEFF_LIMIT 1.0e-16
#define MAX_ORDER 4
#define MAX_ITERATIONS 50
#define COEFF_LIMIT 1.0e-16

typedef struct
{
	int ord;
	double coef[MAX_ORDER+1];
} POLYNOMIAL;

/*************
 * DESCRIPTION:   Solve the quadratic equation:
 *     x[0] * x^2 + x[1] * x + x[2] = 0.
 *    The roots themselves are returned in y[0], y[1].
 * INPUT:         x,y
 * OUTPUT:        number of real roots
 *************/
int SolveQuadratic(double *x, double *y)
{
	double d, t, a, b, c;

	a = x[0];
	b = -x[1];
	c = x[2];

	if(a == 0.)
	{
		if(b == 0.)
			return 0;

		y[0] = c / b;

		return 1;
	}

	d = b * b - 4. * a * c;

	// Treat values of d around 0 as 0.
	if(d < -EPSILON)
	 return 0;
	else
	{
		if(d < COEFF_LIMIT)
		{
			y[0] = 0.5 * b / a;
			return 1;
		}
	}

	d = sqrt(d);

	t = 1. / (2.0 * a);

	y[0] = (b + d) * t;
	y[1] = (b - d) * t;

	return 2;
}

/*************
 * DESCRIPTION:   Solve the cubic equation:
 *
 *    x[0] * x^3 + x[1] * x^2 + x[2] * x + x[3] = 0.
 *
 *    The result of this function is an integer that tells how many real
 *    roots exist.  Determination of how many are distinct is up to the
 *    process that calls this routine.  The roots that exist are stored
 *    in (y[0], y[1], y[2]).
 * INPUT:         x,y
 * OUTPUT:        number of real roots
 *************/
int SolveCubic(double *x, double *y)
{
	double Q, R, Q3, R2, sQ, d, an, theta;
	double A2, a0, a1, a2, a3;

	if(fabs(x[0]) < COEFF_LIMIT)
	{
		return SolveQuadratic(&x[1], y);
	}
	else
	{
		a0 = 1./x[0];
		if(a0 != 1.)
		{
			a1 = x[1] * a0;
			a2 = x[2] * a0;
			a3 = x[3] * a0;
		}
		else
		{
			a1 = x[1];
			a2 = x[2];
			a3 = x[3];
		}
	}

	A2 = a1 * a1;
	Q = (A2 - 3. * a2) * 0.11111111;
	R = (a1 * (A2 - 4.5 * a2) + 13.5 * a3) * 0.0370370370;
	Q3 = Q * Q * Q;
	R2 = R * R;
	d = Q3 - R2;
	an = a1 * 0.33333333;

	if (d >= 0.)
	{
		// Three real roots
		d = R / sqrt(Q3);
		theta = acos(d) * 0.33333333;
		sQ = -2.0 * sqrt(Q);

		y[0] = sQ * cos(theta) - an;
		y[1] = sQ * cos(theta + TWO_PI_3) - an;
		y[2] = sQ * cos(theta + TWO_PI_43) - an;

		return 3;
	}

	sQ = pow(sqrt(R2 - Q3) + fabs(R), 0.33333333);

	if(R < 0.)
		y[0] = (sQ + Q / sQ) - an;
	else
		y[0] = -(sQ + Q / sQ) - an;

	return 1;
}

/*************
 * DESCRIPTION:   Calculates the modulus of u(x) / v(x) leaving it in r.
 *    It returns 0 if r(x) is a constant.
 *
 *    NOTE: This function assumes the leading coefficient of v is 1 or -1.
 * INPUT:         u,v
 *                r
 * OUTPUT:
 *************/
static int Modp(POLYNOMIAL *u, POLYNOMIAL *v, POLYNOMIAL *r)
{
	int i, k, j;

	for(i = 0; i < u->ord; i++)
		r[i] = u[i];

	if(v->coef[v->ord] < 0.0)
	{
		for(k = u->ord - v->ord - 1; k >= 0; k -= 2)
			r->coef[k] = -r->coef[k];

		for(k = u->ord - v->ord; k >= 0; k--)
		{
			for(j = v->ord + k - 1; j >= k; j--)
			{
				r->coef[j] = -r->coef[j] - r->coef[v->ord + k] * v->coef[j - k];
			}
		}
	}
	else
	{
		for(k = u->ord - v->ord; k >= 0; k--)
		{
			for(j = v->ord + k - 1; j >= k; j--)
			{
				r->coef[j] -= r->coef[v->ord + k] * v->coef[j - k];
			}
		}
	}

	k = v->ord - 1;

	while (k >= 0 && fabs(r->coef[k]) < COEFF_LIMIT)
	{
		r->coef[k] = 0.0;
		k--;
	}

	r->ord = (k < 0) ? 0 : k;

	return r->ord;
}

/*************
 * DESCRIPTION:   Build the sturmian sequence for a polynomial
 * INPUT:         ord
 *                sseq
 * OUTPUT:
 *************/
static int BuildSturm(int ord, POLYNOMIAL *sseq)
{
	int i;
	double f, *fp, *fc;
	POLYNOMIAL *sp;

	sseq[0].ord = ord;
	sseq[1].ord = ord - 1;

	// calculate the derivative and normalize the leading coefficient
	f = 1./fabs(sseq[0].coef[ord] * ord);
	fp = sseq[1].coef;
	fc = sseq[0].coef + 1;

	for (i = 1; i <= ord; i++)
		*fp++ = *fc++ * i * f;

	// construct the rest of the Sturm sequence
	for (sp = sseq + 2; Modp(sp - 2, sp - 1, sp); sp++)
	{
		// reverse the sign and normalize
		f = -1./fabs(sp->coef[sp->ord]);

		for (fp = &sp->coef[sp->ord]; fp >= sp->coef; fp--)
			*fp *= f;
	}

	// reverse the sign
	sp->coef[0] = -sp->coef[0];

	return(sp - sseq);
}

/*************
 * DESCRIPTION:   Find out how many visible intersections there are
 * INPUT:         np
 *                sseq
 *                atzer
 *                atpos
 * OUTPUT:
 *************/
static int VisibleRoots(int np, POLYNOMIAL *sseq, int *atzer, int *atpos)
{
	int atposinf, atzero;
	POLYNOMIAL *s;
	double f, lf;

	atposinf = atzero = 0;

	// changes at positve infinity
	lf = sseq[0].coef[sseq[0].ord];

	for (s = sseq + 1; s <= sseq + np; s++)
	{
		f = s->coef[s->ord];

		if(lf == 0.0 || lf * f < 0)
			atposinf++;

		lf = f;
	}

	// changes at zero
	lf = sseq[0].coef[0];

	for (s = sseq+1; s <= sseq + np; s++)
	{
		f = s->coef[0];

		if(lf == 0.0 || lf * f < 0)
			atzero++;

		lf = f;
	}

	*atzer = atzero;
	*atpos = atposinf;

	return(atzero - atposinf);
}

/*************
 * DESCRIPTION:
 * INPUT:         x
 *                n
 *                coeffs
 * OUTPUT:
 *************/
static double PolyEval(double x, int n, double *coeffs)
{
  register int i;
  double val;

  val = coeffs[n];

  for(i = n-1; i >= 0; i--)
	 val = val * x + coeffs[i];

  return val;
}

/*************
 * DESCRIPTION:   Return the number of sign changes in the Sturm sequence in
 *    sseq at the value a.
 * INPUT:         np
 *                sseq
 *                a
 * OUTPUT:        sign changes
 *************/
static int NumChanges(int np, POLYNOMIAL *sseq, double a)
{
	int changes;
	double f, lf;
	POLYNOMIAL *s;

	changes = 0;

	lf = PolyEval(a, sseq[0].ord, sseq[0].coef);

	for (s = sseq + 1; s <= sseq + np; s++)
	{
		f = PolyEval(a, s->ord, s->coef);

		if(lf == 0.0 || lf * f < 0)
			changes++;

		lf = f;
	}

	return changes;
}

/*************
 * DESCRIPTION:   Close in on a root by using regula-falsa
 * INPUT:         order
 *                coef
 *                a
 *                b
 *                val
 * OUTPUT:
 *************/
static int RegulaFalsa(int order, double *coef, double a, double b, double *val)
{
	int its;
	double fa, fb, x, fx, lfx;

	fa = PolyEval(a, order, coef);
	fb = PolyEval(b, order, coef);

	if(fa * fb > 0.0)
		return 0;

	if(fabs(fa) < COEFF_LIMIT)
	{
		*val = a;
		return 1;
	}

	if(fabs(fb) < COEFF_LIMIT)
	{
		*val = b;
		return 1;
	}

	lfx = fa;

	for(its = 0; its < MAX_ITERATIONS; its++)
	{
		x = (fb * a - fa * b) / (fb - fa);

		fx = PolyEval(x, order, coef);

		if(fabs(x) > EPSILON)
		{
			if (fabs(fx / x) < EPSILON)
			{
				*val = x;
				return 1;
			}
		}
		else
		{
			if(fabs(fx) < EPSILON)
			{
				*val = x;
				return 1;
			}
		}

		if(fa < 0)
		{
			if(fx < 0)
			{
				a = x;
				fa = fx;

				if((lfx * fx) > 0)
					fb *= .5;
			}
			else
			{
				b = x;
				fb = fx;

				if((lfx * fx) > 0)
					fa *= .5;
			}
		}
		else
		{
			if(fx < 0)
			{
				b = x;
				fb = fx;

				if((lfx * fx) > 0)
					fa *= .5;
			}
			else
			{
				a = x;
				fa = fx;

				if((lfx * fx) > 0)
					fb *= .5;
			}
		}

		// Check for underflow in the domain
		if(fabs(b-a) < EPSILON)
		{
			*val = x;
			return 1;
		}

		lfx = fx;
	}

	return 0;
}

/*************
 * DESCRIPTION:   Uses a bisection based on the sturm sequence for the polynomial
 *    described in sseq to isolate intervals in which roots occur,
 *    the roots are returned in the roots array in order of magnitude.
 *
 *    NOTE: This routine has one severe bug: When the interval containing the
 *          root [min, max] has a root at one of its endpoints, as well as one
 *          within the interval, the root at the endpoint will be returned
 *          rather than the one inside.
 * INPUT:         np
 *                sseq
 *                min_value
 *                max_value
 *                atmin
 *                atmax
 *                roots
 * OUTPUT:
 *************/
static int SBisect(int np, POLYNOMIAL *sseq, double min_value, double max_value, int atmin, int atmax, double *roots)
{
	double mid;
	int n1, n2, its, atmid;

	if((atmin - atmax) == 1)
	{
		// first try using regula-falsa to find the root
		if(RegulaFalsa(sseq->ord, sseq->coef, min_value, max_value, roots))
			return 1;
		else
		{
			// That failed, so now find it by bisection
			for(its = 0; its < MAX_ITERATIONS; its++)
			{
				mid = (min_value + max_value) * .5;

				atmid = NumChanges(np, sseq, mid);

				if(fabs(mid) > EPSILON)
				{
					if(fabs((max_value - min_value) / mid) < EPSILON)
					{
						roots[0] = mid;
						return 1;
					}
				}
				else
				{
					if(fabs(max_value - min_value) < EPSILON)
					{
						roots[0] = mid;
						return 1;
					}
				}

				if((atmin - atmid) == 0)
					min_value = mid;
				else
					max_value = mid;
			}

			// Bisection took too long - just return what we got
			roots[0] = mid;

			return 1;
		}
	}

	// There is more than one root in the interval
	// Bisect to find new intervals
	for(its = 0; its < MAX_ITERATIONS; its++)
	{
		mid = (min_value + max_value) * .5;
		atmid = NumChanges(np, sseq, mid);

		n1 = atmin - atmid;
		n2 = atmid - atmax;

		if((n1 != 0) && (n2 != 0))
		{
			n1 = SBisect(np, sseq, min_value, mid, atmin, atmid, roots);
			n2 = SBisect(np, sseq, mid, max_value, atmid, atmax, &roots[n1]);

			return n1+n2;
		}

		if(n1 == 0)
			min_value = mid;
		else
			max_value = mid;
  }

  // Took too long to bisect. Just return what we got
  roots[0] = mid;

  return 1;
}

/*************
 * DESCRIPTION:   Root solver based on the Sturm sequences for a polynomial
 * INPUT:         order
 *                coeffs
 *                roots
 * OUTPUT:        number of real roots
 *************/
int PolySolve(int order, double *coeffs, double *roots)
{
	POLYNOMIAL sseq[MAX_ORDER+1];
	double min_value, max_value;
	int i, nroots, np, atmin, atmax;

	if(fabs(coeffs[0]) < COEFF_LIMIT)
		return SolveQuadratic(&coeffs[1], roots);

	// Put the coefficients into the top of the stack
	for(i = 0; i <= order; i++)
		sseq[0].coef[order-i] = coeffs[i];

	// Build the Sturm sequence
	np = BuildSturm(order, &sseq[0]);

	// Get the total number of visible roots
	if((nroots = VisibleRoots(np, sseq, &atmin, &atmax)) == 0)
		return 0;

	// Bracket the roots
	min_value = 0.0;
	max_value = 1.0e7;

	atmin = NumChanges(np, sseq, min_value);
	atmax = NumChanges(np, sseq, max_value);

	nroots = atmin - atmax;

	if(nroots == 0)
		return 0;

	// perform the bisection
	return(SBisect(np, sseq, min_value, max_value, atmin, atmax, roots));
}
