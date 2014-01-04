/***************
 * MODULE:			ddxddv.h
 * PROJECT:			Scenario
 * DESCRIPTION:   DDX/DDV routines (definition file)
 *	AUTHORS:			Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		06.09.97	mh		initial release
 ***************/

#include "typedefs.h"

void AFXAPI DDV_MinFloat(CDataExchange *, float const &, float, BOOL);
void AFXAPI DDV_MinMaxFloatExclude(CDataExchange *, float const &, float, float);
