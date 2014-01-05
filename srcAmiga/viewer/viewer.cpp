#include <stdlib.h>
#include <stdio.h>

#include <exec/memory.h>
#include <intuition/intuitionbase.h>
#include <cybergraphics/cybergraphics.h>

#include <proto/cybergraphics.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#include "loader.h"

#define CYBERGFXVERSION  41L
#define CYBERGFXNAME "cybergraphics.library"

#define SCREEN_MIN_DEPTH      15L

#define MIN(a,b) ((a)<(b)?(a):(b))
#define SIGMASK(w) (1L<<((w)->UserPort->mp_SigBit))
#define GETIMSG(w) ((struct IntuiMessage *)GetMsg((w)->UserPort))

struct IntuitionBase *IntuitionBase;
struct Library *CyberGfxBase;

char Version[] = "$VER: RayStorm Image Viewer 1.0 (" __DATE__  ")";

ULONG BestCModeIDTags(Tag a, ...)
{
	return BestCModeIDTagList((struct TagItem*)&a);
}

/* display error message */

void ErrorF(char *Format,...)
{
	void *Data;
	struct EasyStruct EasyStruct;
	ULONG IDCMPFlags;

	Data = &Format+1L;
	if (((struct Process *)FindTask(NULL))->pr_CLI)
	{
		VPrintf(Format,Data);
		FPutC(Output(),'\n');
		Flush(Output());
	}
	else
	{
		EasyStruct.es_StructSize = sizeof(struct EasyStruct);
		EasyStruct.es_Flags = 0L;
		EasyStruct.es_Title = "Error";
		EasyStruct.es_TextFormat = Format;
		EasyStruct.es_GadgetFormat = "Ok";

		IDCMPFlags = 0L;
		EasyRequestArgs(NULL,&EasyStruct,&IDCMPFlags,Data);
	}
}

/* screen depth fallback */

ULONG NextDepth(ULONG Depth)

{
	switch (Depth)
	{
		case 24L: /* 24Bit not available? Try 16Bit!      */
			return 16L;
		case 16L: /* 15Bit not available? Try 15Bit!      */
			return 15L;
		default:  /* Not even 15Bit available? Forget it! */
			return 0L;
	}
}

/* main program */

static char arg_template[] = "FILE/A,16BIT/S";

int main(int argc, char **argv)
{
	ULONG DisplayID,Depth;
	struct Screen *CyberScreen;
	struct Window *WriteWindow;
	LONG done;
	struct IntuiMessage *IntMsg;
	IMAGE image;
	LONG src_x,src_y, old_src_x,old_src_y;
	ULONG argarray[2];
	struct RDArgs *rdargs;
	char *file;

	rdargs = ReadArgs((STRPTR)arg_template, (LONG*)argarray, NULL);
	if(!rdargs)
		return 20;

	file = (char*)argarray[0];

	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",39L);
	if(!IntuitionBase)
		return 20;

	CyberGfxBase = OpenLibrary(CYBERGFXNAME,CYBERGFXVERSION);
	if(!CyberGfxBase)
	{
		ErrorF ("Can't open \"%s\" version %ld or newer.",
			CYBERGFXNAME,CYBERGFXVERSION);
		CloseLibrary (&IntuitionBase->LibNode);
		return 10;
	}

	if(!Load(file, &image))
	{
		ErrorF ("Can't load image.");
		FreeArgs(rdargs);
		CloseLibrary (CyberGfxBase);
		CloseLibrary (&IntuitionBase->LibNode);
		return 10;
	}

	/* Let CyberGraphX search a display mode for us! */

	if(argarray[1])
		Depth = 16;
	else
		Depth = 24;

	FreeArgs(rdargs);

	while (Depth)
	{
		DisplayID = BestCModeIDTags(CYBRBIDTG_NominalWidth, image.width,
			CYBRBIDTG_NominalHeight, image.height,
			CYBRBIDTG_Depth, Depth,
			TAG_DONE);
		if(DisplayID != INVALID_ID)
		{
			/* Because older version of the "cybergraphics.library" don't handle */
			/* CYBRBIDTG_Depth properly we query the real depth of the mode.     */
			Depth = GetCyberIDAttr(CYBRIDATTR_DEPTH,DisplayID);
			break;
		}
		/* retry with less bits per pixel */
		Depth = NextDepth(Depth);
	}

	if (Depth < SCREEN_MIN_DEPTH)
	{
		ErrorF ("Can't find suitable display mode for %ldx%ldx.",
			image.width, image.height);
		CloseLibrary (CyberGfxBase);
		CloseLibrary (&IntuitionBase->LibNode);
		Cleanup(&image);
		return 5;
	}

	/* open screen, but let Intuition choose the actual dimensions */

	CyberScreen = OpenScreenTags(
		NULL,
		SA_Title,"RayStorm Image Viewer (C) 1998 Andreas Heumann",
		SA_DisplayID,DisplayID,
		SA_Depth,Depth,
		TAG_DONE);
	if(!CyberScreen)
	{
		ErrorF ("Can't open screen.");
		CloseLibrary (CyberGfxBase);
		CloseLibrary (&IntuitionBase->LibNode);
		Cleanup(&image);
		return 5;
	}

	/* create Write window */

	WriteWindow = OpenWindowTags(
		NULL,
		WA_Title,"Viewer",
		WA_Flags,
			WFLG_ACTIVATE|WFLG_SIMPLE_REFRESH|WFLG_RMBTRAP,
		WA_IDCMP,
			IDCMP_REFRESHWINDOW|IDCMP_VANILLAKEY|IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS,
		WA_Borderless, TRUE,
		WA_Left, 0,
		WA_Top, 0,
		WA_Width, image.width,
		WA_Height, image.height,
		WA_CustomScreen,CyberScreen,
		TAG_DONE);
	if(!WriteWindow)
	{
		ErrorF ("Can't open write window.");
		CloseScreen (CyberScreen);
		CloseLibrary (CyberGfxBase);
		CloseLibrary (&IntuitionBase->LibNode);
		Cleanup(&image);
		return 5;
	}

	src_x = 0;
	src_y = 0;
	WritePixelArray(
		image.colormap,
		src_x, src_y,
		image.width*sizeof(SMALL_COLOR),
		WriteWindow->RPort,
		0, 0,
		image.width,
		image.height,
		RECTFMT_RGBA);

	/* event loop */

	done = FALSE;
	while (!done)
	{
		(void)Wait(SIGMASK(WriteWindow));

		while (IntMsg=GETIMSG(WriteWindow))
		{
			switch (IntMsg->Class)
			{
				case IDCMP_REFRESHWINDOW:
					BeginRefresh(WriteWindow);
					WritePixelArray(
						image.colormap,
						src_x, src_y,
						image.width*sizeof(SMALL_COLOR),
						WriteWindow->RPort,
						0, 0,
						image.width,
						image.height,
						RECTFMT_RGBA);
					EndRefresh(WriteWindow,TRUE);
					break;
				case IDCMP_VANILLAKEY:
					switch(IntMsg->Qualifier & 0x7fff)
					{
						case 0:
							switch (IntMsg->Code)
							{
								case 27:
									done = TRUE;
									break;
							}
							break;
					}
					break;
				case IDCMP_RAWKEY:
					old_src_x = src_x;
					old_src_y = src_y;
					switch(IntMsg->Code)
					{
						case CURSORRIGHT:
							if(src_x + WriteWindow->Width + WriteWindow->LeftEdge < image.width)
							{
								src_x += 8;
								if(src_x + WriteWindow->Width + WriteWindow->LeftEdge > image.width)
									src_x = image.width - WriteWindow->Width - WriteWindow->LeftEdge;
							}
							break;
						case CURSORLEFT:
							if(src_x > 0)
							{
								src_x -= 8;
								if(src_x < 0)
									src_x = 0;
							}
							break;
						case CURSORDOWN:
							if(src_y + WriteWindow->Height + WriteWindow->TopEdge < image.height)
							{
								src_y += 8;
								if(src_y + WriteWindow->Height + WriteWindow->TopEdge > image.height)
									src_y = image.height - WriteWindow->Height - WriteWindow->TopEdge;
							}
							break;
						case CURSORUP:
							if(src_y > 0)
							{
								src_y -= 8;
								if(src_y < 0)
									src_y = 0;
							}
							break;
					}
					if((old_src_x != src_x) || (old_src_y != src_y))
					{
						if(old_src_x < src_x)
						{
							ClipBlit(
								WriteWindow->RPort,
								src_x - old_src_x,
								0,
								WriteWindow->RPort,
								0,
								0,
								WriteWindow->Width + WriteWindow->LeftEdge - (src_x - old_src_x),
								WriteWindow->Height + WriteWindow->TopEdge,
								0xc0);
							WritePixelArray(
								image.colormap,
								old_src_x + WriteWindow->Width + WriteWindow->LeftEdge,
								src_y,
								image.width*sizeof(SMALL_COLOR),
								WriteWindow->RPort,
								WriteWindow->Width + WriteWindow->LeftEdge - (src_x - old_src_x),
								0,
								src_x - old_src_x,
								WriteWindow->Height + WriteWindow->TopEdge,
								RECTFMT_RGBA);
						}
						else if(old_src_x > src_x)
						{
							ClipBlit(
								WriteWindow->RPort,
								0,
								0,
								WriteWindow->RPort,
								old_src_x - src_x,
								0,
								WriteWindow->Width + WriteWindow->LeftEdge - (old_src_x - src_x),
								WriteWindow->Height + WriteWindow->TopEdge,
								0xc0);
							WritePixelArray(
								image.colormap,
								src_x,
								src_y,
								image.width*sizeof(SMALL_COLOR),
								WriteWindow->RPort,
								0,
								0,
								old_src_x - src_x,
								WriteWindow->Height + WriteWindow->TopEdge,
								RECTFMT_RGBA);
						}
						else if(old_src_y < src_y)
						{
							ClipBlit(
								WriteWindow->RPort,
								0,
								src_y - old_src_y,
								WriteWindow->RPort,
								0,
								0,
								WriteWindow->Width + WriteWindow->LeftEdge,
								WriteWindow->Height + WriteWindow->TopEdge - (src_y - old_src_y),
								0xc0);
							WritePixelArray(
								image.colormap,
								src_x,
								old_src_y + WriteWindow->Height + WriteWindow->TopEdge,
								image.width*sizeof(SMALL_COLOR),
								WriteWindow->RPort,
								0,
								WriteWindow->Height + WriteWindow->TopEdge - (src_y - old_src_y),
								WriteWindow->Width + WriteWindow->LeftEdge,
								src_y - old_src_y,
								RECTFMT_RGBA);
						}
						else if(old_src_y > src_y)
						{
							ClipBlit(
								WriteWindow->RPort,
								0,
								0,
								WriteWindow->RPort,
								0,
								old_src_y - src_y,
								WriteWindow->Width + WriteWindow->LeftEdge,
								WriteWindow->Height + WriteWindow->TopEdge - (old_src_y - src_y),
								0xc0);
							WritePixelArray(
								image.colormap,
								src_x,
								src_y,
								image.width*sizeof(SMALL_COLOR),
								WriteWindow->RPort,
								0,
								0,
								WriteWindow->Width + WriteWindow->LeftEdge,
								old_src_y - src_y,
								RECTFMT_RGBA);
						}
					}
					break;
				case IDCMP_MOUSEBUTTONS:
					done = TRUE;
					break;
			}

			ReplyMsg (&IntMsg->ExecMessage);
		}
	}

	/* cleanup */

	CloseWindow(WriteWindow);
	CloseScreen(CyberScreen);
	CloseLibrary(CyberGfxBase);
	CloseLibrary(&IntuitionBase->LibNode);
	Cleanup(&image);

	return 0;
}


