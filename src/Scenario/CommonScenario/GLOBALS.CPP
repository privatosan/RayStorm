/***************
 * PROGRAM:       Modeler
 * NAME:          globals.cpp
 * DESCRIPTION:   global data class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.06.96 ah    initial release
 *    13.12.96 ah    added minobjects
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

GLOBALS global;

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
GLOBALS::GLOBALS()
{
	backpic = NULL;
	reflmap = NULL;
	renderdpic = NULL;
	ToDefaults();
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
GLOBALS::~GLOBALS()
{
	if(backpic)
		delete [ ] backpic;
	if(reflmap)
		delete [ ] reflmap;
	if(renderdpic)
		delete [ ] renderdpic;
}

/*************
 * DESCRIPTION:   set all values to defaults
 * INPUT:         none
 * OUTPUT:        none
 *************/
void GLOBALS::ToDefaults()
{
	enableback = FALSE;
	if(backpic)
	{
		delete [ ] backpic;
		backpic = NULL;
	}
	enablerefl = FALSE;
	if(reflmap)
	{
		delete [ ] reflmap;
		reflmap = NULL;
	}
	if(renderdpic)
	{
		delete [ ] renderdpic;
		renderdpic = NULL;
	}

	backcol.r = backcol.g = backcol.b = 0.f;
#ifdef __AMIGA__
	strcpy(picformat, "ILBM");
#else
	strcpy(picformat, "TGA");
#endif
	ambient.r = ambient.g = ambient.b = 0.f;
	fog.r = fog.g = fog.b = 1.f;
	flen = 0.f;
	fheight = 0.f;
	antialias = 1;
	anticont.r = .2f;
	anticont.g = .15f;
	anticont.b = .3f;
	gauss = 1.3f;
	distrib = 1;
	softshad = 1;
	randjit = FALSE;
	quick = FALSE;
	show = TRUE;
	octreedepth = 3;
	xres = 160;
	yres = 128;
	xmin = ymin = 0.f;
	xmax = ymax = 1.f;
	enablearea = FALSE;
	minobjects = 3;
	hyper_samples = 100;
	hyper_min_dens = 0.001f;
	hyper_max_dens = 0.01f;

	SetVector(&cube_size, 1.f, 1.f, 1.f);
	sphere_radius = 1.f;
	sphere_divisions = 12;
	sphere_slices = 6;
	SetVector(&plane_size, 1.f, 1.f, 1.f);
	plane_divisions_x = 10;
	plane_divisions_y = 10;
	tube_radius = 1.f;
	tube_height = 1.f;
	tube_divisions = 12;
	tube_slices = 1;
	tube_close_top = TRUE;
	tube_close_bottom = TRUE;
	cone_radius = 1.f;
	cone_height = 1.f;
	cone_divisions = 12;
	cone_slices = 1;
	cone_close_bottom = TRUE;
	torus_radius = 1.f;
	torus_thickness = .25f;
	torus_divisions = 10;
	torus_slices = 8;
}

/*************
 * DESCRIPTION:   Set the name of the background picture
 * INPUT:         name     name of picture file
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL GLOBALS::SetBackPic(char *name)
{
	if(backpic)
		delete backpic;

	if(name)
	{
		backpic = new char[strlen(name)+1];
		if(!backpic)
			return FALSE;

		strcpy(backpic,name);
	}
	else
		backpic = NULL;

	return TRUE;
}

/*************
 * DESCRIPTION:   Set the name of the renderd picture
 * INPUT:         name     name of picture file
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL GLOBALS::SetRenderdPic(char *name)
{
	if(renderdpic)
		delete renderdpic;

	if(name)
	{
		renderdpic = new char[strlen(name)+1];
		if(!renderdpic)
			return FALSE;

		strcpy(renderdpic,name);
	}
	else
		renderdpic = NULL;

	return TRUE;
}

/*************
 * DESCRIPTION:   Set the name of the global reflection picture
 * INPUT:         name     name of picture file
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL GLOBALS::SetReflMap(char *name)
{
	if(reflmap)
		delete reflmap;

	if(name)
	{
		reflmap = new char[strlen(name)+1];
		if(!reflmap)
			return FALSE;

		strcpy(reflmap,name);
	}
	else
		reflmap = NULL;

	return TRUE;
}

/*************
 * DESCRIPTION:   Set the format of the picture to save
 * INPUT:         name     name of format (ILBM/PNG/TGA)
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL GLOBALS::SetPicType(char *name)
{
	strcpy(picformat, name);

	return TRUE;
}

/*************
 * DESCRIPTION:   transfer global data to RayStorm Interface
 * INPUT:         -
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult GLOBALS::ToRSI(rsiCONTEXT *rc)
{
	rsiResult err;

	if(enableback)
	{
		err = PPC_STUB(rsiSetWorld)(CTXT, rsiTWorldBackdropPic, backpic, rsiTDone);
		if (err)
			return err;
	}
	if(enablerefl)
	{
		err = PPC_STUB(rsiSetWorld)(CTXT, rsiTWorldReflectionMap, reflmap, rsiTDone);
		if (err)
			return err;
	}

	err = PPC_STUB(rsiSetWorld)(CTXT,
		rsiTWorldBackground,       &backcol,
		rsiTWorldAmbient,          &ambient,
		rsiTWorldFogLength,        flen,
		rsiTWorldFogHeight,        fheight,
		rsiTWorldFogColor,         &fog,
		rsiTWorldMinObjects,       minobjects,
		rsiTWorldHyperSteps,       hyper_samples,
		rsiTWorldHyperMinDeltaDens,hyper_min_dens,
		rsiTWorldHyperMaxDeltaDens,hyper_max_dens,
		rsiTDone);
	if (err)
		return err;

	err = PPC_STUB(rsiSetAntialias)(CTXT,
		rsiTAntiSamples,  antialias,
		rsiTAntiContrast, &anticont,
		rsiTAntiFilter,   gauss,
		rsiTDone);
	if (err)
		return err;

	err = PPC_STUB(rsiSetDistribution)(CTXT, distrib, softshad);
	if (err)
		return err;

	err = PPC_STUB(rsiSetOctreeDepth)(CTXT, octreedepth);
	if(err)
		return err;

	err = PPC_STUB(rsiSetScreen)(CTXT, xres, yres);
	if(err)
		return err;

	return PPC_STUB(rsiSetRenderField)(CTXT, 0,0, xres,yres);
}
