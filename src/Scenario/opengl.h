/***************
 * PROGRAM:       Modeler
 * NAME:          opengl.h
 * DESCRIPTION:   OpenGL platform specific implementation
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.03.98 ah    initial release
 ***************/

#ifndef OPENGL_H
#define OPENGL_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#include <gl/gl.h>

#ifdef __AMIGA__
#include <gl/Amigamesa.h>
#include <libraries/mui.h>
#endif // __AMIGA__

#ifdef __AMIGA__
typedef struct amigamesa_context OPENGL_CONTEXT;
#else
typedef HGLRC OPENGL_CONTEXT;
#endif // __AMIGA__

class OPENGL
{
	private:
#ifdef __AMIGA__
		OPENGL_CONTEXT *context;
		Object *win;
		ULONG left,bottom,width,height;
#else
		OPENGL_CONTEXT context;
		CView *pView;
		CPalette m_cPalette, *m_pOldPalette;
		CDC *pDC;
		CRect m_oldRect;
#endif

	public:
#ifdef __AMIGA__
		OPENGL();
		void Initialize(Object *w) { win = w; }
#else
		void Initialize(CView *pView_) { pView = pView_; }
		BOOL SetupPixelFormat();
		unsigned char ComponentFromIndex(int, UINT, UINT);
		void CreateRGBPalette();
#endif
		void DestroyContext();
		OPENGL()
		{
			context = NULL;
#ifdef __AMIGA__
			left = 0xFFFFFFFF;
			bottom = 0xFFFFFFFF;
			width = 0xFFFFFFFF;
			height = 0xFFFFFFFF;
#endif
		}
		~OPENGL()
		{
			if(context)
				DestroyContext();
		}
#ifdef __AMIGA__
		BOOL CreateContext(ULONG, ULONG, ULONG, ULONG, BOOL);
#else // __AMIGA__
		BOOL CreateContext(ULONG, ULONG, ULONG, ULONG);
#endif // __AMIGA__
		void SwapBuffers();
		void MakeCurrent();
		BOOL IsInitialized() { return context ? TRUE : FALSE; }
		void Viewport(int, int, int, int);
};

#endif
