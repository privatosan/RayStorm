#include <libraries/mui.h>
#include <pragma/muimaster_lib.h>
#include <pragma/exec_lib.h>
#include <pragma/intuition_lib.h>
#include <exec/memory.h>
#include <clib/alib_protos.h>

struct ObjApp
{
	APTR  App;
	APTR  WI_label_0;
	APTR  s_fname, s_lname, s_street, s_city, s_country;
	APTR  s_mail, cm_amiga, cm_hd, cm_pc, cy_cpu, cy_form, cy_cur;
	APTR  b_info, b_print, b_save, b_show, b_cancel;
	APTR  tx_price;
};

#define __VERS__ "2.0"

extern struct ObjApp * CreateApp(void);
extern void DisposeApp(struct ObjApp *);
