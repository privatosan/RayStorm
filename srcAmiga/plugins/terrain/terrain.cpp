/***************
 * MODUL:         terrain
 * NAME:          terrain.cpp
 * DESCRIPTION:   Terrain Texture
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.09.98 ah    initial release
 ***************/

#include <stdlib.h>
#include <string.h>

#ifdef __AMIGA__
#include "pluginlib.h"
#endif // __AMIGA__

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef DIALOG_H
#include "dialog.h"
#endif

#ifndef TEXTURE_H
#include "rtexture.h"
#endif

#ifndef PLUGIN_H
#include "plugin.h"
#endif

#define IMAGE_WIDTH 128
#define IMAGE_HEIGHT 128

DIALOG_ELEMENT dialog[] =
{
	DIALOG_HGROUP,
		DIALOG_COLGROUP, 2,
			DIALOG_LABEL, DE("Width"),
			DIALOG_NUMBER,
				DIALOG_NUMBER_POSITIVE, TRUE,
			DIALOG_END,
			DIALOG_LABEL, DE("Height"),
			DIALOG_NUMBER,
				DIALOG_NUMBER_POSITIVE, TRUE,
			DIALOG_END,
			DIALOG_LABEL, DE("Steepness %"),
			DIALOG_SLIDER, 0, 100,
			DIALOG_LABEL, DE("Waterlevel %"),
			DIALOG_SLIDER, 0, 100,
			DIALOG_LABEL, DE("Random seed"),
			DIALOG_NUMBER,
				DIALOG_NUMBER_POSITIVE, TRUE,
			DIALOG_END,
		DIALOG_END,
		DIALOG_IMAGE, IMAGE_WIDTH,IMAGE_HEIGHT,
	DIALOG_END,
	DIALOG_FINISH
};

typedef struct
{
	ULONG width;
	ULONG height;
	ULONG steepness;
	ULONG waterlevel;
	ULONG seed;
	void *image;
} DIALOG_DATA;

typedef struct
{
	PLUGIN_INFO pinfo;
	ULONG old_width, old_height;
	ULONG old_steepness;
	ULONG old_waterlevel;
	ULONG old_seed;
	float min, max;
	float *map;
} TERRAIN_INFO;

DIALOG_DATA default_data =
{
	128, 128,
	50,
	0,
	12345
};

/* Prototypes */
void generate(float*, int width, int height, int x, int y, int cur_width, int cur_height, float steepness);
void calcwater();

#define RAND() ((float)rand()/(float)RAND_MAX)

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
static void SAVEDS plugin_cleanup(PLUGIN_INFO *tinfo)
{
	TERRAIN_INFO *info = (TERRAIN_INFO*)tinfo;

	if(info->map)
		delete info->map;
}

/*************
 * DESCRIPTION:   this function is called whenever the GUI is changed
 * INPUT:         info     info structure
 *                data     data structure
 * OUTPUT:        -
 *************/
static void SAVEDS plugin_update(PLUGIN_INFO *tinfo, DIALOG_DATA *data)
{
	SMALL_COLOR image[IMAGE_WIDTH*IMAGE_HEIGHT];
	int i,x,y;
	float value;
	float waterlevel;
	float scalex,scaley, invheight,min,max;
	SMALL_COLOR color;
	TERRAIN_INFO *info = (TERRAIN_INFO*)tinfo;
	BOOL newmap = FALSE;

	if((data->width == 0) || (data->height == 0))
		return;

	if(!info->map || (data->width != info->old_width) || (data->height != info->old_height))
	{
		info->map = new float[data->width*data->height];
		if(!info->map)
			return;

		newmap = TRUE;
	}

	if((data->seed != info->old_seed) || (data->width != info->old_width) || (data->height != info->old_height) || (data->steepness != info->old_steepness))
	{
		info->old_height = data->height;
		info->old_width = data->width;
		info->old_steepness = data->steepness;
		info->old_seed = data->seed;

		memset(info->map, 0, sizeof(float)*data->width*data->height);

		/* initialize random generator */
		srand(data->seed);

		info->map[0] = RAND();
		info->map[(data->width-1)] = RAND();
		info->map[(data->height-1)*data->width] = RAND();
		info->map[(data->width-1)+(data->height-1)*data->width] = RAND();

		generate(info->map, data->width,data->height, 0,0, data->width,data->height, (float)data->steepness * 0.01f);

		// find maximum and minimum
		max = min = info->map[0];
		for(i=data->width*data->height-1; i>=0; i--)
		{
			if(info->map[i] > max)
				max = info->map[i];
			else if(info->map[i] < min)
				min = info->map[i];
		}

		info->max = max;
		info->min = min;

		newmap = TRUE;
	}

	if((data->waterlevel != info->old_waterlevel) || newmap)
	{
		info->old_waterlevel = data->waterlevel;

		waterlevel = (float)data->waterlevel * 2.55f;

		if(info->max-info->min > 0.f)
			invheight = 255.f/(info->max-info->min);
		else
			invheight = 255.f;

		scalex = (float)data->width / (float)IMAGE_WIDTH;
		scaley = ((float)data->height*(float)data->width) / (float)IMAGE_HEIGHT;
		for(x=0; x<IMAGE_WIDTH; x++)
		{
			for(y=0; y<IMAGE_HEIGHT; y++)
			{
				value = (info->map[(int)((float)x*scalex + (float)y*scaley)] - info->min) * invheight;
				if(value < waterlevel)
					value = waterlevel;
				color.r = (UBYTE)value;
				color.g = color.b = color.r;
				image[x + y*IMAGE_WIDTH] = color;
			}
		}
	}
	info->pinfo.DoMethod(data->image, PLUG_IMAGE_DISPLAY, image);
}

/*************
 * DESCRIPTION:   this is the work function
 * INPUT:         info     info structure
 *                data     data structure
 * OUTPUT:        -
 *************/
static void SAVEDS plugin_work(PLUGIN_INFO *tinfo, DIALOG_DATA *data)
{
	int x,y;
	float value;
	float waterlevel;
	float invheight;
	TERRAIN_INFO *info = (TERRAIN_INFO*)tinfo;
	VECTOR *points, *cur_point;
	PLUGIN_EDGE *edge, *edges;
	PLUGIN_TRIANGLE *triangle, *triangles;
	float *cur_map;
	void *mesh;
	int xdivs,ydivs;

	xdivs = data->width-1;
	ydivs = data->height-1;

	// create points
	points = new VECTOR[data->width*data->height];
	if(!points)
		return;

	mesh = info->pinfo.MeshCreate();
	if(!mesh)
	{
		delete points;
		return;
	}

	waterlevel = (float)data->waterlevel * 0.01f;

	if(info->max-info->min > 0.f)
		invheight = 1.f/(info->max-info->min);
	else
		invheight = 1.f;

	cur_point = points;
	cur_map = info->map;
	for(y=0; y<data->height; y++)
	{
		for(x=0; x<data->width; x++)
		{
			value = (*cur_map - info->min) * invheight;
			if(value < waterlevel)
				value = waterlevel;
			cur_point->x = ((float)x / (float)(xdivs)) - 0.5f;
			cur_point->y = value;
			cur_point->z = ((float)y / (float)(ydivs)) - 0.5f;
			cur_map++;
			cur_point++;
		}
	}

	info->pinfo.MeshPoints(mesh, data->width*data->height, points);

	delete points;

	// create edges
	edges = new PLUGIN_EDGE[3*xdivs*ydivs + xdivs + ydivs];
	if(!edges)
		return;

	edge = edges;
	for(y=0; y<ydivs; y++)
	{
		for(x=0; x<xdivs; x++)
		{
			edge->p1 = y*(xdivs+1)+x;
			edge->p2 = y*(xdivs+1)+x+1;
			edge++;
		}
		for(x=0; x<=xdivs; x++)
		{
			edge->p1 = y*(xdivs+1)+x;
			edge->p2 = (y+1)*(xdivs+1)+x;
			edge++;
		}
		for(x=0; x<xdivs; x++)
		{
			edge->p1 = y*(xdivs+1)+x+1;
			edge->p2 = (y+1)*(xdivs+1)+x;
			edge++;
		}
	}
	for(x=0; x<xdivs; x++)
	{
		edge->p1 = ydivs*(xdivs+1)+x;
		edge->p2 = ydivs*(xdivs+1)+x+1;
		edge++;
	}

	info->pinfo.MeshEdges(mesh, 3*xdivs*ydivs + xdivs + ydivs, edges);

	delete edges;

	// create triangles
	triangles = new PLUGIN_TRIANGLE[xdivs*ydivs*2];
	if(!triangles)
		return;

	triangle = triangles;
	for(y=0; y<ydivs; y++)
	{
		for(x=0; x<xdivs; x++)
		{
			triangle->e1 = y*(2*xdivs+xdivs+1)+x;
			triangle->e2 = y*(2*xdivs+xdivs+1)+xdivs+xdivs+1+x;
			triangle->e3 = y*(2*xdivs+xdivs+1)+xdivs+x;
			triangle++;
			triangle->e1 = y*(2*xdivs+xdivs+1)+xdivs+x+1;
			triangle->e2 = (y+1)*(2*xdivs+xdivs+1)+x;
			triangle->e3 = y*(2*xdivs+xdivs+1)+xdivs+xdivs+1+x;
			triangle++;
		}
	}

	info->pinfo.MeshTriangles(mesh, xdivs*ydivs*2, triangles);

	delete triangles;
}

/*************
 * DESCRIPTION:   This is the init function for the plugins
 * INPUT:         -
 * OUTPUT:        pointer to plugin info, NULL if failed
 *************/
#ifdef __PPC__
extern "C" PLUGIN_INFO* SAVEDS plugin_init(struct Library *PluginBase)
#else
extern "C" PLUGIN_INFO* plugin_init()
#endif // __AMIGA__
{
	TERRAIN_INFO *info;

	info = new TERRAIN_INFO;
	if(info)
	{
		info->pinfo.name = "Terrain";
		info->pinfo.dialog = dialog;
		info->pinfo.defaultdata = &default_data;
		info->pinfo.datasize = sizeof(DIALOG_DATA);
		info->pinfo.cleanup = plugin_cleanup;
		info->pinfo.update = (void (*)(PLUGIN_INFO*, void*))plugin_update;
		info->pinfo.work = (void (*)(PLUGIN_INFO*, void*))plugin_work;

		info->old_width = 0;
		info->old_height = 0;
		info->old_waterlevel = 0;
		info->old_seed = 0;
		info->max = 0.f;
		info->min = 0.f;
		info->map = NULL;
	}

	return (PLUGIN_INFO*)info;
}

/*******************************************************
 * generate landscape
 *******************************************************/

void generate(float *map, int width, int height, int x, int y, int cur_width,int cur_height, float steepness)
{
  static unsigned char bittab[4][8] = {1,1,1,0,0,1,0,0, 1,1,1,1,0,1,0,1, 1,1,1,0,1,1,1,0, 1,1,1,1,1,1,1,1};
  int ptr;
  float *mapptr;
  int halfwidth = cur_width >> 1;
  int halfheight = cur_height >> 1;

  /* generate terrain */
	if(cur_width>2)
	{
		mapptr = &map[(x+halfwidth)+(y*width)];
		if(!*mapptr)
		{
			*mapptr = (map[x+(y*width)]+map[(x+cur_width-1)+(y*width)])*.5f +
				((RAND()-.5f)*steepness*cur_width)/width;
		}
		mapptr = &map[(x+halfwidth)+((y+cur_height-1)*width)];
		if(!*mapptr)
		{
			*mapptr = (map[x+((y+cur_height-1)*width)]+
				map[(x+cur_width-1)+((y+cur_height-1)*width)])*.5f +
				((RAND()-.5f)*steepness*cur_width)/width;
		}
	}
	if(cur_height>2)
	{
		mapptr = &map[x+((y+halfheight)*width)];
		if(!*mapptr)
		{
			*mapptr = (map[x+(y*width)]+map[x+((y+cur_height-1)*width)])*.5f+
				((RAND()-.5f)*steepness*cur_height)/width;
		}
		mapptr = &map[(x+cur_width-1)+((y+halfheight)*width)];
		if(!*mapptr)
		{
			*mapptr = (map[(x+cur_width-1)+(y*width)]+
				map[(x+cur_width-1)+((y+cur_height-1)*width)])*.5f+
				((RAND()-.5f)*steepness*cur_height)/width;
		}
	}
	if(cur_width>2 && cur_height>2)
	{
		mapptr = &map[(x+halfwidth)+((y+halfheight)*width)];
		if(!*mapptr)
		{
			*mapptr = (map[x+(y*width)]+map[(x+cur_width-1)+(y*width)]+
				map[(x+cur_width-1)+((y+cur_height-1)*width)]+
				map[x+((y+cur_height-1)*width)])*.25f+
				((RAND()-.5f)*steepness*cur_width*cur_height)/(width*height);
		}
	}

	/* End of recursion */
	if((cur_width<=3) && (cur_height<=3))
		return;

	/* start recursion */
	ptr = ((cur_width & 1)<<1) + (cur_height & 1);

	generate(map,width,height,x          ,y           ,halfwidth+bittab[ptr][0],halfheight+bittab[ptr][1], steepness);
	generate(map,width,height,x          ,y+halfheight,halfwidth+bittab[ptr][2],halfheight+bittab[ptr][3], steepness);
	generate(map,width,height,x+halfwidth,y           ,halfwidth+bittab[ptr][4],halfheight+bittab[ptr][5], steepness);
	generate(map,width,height,x+halfwidth,y+halfheight,halfwidth+bittab[ptr][6],halfheight+bittab[ptr][7], steepness);
}

