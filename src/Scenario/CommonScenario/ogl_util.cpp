/***************
 * PROGRAM:       Modeler
 * NAME:          ogl_util.cpp
 * DESCRIPTION:   OpenGL utility functions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    15.03.98 ah    initial release
 ***************/

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef OPENGL_UTIL_H
#include "ogl_util.h"
#endif

#ifndef MAGIC_H
#include "magic.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef OPENGL_H
#include "opengl.h"
#endif // OPENGL_H

#include <gl/glu.h>

/*************
 * DESCRIPTION:   Convert a matrix and load it to the opengl matrix stack
 * INPUT:         m     Matrix in scenario format
 * OUTPUT:        -
 *************/
void OGLLoadMatrix(MATRIX *m)
{
	GLfloat mat[16];

	mat[0] = m->m[5];
	mat[1] = m->m[6];
	mat[2] = m->m[7];
	//mat[3] = m->m[4];
	mat[3] = m->m[12];
	mat[4] = m->m[9];
	mat[5] = m->m[10];
	mat[6] = m->m[11];
	mat[7] = m->m[8];
	mat[8] = m->m[13];
	mat[9] = m->m[14];
	mat[10] = m->m[15];
	//mat[11] = m->m[12];
	mat[11] = m->m[4];
	mat[12] = m->m[1];
	mat[13] = m->m[2];
	mat[14] = m->m[3];
	mat[15] = m->m[0];
	glMultMatrixf(mat);
}

extern COLOR colors[COLOR_MAX];
void SetGLColor(int index)
{
	float color[4];

	color[0] = colors[index].r;
	color[1] = colors[index].g;
	color[2] = colors[index].b;
	color[3] = 1.f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)color);

	color[0] = color[1] = color[2] = color[3] = 0.f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)color);
}


void OpenGLDrawMesh(TRIANGLE *trias, EDGE *edges, VECTOR *points, VECTOR *normals, ULONG acttrias)
{
	USHORT p1,p2,p3;
	ULONG i;
#ifndef NORMAL
	VECTOR e1,e2,n;
#endif // NORMAL

	glBegin(GL_TRIANGLES);
	for(i=0; i<acttrias; i++)
	{
		p1 = edges[trias->e[0]].p[0];
		p2 = edges[trias->e[0]].p[1];
		if((p1 != edges[trias->e[1]].p[0]) && (p2 != edges[trias->e[1]].p[0]))
			p3 = edges[trias->e[1]].p[0];
		else
			p3 = edges[trias->e[1]].p[1];

#ifdef NORMAL
		glNormal3fv((GLfloat*)&normals[trias->n[0]]);
		glVertex3fv((GLfloat*)&points[p1]);
		glNormal3fv((GLfloat*)&normals[trias->n[1]]);
		glVertex3fv((GLfloat*)&points[p2]);
		glNormal3fv((GLfloat*)&normals[trias->n[2]]);
		glVertex3fv((GLfloat*)&points[p3]);
#else // NORMAL
		VecSub(&points[p3], &points[p1], &e1);
		VecSub(&points[p3], &points[p2], &e2);
		VecNormCross(&e2, &e1, &n);
		glNormal3fv((GLfloat*)&n);
		glVertex3fv((GLfloat*)&points[p1]);
		glVertex3fv((GLfloat*)&points[p2]);
		glVertex3fv((GLfloat*)&points[p3]);
#endif // NORMAL
		trias++;
	}
	glEnd();
}

void OpenGLDrawBox(VECTOR *min, VECTOR *max)
{
	VECTOR v1,v2,v3,v4,v5,v6,v7,v8;

	v1 = *min;
	SetVector(&v2, min->x, min->y, max->z);
	SetVector(&v3, min->x, max->y, min->z);
	SetVector(&v4, min->x, max->y, max->z);
	SetVector(&v5, max->x, min->y, min->z);
	SetVector(&v6, max->x, min->y, max->z);
	SetVector(&v7, max->x, max->y, min->z);
	v8 = *max;
	glBegin(GL_QUADS);
	glNormal3f(0.f, 0.f, -1.f);
	glVertex3fv((GLfloat*)&v1);
	glVertex3fv((GLfloat*)&v5);
	glVertex3fv((GLfloat*)&v7);
	glVertex3fv((GLfloat*)&v3);

	glNormal3f(0.f, 1.f, 0.f);
	glVertex3fv((GLfloat*)&v3);
	glVertex3fv((GLfloat*)&v7);
	glVertex3fv((GLfloat*)&v8);
	glVertex3fv((GLfloat*)&v4);

	glNormal3f(0.f, 0.f, 1.f);
	glVertex3fv((GLfloat*)&v4);
	glVertex3fv((GLfloat*)&v8);
	glVertex3fv((GLfloat*)&v6);
	glVertex3fv((GLfloat*)&v2);

	glNormal3f(0.f, -1.f, 0.f);
	glVertex3fv((GLfloat*)&v2);
	glVertex3fv((GLfloat*)&v6);
	glVertex3fv((GLfloat*)&v5);
	glVertex3fv((GLfloat*)&v1);

	glNormal3f(1.f, 0.f, 0.f);
	glVertex3fv((GLfloat*)&v5);
	glVertex3fv((GLfloat*)&v6);
	glVertex3fv((GLfloat*)&v8);
	glVertex3fv((GLfloat*)&v7);

	glNormal3f(-1.f, 0.f, 0.f);
	glVertex3fv((GLfloat*)&v2);
	glVertex3fv((GLfloat*)&v1);
	glVertex3fv((GLfloat*)&v3);
	glVertex3fv((GLfloat*)&v4);
	glEnd();
}


void OpenGLDrawCamera(DISPLAY *disp, float dx,float dy,float dz, float dx1,float dy1,float dz1, VECTOR *size)
{
	VECTOR u,v;
	MATRIX m;

	glBegin(GL_QUADS);
	glNormal3f(0.f, 0.f, 1.f);
	glVertex3f(-dx1, -dy1, dz1);
	glVertex3f(-dx1,  dy1, dz1);
	glVertex3f( dx1,  dy1, dz1);
	glVertex3f( dx1, -dy1, dz1);

	glNormal3f(-0.957826f, 0.f, -0.287348f);
	glVertex3f(-dx1, -dy1, dz1);
	glVertex3f(-dx1,  dy1, dz1);
	glVertex3f( -dx,   dy,  dz);
	glVertex3f( -dx,  -dy,  dz);

	glNormal3f(0.957826f, 0.f, -0.287348f);
	glVertex3f( dx1, -dy1, dz1);
	glVertex3f( dx1,  dy1, dz1);
	glVertex3f(  dx,   dy,  dz);
	glVertex3f(  dx,  -dy,  dz);

	glNormal3f(0.f, -0.957826f, -0.287348f);
	glVertex3f(-dx1, -dy1, dz1);
	glVertex3f( dx1, -dy1, dz1);
	glVertex3f(  dx,  -dy,  dz);
	glVertex3f( -dx,  -dy,  dz);

	glNormal3f(0.f, 0.957826f, -0.287348f);
	glVertex3f(-dx1,  dy1, dz1);
	glVertex3f( dx1,  dy1, dz1);
	glVertex3f(  dx,   dy,  dz);
	glVertex3f( -dx,   dy,  dz);
	glEnd();

	SetVector(&u, 0.f, size->z*.9f, -size->z*.4f);
	m.SetTMatrix(&u);
	glPushMatrix();
	OGLLoadMatrix(&m);
	m.SetRotXMatrix(45.f);
	glPushMatrix();
	OGLLoadMatrix(&m);
	SetVector(&u, -size->z*.1f, -size->z*.283f, -size->z*.283f);
	SetVector(&v,  size->z*.1f,  size->z*.283f,  size->z*.283f);
	OpenGLDrawBox(&u,&v);
	glPopMatrix();
	glPopMatrix();

	SetVector(&u, 0.f, size->z*.9f, size->z*.4f);
	m.SetTMatrix(&u);
	glPushMatrix();
	OGLLoadMatrix(&m);
	m.SetRotXMatrix(45.f);
	glPushMatrix();
	OGLLoadMatrix(&m);
	SetVector(&u, -size->z*.1f, -size->z*.283f, -size->z*.283f);
	SetVector(&v,  size->z*.1f,  size->z*.283f,  size->z*.283f);
	OpenGLDrawBox(&u,&v);
	glPopMatrix();
	glPopMatrix();
}

void OpenGLDrawCone(VECTOR *s, ULONG flags)
{
	VECTOR bottom,bottom1, centertop, size;
	int i;
	VECTOR norm, e0,e1;
	static GLuint dlist = 0;

	if(!dlist)
	{
		dlist = glGenLists(2);
		if(dlist)
			SetVector(&size, 1.f,1.f,1.f);
		else
			size = *s;

		if(!(flags & OBJECT_OPENBOTTOM))
		{
			if(dlist)
				glNewList(dlist+1, GL_COMPILE);
			else
				glEnable(GL_CULL_FACE);
			glBegin(GL_TRIANGLE_FAN);
			glNormal3f(0.f, -1.f, 0.f);
			glVertex3f(0.f, 0.f, 0.f);

			for(i = 0; i <= 360; i += OGL_CONE_DIVS)
				glVertex3f(sin(i*PI_180)*size.x, 0.f, cos(i*PI_180)*size.z);

			glEnd();
			if(dlist)
				glEndList();
		}

		if(dlist)
			glNewList(dlist, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.f, 1.f, 0.f);
		SetVector(&centertop, 0.f, size.y, 0.f);
		glVertex3fv((GLfloat*)&centertop);

		SetVector(&bottom, 0.f, 0.f, size.z);
		SetVector(&bottom1, sin((360-OGL_CONE_DIVS)*PI_180)*size.x, 0.f, cos((360-OGL_CONE_DIVS)*PI_180)*size.z);
		VecSub(&centertop, &bottom, &e0);
		VecSub(&centertop, &bottom1, &e1);
		VecNormCross(&e0, &e1, &norm);
		glNormal3fv((GLfloat*)&norm);
		glVertex3fv((GLfloat*)&bottom);

		for(i = 360-2*OGL_CONE_DIVS; i >= -OGL_CONE_DIVS; i -= OGL_CONE_DIVS)
		{
			bottom = bottom1;
			SetVector(&bottom1, sin(i*PI_180)*size.x, 0.f, cos(i*PI_180)*size.z);
			VecSub(&centertop, &bottom, &e0);
			VecSub(&centertop, &bottom1, &e1);
			VecNormCross(&e0, &e1, &norm);
			glNormal3fv((GLfloat*)&norm);
			glVertex3fv((GLfloat*)&bottom);
		}

		glEnd();

		if(dlist)
			glEndList();
		else
		{
			if(!(flags & OBJECT_OPENBOTTOM))
				glDisable(GL_CULL_FACE);
		}
	}

	if(dlist)
	{
		glPushMatrix();
		glScalef(s->x, s->y, s->z);
		if(!(flags & OBJECT_OPENBOTTOM))
		{
			glEnable(GL_CULL_FACE);
			glCallList(dlist+1);
		}
		glCallList(dlist);
		if(!(flags & OBJECT_OPENBOTTOM))
			glDisable(GL_CULL_FACE);
		glPopMatrix();
	}
}

void OpenGLDrawSphere(float radius)
{
	VECTOR u,v;
	int i;
	float f1,f2, r;
	int j;
	static GLuint dlist = 0;

	if(!dlist)
	{
		dlist = glGenLists(1);
		if(dlist)
		{
			glNewList(dlist, GL_COMPILE);
			r = 1.f;
		}
		else
			r = radius;

		glEnable(GL_CULL_FACE);

		// Bottom
		glBegin(GL_TRIANGLE_FAN);
		f1 = -cos((90-OGL_SPHERE_DIVS)*PI_180);
		f2 = -cos(OGL_SPHERE_DIVS*PI_180);
		SetVector(&u, 0.f, f2, f1);
		f2 *= r;

		glNormal3f(0.f, -1.f, 0.f);
		glVertex3f(0.f, -r, 0.f);

		for(i = 0; i <= 360; i += OGL_SPHERE_DIVS)
		{
			u.x = sin(i*PI_180)*f1;
			u.z = cos(i*PI_180)*f1;
			glNormal3fv((GLfloat*)&u);
			glVertex3f(u.x*r, f2, u.z*r);
		}
		glEnd();

		for(j=OGL_SPHERE_DIVS; j<180-OGL_SPHERE_DIVS; j+=OGL_SPHERE_DIVS)
		{
			v.y = -cos((j+OGL_SPHERE_DIVS)*PI_180);
			u.y = -cos(j*PI_180);
			f1 = -sin((j+OGL_SPHERE_DIVS)*PI_180);
			f2 = -sin(j*PI_180);
			glBegin(GL_TRIANGLE_STRIP);
			for(i = 0; i <= 360; i += OGL_SPHERE_DIVS)
			{
				v.x = u.x = sin(i*PI_180);
				v.z = u.z = cos(i*PI_180);
				u.x *= f2;
				u.z *= f2;
				v.x *= f1;
				v.z *= f1;
				glNormal3fv((GLfloat*)&u);
				glVertex3f(u.x*r, u.y*r, u.z*r);
				glNormal3fv((GLfloat*)&v);
				glVertex3f(v.x*r, v.y*r, v.z*r);
			}
			glEnd();
		}

		// Top
		glBegin(GL_TRIANGLE_FAN);
		f1 = -cos((90-OGL_SPHERE_DIVS)*PI_180);
		f2 = cos(OGL_SPHERE_DIVS*PI_180);
		SetVector(&u, 0.f, f2, f1);
		f2 *= r;

		glNormal3f(0.f, 1.f, 0.f);
		glVertex3f(0.f, r, 0.f);

		for(i = 360; i >= 0; i -= OGL_SPHERE_DIVS)
		{
			u.x = sin(i*PI_180)*f1;
			u.z = cos(i*PI_180)*f1;
			glNormal3fv((GLfloat*)&u);
			glVertex3f(u.x*r, f2, u.z*r);
		}
		glEnd();

		glDisable(GL_CULL_FACE);

		if(dlist)
			glEndList();
	}

	if(dlist)
	{
		glPushMatrix();
		glScalef(radius, radius, radius);
		glCallList(dlist);
		glPopMatrix();
	}
}

void OpenGLDrawCylinder(VECTOR *size, ULONG flags)
{
	VECTOR bottom, norm;
	int i;

	if(!(flags & (OBJECT_OPENTOP | OBJECT_OPENBOTTOM)))
		glEnable(GL_CULL_FACE);

	if(!(flags & OBJECT_OPENBOTTOM))
	{
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.f, -1.f, 0.f);
		glVertex3f(0.f, 0.f, 0.f);

		for(i = 0; i <= 360; i += OGL_CYLINDER_DIVS)
			glVertex3f(sin(i*PI_180)*size->x, 0.f, cos(i*PI_180)*size->z);

		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	for(i = 0; i <= 360; i += OGL_CYLINDER_DIVS)
	{
		SetVector(&norm, sin(i*PI_180), 0.f, cos(i*PI_180));

		SetVector(&bottom, norm.x*size->x, 0.f, norm.z*size->z);

		glNormal3fv((GLfloat*)&norm);
		glVertex3fv((GLfloat*)&bottom);
		glVertex3f(bottom.x, size->y, bottom.z);
	}
	glEnd();

	if(!(flags & OBJECT_OPENTOP))
	{
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.f, 1.f, 0.f);
		glVertex3f(0.f, size->y, 0.f);

		for(i = 360; i >= 0; i -= OGL_CYLINDER_DIVS)
			glVertex3f(sin(i*PI_180)*size->x, size->y, cos(i*PI_180)*size->z);

		glEnd();
	}

	if(!(flags & (OBJECT_OPENTOP | OBJECT_OPENBOTTOM)))
		glDisable(GL_CULL_FACE);
}

void OpenGLDrawPointLight(VECTOR *s)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glVertex3fv((GLfloat*)s);
	glVertex3f(-s->x, -s->y, -s->z);

	glVertex3f(-s->x,  s->y,  s->z);
	glVertex3f( s->x, -s->y, -s->z);

	glVertex3f( s->x,  s->y, -s->z);
	glVertex3f(-s->x, -s->y,  s->z);

	glVertex3f(-s->x,  s->y, -s->z);
	glVertex3f( s->x, -s->y,  s->z);
	glEnd();
	glEnable(GL_LIGHTING);
}

void OpenGLDrawSpotLight(VECTOR *s, VECTOR *size, float angle)
{
	float t;
	VECTOR u;

	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glVertex3fv((GLfloat*)s);
	glVertex3f(-s->x, -s->y, -s->z);

	glVertex3f(-s->x,  s->y,  s->z);
	glVertex3f( s->x, -s->y, -s->z);

	glVertex3f( s->x,  s->y, -s->z);
	glVertex3f(-s->x, -s->y,  s->z);

	glVertex3f(-s->x,  s->y, -s->z);
	glVertex3f( s->x, -s->y,  s->z);

	t = tan(angle) * size->x * 10.f;
	SetVector(&u, 0.f, 0.f, 0.f);

	glVertex3fv((GLfloat*)&u);
	glVertex3f( t, 0.f, size->x * 10.f);

	glVertex3fv((GLfloat*)&u);
	glVertex3f(-t, 0.f, size->x * 10.f);

	glVertex3fv((GLfloat*)&u);
	glVertex3f(0.f,  t, size->x * 10.f);

	glVertex3fv((GLfloat*)&u);
	glVertex3f(0.f, -t, size->x * 10.f);

	t *= 0.707f;
	glVertex3fv((GLfloat*)&u);
	glVertex3f( t,  t, size->x * 10.f);

	glVertex3fv((GLfloat*)&u);
	glVertex3f(-t,  t, size->x * 10.f);

	glVertex3fv((GLfloat*)&u);
	glVertex3f(-t, -t, size->x * 10.f);

	glVertex3fv((GLfloat*)&u);
	glVertex3f( t, -t, size->x * 10.f);

	glEnd();
	glEnable(GL_LIGHTING);
}

void OpenGLDrawPlane(VECTOR *size)
{
	glBegin(GL_QUADS);
	glNormal3f(0.f, 1.f, 0.f);
	glVertex3f(-size->x, 0.f, -size->z);
	glVertex3f( size->x, 0.f, -size->z);
	glVertex3f( size->x, 0.f,  size->z);
	glVertex3f(-size->x, 0.f,  size->z);
	glEnd();
}

void OpenGLDrawSelBox(VECTOR *min, VECTOR *min1, VECTOR *max, VECTOR *max1)
{
	SetGLColor(COLOR_SELBOX);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glVertex3fv((GLfloat*)max);
	glVertex3f(max1->x, max->y, max->z);
	glVertex3fv((GLfloat*)max);
	glVertex3f(max->x, max1->y, max->z);
	glVertex3fv((GLfloat*)max);
	glVertex3f(max->x, max->y, max1->z);

	glVertex3f(max->x, max->y, min->z);
	glVertex3f(max1->x, max->y, min->z);
	glVertex3f(max->x, max->y, min->z);
	glVertex3f(max->x, max1->y, min->z);
	glVertex3f(max->x, max->y, min->z);
	glVertex3f(max->x, max->y, min1->z);

	glVertex3f(max->x, min->y, max->z);
	glVertex3f(max1->x, min->y, max->z);
	glVertex3f(max->x, min->y, max->z);
	glVertex3f(max->x, min1->y, max->z);
	glVertex3f(max->x, min->y, max->z);
	glVertex3f(max->x, min->y, max1->z);

	glVertex3f(max->x, min->y, min->z);
	glVertex3f(max1->x, min->y, min->z);
	glVertex3f(max->x, min->y, min->z);
	glVertex3f(max->x, min1->y, min->z);
	glVertex3f(max->x, min->y, min->z);
	glVertex3f(max->x, min->y, min1->z);

	glVertex3f(min->x, max->y, max->z);
	glVertex3f(min1->x, max->y, max->z);
	glVertex3f(min->x, max->y, max->z);
	glVertex3f(min->x, max1->y, max->z);
	glVertex3f(min->x, max->y, max->z);
	glVertex3f(min->x, max->y, max1->z);

	glVertex3f(min->x, max->y, min->z);
	glVertex3f(min1->x, max->y, min->z);
	glVertex3f(min->x, max->y, min->z);
	glVertex3f(min->x, max1->y, min->z);
	glVertex3f(min->x, max->y, min->z);
	glVertex3f(min->x, max->y, min1->z);

	glVertex3f(min->x, min->y, max->z);
	glVertex3f(min1->x, min->y, max->z);
	glVertex3f(min->x, min->y, max->z);
	glVertex3f(min->x, min1->y, max->z);
	glVertex3f(min->x, min->y, max->z);
	glVertex3f(min->x, min->y, max1->z);

	glVertex3fv((GLfloat*)min);
	glVertex3f(min1->x, min->y, min->z);
	glVertex3fv((GLfloat*)min);
	glVertex3f(min->x, min1->y, min->z);
	glVertex3fv((GLfloat*)min);
	glVertex3f(min->x, min->y, min1->z);
	glEnd();
	glEnable(GL_LIGHTING);
}

void OpenGLDrawGrid(DISPLAY *display)
{
	float dx, dy, t1, t2, viewh, viewv;
	int i;
	float delta;

	SetGLColor(COLOR_GRID);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x8888);

	switch (display->view->viewmode)
	{
		case VIEW_CAMERA:
		case VIEW_PERSP:
			dx = -SUBDIV*display->gridsize;
			t1 = SUBDIV*display->gridsize;
			glBegin(GL_LINES);
			for (i = 0; i <= 2*SUBDIV; i++)
			{
				glVertex3f(t1, 0.f, dx);
				glVertex3f(-t1, 0.f, dx);

				glVertex3f(dx, 0.f, t1);
				glVertex3f(dx, 0.f, -t1);
				dx += display->gridsize;
			}
			glEnd();
			glDisable(GL_LINE_STIPPLE);
			return;

		case VIEW_FRONT:
			viewh = display->view->viewpoint.x;
			viewv = display->view->viewpoint.y;
			break;

		case VIEW_RIGHT:
			viewh = display->view->viewpoint.z;
			viewv = display->view->viewpoint.y;
			break;

		case VIEW_TOP:
			viewh = display->view->viewpoint.x;
			viewv = display->view->viewpoint.z;
			break;
	}

	t1 = display->view->width/display->view->zoom*0.5f + 2*display->gridsize;
	t2 = display->view->height/display->view->zoom*0.5f + 2*display->gridsize;
	dx = viewh - fmod(viewh, display->gridsize) - (t1 - fmod(t1, display->gridsize));
	dy = viewv - fmod(viewv, display->gridsize) - (t2 - fmod(t2, display->gridsize));

	delta = 0.f;
	t1 *= 2.f;
	t2 *= 2.f;
	if (display->view->zoom*display->gridsize > MIN_GRIDPIXELS)
	{
		switch (display->view->viewmode)
		{
			case VIEW_FRONT:
				glBegin(GL_LINES);
				while(delta < t1)
				{
					glVertex3f(dx+delta, dy, 0.f);
					glVertex3f(dx+delta, dy+t2, 0.f);
					delta += display->gridsize;
				}
				delta = 0.f;
				while(delta < t2)
				{
					glVertex3f(dx, dy+delta, 0.f);
					glVertex3f(dx+t1, dy+delta, 0.f);
					delta += display->gridsize;
				}
				glEnd();
				glDisable(GL_LINE_STIPPLE);
				glBegin(GL_LINES);
				glVertex3f(0.f, dy, 0.f);
				glVertex3f(0.f, dy+t2, 0.f);
				glVertex3f(dx, 0.f, 0.f);
				glVertex3f(dx+t1, 0.f, 0.f);
				glEnd();
				break;
			case VIEW_RIGHT:
				glBegin(GL_LINES);
				while(delta < t1)
				{
					glVertex3f(0.f, dy, dx+delta);
					glVertex3f(0.f, dy+t2, dx+delta);
					delta += display->gridsize;
				}
				delta = 0.f;
				while(delta < t2)
				{
					glVertex3f(0.f, dy+delta, dx);
					glVertex3f(0.f, dy+delta, dx+t1);
					delta += display->gridsize;
				}
				glEnd();
				glDisable(GL_LINE_STIPPLE);
				glBegin(GL_LINES);
				glVertex3f(0.f, dy, 0.f);
				glVertex3f(0.f, dy+t2, 0.f);
				glVertex3f(0.f, 0.f, dx);
				glVertex3f(0.f, 0.f, dx+t1);
				glEnd();
				break;
			case VIEW_TOP:
				glBegin(GL_LINES);
				while(delta < t1)
				{
					glVertex3f(dx+delta, 0.f, dy);
					glVertex3f(dx+delta, 0.f, dy+t2);
					delta += display->gridsize;
				}
				delta = 0.f;
				while(delta < t2)
				{
					glVertex3f(dx, 0.f, dy+delta);
					glVertex3f(dx+t1, 0.f, dy+delta);
					delta += display->gridsize;
				}
				glEnd();
				glDisable(GL_LINE_STIPPLE);
				glBegin(GL_LINES);
				glVertex3f(0.f, 0.f, dy);
				glVertex3f(0.f, 0.f, dy+t2);
				glVertex3f(dx, 0.f, 0.f);
				glVertex3f(dx+t1, 0.f, 0.f);
				glEnd();
				break;
		}
	}
}

void OpenGLDrawDirectionalLight(VECTOR *size)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glVertex3f(-size->x, -size->y, 0.f);
	glVertex3f(-size->x, -size->y, size->z);
	glVertex3f(-size->x, 0.f, 0.f);
	glVertex3f(-size->x, 0.f, size->z);
	glVertex3f(-size->x, size->y, 0.f);
	glVertex3f(-size->x, size->y, size->z);
	glVertex3f(0.f, -size->y, 0.f);
	glVertex3f(0.f, -size->y, size->z);
	glVertex3f(0.f, size->y, 0.f);
	glVertex3f(0.f, size->y, size->z);
	glVertex3f(size->x, -size->y, 0.f);
	glVertex3f(size->x, -size->y, size->z);
	glVertex3f(size->x, 0.f, 0.f);
	glVertex3f(size->x, 0.f, size->z);
	glVertex3f(size->x, size->y, 0.f);
	glVertex3f(size->x, size->y, size->z);
	glEnd();
	glEnable(GL_LIGHTING);
}

void OpenGLDrawRenderLines(DISPLAY *display, RECTANGLE *r, BOOL remove)
{
	int w, h;
	GLfloat projmatrix[16];

	if(remove)
		SetGLColor(COLOR_BKGROUND);
	else
		SetGLColor(COLOR_UNSELECTED);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x8888);
	glGetFloatv(GL_PROJECTION_MATRIX, projmatrix);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		0,
		display->view->width,
		0,
		display->view->height,
		-100.f, 100.f);

	if (r->top == display->view->top)
	{
		glBegin(GL_LINES);
		glVertex2f((float)r->left, (float)r->top);
		glVertex2f((float)r->left, (float)r->bottom);
		glVertex2f((float)r->right, (float)r->top);
		glVertex2f((float)r->right, (float)r->bottom);
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
		glVertex2f((float)r->left, (float)r->top);
		glVertex2f((float)r->right, (float)r->top);
		glVertex2f((float)r->left, (float)r->bottom);
		glVertex2f((float)r->right, (float)r->bottom);
		glEnd();
	}

	if (global.enablearea)
	{
		w = r->right - r->left;
		h = r->bottom - r->top;
		glBegin(GL_LINES);
		glVertex2f(w*global.xmin + r->left, display->view->height - h*global.ymin + r->top);
		glVertex2f(w*global.xmax + r->left, display->view->height - h*global.ymin + r->top);
		glVertex2f(w*global.xmax + r->left, display->view->height - h*global.ymin + r->top);
		glVertex2f(w*global.xmax + r->left, display->view->height - h*global.ymax + r->top);
		glVertex2f(w*global.xmax + r->left, display->view->height - h*global.ymax + r->top);
		glVertex2f(w*global.xmin + r->left, display->view->height - h*global.ymax + r->top);
		glVertex2f(w*global.xmin + r->left, display->view->height - h*global.ymax + r->top);
		glVertex2f(w*global.xmin + r->left, display->view->height - h*global.ymin + r->top);
		glEnd();

	}
	glDisable(GL_LINE_STIPPLE);
	glLoadMatrixf(projmatrix);
	glMatrixMode(GL_MODELVIEW);
}

void OpenGLSetupCamera(DISPLAY *display, COLOR *clearcolor, VECTOR *pos, VECTOR *look, VECTOR *up, float near_dist, float far_dist)
{
	glClearColor(clearcolor->r, clearcolor->g, clearcolor->b, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(display->camera->vfov*2.f*INV_PI_180, -(float)display->view->width/(float)display->view->height, near_dist, far_dist);
	gluLookAt(
		pos->x, pos->y, pos->z,
		look->x, look->y, look->z,
		up->x, up->y, up->z);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DITHER);
	glDisable(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void OpenGLSetupPerspective(DISPLAY *display, COLOR *clearcolor, VECTOR *pos, VECTOR *look, VECTOR *up, float near_dist, float far_dist)
{
	glClearColor(clearcolor->r, clearcolor->g, clearcolor->b, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((float)global.yres/(float)global.xres*INV_PI_180, -(float)display->view->width/(float)display->view->height, near_dist, far_dist);
	gluLookAt(
		pos->x*display->view->dist, pos->y*display->view->dist, pos->z*display->view->dist,
		look->x, look->y, look->z,
		up->x, up->y, up->z);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DITHER);
	glDisable(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void OpenGLSetupRight(DISPLAY *display, COLOR *clearcolor)
{
	static GLfloat right_matrix[16] =
	{
		0.f, 0.f, -1.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	glClearColor(clearcolor->r, clearcolor->g, clearcolor->b, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		-(display->view->width>>1)/display->view->zoom + display->view->viewpoint.z,
		(display->view->width>>1)/display->view->zoom + display->view->viewpoint.z,
		-(display->view->height>>1)/display->view->zoom + display->view->viewpoint.y,
		(display->view->height>>1)/display->view->zoom + display->view->viewpoint.y,
		display->bboxmax.x, display->bboxmin.x);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(right_matrix);
	glEnable(GL_DITHER);
	glDisable(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void OpenGLSetupTop(DISPLAY *display, COLOR *clearcolor)
{
	static GLfloat top_matrix[16] =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, -1.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	glClearColor(clearcolor->r, clearcolor->g, clearcolor->b, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		-(display->view->width>>1)/display->view->zoom + display->view->viewpoint.x,
		(display->view->width>>1)/display->view->zoom + display->view->viewpoint.x,
		-(display->view->height>>1)/display->view->zoom + display->view->viewpoint.z,
		(display->view->height>>1)/display->view->zoom + display->view->viewpoint.z,
		display->bboxmax.y, display->bboxmin.y);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(top_matrix);
	glEnable(GL_DITHER);
	glDisable(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void OpenGLSetupFront(DISPLAY *display, COLOR *clearcolor)
{
	static GLfloat front_matrix[16] =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, -1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	glClearColor(clearcolor->r, clearcolor->g, clearcolor->b, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		-(display->view->width>>1)/display->view->zoom + display->view->viewpoint.x,
		(display->view->width>>1)/display->view->zoom + display->view->viewpoint.x,
		-(display->view->height>>1)/display->view->zoom + display->view->viewpoint.y,
		(display->view->height>>1)/display->view->zoom + display->view->viewpoint.y,
		display->bboxmin.z, display->bboxmax.z);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(front_matrix);
	glEnable(GL_DITHER);
	glDisable(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void OpenGLInitSelect(int x, int y, int left, int right, int top, int bottom, ULONG *select_buffer)
{
	GLint vp[4];

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	vp[0] = left;
	vp[1] = top;
	vp[2] = right-left+1;
	vp[3] = bottom-top+1;
	gluPickMatrix(x, vp[3]-y, 2,2, vp);
	glPushMatrix();

	glSelectBuffer(OPENGL_SELECT_BUFFER_SIZE, (GLuint*)select_buffer);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);
}

ULONG OpenGLQuitSelect()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	return (ULONG)glRenderMode(GL_RENDER);
}

