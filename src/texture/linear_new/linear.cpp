/***************
 * MODUL:         linear
 * NAME:          linear.cpp
 * DESCRIPTION:   Linear Texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    05.09.98 ah    initial release
 ***************/

#ifdef __AMIGA__
#include "texturelib.h"
#endif // __AMIGA__

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef DIALOG_H
#include "dialog.h"
#endif

#ifndef TEXTURE_H
#include "rtexture.h"
#endif

DIALOG_ELEMENT dialog[] =
{
   DIALOG_COLGROUP, 3,
      DIALOG_LABEL, DE("Diffuse"),
      DIALOG_COLORBOX,
      DIALOG_CHECKBOX,
      DIALOG_LABEL, DE("Reflective"),
      DIALOG_COLORBOX,
      DIALOG_CHECKBOX,
      DIALOG_LABEL, DE("Transparent"),
      DIALOG_COLORBOX,
      DIALOG_CHECKBOX,
   DIALOG_END,
   DIALOG_FINISH
};

typedef struct
{
   COLOR diffuse;
   ULONG diffuse_enabled;
   COLOR reflect;
   ULONG reflect_enabled;
   COLOR transpar;
   ULONG transpar_enabled;
} DIALOG_DATA;

typedef struct
{
   TEXTURE_INFO tinfo;
} LINEAR_INFO;

DIALOG_DATA default_data =
{
   { 0.f, 0.f, 0.f },
   1,
   { 0.f, 0.f, 0.f },
   0,
   { 0.f, 0.f, 0.f },
   0
};

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
static void SAVEDS texture_cleanup(TEXTURE_INFO *tinfo)
{
}

/*************
 * DESCRIPTION:   this is the work function
 * INPUT:         info     info structure
 *                params   pointer to user texture parameters
 *                patch    pointer to patch structure
 *                v        hit position - relative to texture axis
 * OUTPUT:        -
 *************/
static void SAVEDS texture_work(LINEAR_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
   float y, one_minus_y;

   one_minus_y = 1.f - v->y;

   if(one_minus_y >= 1.f)
      return;
   if(one_minus_y <= 0.f)
   {
      if(params->diffuse_enabled)
         patch->diffuse = params->diffuse;
      if(params->reflect_enabled)
         patch->reflect = params->reflect;
      if(params->transpar_enabled)
         patch->transpar = params->transpar;
   }
   else
   {
      y = v->y;
      if(params->diffuse_enabled)
      {
         patch->diffuse.r = patch->diffuse.r * one_minus_y + params->diffuse.r * y;
         patch->diffuse.g = patch->diffuse.g * one_minus_y + params->diffuse.g * y;
         patch->diffuse.b = patch->diffuse.b * one_minus_y + params->diffuse.b * y;
      }
      if(params->reflect_enabled)
      {
         patch->reflect.r = patch->reflect.r * one_minus_y + params->reflect.r * y;
         patch->reflect.g = patch->reflect.g * one_minus_y + params->reflect.g * y;
         patch->reflect.b = patch->reflect.b * one_minus_y + params->reflect.b * y;
      }
      if(params->transpar_enabled)
      {
         patch->transpar.r = patch->transpar.r * one_minus_y + params->transpar.r * y;
         patch->transpar.g = patch->transpar.g * one_minus_y + params->transpar.g * y;
         patch->transpar.b = patch->transpar.b * one_minus_y + params->transpar.b * y;
      }
   }
}

/*************
 * DESCRIPTION:   This is the init function for the textures.
 * INPUT:         -
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
#ifdef __PPC__
extern "C" TEXTURE_INFO* SAVEDS texture_init(struct Library *TextureBase)
#else
extern "C" TEXTURE_INFO* texture_init()
#endif // __AMIGA__
{
   LINEAR_INFO *info;

   info = new LINEAR_INFO;
   if(info)
   {
      info->tinfo.name = "Linear";
      info->tinfo.dialog = dialog;
      info->tinfo.defaultdata = &default_data;
      info->tinfo.datasize = sizeof(DIALOG_DATA);
      info->tinfo.cleanup = texture_cleanup;
      info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
   }

   return (TEXTURE_INFO*)info;
}

