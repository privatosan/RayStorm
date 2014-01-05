/***************
 * MODULE:     stats.cpp
 * PROJECT:    RayStorm
 * DESCRIPTION:   statistics for renderer
 * AUTHORS:    Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    05.12.96 ah    initial release
 ***************/

#ifdef __STATISTICS__

#ifndef STATS_H
#include <stats.h>
#endif

static STAT stats[STAT_LAST];

static char *desc[STAT_LAST] =
{
	"Octree tests",
	"Supersampled pixels",
	"Shadow tests",
	"Shadow cache hits",
	"Transparenc rays",
	"Reflective rays",
	"Sphere tests",
	"Sphere hits",
	"Triangle tests",
	"Triangle hits",
	"Plane tests",
	"Plane hits",
	"Box tests",
	"Box hits",
	"CSG tests",
	"CSG hits"
	"Cylinder tests",
	"Cylinder hits",
	"Sor tests",
	"Sor hits"
	"Cone tests",
	"Cone hits"
};

/************
 * FUNCTION:      InitStatistics
 * DESCRIPTION:   initialize statistic values
 * INPUT:         -
 * OUTPUT:        -
 *************/
void InitStatistics()
{
	int i;

	for(i=0; i<STAT_LAST; i++)
	{
		stats[i].type = i;
		stats[i].value = 0;
	}
}

/************
 * FUNCTION:      GetStatistics
 * DESCRIPTION:   get statistics on rendering process
 * INPUT:         -
 * OUTPUT:        rsiResult
 *************/
BOOL GetStatistics(STAT **stat, int *num)
{
	*stat = stats;
	*num = STAT_LAST;
	return TRUE;
}

/************
 * FUNCTION:      GetDescription
 * DESCRIPTION:   get description string for statistic value
 * INPUT:         stat     statistic
 * OUTPUT:        description string
 *************/
char *GetDescription(STAT *stat)
{
	if(stat->type < STAT_LAST)
		return desc[stat->type];

	return NULL;
}

/************
 * FUNCTION:      IncStatistic
 * DESCRIPTION:   increase a statistic value
 * INPUT:         num      number of values
 * OUTPUT:        -
 *************/
void IncStatistic(int num)
{
	stats[num].value++;
}

#endif
