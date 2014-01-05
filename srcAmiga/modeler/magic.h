/***************
 * MODULE:        magic.h
 * PROJECT:       Scenario
 * DESCRIPTION:   magic numbers, platform dependent constants
 * AUTHORS:       Mike Hesser, Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.10.96 mh    initial release
 *    11.01.97 ah    added SPHERE_DIVS and PLANE_DIVS
 *    25.03.97 ah    added CYLINDER_DIVS
 *    01.04.97 ah    added CONE_DIVS
 *    09.04.97 ah    added SOR_DIVS and SOR_STEP
 ***************/

#ifndef MAGIC_H
#define MAGIC_H

#ifdef __AMIGA__
	#define TRACKWIDTH            3        // half width of tracker
	#define ZOOM_VIEWER_MAXVELOC  10       // maximum zoom velocity of viewer
	#define ZOOM_VIEWER_VELOC     70       // zoom velocity of viewer
	#define ALIGN_VIEWER_VELOC    0.2      // align velocity of viewer
	#define MOVE_VIEWER_VELOC     0.001    // move velocity of viewer
	#define MOVE_OBJECT_VELOC     0.001    // move velocity of object
	#define ALIGN_OBJECT_VELOC    0.5      // align velocity of object
	#define SCALE_OBJECT_VELOC    0.01     // scale velocity of object
	#define SCALE_OBJECT_VELOC2   0.02     // scale velocity of object (variant)
	#define SCALE_MOUSE_DELTA     1        // mouse delta scale factor
	#define SUBDIV                5        // number of lines of the world grid
	#define MIN_GRIDPIXELS        8        // minimum distance of grid lines in orthogonal modes
	#define SAFETY_ZONE           ((gfx.GetTextExtentX("X")>>1)+1)
														// extra area for blitting
	#define SPHERE_DIVS           20       // degree per divsion of wireframe sphere
	#define OGL_SPHERE_DIVS       30       // degree per divsion of OpenGL sphere
	#define PLANE_DIVS            5        // divisions of wireframe plane
	#define CURSOR_MOVE           0.25     // movement with cursor in orthogonal views
	#define ZOOM_INOUT            2        // Zoomin in and out
	#define CYLINDER_DIVS         20       // degree per divsion of wireframe cylinder
	#define OGL_CYLINDER_DIVS     30       // degree per divsion of OpenGL cylinder
	#define CONE_DIVS             20       // degree per divsion of wireframe cone
	#define OGL_CONE_DIVS         30       // degree per divsion of OpenGL cone
	#define SOR_DIVS              20       // degree per divsion of wireframe sor
	#define OGL_SOR_DIVS          30       // degree per divsion of OpenGL sor
	#define SOR_STEP              10       // steps per slice
#else
	#define TRACKWIDTH            3
	#define ZOOM_VIEWER_MAXVELOC  10
	#define ZOOM_VIEWER_VELOC     120
	#define ALIGN_VIEWER_VELOC    0.1
	#define MOVE_VIEWER_VELOC     0.001
	#define MOVE_OBJECT_VELOC     0.001
	#define ALIGN_OBJECT_VELOC    0.2
	#define SCALE_OBJECT_VELOC    0.01
	#define SCALE_OBJECT_VELOC2   0.02
	#define SCALE_MOUSE_DELTA     1.5
	#define SUBDIV                10
	#define MIN_GRIDPIXELS        8
	#define SAFETY_ZONE           20
	#define SPHERE_DIVS           15
	#define OGL_SPHERE_DIVS       15
	#define PLANE_DIVS            5
	#define CURSOR_MOVE           0.25
	#define ZOOM_INOUT            1.5
	#define CYLINDER_DIVS         15
	#define OGL_CYLINDER_DIVS     30
	#define CONE_DIVS             15
	#define OGL_CONE_DIVS         30
	#define SOR_DIVS              20
	#define OGL_SOR_DIVS          30
	#define SOR_STEP              15
#endif

#endif


