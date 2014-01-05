/***************
 * PROGRAM:       Modeler
 * NAME:          ogl_util.h
 * DESCRIPTION:   OpenGL utility functions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    15.03.98 ah    initial release
 ***************/

#ifndef OGL_UTIL_H
#define OGL_UTIL_h

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

void OpenGLDrawMesh(TRIANGLE*, EDGE*, VECTOR*, VECTOR*, ULONG);
void OpenGLDrawCamera(DISPLAY *disp, float dx,float dy,float dz, float dx1,float dy1,float dz1, VECTOR *size);
void OpenGLDrawCone(VECTOR *size, ULONG flags);
void OpenGLDrawBox(VECTOR *min, VECTOR *max);
void OpenGLDrawSphere(float radius);
void OpenGLDrawCylinder(VECTOR *size, ULONG flags);
void OpenGLDrawPointLight(VECTOR *s);
void OpenGLDrawSpotLight(VECTOR *s, VECTOR *size, float angle);
void OpenGLDrawDirectionalLight(VECTOR *size);
void OpenGLDrawPlane(VECTOR *size);
void OpenGLDrawSelBox(VECTOR *min, VECTOR *min1, VECTOR *max, VECTOR *max1);
void OpenGLDrawGrid(DISPLAY *display);
void OpenGLDrawRenderLines(DISPLAY *display, RECTANGLE *r, BOOL remove);
void OpenGLSetupCamera(DISPLAY *display, COLOR *clearcolor, VECTOR *pos, VECTOR *look, VECTOR *up, float near, float far);
void OpenGLSetupPerspective(DISPLAY *display, COLOR *clearcolor, VECTOR *pos, VECTOR *look, VECTOR *up, float near, float far);
void OpenGLSetupRight(DISPLAY *display, COLOR *clearcolor);
void OpenGLSetupTop(DISPLAY *display, COLOR *clearcolor);
void OpenGLSetupFront(DISPLAY *display, COLOR *clearcolor);

void OGLLoadMatrix(MATRIX*);

#endif
