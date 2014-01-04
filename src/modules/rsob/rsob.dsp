# Microsoft Developer Studio Project File - Name="rsob" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=rsob - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rsob.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rsob.mak" CFG="rsob - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rsob - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rsob - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RayStorm/modules/rsob", RHDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rsob - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "rsob___Win32_Release"
# PROP BASE Intermediate_Dir "rsob___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RSOB_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\commonutil" /I "..\..\vecmath" /I "..\..\scenario\commonscenario" /I "..\..\scenario" /I "..\..\rsi" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RSOB_EXPORTS" /D "__MATRIX_STACK__" /D "__MFC__" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\rsob.dll ..\..\..\..\raystorm\modules\object
# End Special Build Tool

!ELSEIF  "$(CFG)" == "rsob - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rsob___Win32_Debug"
# PROP BASE Intermediate_Dir "rsob___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RSOB_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\commonutil" /I "..\..\vecmath" /I "..\..\scenario\commonscenario" /I "..\..\scenario" /I "..\..\rsi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RSOB_EXPORTS" /D "__MATRIX_STACK__" /D "__MFC__" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\rsob.dll ..\..\..\..\raystorm\modules\object
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "rsob - Win32 Release"
# Name "rsob - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\iff_util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\list.cpp
# End Source File
# Begin Source File

SOURCE=.\rsob.cpp
# End Source File
# Begin Source File

SOURCE=.\rsob.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\brush.h
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\COLDEF.H
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\color.h
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\Display.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\Fileutil.h
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\gfx.h
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\iff_util.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\Iffparse.h
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\object.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\objlink.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\Rscn.h
# End Source File
# Begin Source File

SOURCE=..\..\Rsi\Rsi.h
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\surface.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\typedefs.h
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\undo.h
# End Source File
# Begin Source File

SOURCE=..\..\vecmath\VECMATH.H
# End Source File
# Begin Source File

SOURCE=..\..\Scenario\CommonScenario\view.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
