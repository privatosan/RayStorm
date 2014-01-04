# Microsoft Developer Studio Project File - Name="bump" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=bump - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bump.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bump.mak" CFG="bump - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bump - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "bump - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RayStorm/texture/bump", ANDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bump - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp2 /MD /W3 /GX /O2 /I "..\common" /I "..\..\commonutil" /I "..\..\vecmath" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "__DNOISE__" /D "_WINDLL" /D "_MBCS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:".\Release\bump.itx"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  release\bump.itx  ..\..\..\..\raystorm\textures\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "bump - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp2 /MD /W3 /GX /Zi /O2 /I "..\common" /I "..\..\commonutil" /I "..\..\vecmath" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "__DNOISE__" /D "_WINDLL" /D "_MBCS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:".\Debug\bump.itx"

!ENDIF 

# Begin Target

# Name "bump - Win32 Release"
# Name "bump - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=bump.cpp
# End Source File
# Begin Source File

SOURCE=Cppcore.cpp
# End Source File
# Begin Source File

SOURCE=CPPPAGES.CPP
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\noise.cpp
# End Source File
# Begin Source File

SOURCE=..\common\SSEDIT.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\cppcore.h
# End Source File
# Begin Source File

SOURCE=.\cpppages.h
# End Source File
# Begin Source File

SOURCE=..\common\imdefs.h
# End Source File
# Begin Source File

SOURCE=..\common\imtextur.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\noise.h
# End Source File
# Begin Source File

SOURCE=..\common\ssedit.h
# End Source File
# Begin Source File

SOURCE=..\common\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\common\txdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\typedefs.h
# End Source File
# Begin Source File

SOURCE=..\..\vecmath\VECMATH.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
