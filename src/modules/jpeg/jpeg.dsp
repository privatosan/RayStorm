# Microsoft Developer Studio Project File - Name="Jpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Jpeg - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpeg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeg.mak" CFG="Jpeg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Jpeg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Jpeg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RayStorm/modules/jpeg", NDDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Jpeg - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O1 /I "..\..\rsi" /I "..\\" /I "..\..\commonutil" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
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
PostBuild_Cmds=copy release\jpeg.dll ..\..\..\..\raystorm\modules\picture
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Jpeg - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /O1 /I "..\..\rsi" /I "..\\" /I "..\..\commonutil" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
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
PostBuild_Cmds=copy debug\jpeg.dll ..\..\..\..\raystorm\modules\picture
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Jpeg - Win32 Release"
# Name "Jpeg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=JCAPIMIN.C
# End Source File
# Begin Source File

SOURCE=JCAPISTD.C
# End Source File
# Begin Source File

SOURCE=JCCOEFCT.C
# End Source File
# Begin Source File

SOURCE=JCCOLOR.C
# End Source File
# Begin Source File

SOURCE=JCDCTMGR.C
# End Source File
# Begin Source File

SOURCE=JCHUFF.C
# End Source File
# Begin Source File

SOURCE=JCINIT.C
# End Source File
# Begin Source File

SOURCE=JCMAINCT.C
# End Source File
# Begin Source File

SOURCE=JCMARKER.C
# End Source File
# Begin Source File

SOURCE=JCMASTER.C
# End Source File
# Begin Source File

SOURCE=JCOMAPI.C
# End Source File
# Begin Source File

SOURCE=JCPARAM.C
# End Source File
# Begin Source File

SOURCE=JCPHUFF.C
# End Source File
# Begin Source File

SOURCE=JCPREPCT.C
# End Source File
# Begin Source File

SOURCE=JCSAMPLE.C
# End Source File
# Begin Source File

SOURCE=JCTRANS.C
# End Source File
# Begin Source File

SOURCE=JDAPIMIN.C
# End Source File
# Begin Source File

SOURCE=JDAPISTD.C
# End Source File
# Begin Source File

SOURCE=JDATADST.C
# End Source File
# Begin Source File

SOURCE=JDATASRC.C
# End Source File
# Begin Source File

SOURCE=JDCOEFCT.C
# End Source File
# Begin Source File

SOURCE=JDCOLOR.C
# End Source File
# Begin Source File

SOURCE=JDDCTMGR.C
# End Source File
# Begin Source File

SOURCE=JDHUFF.C
# End Source File
# Begin Source File

SOURCE=JDINPUT.C
# End Source File
# Begin Source File

SOURCE=JDMAINCT.C
# End Source File
# Begin Source File

SOURCE=JDMARKER.C
# End Source File
# Begin Source File

SOURCE=JDMASTER.C
# End Source File
# Begin Source File

SOURCE=JDMERGE.C
# End Source File
# Begin Source File

SOURCE=JDPHUFF.C
# End Source File
# Begin Source File

SOURCE=JDPOSTCT.C
# End Source File
# Begin Source File

SOURCE=JDSAMPLE.C
# End Source File
# Begin Source File

SOURCE=JDTRANS.C
# End Source File
# Begin Source File

SOURCE=JERROR.C
# End Source File
# Begin Source File

SOURCE=JFDCTFLT.C
# End Source File
# Begin Source File

SOURCE=JFDCTFST.C
# End Source File
# Begin Source File

SOURCE=JFDCTINT.C
# End Source File
# Begin Source File

SOURCE=JIDCTFLT.C
# End Source File
# Begin Source File

SOURCE=JIDCTFST.C
# End Source File
# Begin Source File

SOURCE=JIDCTINT.C
# End Source File
# Begin Source File

SOURCE=JIDCTRED.C
# End Source File
# Begin Source File

SOURCE=JMEMMGR.C
# End Source File
# Begin Source File

SOURCE=JMEMNOBS.C
# End Source File
# Begin Source File

SOURCE=Jpeg.c
# End Source File
# Begin Source File

SOURCE=Jpeg.def
# End Source File
# Begin Source File

SOURCE=JQUANT1.C
# End Source File
# Begin Source File

SOURCE=JQUANT2.C
# End Source File
# Begin Source File

SOURCE=JUTILS.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\Rsi\COLOR.H
# End Source File
# Begin Source File

SOURCE=.\jchuff.h
# End Source File
# Begin Source File

SOURCE=.\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\jdct.h
# End Source File
# Begin Source File

SOURCE=.\jdhuff.h
# End Source File
# Begin Source File

SOURCE=.\jerror.h
# End Source File
# Begin Source File

SOURCE=.\jinclude.h
# End Source File
# Begin Source File

SOURCE=.\jmemsys.h
# End Source File
# Begin Source File

SOURCE=.\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\jpegint.h
# End Source File
# Begin Source File

SOURCE=.\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\jversion.h
# End Source File
# Begin Source File

SOURCE=..\picload.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtil\typedefs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
