/***************
 * MODUL:         raystorm
 * NAME:          raystorm.h
 * DESCRIPTION:   raytrace prototypes
 * AUTHORS:       Andreas Heumann
 * HISTORY:       DATE     NAME  COMMENT
 *       05.02.98 ah    initial release
 ***************/

#ifndef RAYSTORM_H
#define RAYSTORM_H

void WriteLog(rsiCONTEXT*, char*);
void UpdateStatus(rsiCONTEXT*, float, float, int, int, rsiSMALL_COLOR*);
BOOL CheckCancel(rsiCONTEXT*);
void Cooperate(rsiCONTEXT*);

#endif /* RAYSTORM_H */
