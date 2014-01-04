# Microsoft Developer Studio Project File - Name="Common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="Common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RayStorm/Common", LHAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W2 /GX /O2 /I "..\." /I "..\res" /I "..\..\rsi" /I "..\dialogs" /I "..\..\modules" /I "..\..\texture" /I "..\..\vecmath" /I "..\..\commonutil" /I "..\controls" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__MFC__" /D "__MATRIX_STACK__" /D "VC_EXTRALEAN" /D "_AFXDLL" /D "__OPENGL__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W2 /Gm /GX /ZI /Od /I "..\." /I "..\res" /I "..\..\rsi" /I "..\dialogs" /I "..\..\modules" /I "..\..\texture" /I "..\..\vecmath" /I "..\..\commonutil" /I "..\controls" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__MFC__" /D "__MATRIX_STACK__" /D "VC_EXTRALEAN" /D "__OPENGL__" /D "_AFXDLL" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Common - Win32 Release"
# Name "Common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\box.cpp
# End Source File
# Begin Source File

SOURCE=.\brush.cpp
# End Source File
# Begin Source File

SOURCE=.\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\cone.cpp
# End Source File
# Begin Source File

SOURCE=.\csg.cpp
# End Source File
# Begin Source File

SOURCE=.\cylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\Display.cpp
# End Source File
# Begin Source File

SOURCE=.\errors.cpp
# End Source File
# Begin Source File

SOURCE=.\flares.cpp
# End Source File
# Begin Source File

SOURCE=.\GLOBALS.CPP
# End Source File
# Begin Source File

SOURCE=.\iff_util.cpp
# End Source File
# Begin Source File

SOURCE=.\image.cpp
# End Source File
# Begin Source File

SOURCE=.\Light.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\list.cpp
# End Source File
# Begin Source File

SOURCE=.\mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\Objhand.cpp
# End Source File
# Begin Source File

SOURCE=.\ogl_util.cpp
# End Source File
# Begin Source File

SOURCE=.\plane.cpp
# End Source File
# Begin Source File

SOURCE=.\prefs.cpp
# End Source File
# Begin Source File

SOURCE=.\Preview.cpp
# End Source File
# Begin Source File

SOURCE=.\project.cpp
# End Source File
# Begin Source File

SOURCE=.\sci.cpp
# End Source File
# Begin Source File

SOURCE=.\sor.cpp
# End Source File
# Begin Source File

SOURCE=.\sphere.cpp
# End Source File
# Begin Source File

SOURCE=.\star.cpp
# End Source File
# Begin Source File

SOURCE=.\surface.cpp
# End Source File
# Begin Source File

SOURCE=.\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\Undo.cpp
# End Source File
# Begin Source File

SOURCE=.\View.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\box.h
# End Source File
# Begin Source File

SOURCE=..\BrowserView.h
# End Source File
# Begin Source File

SOURCE=.\brush.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=..\CamView.h
# End Source File
# Begin Source File

SOURCE=..\COLDEF.H
# End Source File
# Begin Source File

SOURCE=.\color.h
# End Source File
# Begin Source File

SOURCE=.\cone.h
# End Source File
# Begin Source File

SOURCE=.\csg.h
# End Source File
# Begin Source File

SOURCE=..\DIALOGS\CustTree.h
# End Source File
# Begin Source File

SOURCE=.\cylinder.h
# End Source File
# Begin Source File

SOURCE=..\..\texture\dialog.h
# End Source File
# Begin Source File

SOURCE=.\Display.h
# End Source File
# Begin Source File

SOURCE=.\errors.h
# End Source File
# Begin Source File

SOURCE=..\CONTROLS\FileButt.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\Fileutil.h
# End Source File
# Begin Source File

SOURCE=.\flares.h
# End Source File
# Begin Source File

SOURCE=.\gfx.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\iff_util.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\Iffparse.h
# End Source File
# Begin Source File

SOURCE=.\image.h
# End Source File
# Begin Source File

SOURCE=.\Light.h
# End Source File
# Begin Source File

SOURCE=.\magic.h
# End Source File
# Begin Source File

SOURCE=..\MaterialView.h
# End Source File
# Begin Source File

SOURCE=.\mesh.h
# End Source File
# Begin Source File

SOURCE=.\Mouse.h
# End Source File
# Begin Source File

SOURCE=.\object.h
# End Source File
# Begin Source File

SOURCE=.\Objhand.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\objlink.h
# End Source File
# Begin Source File

SOURCE=.\ogl_util.h
# End Source File
# Begin Source File

SOURCE=..\opengl.h
# End Source File
# Begin Source File

SOURCE=..\CONTROLS\PicCtl.h
# End Source File
# Begin Source File

SOURCE=..\DIALOGS\PicDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\modules\picload.h
# End Source File
# Begin Source File

SOURCE=.\plane.h
# End Source File
# Begin Source File

SOURCE=..\PREFHDR.H
# End Source File
# Begin Source File

SOURCE=.\prefs.h
# End Source File
# Begin Source File

SOURCE=.\Preview.h
# End Source File
# Begin Source File

SOURCE=.\project.h
# End Source File
# Begin Source File

SOURCE=..\DIALOGS\RendDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\Rscn.h
# End Source File
# Begin Source File

SOURCE=..\..\Rsi\Rsi.h
# End Source File
# Begin Source File

SOURCE=..\..\texture\rtexture.h
# End Source File
# Begin Source File

SOURCE=.\SCI.H
# End Source File
# Begin Source File

SOURCE=.\sor.h
# End Source File
# Begin Source File

SOURCE=.\sphere.h
# End Source File
# Begin Source File

SOURCE=.\star.h
# End Source File
# Begin Source File

SOURCE=.\surface.h
# End Source File
# Begin Source File

SOURCE=.\texture.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\typedefs.h
# End Source File
# Begin Source File

SOURCE=.\undo.h
# End Source File
# Begin Source File

SOURCE=.\utility.h
# End Source File
# Begin Source File

SOURCE=..\..\vecmath\VECMATH.H
# End Source File
# Begin Source File

SOURCE=.\view.h
# End Source File
# Begin Source File

SOURCE=..\CONTROLS\ViewButt.h
# End Source File
# End Group
# End Target
# End Project
