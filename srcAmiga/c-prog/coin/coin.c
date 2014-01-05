/*
 * Ueberlegungen: Es soll eine Muenze rollen, langsam umkippen und dann immer schneller werdend kreisen, um
 *   schliesslich liegen zu bleiben
 * - die Muenze mit dem Radius 1 steht am Anfang senkrecht
 * - der Mittelpunkt bewegt sich in einer Spiralform
 * - die Neigung folgt dieser Form entsprechend
 */

#include <stdio.h>
#include <math.h>

#include "/rexxlib/rexxlib.h"
#include "pragma/dos_lib.h"

#ifndef PI
#define PI 3.1415
#endif

typedef struct
{
	float x,y,z;
} VECTOR;

#define FRAMES		41
#define TIME_PER_FRAME	(1./(FRAMES-1))
#define ROTATIONS	1
#define RADIUS		10.

#define EPSILON 1e-7

static char buffer[256];

void SendMsg( char *msg )
{
	int result;

	Rexx_SendMsg( msg, &result );
	if( result )
		printf( "Command %s returned %d\n", msg, result );
}

float VecNormalize( VECTOR *v )
{
	float d, d1;

	d = v->x*v->x + v->y*v->y + v->z*v->z;

	if( d < EPSILON )
		return 0.;

	d = sqrt( d );
	d1 = 1/d;
	v->x *= d1;
	v->y *= d1;
	v->z *= d1;

	return d;
}

void SetVector( VECTOR *v, float x, float y, float z )
{
	v->x = x;
	v->y = y;
	v->z = z;
}

void PrintVector( VECTOR *v )
{
	printf( "%.2f %.2f %.2f; ", v->x, v->y, v->z );
}

void main()
{
	float time;
	float dist = 0;
	VECTOR pos;
	VECTOR angle;
	int frame;

	if( Rexx_Init("RAYSTORM") )
	{
		printf( "Can't find RayStorm" );
		exit(0);
	}

	// do initialiations
	SendMsg("OBJECTPATH ray:objects");
	SendMsg("SETSCREEN 100 100");
	SendMsg("SETCAMERA <0,80,0> <0,0,0> <0,0,1> 20 20");
	SendMsg("POINTLIGHT <100,50,50>");

/*	SendMsg("NEWSURFACE plane");
	SendMsg("DIFFUSE [50,100,255]");
	SendMsg("PLANE plane");*/

	SendMsg("NEWSURFACE coin");

	sprintf( buffer,"NEWACTOR coin <%.2f,1,0>",RADIUS );
	SendMsg( buffer );

	for( time = 0; time <= 1.01 ; time += TIME_PER_FRAME )
	{
		SetVector( &pos, ( 1. - time ) * RADIUS * cos( time * ROTATIONS * 2 * PI ),
			1 - time * time,
			( 1. - time ) * RADIUS * sin( time * ROTATIONS * 2 * PI ) );

		dist += RADIUS * ( 1. - time + 0.5 * TIME_PER_FRAME ) * ROTATIONS / FRAMES;
		SetVector( &angle, dist * 360.,
			time * ROTATIONS * 360, 0 );
//			asin( 1 - time*time) * 180 / PI );

		sprintf( buffer, "POSITION %.2f %.2f <%.2f,%.2f,%.2f>",
			time, time+TIME_PER_FRAME, pos.x, pos.y, pos.z );
		SendMsg( buffer );
		sprintf( buffer, "ALIGNMENT %.2f %.2f <%.2f,%.2f,%.2f>",
			time, time+TIME_PER_FRAME, angle.x, angle.y, angle.z );
		SendMsg( buffer );
	}

	sprintf( buffer,"LOADOBJ coin.iob <%.2f,1,0> <0,0,90> SURFACE=coin ACTOR=coin", RADIUS );
//	sprintf( buffer,"SPHERE coin <%.2f,1,0> 1 ACTOR=coin", RADIUS );
	SendMsg( buffer );

	frame = 0;
	for( time = 0; time <= 1 ; time += TIME_PER_FRAME )
	{
		sprintf( buffer, "STARTRENDER QUICK FROM=%2f TO=%2f", time, time+TIME_PER_FRAME );
		SendMsg( buffer );
		sprintf( buffer, "SAVEPIC coin%03d.iff", frame );
		SendMsg( buffer );
/*		sprintf( buffer, "view coin%03d.iff", frame );
		Execute( buffer, NULL, NULL );*/
		frame++;
	}

	SendMsg("CLEANUP");

	Rexx_Cleanup();

	exit(0);
}
