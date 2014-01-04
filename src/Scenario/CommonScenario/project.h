/***************
 * PROGRAM:       Modeler
 * NAME:          project.h
 * DESCRIPTION:   project class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.05.96 ah    initial release
 ***************/

#ifndef PROJECT_H
#define PROJECT_H

class PROJECT
{
	public:
		char *name;          // project name
		BOOL changed;        // TRUE if project was changed else FALSE

	public:
		PROJECT();
		~PROJECT();
		char *Save(char*,DISPLAY*);
		char *Load(char*,DISPLAY*);
};

char *ProjectSave(char *,DISPLAY *);
char *ProjectLoad(char *,DISPLAY *);
BOOL SetProjectName(char*);
char *GetProjectName();
void SetProjectChanged(BOOL);
BOOL GetProjectChanged();
BOOL IsTitled();

extern ULONG rscn_version;

#endif
