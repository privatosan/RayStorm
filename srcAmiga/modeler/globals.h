/***************
 * PROGRAM:       Modeler
 * NAME:          globals.h
 * DESCRIPTION:   definitions for global data class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.06.96 ah    initial release
 *    13.12.96 ah    added minobjects
 *    06.02.97 ah    added SetRenderWindow()
 *    07.02.97 ah    added SetEnableArea() and GetEnableArea()
 *    25.04.97 ah    added enableback and enablerefl
 *    21.08.98 ah    values of the mesh create requester are now saved in the globals struct
 ***************/

#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class GLOBALS
{
	public:
		COLOR backcol;
		char *backpic;
		BOOL enableback;
		char *reflmap;
		BOOL enablerefl;
		char *renderdpic;             // output name
		char picformat[8];
		COLOR ambient;
		COLOR fog;
		float flen;
		float fheight;
		int antialias;                // antialiasing value
		COLOR anticont;               // antialiasing contrast
		float gauss;                  // gaussian filter width
		int distrib;
		int softshad;
		BOOL randjit,quick,show;
		int octreedepth;
		ULONG xres,yres;              // resolution
		float xmin,ymin,xmax,ymax;    // render area
		BOOL enablearea;              // enable render area
		ULONG minobjects;             // minumum octree objects

		// mesh create presets
		VECTOR cube_size;

		float sphere_radius;
		ULONG sphere_divisions;
		ULONG sphere_slices;

		VECTOR plane_size;
		ULONG plane_divisions_x;
		ULONG plane_divisions_y;

		float tube_radius;
		float tube_height;
		ULONG tube_divisions;
		ULONG tube_slices;
		BOOL tube_close_top;
		BOOL tube_close_bottom;

		float cone_radius;
		float cone_height;
		ULONG cone_divisions;
		ULONG cone_slices;
		BOOL cone_close_bottom;

		float torus_radius;
		float torus_thickness;
		ULONG torus_divisions;
		ULONG torus_slices;

		GLOBALS();
		~GLOBALS();
		void ToDefaults();
		BOOL SetBackPic(char*);
		BOOL SetReflMap(char*);
		BOOL SetRenderdPic(char*);
		BOOL SetPicType(char*);
		void SetRenderWindow(float min_x, float min_y, float max_x, float max_y)
		{
			xmin = min_x;
			ymin = min_y;
			xmax = max_x;
			ymax = max_y;
		}
		void SetEnableArea(BOOL value) { enablearea = value; }
		BOOL GetEnableArea() { return enablearea; }
		void SetEnableBack(BOOL value) { enableback = value; }
		BOOL GetEnableBack() { return enableback; }
		void SetEnableRefl(BOOL value) { enablerefl = value; }
		BOOL GetEnableRefl() { return enablerefl; }
		rsiResult ToRSI(rsiCONTEXT*);
};

extern GLOBALS global;

#endif
