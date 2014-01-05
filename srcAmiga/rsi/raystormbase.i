;***************
;* MODUL:         RayStorm
;* NAME:          raystormbase.i
;* VERSION:       1.0 03.06.1996
;* DESCRIPTION:   definitions for RayStorm run-time library
;*	AUTHORS:			Andreas Heumann, Mike Hesser
;* HISTORY:
;*    DATE		NAME	COMMENT
;*		03.06.96	ah		initial release
;***************/

   IFND  RAYSTORM_BASE_I
RAYSTORM_BASE_I SET 1


   IFND  EXEC_TYPES_I
   INCLUDE  "exec/types.i"
   ENDC   ; EXEC_TYPES_I

   IFND  EXEC_LIBRARIES_I
   INCLUDE  "exec/libraries.i"
   ENDC   ; EXEC_LIBRARIES_I

;--------------------------
; library data structures
;--------------------------

   STRUCTURE RayStormBase,LIB_SIZE
   UBYTE   sb_Flags
   UBYTE   sb_pad
   ULONG   sb_SysLib
   ULONG   sb_DosLib
   ULONG   sb_SegList
   LABEL   RayStormBase_SIZEOF


RAYSTORMNAME   MACRO
      DC.B   'raystorm.library',0
      ENDM

   ENDC  ;RAYSTORM_BASE_I

