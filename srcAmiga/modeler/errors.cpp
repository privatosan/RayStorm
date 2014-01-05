/***************
 * PROGRAM:       Modeler
 * NAME:          errors.h
 * DESCRIPTION:   error definitions
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 ***************/

#ifndef ERRORS_H
#define ERRORS_H

char *errors[] =
{
	"",
	"Not enough memory",
	"Can't open object",
	"Error reading object",
	"Can't open object type file ('modules/object/types')",
	"Error reading object type file",
	"An error occcured while invoking object handler",
	"Unknown object format",
	"Can't open file",
	"Error in iffparse",
	"Can't write to file",
	"No RayStorm material file",
	"Corrupt RayStorm material file",
	"No RayStorm light file",
	"Corrupt RayStorm light file",
	"No RayStorm scene file",
	"Mangled RayStorm scene file",
	"Can't handle RayStorm scene file with this version",
	"There is no camera defined, please set the active camera",
	"Can't save object because of one of the following reasons:\n- no object is selected\n- the selected object is an external object\n- the selected object is grouped to another object.",

#ifdef __AMIGA__
	"Can't open intuition.library V36",
	"Can't open graphics.library V36",
	"Can't open muimaster.library V12",
	"Can't open utility.library V36",
	"Can't open iffparse.library",
	"Can't open asl.library",
	"Failed to create MUI custom class",
#ifdef __PPC__
	"Can't open raystormppc.library V2",
#else
	"Can't open raystorm.library V2",
#endif
	"Can't open window",
	"This is the 68020 version of Scenario\nbut you don't have a 68020 or better based machine.",
	"This is the 68020/881 version of Scenario\nbut you don't have a 68020/881 or better based machine.",
	"This is the 68040 version of Scenario\nbut you don't have a 68040 or better based machine.",
	"This is the 68060 version of Scenario\nbut you don't have a 68060 or better based machine.",
	"This is the PowerPC version of Scenario\nbut you don't have a PowerPC based machine.",
	"Need at least V2 object handler.",
	"You need powerpc.library V8 (WarpOS) or higher to run the PowerPC version.",
	"Installation problem.\nYou are trying to use the M68k version of Scenario\nwith a non M68k version of the raystorm.library.",
	"Installation problem.\nYou are trying to use the WarpOS PowerPC version of Scenario\nwith a non WarpOS PowerPC version of the raystorm.library.",
#endif
};

#endif
