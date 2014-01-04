# Microsoft Developer Studio Project File - Name="Png" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Png - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "png.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "png.mak" CFG="Png - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Png - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Png - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RayStorm/modules/png", XFDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Png - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /GX /O1 /I "..\\" /I ".\zlib" /I "..\..\rsi" /I "..\..\commonutil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
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
PostBuild_Cmds=copy release\png.dll ..\..\..\..\raystorm\modules\picture
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Png - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /Gm /GX /Zi /O1 /I "..\\" /I ".\zlib" /I "..\..\rsi" /I "..\..\commonutil" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\png.dll ..\..\..\..\raystorm\modules\picture
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Png - Win32 Release"
# Name "Png - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=ZLIB\ADLER32.C
# End Source File
# Begin Source File

SOURCE=ZLIB\COMPRESS.C
# End Source File
# Begin Source File

SOURCE=ZLIB\CRC32.C
# End Source File
# Begin Source File

SOURCE=ZLIB\DEFLATE.C
# End Source File
# Begin Source File

SOURCE=ZLIB\GZIO.C
# End Source File
# Begin Source File

SOURCE=ZLIB\INFBLOCK.C
# End Source File
# Begin Source File

SOURCE=ZLIB\INFCODES.C
# End Source File
# Begin Source File

SOURCE=ZLIB\INFFAST.C
# End Source File
# Begin Source File

SOURCE=ZLIB\INFLATE.C
# End Source File
# Begin Source File

SOURCE=ZLIB\INFTREES.C
# End Source File
# Begin Source File

SOURCE=ZLIB\INFUTIL.C
# End Source File
# Begin Source File

SOURCE=.\zlib\maketree.c
# End Source File
# Begin Source File

SOURCE="PNG-HAND.C"
# End Source File
# Begin Source File

SOURCE=PNG.C
# End Source File
# Begin Source File

SOURCE=png.def
# End Source File
# Begin Source File

SOURCE=PNGERROR.C
# End Source File
# Begin Source File

SOURCE=.\pngget.c
# End Source File
# Begin Source File

SOURCE=PNGMEM.C
# End Source File
# Begin Source File

SOURCE=PNGPREAD.C
# End Source File
# Begin Source File

SOURCE=PNGREAD.C
# End Source File
# Begin Source File

SOURCE=.\pngrio.c
# End Source File
# Begin Source File

SOURCE=PNGRTRAN.C
# End Source File
# Begin Source File

SOURCE=PNGRUTIL.C
# End Source File
# Begin Source File

SOURCE=.\pngset.c
# End Source File
# Begin Source File

SOURCE=PNGTRANS.C
# End Source File
# Begin Source File

SOURCE=.\pngwio.c
# End Source File
# Begin Source File

SOURCE=PNGWRITE.C
# End Source File
# Begin Source File

SOURCE=PNGWTRAN.C
# End Source File
# Begin Source File

SOURCE=PNGWUTIL.C
# End Source File
# Begin Source File

SOURCE=ZLIB\TREES.C
# End Source File
# Begin Source File

SOURCE=ZLIB\UNCOMPR.C
# End Source File
# Begin Source File

SOURCE=ZLIB\ZUTIL.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\Rsi\COLOR.H
# End Source File
# Begin Source File

SOURCE=.\zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=.\zlib\infblock.h
# End Source File
# Begin Source File

SOURCE=.\zlib\infcodes.h
# End Source File
# Begin Source File

SOURCE=.\zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=.\zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\zlib\infutil.h
# End Source File
# Begin Source File

SOURCE=..\picload.h
# End Source File
# Begin Source File

SOURCE=.\png.h
# End Source File
# Begin Source File

SOURCE=.\pngconf.h
# End Source File
# Begin Source File

SOURCE=.\zlib\trees.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\typedefs.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
