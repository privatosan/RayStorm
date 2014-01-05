#ifndef CODEKEY_STRINGS_H
#define CODEKEY_STRINGS_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_0003 0
#define MSG_0004 1
#define MSG_0006 2
#define MSG_0010 3
#define MSG_0011 4
#define MSG_0012 5
#define MSG_0016 6
#define MSG_0019 7
#define MSG_0021 8
#define MSG_0022 9
#define MSG_0024 10
#define MSG_0025 11
#define MSG_0026 12
#define MSG_0027 13
#define MSG_0028 14
#define MSG_0029 15

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_0003_STR "OK"
#define MSG_0004_STR "Error opening '%s'."
#define MSG_0006_STR "Error writing to '%s'."
#define MSG_0010_STR "Keyfile %s for %s created successfully."
#define MSG_0011_STR "Failed to open %s.\n"
#define MSG_0012_STR "RayStorm-Register"
#define MSG_0016_STR "Makes key for RayStorm"
#define MSG_0019_STR "Name:"
#define MSG_0021_STR "Computer:"
#define MSG_0022_STR "Serialnum:"
#define MSG_0024_STR "File:"
#define MSG_0025_STR "Please select a file..."
#define MSG_0026_STR "Make"
#define MSG_0027_STR "Exit"
#define MSG_0028_STR "Failed to create Application."
#define MSG_0029_STR "Keyfile type"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {MSG_0003,(STRPTR)MSG_0003_STR},
    {MSG_0004,(STRPTR)MSG_0004_STR},
    {MSG_0006,(STRPTR)MSG_0006_STR},
    {MSG_0010,(STRPTR)MSG_0010_STR},
    {MSG_0011,(STRPTR)MSG_0011_STR},
    {MSG_0012,(STRPTR)MSG_0012_STR},
    {MSG_0016,(STRPTR)MSG_0016_STR},
    {MSG_0019,(STRPTR)MSG_0019_STR},
    {MSG_0021,(STRPTR)MSG_0021_STR},
    {MSG_0022,(STRPTR)MSG_0022_STR},
    {MSG_0024,(STRPTR)MSG_0024_STR},
    {MSG_0025,(STRPTR)MSG_0025_STR},
    {MSG_0026,(STRPTR)MSG_0026_STR},
    {MSG_0027,(STRPTR)MSG_0027_STR},
    {MSG_0028,(STRPTR)MSG_0028_STR},
    {MSG_0029,(STRPTR)MSG_0029_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x04"
    MSG_0003_STR "\x00\x00"
    "\x00\x00\x00\x01\x00\x14"
    MSG_0004_STR "\x00"
    "\x00\x00\x00\x02\x00\x18"
    MSG_0006_STR "\x00\x00"
    "\x00\x00\x00\x03\x00\x28"
    MSG_0010_STR "\x00"
    "\x00\x00\x00\x04\x00\x14"
    MSG_0011_STR "\x00"
    "\x00\x00\x00\x05\x00\x12"
    MSG_0012_STR "\x00"
    "\x00\x00\x00\x06\x00\x18"
    MSG_0016_STR "\x00\x00"
    "\x00\x00\x00\x07\x00\x06"
    MSG_0019_STR "\x00"
    "\x00\x00\x00\x08\x00\x0A"
    MSG_0021_STR "\x00"
    "\x00\x00\x00\x09\x00\x0C"
    MSG_0022_STR "\x00\x00"
    "\x00\x00\x00\x0A\x00\x06"
    MSG_0024_STR "\x00"
    "\x00\x00\x00\x0B\x00\x18"
    MSG_0025_STR "\x00"
    "\x00\x00\x00\x0C\x00\x06"
    MSG_0026_STR "\x00\x00"
    "\x00\x00\x00\x0D\x00\x06"
    MSG_0027_STR "\x00\x00"
    "\x00\x00\x00\x0E\x00\x1E"
    MSG_0028_STR "\x00"
    "\x00\x00\x00\x0F\x00\x0E"
    MSG_0029_STR "\x00\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};


#ifdef CATCOMP_CODE

STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#define XLocaleBase LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#define LocaleBase XLocaleBase
#undef XLocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* CODEKEY_STRINGS_H */
