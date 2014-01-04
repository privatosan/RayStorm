/***************
 * PROGRAM:       Modeler
 * NAME:          opengl.cpp
 * DESCRIPTION:   OpenGL platform specific implementation
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.03.98 ah    initial release
 ***************/

#ifndef OPENGL_H
#include "opengl.h"
#endif

#include <gl/glu.h>

BOOL OPENGL::SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
		1,											// version number
		PFD_DRAW_TO_WINDOW |					// support window
		PFD_SUPPORT_OPENGL |					// support OpenGL
		PFD_DOUBLEBUFFER,						// double buffered
		PFD_TYPE_RGBA,							// RGBA type
		24,										// 24-bit color depth
		0, 0, 0, 0, 0, 0,						// color bits ignored
		0,											// no alpha buffer
		0,											// shift bit ignored
		0,											// no accumulation buffer
		0, 0, 0, 0,								// accum bits ignored
		32,										// 32-bit z-buffer
		0,											// no stencil buffer
		0,											// no auxiliary buffer
		PFD_MAIN_PLANE,						// main layer
		0,											// reserved
		0, 0, 0									// layer masks ignored
	};
	int pixelformat;

	if ((pixelformat = ChoosePixelFormat(pDC->GetSafeHdc(), &pfd)) == 0)
		return FALSE;

	if (SetPixelFormat(pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
		return FALSE;

	return TRUE;
}


BOOL OPENGL::CreateContext(ULONG left, ULONG top, ULONG width, ULONG height)
{
	int		n;
	GLfloat	fMaxObjSize, fAspect;
	GLfloat	fNearPlane, fFarPlane;
	HDC hDC;

	if(context)
		DestroyContext();
   
	pDC = pView->GetDC();

	if (!SetupPixelFormat())
		return FALSE;

	hDC = pDC->GetSafeHdc();
	context = wglCreateContext(hDC);
	if (!context)
		return FALSE;

	if(!wglMakeCurrent(hDC, context))
	{
		DWORD err;
		err = GetLastError();
		return FALSE;
	}

	m_oldRect.SetRect(left, top, left + width, top + height);

	return TRUE;
}

void OPENGL::SwapBuffers()
{
	::SwapBuffers(pDC->GetSafeHdc());
}

void OPENGL::DestroyContext()
{
	::wglMakeCurrent(NULL,  NULL);

	if (context)
		::wglDeleteContext(context);

	context = NULL;
}

void OPENGL::MakeCurrent()
{
	wglMakeCurrent(pDC->GetSafeHdc(), context);
}

void OPENGL::Viewport(int left, int top, int width, int height)
{
	if(context)
	{
		MakeCurrent();
		glViewport(left, top, width, height);
	}
}
