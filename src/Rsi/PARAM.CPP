/***************
 * MODUL:         param
 * NAME:          param.cpp
 * DESCRIPTION:   Funtions for parsing variable parameter lists
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.06.95 ah    initial release
 *    19.02.98 ah    added rsiTMore for for argument arrays
 ***************/

#ifndef PARAM_H
#include "param.h"
#endif

#define rsiTDone  0
#define rsiTMore  1

/*************
 * DESCRIPTION:   gets a pointer from the argument list
 * INPUT:         list     argument list
 * OUTPUT:        pointer
 *************/
static void *GetPointer(va_list *list)
{
	void *ptr;
#ifdef __WATCOMC__
	ptr = *((void**)**list);
#else
	ptr = *((void**)*list);
#endif
	va_arg(*list,void*);
	return ptr;
}

/*************
 * DESCRIPTION:   gets a int from the argument list
 * INPUT:         list     argument list
 * OUTPUT:        int
 *************/
static int GetInt(va_list *list)
{
	int num;

#ifdef __WATCOMC__
	num = *((int*)**list);
#else
	num = *((int*)*list);
#endif
	va_arg(*list,int);
	return num;
}

/*************
 * DESCRIPTION:   gets a float from the argument list
 * INPUT:         list     argument list
 * OUTPUT:        float
 *************/
static float GetFloat(va_list *list)
{
	float fnum;

	fnum = (float)*((double*)*list);
	va_arg(*list,double);
	return fnum;
}

/*************
 * DESCRIPTION:   Gets the paramters from the argument list.
 *    Parses the arguments with a argument description.
 * INPUT:         list     argument list
 * OUTPUT:        TRUE if failed else FALSE
 *************/
BOOL GetParams(va_list *list, PARAM *para)
{
	BOOL done = FALSE, identified = FALSE;
	PARAM *curpara;
	int type;
	int ret = FALSE;

	while(!done)
	{
		// Watcom seems to have a bug
#ifdef __WATCOMC__
		type = *((ULONG*)(**list));
#else
		type = *((ULONG*)(*list));
#endif
		if(type == rsiTDone)
		{
			done = TRUE;
			continue;
		}
		va_arg(*list,int);
		if(type == rsiTMore)
		{
			*list = *((va_list*)(*list));
			continue;
		}

		curpara = para;
		while(curpara->tag_id != rsiTDone)
		{
			if(curpara->tag_id == type)
			{
				identified = TRUE;
				switch(curpara->type)
				{
					case rsiTypeInt:
						*((int*)curpara->data) = GetInt(list);
						break;
					case rsiTypeFloat:
						*((float*)curpara->data) = GetFloat(list);
						break;
					case rsiTypePointer:
						*((void**)curpara->data) = GetPointer(list);
						break;
				}
			}
			curpara++;
		}
		if(!identified)
			return TRUE;
	}
	va_end(*list);
	return ret;
}

