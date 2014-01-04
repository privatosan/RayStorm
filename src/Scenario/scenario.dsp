# Microsoft Developer Studio Project File - Name="scenario" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=scenario - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "scenario.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scenario.mak" CFG="scenario - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scenario - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "scenario - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RayStorm/Scenario", OHAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "scenario - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /GX /O2 /I "." /I ".\res" /I "..\commonutil" /I ".\commonscenario" /I ".\controls" /I ".\dialogs" /I "..\vecmath" /I "..\texture" /I "..\modules" /I "..\rsi" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__MFC__" /D "__MATRIX_STACK__" /D "VC_EXTRALEAN" /D "_AFXDLL" /D "__OPENGL__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\rsi\release\rsi.lib opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:yes /machine:I386
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy    release\scenario.exe    ..\..\..\raystorm\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "scenario - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /Gm /ZI /Od /I "." /I ".\res" /I "..\commonutil" /I ".\commonscenario" /I ".\controls" /I ".\dialogs" /I "..\vecmath" /I "..\texture" /I "..\modules" /I "..\rsi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "__MFC__" /D "__MATRIX_STACK__" /D "VC_EXTRALEAN" /D "__OPENGL__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\rsi\debug\rsi.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy    debug\scenario.exe    ..\..\..\raystorm\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "scenario - Win32 Release"
# Name "scenario - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BrowserView.cpp
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\BseButt.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\CameraDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CamView.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\CMeshDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\ColButt.cpp
# End Source File
# Begin Source File

SOURCE=.\Coldef.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\color.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\CSGDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\CustTree.cpp
# End Source File
# Begin Source File

SOURCE=.\ddxddv.cpp
# End Source File
# Begin Source File

SOURCE=.\Document.cpp
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\FileButt.cpp
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\Fileutil.cpp
# End Source File
# Begin Source File

SOURCE=.\gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\GlobalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\GridDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\LightDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MaterialView.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\MeshDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\opengl.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\OSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\PicCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\PicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\PrefDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\PrimDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\RendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Scenario.cpp
# End Source File
# Begin Source File

SOURCE=.\RES\scenario.rc
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\SlideCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\SurfDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\TransDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\utility.cpp
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\ViewButt.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Rsi\ACTOR.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\box.h
# End Source File
# Begin Source File

SOURCE=.\BrowserView.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\BRUSH.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\brush.h
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\BseButt.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\CAMERA.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\camera.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\CameraDlg.h
# End Source File
# Begin Source File

SOURCE=.\CamView.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\CMeshDlg.h
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\ColButt.h
# End Source File
# Begin Source File

SOURCE=.\COLDEF.H
# End Source File
# Begin Source File

SOURCE=..\Rsi\COLOR.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\color.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\CSG.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\csg.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\CSGDlg.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\CustTree.h
# End Source File
# Begin Source File

SOURCE=.\ddxddv.h
# End Source File
# Begin Source File

SOURCE=..\texture\dialog.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\Display.h
# End Source File
# Begin Source File

SOURCE=.\Document.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\dof.h
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\FileButt.h
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\Fileutil.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\flares.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\gfx.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\GlobalDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\globals.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\GridDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\iff_util.h
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\Iffparse.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\INTERSEC.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\Light.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\LightDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\magic.h
# End Source File
# Begin Source File

SOURCE=.\Mainfrm.h
# End Source File
# Begin Source File

SOURCE=.\MaterialView.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\mesh.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\MeshDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\Mouse.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\OBJECT.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\object.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\OCTREE.H
# End Source File
# Begin Source File

SOURCE=.\opengl.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\OSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\PicCtl.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\PicDlg.h
# End Source File
# Begin Source File

SOURCE=..\modules\picload.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\plane.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\PrefDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\prefs.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\Preview.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\PrimDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\project.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\RendDlg.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\Rsi.h
# End Source File
# Begin Source File

SOURCE=..\texture\rtexture.h
# End Source File
# Begin Source File

SOURCE=.\SCENARIO.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\SCI.H
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\SlideCtl.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\sphere.h
# End Source File
# Begin Source File

SOURCE=.\SplitEx.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\star.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\SURFACE.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\surface.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\SurfDlg.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\texture.h
# End Source File
# Begin Source File

SOURCE=.\DIALOGS\TransDlg.h
# End Source File
# Begin Source File

SOURCE=..\CommonUtil\typedefs.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\undo.h
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\utility.h
# End Source File
# Begin Source File

SOURCE=..\vecmath\VECMATH.H
# End Source File
# Begin Source File

SOURCE=.\CommonScenario\view.h
# End Source File
# Begin Source File

SOURCE=.\CONTROLS\ViewButt.h
# End Source File
# Begin Source File

SOURCE=..\Rsi\VOXEL.H
# End Source File
# Begin Source File

SOURCE=..\Rsi\WORLD.H
# End Source File
# End Group
# Begin Group "Bitmaps"

# PROP Default_Filter "bmp,ico"
# Begin Source File

SOURCE=.\RES\BITMAPS\AddNew.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\browse.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\BROWSER.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\Delete.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\file.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\folder.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\MoveDown.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\MoveUp.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\name.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\SCENARIO.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\BITMAPS\view.bmp
# End Source File
# End Group
# End Target
# End Project
