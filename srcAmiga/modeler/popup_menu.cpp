/***************
 * PROGRAM:       Modeler
 * NAME:          popup_menu.cpp
 * VERSION:       1.0 26.01.1996
 * DESCRIPTION:   class for popup menu
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    26.01.96 ah    initial release
 ***************/

#include <string.h>

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

#ifdef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif

#include <pragma/exec_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/intuition_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef POPUP_MENU_H
#include "popup_menu.h"
#endif

static void DrawWindow(struct Window *,struct DrawInfo *,POP_ITEM *,int,int,BOOL);
static void DrawFrame(struct Window *,int,struct DrawInfo *,char *,BOOL);

/*************
 * FUNCTION:      Popup_Menu
 * DESCRIPTION:   create popup menu
 * INPUT:         list     pointer to a list of menu items (text pointer of last item is NULL)
 *                screen   screen to open menu on
 * OUTPUT:        selected menu action, -1 if failed
 *************/
int Popup_Menu(POP_ITEM *list, struct Screen *screen)
{
	int items;
	struct Window *win;
	struct IntuiMessage *message;
	BOOL done=FALSE;
	WORD x,y, count,active=0,old_active;
	ULONG imsgcode,imsgclass;
	struct DrawInfo *drinfo;
	int itemheight;
	int maxlen=0;
	char *maxstr;

	items = 0;
	while(list[items].text)
	{
		if(strlen(list[items].text)>maxlen)
		{
			maxlen = strlen(list[items].text);
			maxstr = list[items].text;
		}
		items++;
	}

	x = screen->MouseX;
	y = screen->MouseY;
	drinfo = GetScreenDrawInfo(screen);
	if(drinfo)
	{
		itemheight = drinfo->dri_Font->tf_YSize+1;
		maxlen = TextLength(&screen->RastPort,maxstr,maxlen)+8;

		win = OpenWindowTags(NULL,
			WA_IDCMP,           IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS,
			WA_Left,            x - (maxlen>>1),
			WA_Top,             y - (itemheight>>1) - 2,
			WA_Width,           maxlen+8,
			WA_Height,          4+items*itemheight,
			WA_CustomScreen,    screen,
			WA_Activate,        TRUE,
			WA_SizeGadget,      FALSE,
			WA_DragBar,         FALSE,
			WA_DepthGadget,     FALSE,
			WA_CloseGadget,     FALSE,
			WA_Borderless,      TRUE,
			WA_AutoAdjust,      TRUE,
			WA_RMBTrap,         TRUE,
/*         WA_SimpleRefresh,   TRUE,
			WA_NoCareRefresh,   TRUE,*/
			WA_SmartRefresh,    TRUE,
			WA_ReportMouse,     TRUE,
		TAG_END);
		if(!win)
			return -1;

		DrawWindow(win,drinfo,list,items,-1,FALSE);
		DrawWindow(win,drinfo,list,items,0,TRUE);

		while(!done)
		{
			while((message=(struct IntuiMessage *)GetMsg(win->UserPort)))
			{
				imsgclass = message->Class;
				imsgcode = message->Code;
				x = win->MouseX;
				y = win->MouseY;
				ReplyMsg((struct Message *)message);

				switch(imsgclass)
				{
					case IDCMP_MOUSEMOVE:
						count = (y>=2) ? (y-2)/itemheight : ~0;
						old_active = active;

						if((x>2) && (x<(win->Width - 2))
							&& (count>=0) && (count<items))
						{
							if(old_active != count)
							{
								active = count;
								DrawWindow(win,drinfo,list,items,active,TRUE);
								if(old_active != -1)
									DrawWindow(win,drinfo,list,items,old_active,FALSE);
							}
						}
						else
						{
							if(active != -1)
							{
								DrawWindow(win,drinfo,list,items,active,FALSE);
								active = -1;
							}
						}
						break;
					case IDCMP_MOUSEBUTTONS:
						if(imsgcode == MENUUP)
							done = TRUE;
						break;
				}
			}
		}
	}

	FreeScreenDrawInfo(screen,drinfo);

	if(win)
		CloseWindow(win);

	if((x>2) && (x<(win->Width - 2))
		&& (count>=0) && (count<items))
		return list[active].action;
	else
		return POP_NONE;
}

/*************
 * FUNCTION:      DrawWindow
 * DESCRIPTION:   draw the popup list
 * INPUT:         win      window
 *                dri      draw info
 *                list     item list
 *                items    amount of items
 *                which    item to draw; if -1 draw all items
 *                active   if TRUE draw active item else draw inactive item
 * OUTPUT:        -
 *************/
static void DrawWindow(struct Window *win,struct DrawInfo *dri,POP_ITEM *list,
	int items,int which,BOOL active)
{
	int i;
	struct RastPort *rp = win->RPort;

	SetDrMd(rp,JAM1);

	// If we want to draw all entries then we draw window borders too.
	if(which == -1)
	{
		// Set background to MENU background color.
		SetRast(rp,(ULONG)dri->dri_Pens[BARBLOCKPEN]);

		SetAPen(rp,(ULONG)dri->dri_Pens[BARDETAILPEN]);
		Move(rp,0,-1+win->Height);
		Draw(rp,0,0);
		Draw(rp,-1+win->Width,0);
		Draw(rp,-1L+win->Width,-1+win->Height);
		Draw(rp,1,-1+win->Height);
		Draw(rp,1,1);
		Move(rp,-2+win->Width,1);
		Draw(rp,-2+win->Width,-2+win->Height);

		i = 0;
		while(list[i].text)
		{
			DrawFrame(win,i,dri,list[i].text,active);
			i++;
		}
	}
	else
		DrawFrame(win,which,dri,list[which].text,active);

}

/*************
 * FUNCTION:      DrawFrame
 * DESCRIPTION:   draw a single item
 * INPUT:         win      window
 *                which    item to draw; if -1 draw all items
 *                dri      draw info
 *                name     item string
 *                active   if TRUE draw active item else draw inactive item
 * OUTPUT:        -
 *************/
static void DrawFrame(struct Window *win,int pos,
	struct DrawInfo *dri,char *name,BOOL active)
{
	int itemheight=dri->dri_Font->tf_YSize+1;
	ULONG textpen,bpen;
	ULONG top,width,bottom,maxx,textwidth;
	struct RastPort *rp = win->RPort;

	textpen = dri->dri_Pens[TEXTPEN];

	if(active)
	{
		bpen = dri->dri_Pens[BARDETAILPEN];
		textpen = dri->dri_Pens[BARBLOCKPEN];
	}
	else
	{
		bpen = dri->dri_Pens[BARBLOCKPEN];
		textpen = dri->dri_Pens[BARDETAILPEN];
	}

	top = 2 + pos*itemheight;
	bottom = top + itemheight-1;
	maxx = win->Width - 4;

	SetAPen(rp,bpen);
	RectFill(rp,4,top,maxx-1,bottom);

	SetAPen(rp,textpen);

	width = win->Width-10;

	textwidth = TextLength(rp,name,(ULONG)strlen(name));

	Move(rp,5+(width-textwidth)/2,1+top+dri->dri_Font->tf_Baseline);
	Text(rp,name,strlen(name));
}
