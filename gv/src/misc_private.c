/*
**
** misc_private.c
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

/*
#define MESSAGES
*/
#include "message.h"

#include <stdlib.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_XAW(Cardinals.h)
#include INC_X11(IntrinsicP.h)
#include INC_XAW(LabelP.h)
#include "GhostviewP.h"

#include "types.h"
#include "misc_private.h"

/*############################################################*/
/* update_label */
/*############################################################*/

void
update_label(widget,text)
   Widget widget;
   char   *text;
{
   LabelWidget w = (LabelWidget) widget;
   int shadow;

   BEGINMESSAGE1(update_label)

   if (!XtIsRealized(widget)) {
      INFMESSAGE(not realized) ENDMESSAGE(update_label)
      return;
   }

   shadow = w->threeD.shadow_width;
   if ((w->core.width > shadow+shadow) && (w->core.height > shadow+shadow)) {
      XClearArea(XtDisplay(widget),XtWindow(widget),
                 shadow,shadow,
                 (Dimension)(w->core.width-shadow-shadow),
                 (Dimension)(w->core.height-shadow-shadow), (text) ? False : True);
   }
   if (text) {      /* most of the following comes from X11/Xaw/Label.c */
      Position x,y;
      INFSMESSAGE(update_label,text)
      y = w->label.label_y + w->label.font->max_bounds.ascent;
      if (w->label.justify == XtJustifyCenter) {
         unsigned int width;
         int len = (int) strlen(text);
         XFontStruct *fs = w->label.font;
         if   (w->label.encoding) width = XTextWidth16 (fs, (XChar2b*)text, (int)(len/2) );
         else                     width = XTextWidth   (fs, text          , (int)(len)   );
         x = (Position) ((w->core.width-width)/2);
      } else {
         x = w->label.internal_width + w->threeD.shadow_width;
      }

      if (w->label.encoding) {
	 XDrawString16(XtDisplay(widget), XtWindow(widget),
                       w->label.normal_GC,
	  	       x, y,(XChar2b*)text, (int)(strlen(text)));
      } else {
         XDrawString(XtDisplay(widget), XtWindow(widget),
                       w->label.normal_GC,
	  	       x, y, text, (int)(strlen(text)));
      }
   }

   ENDMESSAGE1(update_label)
}

