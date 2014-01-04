# Microsoft Developer Studio Generated NMAKE File, Based on scenario.dsp
!IF "$(CFG)" == ""
CFG=scenario - Win32 Debug
!MESSAGE No configuration specified. Defaulting to scenario - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "scenario - Win32 Release" && "$(CFG)" != "scenario - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "scenario - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\scenario.exe"

!ELSE 

ALL : "iffparse - Win32 Release" "vecmath - Win32 Release" "Common - Win32 Release" "$(OUTDIR)\scenario.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Common - Win32 ReleaseCLEAN" "vecmath - Win32 ReleaseCLEAN" "iffparse - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\BrowserView.obj"
	-@erase "$(INTDIR)\BseButt.obj"
	-@erase "$(INTDIR)\CameraDlg.obj"
	-@erase "$(INTDIR)\CamView.obj"
	-@erase "$(INTDIR)\CMeshDlg.obj"
	-@erase "$(INTDIR)\ColButt.obj"
	-@erase "$(INTDIR)\Coldef.obj"
	-@erase "$(INTDIR)\CSGDlg.obj"
	-@erase "$(INTDIR)\CustTree.obj"
	-@erase "$(INTDIR)\ddxddv.obj"
	-@erase "$(INTDIR)\Document.obj"
	-@erase "$(INTDIR)\FileButt.obj"
	-@erase "$(INTDIR)\Fileutil.obj"
	-@erase "$(INTDIR)\gfx.obj"
	-@erase "$(INTDIR)\GlobalDlg.obj"
	-@erase "$(INTDIR)\GridDlg.obj"
	-@erase "$(INTDIR)\LightDlg.obj"
	-@erase "$(INTDIR)\Mainfrm.obj"
	-@erase "$(INTDIR)\MaterialView.obj"
	-@erase "$(INTDIR)\MeshDlg.obj"
	-@erase "$(INTDIR)\OSelDlg.obj"
	-@erase "$(INTDIR)\PicCtl.obj"
	-@erase "$(INTDIR)\PicDlg.obj"
	-@erase "$(INTDIR)\PrefDlg.obj"
	-@erase "$(INTDIR)\PrimDlg.obj"
	-@erase "$(INTDIR)\RendDlg.obj"
	-@erase "$(INTDIR)\Scenario.obj"
	-@erase "$(INTDIR)\scenario.res"
	-@erase "$(INTDIR)\SlideCtl.obj"
	-@erase "$(INTDIR)\SplitEx.obj"
	-@erase "$(INTDIR)\SurfDlg.obj"
	-@erase "$(INTDIR)\TransDlg.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\ViewButt.obj"
	-@erase "$(OUTDIR)\scenario.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /GX /O2 /I "." /I ".\res" /I "..\commonutil" /I ".\commonscenario" /I ".\controls" /I ".\dialogs" /I "..\vecmath" /I "..\texture" /I "..\modules" /I "..\rsi" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__MFC__" /D "__MATRIX_STACK__" /D "VC_EXTRALEAN" /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scenario.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scenario.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\rsi\release\rsi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\scenario.pdb" /machine:I386 /out:"$(OUTDIR)\scenario.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BrowserView.obj" \
	"$(INTDIR)\CamView.obj" \
	"$(INTDIR)\Coldef.obj" \
	"$(INTDIR)\ddxddv.obj" \
	"$(INTDIR)\Document.obj" \
	"$(INTDIR)\gfx.obj" \
	"$(INTDIR)\Mainfrm.obj" \
	"$(INTDIR)\MaterialView.obj" \
	"$(INTDIR)\Scenario.obj" \
	"$(INTDIR)\utility.obj" \
	"$(INTDIR)\scenario.res" \
	"$(INTDIR)\PicCtl.obj" \
	"$(INTDIR)\ColButt.obj" \
	"$(INTDIR)\FileButt.obj" \
	"$(INTDIR)\BseButt.obj" \
	"$(INTDIR)\SlideCtl.obj" \
	"$(INTDIR)\ViewButt.obj" \
	"$(INTDIR)\TransDlg.obj" \
	"$(INTDIR)\CMeshDlg.obj" \
	"$(INTDIR)\CSGDlg.obj" \
	"$(INTDIR)\CustTree.obj" \
	"$(INTDIR)\GlobalDlg.obj" \
	"$(INTDIR)\GridDlg.obj" \
	"$(INTDIR)\LightDlg.obj" \
	"$(INTDIR)\MeshDlg.obj" \
	"$(INTDIR)\OSelDlg.obj" \
	"$(INTDIR)\PicDlg.obj" \
	"$(INTDIR)\PrefDlg.obj" \
	"$(INTDIR)\PrimDlg.obj" \
	"$(INTDIR)\RendDlg.obj" \
	"$(INTDIR)\SurfDlg.obj" \
	"$(INTDIR)\CameraDlg.obj" \
	"$(INTDIR)\Fileutil.obj" \
	"$(INTDIR)\SplitEx.obj" \
	".\CommonScenario\Release\Common.lib" \
	"..\vecmath\Release\vecmath.lib" \
	"..\CommonUtil\Release\iffparse.lib"

"$(OUTDIR)\scenario.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "scenario - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\scenario.exe"

!ELSE 

ALL : "iffparse - Win32 Debug" "vecmath - Win32 Debug" "Common - Win32 Debug" "$(OUTDIR)\scenario.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Common - Win32 DebugCLEAN" "vecmath - Win32 DebugCLEAN" "iffparse - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\BrowserView.obj"
	-@erase "$(INTDIR)\BseButt.obj"
	-@erase "$(INTDIR)\CameraDlg.obj"
	-@erase "$(INTDIR)\CamView.obj"
	-@erase "$(INTDIR)\CMeshDlg.obj"
	-@erase "$(INTDIR)\ColButt.obj"
	-@erase "$(INTDIR)\Coldef.obj"
	-@erase "$(INTDIR)\CSGDlg.obj"
	-@erase "$(INTDIR)\CustTree.obj"
	-@erase "$(INTDIR)\ddxddv.obj"
	-@erase "$(INTDIR)\Document.obj"
	-@erase "$(INTDIR)\FileButt.obj"
	-@erase "$(INTDIR)\Fileutil.obj"
	-@erase "$(INTDIR)\gfx.obj"
	-@erase "$(INTDIR)\GlobalDlg.obj"
	-@erase "$(INTDIR)\GridDlg.obj"
	-@erase "$(INTDIR)\LightDlg.obj"
	-@erase "$(INTDIR)\Mainfrm.obj"
	-@erase "$(INTDIR)\MaterialView.obj"
	-@erase "$(INTDIR)\MeshDlg.obj"
	-@erase "$(INTDIR)\OSelDlg.obj"
	-@erase "$(INTDIR)\PicCtl.obj"
	-@erase "$(INTDIR)\PicDlg.obj"
	-@erase "$(INTDIR)\PrefDlg.obj"
	-@erase "$(INTDIR)\PrimDlg.obj"
	-@erase "$(INTDIR)\RendDlg.obj"
	-@erase "$(INTDIR)\Scenario.obj"
	-@erase "$(INTDIR)\scenario.res"
	-@erase "$(INTDIR)\SlideCtl.obj"
	-@erase "$(INTDIR)\SplitEx.obj"
	-@erase "$(INTDIR)\SurfDlg.obj"
	-@erase "$(INTDIR)\TransDlg.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\ViewButt.obj"
	-@erase "$(OUTDIR)\scenario.exe"
	-@erase "$(OUTDIR)\scenario.ilk"
	-@erase "$(OUTDIR)\scenario.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /Gm /ZI /Od /I "." /I ".\res" /I "..\commonutil" /I ".\commonscenario" /I ".\controls" /I ".\dialogs" /I "..\vecmath" /I "..\texture" /I "..\modules" /I "..\rsi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "__MFC__" /D "__MATRIX_STACK__" /D "VC_EXTRALEAN" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scenario.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scenario.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\rsi\debug\rsi.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\scenario.pdb" /debug /machine:I386 /out:"$(OUTDIR)\scenario.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\BrowserView.obj" \
	"$(INTDIR)\CamView.obj" \
	"$(INTDIR)\Coldef.obj" \
	"$(INTDIR)\ddxddv.obj" \
	"$(INTDIR)\Document.obj" \
	"$(INTDIR)\gfx.obj" \
	"$(INTDIR)\Mainfrm.obj" \
	"$(INTDIR)\MaterialView.obj" \
	"$(INTDIR)\Scenario.obj" \
	"$(INTDIR)\utility.obj" \
	"$(INTDIR)\scenario.res" \
	"$(INTDIR)\PicCtl.obj" \
	"$(INTDIR)\ColButt.obj" \
	"$(INTDIR)\FileButt.obj" \
	"$(INTDIR)\BseButt.obj" \
	"$(INTDIR)\SlideCtl.obj" \
	"$(INTDIR)\ViewButt.obj" \
	"$(INTDIR)\TransDlg.obj" \
	"$(INTDIR)\CMeshDlg.obj" \
	"$(INTDIR)\CSGDlg.obj" \
	"$(INTDIR)\CustTree.obj" \
	"$(INTDIR)\GlobalDlg.obj" \
	"$(INTDIR)\GridDlg.obj" \
	"$(INTDIR)\LightDlg.obj" \
	"$(INTDIR)\MeshDlg.obj" \
	"$(INTDIR)\OSelDlg.obj" \
	"$(INTDIR)\PicDlg.obj" \
	"$(INTDIR)\PrefDlg.obj" \
	"$(INTDIR)\PrimDlg.obj" \
	"$(INTDIR)\RendDlg.obj" \
	"$(INTDIR)\SurfDlg.obj" \
	"$(INTDIR)\CameraDlg.obj" \
	"$(INTDIR)\Fileutil.obj" \
	"$(INTDIR)\SplitEx.obj" \
	".\CommonScenario\Debug\Common.lib" \
	"..\vecmath\Debug\vecmath.lib" \
	"..\CommonUtil\Debug\iffparse.lib"

"$(OUTDIR)\scenario.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("scenario.dep")
!INCLUDE "scenario.dep"
!ELSE 
!MESSAGE Warning: cannot find "scenario.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "scenario - Win32 Release" || "$(CFG)" == "scenario - Win32 Debug"
SOURCE=.\BrowserView.cpp

"$(INTDIR)\BrowserView.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CONTROLS\BseButt.cpp

"$(INTDIR)\BseButt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\CameraDlg.cpp

"$(INTDIR)\CameraDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\CamView.cpp

"$(INTDIR)\CamView.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DIALOGS\CMeshDlg.cpp

"$(INTDIR)\CMeshDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\CONTROLS\ColButt.cpp

"$(INTDIR)\ColButt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Coldef.cpp

"$(INTDIR)\Coldef.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DIALOGS\CSGDlg.cpp

"$(INTDIR)\CSGDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\CustTree.cpp

"$(INTDIR)\CustTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ddxddv.cpp

"$(INTDIR)\ddxddv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Document.cpp

"$(INTDIR)\Document.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CONTROLS\FileButt.cpp

"$(INTDIR)\FileButt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\CommonUtil\Fileutil.cpp

"$(INTDIR)\Fileutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gfx.cpp

"$(INTDIR)\gfx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DIALOGS\GlobalDlg.cpp

"$(INTDIR)\GlobalDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\GridDlg.cpp

"$(INTDIR)\GridDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\LightDlg.cpp

"$(INTDIR)\LightDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Mainfrm.cpp

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MaterialView.cpp

"$(INTDIR)\MaterialView.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DIALOGS\MeshDlg.cpp

"$(INTDIR)\MeshDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\OSelDlg.cpp

"$(INTDIR)\OSelDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\CONTROLS\PicCtl.cpp

"$(INTDIR)\PicCtl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\PicDlg.cpp

"$(INTDIR)\PicDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\PrefDlg.cpp

"$(INTDIR)\PrefDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\PrimDlg.cpp

"$(INTDIR)\PrimDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\RendDlg.cpp

"$(INTDIR)\RendDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Scenario.cpp

"$(INTDIR)\Scenario.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RES\scenario.rc

!IF  "$(CFG)" == "scenario - Win32 Release"


"$(INTDIR)\scenario.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\scenario.res" /i "RES" /d "NDEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "scenario - Win32 Debug"


"$(INTDIR)\scenario.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\scenario.res" /i "RES" /d "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ENDIF 

SOURCE=.\CONTROLS\SlideCtl.cpp

"$(INTDIR)\SlideCtl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SplitEx.cpp

"$(INTDIR)\SplitEx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DIALOGS\SurfDlg.cpp

"$(INTDIR)\SurfDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DIALOGS\TransDlg.cpp

"$(INTDIR)\TransDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\utility.cpp

"$(INTDIR)\utility.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CONTROLS\ViewButt.cpp

"$(INTDIR)\ViewButt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "scenario - Win32 Release"

"Common - Win32 Release" : 
   cd ".\CommonScenario"
   $(MAKE) /$(MAKEFLAGS) /F .\Common.mak CFG="Common - Win32 Release" 
   cd ".."

"Common - Win32 ReleaseCLEAN" : 
   cd ".\CommonScenario"
   $(MAKE) /$(MAKEFLAGS) /F .\Common.mak CFG="Common - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "scenario - Win32 Debug"

"Common - Win32 Debug" : 
   cd ".\CommonScenario"
   $(MAKE) /$(MAKEFLAGS) /F .\Common.mak CFG="Common - Win32 Debug" 
   cd ".."

"Common - Win32 DebugCLEAN" : 
   cd ".\CommonScenario"
   $(MAKE) /$(MAKEFLAGS) /F .\Common.mak CFG="Common - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "scenario - Win32 Release"

"vecmath - Win32 Release" : 
   cd "\projects\RayStorm\vecmath"
   $(MAKE) /$(MAKEFLAGS) /F .\vecmath.mak CFG="vecmath - Win32 Release" 
   cd "..\Scenario"

"vecmath - Win32 ReleaseCLEAN" : 
   cd "\projects\RayStorm\vecmath"
   $(MAKE) /$(MAKEFLAGS) /F .\vecmath.mak CFG="vecmath - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Scenario"

!ELSEIF  "$(CFG)" == "scenario - Win32 Debug"

"vecmath - Win32 Debug" : 
   cd "\projects\RayStorm\vecmath"
   $(MAKE) /$(MAKEFLAGS) /F .\vecmath.mak CFG="vecmath - Win32 Debug" 
   cd "..\Scenario"

"vecmath - Win32 DebugCLEAN" : 
   cd "\projects\RayStorm\vecmath"
   $(MAKE) /$(MAKEFLAGS) /F .\vecmath.mak CFG="vecmath - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Scenario"

!ENDIF 

!IF  "$(CFG)" == "scenario - Win32 Release"

"iffparse - Win32 Release" : 
   cd "\projects\RayStorm\CommonUtil"
   $(MAKE) /$(MAKEFLAGS) /F .\iffparse.mak CFG="iffparse - Win32 Release" 
   cd "..\Scenario"

"iffparse - Win32 ReleaseCLEAN" : 
   cd "\projects\RayStorm\CommonUtil"
   $(MAKE) /$(MAKEFLAGS) /F .\iffparse.mak CFG="iffparse - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Scenario"

!ELSEIF  "$(CFG)" == "scenario - Win32 Debug"

"iffparse - Win32 Debug" : 
   cd "\projects\RayStorm\CommonUtil"
   $(MAKE) /$(MAKEFLAGS) /F .\iffparse.mak CFG="iffparse - Win32 Debug" 
   cd "..\Scenario"

"iffparse - Win32 DebugCLEAN" : 
   cd "\projects\RayStorm\CommonUtil"
   $(MAKE) /$(MAKEFLAGS) /F .\iffparse.mak CFG="iffparse - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Scenario"

!ENDIF 


!ENDIF 

