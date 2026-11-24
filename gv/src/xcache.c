/*
**
** xcache.c
**
** Copyright (C) 2026 William Bader
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with GNU gv; see the file COPYING.  If not, write to
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA 02111-1307, USA.
**
*/

/*
  Each X11 request with a reply costs a round trip, which is what makes gv
  slow to start over a remote connection.  Xaw3d asks the server for the
  size of a label bitmap every time one is set, up to ten times for the
  same bitmap, and XCreateFontSet lists every name of a font set twice,
  once for vertical text, and loads the font it picks twice.  The answers
  cannot change, so gv defines XGetGeometry, XListFonts and XLoadQueryFont
  itself, which takes precedence over libX11 for the libraries too,
  answers what it knows, and passes the rest on to libX11.
*/

#include "ac_config.h"

#include <stdlib.h>
#include <string.h>

#include "paths.h"
#include INC_X11(Xlib.h)

#include "xcache.h"

#ifdef HAVE_DLSYM

#include <dlfcn.h>

typedef struct {
  Display *display;
  Pixmap pixmap;
  unsigned int width, height;
} Bitmap;

static Bitmap bitmaps[16];
static int num_bitmaps = 0;

typedef struct Missing {
  struct Missing *next;
  Display *display;
  char pattern[1];
} Missing;

static Missing *missing = NULL;

typedef struct Loaded {
  struct Loaded *next;
  Display *display;
  XFontStruct *font;
  char name[1];
} Loaded;

static Loaded *loaded = NULL;

static int
xcache_isMissing (Display *display, _Xconst char *name)
{
  Missing *m;

  for (m = missing; m; m = m->next)
    if (m->display == display && !strcmp(m->pattern, name))
      return 1;
  return 0;
}

static void
xcache_addMissing (Display *display, _Xconst char *name)
{
  Missing *m = malloc(sizeof(Missing) + strlen(name));

  if (m)
    {
      m->display = display;
      strcpy(m->pattern, name);
      m->next = missing;
      missing = m;
    }
}

/* XFreeFont frees the arrays and the structure with free(). */
static XFontStruct *
xcache_copyFont (XFontStruct *font, Font fid)
{
  XFontStruct *copy = malloc(sizeof(XFontStruct));
  size_t nchars, size;

  if (!copy)
    return NULL;
  *copy = *font;
  copy->ext_data = NULL;
  copy->fid = fid;
  copy->per_char = NULL;
  copy->properties = NULL;
  if (font->per_char)
    {
      nchars = (size_t) (font->max_byte1 - font->min_byte1 + 1) *
               (font->max_char_or_byte2 - font->min_char_or_byte2 + 1);
      size = nchars * sizeof(XCharStruct);
      if (!(copy->per_char = malloc(size)))
        goto fail;
      memcpy(copy->per_char, font->per_char, size);
    }
  if (font->properties && font->n_properties > 0)
    {
      size = font->n_properties * sizeof(XFontProp);
      if (!(copy->properties = malloc(size)))
        goto fail;
      memcpy(copy->properties, font->properties, size);
    }
  return copy;
fail:
  free(copy->per_char);
  free(copy);
  return NULL;
}

void
xcache_addBitmap (Display *display, Pixmap pixmap, unsigned int width, unsigned int height)
{
  if (pixmap == None || num_bitmaps >= (int) (sizeof(bitmaps) / sizeof(bitmaps[0])))
    return;
  bitmaps[num_bitmaps].display = display;
  bitmaps[num_bitmaps].pixmap = pixmap;
  bitmaps[num_bitmaps].width = width;
  bitmaps[num_bitmaps].height = height;
  num_bitmaps++;
}

Status
XGetGeometry (Display *display, Drawable d, Window *root, int *x, int *y,
              unsigned int *width, unsigned int *height,
              unsigned int *border_width, unsigned int *depth)
{
  static Status (*real) (Display *, Drawable, Window *, int *, int *,
                         unsigned int *, unsigned int *,
                         unsigned int *, unsigned int *) = NULL;
  int i;

  for (i = 0; i < num_bitmaps; i++)
    if (bitmaps[i].pixmap == d && bitmaps[i].display == display)
      {
        *root = DefaultRootWindow(display);
        *x = *y = 0;
        *width = bitmaps[i].width;
        *height = bitmaps[i].height;
        *border_width = 0;
        *depth = 1;
        return True;
      }
  if (!real)
    *(void **) &real = dlsym(RTLD_NEXT, "XGetGeometry");
  return real(display, d, root, x, y, width, height, border_width, depth);
}

/* Only the names that match no font are remembered: a list of names that
   were found has to be freed by XFreeFontNames, which expects the memory
   layout of libX11. */
char **
XListFonts (Display *display, _Xconst char *pattern, int maxnames, int *count)
{
  static char **(*real) (Display *, _Xconst char *, int, int *) = NULL;
  char **list;

  if (xcache_isMissing(display, pattern))
    {
      *count = 0;
      return NULL;
    }
  if (!real)
    *(void **) &real = dlsym(RTLD_NEXT, "XListFonts");
  list = real(display, pattern, maxnames, count);
  if (!list)
    xcache_addMissing(display, pattern);
  return list;
}

/* A font loaded again gets its own font ID from XLoadFont, which does not
   wait for a reply, and a copy of the metrics; the metrics are what
   XLoadQueryFont waits for.  Names that match no font share the list of
   XListFonts, as the server matches them the same way. */
XFontStruct *
XLoadQueryFont (Display *display, _Xconst char *name)
{
  static XFontStruct *(*real) (Display *, _Xconst char *) = NULL;
  XFontStruct *font, *copy;
  Loaded *l;

  if (xcache_isMissing(display, name))
    return NULL;
  for (l = loaded; l; l = l->next)
    if (l->display == display && !strcmp(l->name, name))
      {
        Font fid = XLoadFont(display, name);
        if ((copy = xcache_copyFont(l->font, fid)))
          return copy;
        XUnloadFont(display, fid);
        break;
      }
  if (!real)
    *(void **) &real = dlsym(RTLD_NEXT, "XLoadQueryFont");
  font = real(display, name);
  if (!font)
    xcache_addMissing(display, name);
  else if (!l && (l = malloc(sizeof(Loaded) + strlen(name))))
    {
      if ((l->font = xcache_copyFont(font, None)))
        {
          l->display = display;
          strcpy(l->name, name);
          l->next = loaded;
          loaded = l;
        }
      else
        free(l);
    }
  return font;
}

#else /* HAVE_DLSYM */

void
xcache_addBitmap (Display *display, Pixmap pixmap, unsigned int width, unsigned int height)
{
  (void) display; (void) pixmap; (void) width; (void) height;
}

#endif /* HAVE_DLSYM */
