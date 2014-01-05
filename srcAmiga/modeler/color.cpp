/***************
 * PROGRAM:       Modeler
 * NAME:          color.cpp
 * DESCRIPTION:   Functions for the handling of colors
 * AUTHORS:       Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    31.01.98 ah    initial release
 ***************/

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#include <gl/gl.h>

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

static char *colornames[COLOR_MAX] =
{
	"Axis",
	"Background",
	"Brush",
	"Camera",
	"Grid",
	"Hotspot",
	"Unselected object",
	"Selected object",
	"Selection box",
	"Text",
	"Texture",
	"Light"
};

COLOR colors[COLOR_MAX] =
{
	{   1,   1,   1 },   // PEN_AXIS
	{ .6588, .6588, .6588 },   // PEN_BACKGROUND
	{   0,   1,   0 },   // PEN_BRUSH
	{   1,   1,   1 },   // PEN_CAMERA
	{   0,   0,   0 },   // PEN_GRID
	{   1,   0,   0 },   // PEN_HOTSPOT
	{   0,   0,   0 },   // PEN_UNSELECTED
	{   1,   1,   1 },   // PEN_SELECTED
	{   1,   0,   0 },   // PEN_SELBOX
	{   0,   0,   0 },   // PEN_TEXT
	{   0,   0,   1 },   // PEN_TEXTURE
	{   1,   1,   0 },   // PEN_LIGHT
};

char *GetColorString(ULONG index)
{
	if(index >= COLOR_MAX)
		return NULL;

	return colornames[index];
}

void GetColorValue(ULONG index, COLOR *c)
{
	if(index < COLOR_MAX)
		*c = colors[index];
}

void SetColorValue(ULONG index, COLOR *c)
{
	if(index < COLOR_MAX)
		colors[index] = *c;
}

void GetColorPrefs()
{
	int i;
	PREFS p;
	UBYTE *data;

	p.id = ID_COLR;
	if(p.GetPrefs())
	{
		data = (UBYTE*)p.data;
		for(i=0; i<COLOR_MAX && i<(p.length>>2); i++)
		{
			colors[i].r = (float)(data[i*4+1])/255.f;
			colors[i].g = (float)(data[i*4+2])/255.f;
			colors[i].b = (float)(data[i*4+3])/255.f;
		}
	}
	p.data = NULL;
}

void SetColorPrefs()
{
	int i;
	PREFS p;
	UBYTE *data;

	data = new UBYTE[COLOR_MAX*4];
	if(data)
	{
		for(i=0; i<COLOR_MAX; i++)
		{
			data[i*4+0] = 0;
			data[i*4+1] = (UBYTE)(colors[i].r*255.f);
			data[i*4+2] = (UBYTE)(colors[i].g*255.f);
			data[i*4+3] = (UBYTE)(colors[i].b*255.f);
		}

		p.id = ID_COLR;
		p.data = data;
		p.length = COLOR_MAX*4;
		p.AddPrefs();
	}
}

