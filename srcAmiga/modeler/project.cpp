/***************
 * PROGRAM:       Modeler
 * NAME:          project.cpp
 * DESCRIPTION:   display functions
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.05.96 ah    initial release
 *    11.01.97 ah    added box object support
 *    12.03.96 mh    added PTYP (picture type)
 *    25.03.97 ah    added cylinder object support
 *    25.03.97 ah    added cone object support
 *    04.04.97 ah    added csg object support
 *    09.04.97 ah    added sor object support
 ***************/

#include <string.h>

#ifdef __AMIGA__
#include <pragma/dos_lib.h>

#ifndef UTILITY_H
#include "utility.h"
#endif

#else

#include "iffparse.h"

#endif // __AMIGA__

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef VIEW_H
#include "view.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef SPHERE_H
#include "sphere.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef PLANE_H
#include "plane.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef BOX_H
#include "box.h"
#endif

#ifndef CYLINDER_H
#include "cylinder.h"
#endif

#ifndef CONE_H
#include "cone.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef SOR_H
#include "sor.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef IFF_UTIL_H
#include "iff_util.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef ERRORS_H
#include "errors.h"
#endif

ULONG rscn_version;

static PROJECT project;

static char camera[256];

#define UNTITLED "untitled"

typedef struct
{
	// perspective view
	VECTOR vpos;      // virtual camera look position
	VECTOR valign;    // virtual camera alignment to x-axis
	float dist;       // virtual camera distance
	// orthogonal view
	VECTOR viewpoint; // viewpoint
	float zoom;       // zoomfactor
	ULONG viewmode;   // new with v2.1
} OBSERVER;

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
PROJECT::PROJECT()
{
	project.name = NULL;
	SetProjectName(UNTITLED);
	changed = FALSE;
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
PROJECT::~PROJECT()
{
	if(name)
		delete name;
}

/*************
 * DESCRIPTION:   save the current project to disk
 * INPUT:         project     projectname (the project is saved to a
 *    directory with this name)
 *                disp        current display
 * OUTPUT:        error string if failed, else NULL;
 *************/
char *PROJECT::Save(char *filename, DISPLAY *disp)
{
	struct IFFHandle *iff;
	ULONG data[2];
	float fdata[2];
	UBYTE ubdata;
	VIEW *view = disp->view;
	OBSERVER obsv;
	int i;

	// Allocate IFF_File structure.
	iff = AllocIFF();
	if(!iff)
		return errors[ERR_MEM];

	// Set up IFF_File for AmigaDOS I/O.
#ifdef __AMIGA__
	iff->iff_Stream = Open(filename, MODE_NEWFILE);
#else
	iff->iff_Stream = Open_(filename, MODE_NEWFILE);
#endif
	if(!iff->iff_Stream)
	{
		IFFCleanup(iff);
		return errors[ERR_OPEN];
	}
	InitIFFasDOS(iff);

	// Start the IFF transaction.
	if(OpenIFF(iff, IFFF_WRITE))
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}

	PUSH_CHUNK(ID_RSCN);

	data[0] = VERSION;

	WRITE_LONG_CHUNK(ID_VERS, data, 1);

	PUSH_CHUNK(ID_GNRL);
	data[0] = global.xres;
	data[1] = global.yres;

	WRITE_LONG_CHUNK(ID_RESO, data, 2);

	if(global.renderdpic)
		WRITE_CHUNK(ID_PICT, global.renderdpic, strlen(global.renderdpic)+1);

	if(global.picformat)
		WRITE_CHUNK(ID_PTYP, global.picformat, strlen(global.picformat)+1);

	ubdata = disp->display;
	WRITE_CHUNK(ID_DISP, &ubdata, sizeof(UBYTE));
	// changed with v2.1: no longer current viewmode, now current view number
	for(i=0; i<4; i++)
	{
		if(disp->views[i] == disp->view)
		{
			ubdata = i;
			WRITE_CHUNK(ID_VIEW, &ubdata, sizeof(UBYTE));
		}
	}
	WRITE_LONG_CHUNK(ID_GRID, &disp->gridsize, 1);
	data[0] = 0;
	if(disp->grid)
		data[0] |= RSCN_GENERAL_GRID;
	if(disp->gridsnap)
		data[0] |= RSCN_GENERAL_GRIDSNAP;
	if(disp->multiview)
		data[0] |= RSCN_GENERAL_VIEWFOUR;
	if(global.show)
		data[0] |= RSCN_GENERAL_SHOW;
	if(global.enablearea)
		data[0] |= RSCN_GENERAL_ENABLEAREA;
	if(global.enableback)
		data[0] |= RSCN_GENERAL_ENABLEBACK;
	if(global.enablerefl)
		data[0] |= RSCN_GENERAL_ENABLEREFL;
	WRITE_LONG_CHUNK(ID_FLGS, data, 1);

	// write views
	for(i=0; i<4; i++)
	{
		obsv.vpos = disp->views[i]->vpos;
		obsv.valign = disp->views[i]->valign;
		obsv.dist = disp->views[i]->dist;
		obsv.viewpoint = disp->views[i]->viewpoint;
		obsv.zoom = disp->views[i]->zoom;
		obsv.viewmode = disp->views[i]->viewmode;
		WRITE_LONG_CHUNK(ID_OBSV, &obsv, 12);
	}
	WRITE_LONG_CHUNK(ID_FILT, &disp->filter_flags, 1);

	WRITE_LONG_CHUNK(ID_FLEN, &global.flen, 1);
	WRITE_LONG_CHUNK(ID_FHGT, &global.fheight, 1);
	WRITE_LONG_CHUNK(ID_FCOL, &global.fog, 3);
	WRITE_LONG_CHUNK(ID_FILD, &global.xmin, 4);

	POP_CHUNK();

	PUSH_CHUNK(ID_SRFS);

	WriteSurfaces(iff);

	POP_CHUNK();

	PUSH_CHUNK(ID_FRAM);

	fdata[0] = 0.;
	fdata[1] = 0.;
	WRITE_LONG_CHUNK(ID_TIME, fdata, 2);

	data[0] = 0;
	if(global.quick)
		data[0] |= RSCN_FRAME_QUICK;
	if(global.randjit)
		data[0] |= RSCN_FRAME_RANDJIT;
	WRITE_LONG_CHUNK(ID_FLGS, data, 1);

	WRITE_CHUNK(ID_CAMR, disp->camera->GetName(), strlen(disp->camera->GetName())+1);

	WRITE_LONG_CHUNK(ID_AMBT, &global.ambient, 3);

	WRITE_LONG_CHUNK(ID_ANTC, &global.anticont, 3);

	WRITE_LONG_CHUNK(ID_ANTS, &global.antialias, 1);

	WRITE_LONG_CHUNK(ID_BCKC, &global.backcol, 3);

	WRITE_LONG_CHUNK(ID_MOBJ, &global.minobjects, 1);

	if(global.backpic)
		WRITE_CHUNK(ID_BCKP, global.backpic, strlen(global.backpic)+1);

	if(global.reflmap)
		WRITE_CHUNK(ID_REFM, global.reflmap, strlen(global.reflmap)+1);

	ubdata = global.softshad;
	WRITE_CHUNK(ID_SOFT, &ubdata, sizeof(UBYTE));

	ubdata = global.distrib;
	WRITE_CHUNK(ID_MBLR, &ubdata, sizeof(UBYTE));

	ubdata = global.octreedepth;
	WRITE_CHUNK(ID_ODPT, &ubdata, sizeof(UBYTE));

	POP_CHUNK();

	WriteObjects(iff,FALSE);

	POP_CHUNK();

	IFFCleanup(iff);
	return NULL;
}

char *ProjectSave(char *projectname, DISPLAY *disp)
{
	char buffer[256];

	strcpy(buffer, projectname);

	SetProjectChanged(FALSE);

	return project.Save(buffer,disp);
}

/*************
 * DESCRIPTION:   load the current project from disk
 * INPUT:         project     projectname (the project is loaded from a
 *    directory with this name)
 *                disp        current display
 * OUTPUT:        error string if failed, else NULL
 *************/
static char *ParseGeneral(struct IFFHandle *iff, DISPLAY *disp)
{
	struct ContextNode *cn;
	long error = 0;
	ULONG reso[2];
	ULONG flags;
	char buffer[256];
	int len, i;
	OBSERVER obsv;
	UBYTE activeview, currentview=0;
	BOOL viewcamera = FALSE;
	UBYTE display;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC )
			return errors[ERR_IFFPARSE];

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if (!cn)
			continue;

		if(cn->cn_ID == ID_FORM)
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_RESO:
				if(!ReadULONG(iff,reso,2))
					return errors[ERR_RSCNFILE];
				global.xres = reso[0];
				global.yres = reso[1];
				break;
			case ID_PICT:
				len = ReadString(iff,buffer,256);
				if(!global.SetRenderdPic(buffer))
					return errors[ERR_MEM];
				break;
			case ID_PTYP:
				len = ReadString(iff,buffer,256);
				if(!global.SetPicType(buffer))
					return errors[ERR_MEM];
				break;
			case ID_DISP:
				if(!ReadUBYTE(iff,&display,1))
					return errors[ERR_RSCNFILE];
				disp->display = display;
				break;
			case ID_VIEW:
				// new in v2.1: current view number; before: current view mode
				if(rscn_version >= 210)
				{
					if(!ReadUBYTE(iff, &activeview, 1))
						return errors[ERR_RSCNFILE];
				}
				break;
			case ID_GRID:
				if(!ReadFloat(iff,&disp->gridsize,1))
					return errors[ERR_RSCNFILE];
				break;
			case ID_FLGS:
				if(!ReadULONG(iff,&flags,1))
					return errors[ERR_RSCNFILE];
				disp->grid = flags & RSCN_GENERAL_GRID ? TRUE : FALSE;
				disp->gridsnap = flags & RSCN_GENERAL_GRIDSNAP ? TRUE : FALSE;
				if(flags & RSCN_GENERAL_VIEWCAMERA)
					viewcamera = TRUE;
				disp->multiview = flags & RSCN_GENERAL_VIEWFOUR ? TRUE : FALSE;
				global.show = flags & RSCN_GENERAL_SHOW ? TRUE : FALSE;
				global.SetEnableArea(flags & RSCN_GENERAL_ENABLEAREA ? TRUE : FALSE);
				global.SetEnableBack(flags & RSCN_GENERAL_ENABLEBACK ? TRUE : FALSE);
				global.SetEnableRefl(flags & RSCN_GENERAL_ENABLEREFL ? TRUE : FALSE);
				break;
			case ID_OBSV:
				if(rscn_version >= 210)
				{
					if(!ReadFloat(iff,(float*)&obsv,12))
						return errors[ERR_RSCNFILE];
				}
				else
				{
					if(!ReadFloat(iff,(float*)&obsv,11))
						return errors[ERR_RSCNFILE];
				}
				disp->views[currentview]->vpos = obsv.vpos;
				disp->views[currentview]->valign = obsv.valign;
				disp->views[currentview]->dist = obsv.dist;
				disp->views[currentview]->viewpoint = obsv.viewpoint;
				disp->views[currentview]->zoom = obsv.zoom;
				if(rscn_version >= 210)
					disp->views[currentview]->viewmode = obsv.viewmode;
				currentview++;
				break;
			case ID_FLEN:
				if(!ReadFloat(iff,&global.flen,1))
					return errors[ERR_RSCNFILE];
				break;
			case ID_FHGT:
				if(!ReadFloat(iff,&global.fheight,1))
					return errors[ERR_RSCNFILE];
				break;
			case ID_FCOL:
				if(!ReadFloat(iff,(float*)&global.fog,3))
					return errors[ERR_RSCNFILE];
				break;
			case ID_FILD:
				if(!ReadFloat(iff,&global.xmin,4))
					return errors[ERR_RSCNFILE];
				break;
			case ID_MOBJ:
				if(!ReadULONG(iff,&global.minobjects,1))
					return errors[ERR_RSCNFILE];
				break;
			case ID_FILT:
				if(!ReadULONG(iff, (ULONG*)&disp->filter_flags, 1))
					return errors[ERR_RSCNFILE];
				break;
		}
	}
	if(rscn_version >= 210)
	{
		ASSERT((activeview >= 0) && (activeview <= 4));
		disp->view = disp->views[activeview];
	}
	else
	{
		if(disp->multiview)
			disp->views[0]->zoom *= .5f;
		for(i=1; i<4; i++)
		{
			disp->views[i]->vpos = disp->views[0]->vpos;
			disp->views[i]->valign = disp->views[0]->valign;
			disp->views[i]->dist = disp->views[0]->dist;
			disp->views[i]->viewpoint = disp->views[0]->viewpoint;
			disp->views[i]->zoom = disp->views[0]->zoom;
		}
		disp->views[0]->viewmode = VIEW_TOP;
		disp->views[1]->viewmode = viewcamera ? VIEW_CAMERA : VIEW_PERSP;
		disp->views[2]->viewmode = VIEW_FRONT;
		disp->views[3]->viewmode = VIEW_RIGHT;
	}
	return NULL;
}

static char *ParseFrame(struct IFFHandle *iff)
{
	struct ContextNode *cn;
	long error = 0;
	ULONG flags;
	char buffer[256];
	int len;
	UBYTE ubnum;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC )
			return errors[ERR_IFFPARSE];

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if(cn->cn_ID == ID_FORM)
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_FLGS:
				if(!ReadULONG(iff,&flags,1))
					return errors[ERR_RSCNFILE];
				global.quick = flags & RSCN_FRAME_QUICK ? TRUE : FALSE;
				global.randjit = flags & RSCN_FRAME_RANDJIT ? TRUE : FALSE;
				break;
			case ID_CAMR:
				len = ReadString(iff,camera,256);
				break;
			case ID_AMBT:
				if(!ReadFloat(iff,(float*)&global.ambient,3))
					return errors[ERR_RSCNFILE];
				break;
			case ID_ANTS:
				if(!ReadInt(iff,&global.antialias,1))
					return errors[ERR_RSCNFILE];
				break;
			case ID_ANTC:
				if(!ReadFloat(iff,(float*)&global.anticont,3))
					return errors[ERR_RSCNFILE];
				break;
			case ID_BCKC:
				if(!ReadFloat(iff,(float*)&global.backcol,3))
					return errors[ERR_RSCNFILE];
				break;
			case ID_BCKP:
				len = ReadString(iff,buffer,256);
				global.SetBackPic(buffer);
				if(rscn_version < 200)
					global.SetEnableBack(TRUE);
				break;
			case ID_REFM:
				len = ReadString(iff,buffer,256);
				global.SetReflMap(buffer);
				if(rscn_version < 200)
					global.SetEnableRefl(TRUE);
				break;
			case ID_SOFT:
				if(!ReadUBYTE(iff,&ubnum,1))
					return errors[ERR_RSCNFILE];
				global.softshad = ubnum;
				break;
			case ID_MBLR:
				if(!ReadUBYTE(iff,&ubnum,1))
					return errors[ERR_RSCNFILE];
				global.distrib = ubnum;
				break;
			case ID_ODPT:
				if(!ReadUBYTE(iff,&ubnum,1))
					return errors[ERR_RSCNFILE];
				global.octreedepth = ubnum;
				break;
		}
	}
	return NULL;
}

char *PROJECT::Load(char *filename, DISPLAY *disp)
{
	long error = 0;
	struct IFFHandle *iff;
	struct ContextNode *cn;
	char *err;
	OBJECT *where = NULL;
	int dir = INSERT_HORIZ;

	// there is currently no active camera read
	camera[0] = 0;

	iff = AllocIFF();
	if (!iff)
		return errors[ERR_MEM];

#ifdef __AMIGA__
	iff->iff_Stream = Open(filename, MODE_OLDFILE);
#else
	iff->iff_Stream = Open_(filename, MODE_OLDFILE);
#endif
	if (!(iff->iff_Stream))
	{
		IFFCleanup(iff);
		return errors[ERR_OPEN];
	}

	InitIFFasDOS(iff);
	error = OpenIFF(iff, IFFF_READ);
	if (error)
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}

	error = ParseIFF(iff, IFFPARSE_RAWSTEP);
	if (error)
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}
	cn = CurrentChunk(iff);
	if(!cn)
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}
	if((cn->cn_ID != ID_FORM) || (cn->cn_Type != ID_RSCN))
	{
		IFFCleanup(iff);
		return errors[ERR_NORSCNFILE];
	}

	while(!error || error == IFFERR_EOC)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error != IFFERR_EOC)
		{
			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(iff);
			if (cn)
			{
				switch (cn->cn_ID)
				{
					case ID_VERS:
						if(!ReadULONG(iff,&rscn_version,1))
						{
							IFFCleanup(iff);
							return errors[ERR_RSCNFILE];
						}
						if(rscn_version > VERSION)
						{
							IFFCleanup(iff);
							return errors[ERR_WRONGVERS];
						}
						break;
					case ID_FORM:
						switch(cn->cn_Type)
						{
							case ID_GNRL:
								err = ParseGeneral(iff,disp);
								if(err)
								{
									IFFCleanup(iff);
									return err;
								}
								break;
							case ID_FRAM:
								err = ParseFrame(iff);
								if(err)
								{
									IFFCleanup(iff);
									return err;
								}
								break;
							case ID_SRFS:
								err = ParseSurfaces(iff);
								if(err)
								{
									IFFCleanup(iff);
									return err;
								}
								break;
							case ID_CAMR:
								where = ParseCamera(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_SPHR:
								where = ParseSphere(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_PLAN:
								where = ParsePlane(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_PLGT:
							case ID_SLGT:
							case ID_DLGT:
								where = ParseLight(iff, where, dir, cn->cn_Type);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_MESH:
								where = ParseMesh(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_EXTN:
								where = ParseExternal(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_BOX:
								where = ParseBox(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_CYLR:
								where = ParseCylinder(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_CONE:
								where = ParseCone(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_CSG:
								where = ParseCSG(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_SOR:
								where = ParseSOR(iff, where, dir);
								if(!where)
								{
									IFFCleanup(iff);
									return errors[ERR_RSCNFILE];
								}
								dir = INSERT_HORIZ;
								break;
							case ID_HIER:
								dir = INSERT_VERT;
								break;
						}
						break;
				}
			}
		}
		else
		{
			cn = CurrentChunk(iff);
			if (cn)
			{
				if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_HIER))
				{
					if(dir == INSERT_HORIZ)
					{
						while(where->GetPrev())
							where = (OBJECT*)where->GetPrev();
						if(where->GoUp())
							where = (OBJECT*)where->GoUp();
					}
					else
						dir = INSERT_HORIZ;
				}
			}
		}
	}
	if (error != IFFERR_EOF)
	{
		return errors[ERR_IFFPARSE];
	}

	IFFCleanup(iff);

	// we have to set the active camera
	disp->camera = (CAMERA*)GetObjectByName(camera);
	if(!disp->camera)
		err = errors[ERR_NOCAMERA];
	else
		err = NULL;

	// and translate track names to track objects
	// and the surface names to surface pointers
	TranslateNames();

	return err;
}

char *ProjectLoad(char *projectname, DISPLAY *disp)
{
	char buffer[256];

	strcpy(buffer, projectname);
	SetProjectChanged(FALSE);
	global.SetBackPic(NULL);
	global.SetReflMap(NULL);

	return project.Load(buffer,disp);
}

/*************
 * DESCRIPTION:   get the name of the current project
 * INPUT:         -
 * OUTPUT:        project name
 *************/
char *GetProjectName()
{
	return project.name;
}

/*************
 * DESCRIPTION:   set the name of the current project
 * INPUT:         name     project name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SetProjectName(char *name)
{
	if(project.name)
		delete project.name;

	project.name = new char[strlen(name)+1];
	if(!project.name)
		return FALSE;

	strcpy(project.name,name);
	return TRUE;
}

/*************
 * DESCRIPTION:   get the changed state of the current project
 * INPUT:         -
 * OUTPUT:        changed state
 *************/
BOOL GetProjectChanged()
{
	return project.changed;
}

/*************
 * DESCRIPTION:   set the changed state of the current project
 * INPUT:         changed     changed state
 * OUTPUT:        -
 *************/
void SetProjectChanged(BOOL changed)
{
	project.changed = changed;
}

/*************
 * DESCRIPTION:   project has own title ?
 * INPUT:         -
 * OUTPUT:        TRUE if own title, else project has "untitled" name
 *************/
BOOL IsTitled()
{
	return strcmp(project.name, UNTITLED) != 0;
}
