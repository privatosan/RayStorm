# Microsoft Developer Studio Project File - Name="rsi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=rsi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rsi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rsi.mak" CFG="rsi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rsi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rsi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RayStorm/Rsi", FUAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rsi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\commonutil" /I "..\modules" /I "..\vecmath" /I "..\texture" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX"typedefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  release\rsi.dll  ..\..\..\raystorm\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "rsi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\commonutil" /I "..\modules" /I "..\vecmath" /I "..\texture" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  debug\rsi.dll  ..\..\..\raystorm\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "rsi - Win32 Release"
# Name "rsi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp,c,rc"
# Begin Source File

SOURCE=ACTOR.CPP
# End Source File
# Begin Source File

SOURCE=BOX.CPP
# End Source File
# Begin Source File

SOURCE=BRUSH.CPP
# End Source File
# Begin Source File

SOURCE=CAMERA.CPP
# End Source File
# Begin Source File

SOURCE=CONE.CPP
# End Source File
# Begin Source File

SOURCE=context.cpp
# End Source File
# Begin Source File

SOURCE=CSG.CPP
# End Source File
# Begin Source File

SOURCE=CYLINDER.CPP
# End Source File
# Begin Source File

SOURCE=.\Dof.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\Fileutil.cpp
# End Source File
# Begin Source File

SOURCE=Flares.cpp
# End Source File
# Begin Source File

SOURCE=GLOBAL.CPP
# End Source File
# Begin Source File

SOURCE=HTEXTURE.CPP
# End Source File
# Begin Source File

SOURCE=LIB.CPP
# End Source File
# Begin Source File

SOURCE=Light.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\list.cpp
# End Source File
# Begin Source File

SOURCE=OBJECT.CPP
# End Source File
# Begin Source File

SOURCE=OBJHAND.CPP
# End Source File
# Begin Source File

SOURCE=OCTREE.CPP
# End Source File
# Begin Source File

SOURCE=PARAM.CPP
# End Source File
# Begin Source File

SOURCE=PLANE.CPP
# End Source File
# Begin Source File

SOURCE=POLYSOLV.CPP
# End Source File
# Begin Source File

SOURCE=Rsi.cpp
# End Source File
# Begin Source File

SOURCE=.\rsi.rc
# End Source File
# Begin Source File

SOURCE=SOR.CPP
# End Source File
# Begin Source File

SOURCE=SPHERE.CPP
# End Source File
# Begin Source File

SOURCE=Star.cpp
# End Source File
# Begin Source File

SOURCE=SURFACE.CPP
# End Source File
# Begin Source File

SOURCE=Texture.cpp
# End Source File
# Begin Source File

SOURCE=TRIANGLE.CPP
# End Source File
# Begin Source File

SOURCE=VOXEL.CPP
# End Source File
# Begin Source File

SOURCE=WORLD.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\ACTOR.H
# End Source File
# Begin Source File

SOURCE=.\BOX.H
# End Source File
# Begin Source File

SOURCE=.\BRUSH.H
# End Source File
# Begin Source File

SOURCE=.\CAMERA.H
# End Source File
# Begin Source File

SOURCE=.\COLOR.H
# End Source File
# Begin Source File

SOURCE=.\CONE.H
# End Source File
# Begin Source File

SOURCE=.\context.h
# End Source File
# Begin Source File

SOURCE=.\CSG.H
# End Source File
# Begin Source File

SOURCE=.\CYLINDER.H
# End Source File
# Begin Source File

SOURCE=..\texture\dialog.h
# End Source File
# Begin Source File

SOURCE=.\dof.h
# End Source File
# Begin Source File

SOURCE=.\ERROR.H
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\Fileutil.h
# End Source File
# Begin Source File

SOURCE=.\FLARES.H
# End Source File
# Begin Source File

SOURCE=.\HTEXTURE.H
# End Source File
# Begin Source File

SOURCE=.\INTERSEC.H
# End Source File
# Begin Source File

SOURCE=.\LIB.H
# End Source File
# Begin Source File

SOURCE=.\LIGHT.H
# End Source File
# Begin Source File

SOURCE=.\logo.h
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\noise.h
# End Source File
# Begin Source File

SOURCE=.\OBJECT.H
# End Source File
# Begin Source File

SOURCE=.\OBJHAND.H
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\objlink.h
# End Source File
# Begin Source File

SOURCE=.\OCTREE.H
# End Source File
# Begin Source File

SOURCE=.\Param.h
# End Source File
# Begin Source File

SOURCE=..\modules\picload.h
# End Source File
# Begin Source File

SOURCE=.\PLANE.H
# End Source File
# Begin Source File

SOURCE=.\POLYSOLV.H
# End Source File
# Begin Source File

SOURCE=.\Rsi.h
# End Source File
# Begin Source File

SOURCE=..\texture\rtexture.h
# End Source File
# Begin Source File

SOURCE=.\SOR.H
# End Source File
# Begin Source File

SOURCE=.\SPHERE.H
# End Source File
# Begin Source File

SOURCE=.\Star.h
# End Source File
# Begin Source File

SOURCE=.\SURFACE.H
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\TRIANGLE.H
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\typedefs.h
# End Source File
# Begin Source File

SOURCE=..\vecmath\VECMATH.H
# End Source File
# Begin Source File

SOURCE=.\VOXEL.H
# End Source File
# Begin Source File

SOURCE=.\WORLD.H
# End Source File
# End Group
# End Target
# End Project
