#include <math.h>
#include <time.h>
#include <stdio.h>

double estinv(double);
double estinvsqrt(double);

void main()
{
	int i;
	clock_t time, overall=0;
	double t;

	/*printf("%f\n", atan2(1.23f, 2.345f));
	printf("%f\n", atan2_ppc(1.23f, 2.345f));

	printf("%f\n", atan(1.23f));
	printf("%f\n", atan_ppc(1.23f));

	printf("%f\n", pow(2.34f, 1.234f));
	printf("%f\n", pow_ppc(2.34f, 1.234f));

	printf("%f\n", exp(2.34f));
	printf("%f\n", exp_ppc(2.34f));

	printf("%f\n", sqrt(2.34f));
	printf("%f\n", sqrt_ppc(2.34f));

	printf("%f\n", sin(2.34f));
	printf("%f\n", sin_ppc(2.34f));

	printf("%f\n", cos(2.34f));
	printf("%f\n", cos_ppc(2.34f));

	printf("%f\n", acos(0.1234f));
	printf("%f\n", acos_ppc(0.1234f));*/

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = acos(t);
	}
	time = clock()-time;
	overall += time;
	printf("acos %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = atan(t);
	}
	time = clock()-time;
	overall += time;
	printf("atan %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = atan2(t, 1.2345f);
	}
	time = clock()-time;
	overall += time;
	printf("atan2 %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = sqrt(t);
	}
	time = clock()-time;
	overall += time;
	printf("sqrt %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = pow(t, t);
	}
	time = clock()-time;
	overall += time;
	printf("pow %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = exp(t);
	}
	time = clock()-time;
	overall += time;
	printf("exp %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = sin(t);
	}
	time = clock()-time;
	overall += time;
	printf("sin %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = cos(t);
	}
	time = clock()-time;
	overall += time;
	printf("cos %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = log(t);
	}
	time = clock()-time;
	overall += time;
	printf("log %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = log10(t);
	}
	time = clock()-time;
	overall += time;
	printf("log10 %d\n", time);

	/*time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = estinvsqrt(t);
	}
	time = clock()-time;
	overall += time;
	printf("estinvsqrt %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = 1./sqrt(t);
	}
	time = clock()-time;
	overall += time;
	printf("invsqrt %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = estinv(t);
	}
	time = clock()-time;
	overall += time;
	printf("estinv %d\n", time);

	time = clock();
	for(i=0; i<1000000; i++)
	{
		t = i*10.f+0.123f;
		t = 1.f/t;
	}
	time = clock()-time;
	overall += time;
	printf("inv %d\n", time);*/

	printf("overall %d\n", overall);
}
