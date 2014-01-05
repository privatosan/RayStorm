/***************
 * MODULE:     stats.h
 * PROJECT:    RayStorm
 * DESCRIPTION:   statistics for renderer
 * AUTHORS:    Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    05.12.96 ah    initial release
 ***************/

#ifndef STATS_H
#define STATS_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

// types
enum
{
	STAT_OCTREETEST,
	STAT_SUPERSAMPLE,
	STAT_SHADOWTEST,
	STAT_SHADOWCACHEHIT,
	STAT_TRANSRAY,
	STAT_REFLRAY,
	STAT_SPHERETEST,
	STAT_SPHEREHIT,
	STAT_TRIANGLETEST,
	STAT_TRIANGLEHIT,
	STAT_PLANETEST,
	STAT_PLANEHIT,
	STAT_BOXTEST,
	STAT_BOXHIT,
	STAT_CSGTEST,
	STAT_CSGHIT,
	STAT_CYLINDERTEST,
	STAT_CYLINDERHIT,
	STAT_SORTEST,
	STAT_SORHIT,
	STAT_CONETEST,
	STAT_CONEHIT,
	STAT_LAST
};

#ifdef __STATISTICS__

typedef struct
{
	int type;
	int value;
} STAT;

#define STATISTIC(type) IncStatistic(type)

BOOL GetStatistics(STAT **, int *);
char *GetDescription(STAT *);
void InitStatistics();
void IncStatistic(int);

#else

#define STATISTIC(type) ;

#endif

#endif
