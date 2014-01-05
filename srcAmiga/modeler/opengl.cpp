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

#ifndef OBJECT_H
#include "object.h"
#endif

BOOL OPENGL::CreateContext(ULONG left, ULONG top, ULONG width, ULONG height, BOOL directrender)
{
	ULONG bottom;

	struct TagItem tags[] =
	{
		{(long)AMA_Window, (long)_window(win)},
		{AMA_Left,         _mleft(win)+left},
		{AMA_Bottom,       (_window(win))->Height-(top+height)-_mtop(win) - 1},
		{AMA_Width,        width + 1},
		{AMA_Height,       height + 1},
		{AMA_DoubleBuf,    TRUE},
		{AMA_RGBMode,      TRUE},
		{AMA_DirectRender, directrender},
		{TAG_DONE,         NULL},
	};

	left = _mleft(win)+left;
	bottom = (_window(win))->Height-(top+height)-_mtop(win) - 1;
	width = width + 1;
	height = height + 1;
	if(!context || (left != this->left) || (bottom != this->bottom) || (width != this->width) || (height != this->height))
	{
		this->left = left;
		this->bottom = bottom;
		this->width = width;
		this->height = height;

		if(context)
			DestroyContext();

		context = PPC_STUB(AmigaMesaCreateContext)(tags);
		if(!context)
			return FALSE;
	}
	return TRUE;
}

void OPENGL::SwapBuffers()
{
	PPC_STUB(AmigaMesaSwapBuffers)(context);
}

void OPENGL::DestroyContext()
{
	if(context)
	{
		PPC_STUB(AmigaMesaDestroyContext)(context);
		context = NULL;
	}
}

void OPENGL::MakeCurrent()
{
	if(context)
		PPC_STUB(AmigaMesaMakeCurrent)(context, context->buffer);
}

void OPENGL::Viewport(int left, int top, int width, int height)
{
	if(context)
	{
		MakeCurrent();
		PPC_STUB(glViewport)(
			left - (this->left - _mleft(win)),
			this->height - height,
			width, height);
	}
}
