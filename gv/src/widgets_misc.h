/*
**
** widgets_misc.h
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

#ifndef _GV_WIDGETS_MISC_H_
#define _GV_WIDGETS_MISC_H_

extern void			widgets_setSelectedBitmap (
#if NeedFunctionPrototypes
    Widget,
    int
#endif
);

extern void			widgets_setToggle (
#if NeedFunctionPrototypes
    Widget,
    int
#endif
);

extern Widget			widgets_createLabeledLineTextField (
#if NeedFunctionPrototypes
    String,
    Widget
#endif
);

extern Widget			widgets_createLabeledTextField (
#if NeedFunctionPrototypes
    String,
    Widget
#endif
);

extern char *			widgets_getText (
#if NeedFunctionPrototypes
   Widget
#endif
);

extern void			widgets_setText (
#if NeedFunctionPrototypes
   Widget,
   String
#endif
);

extern void			widgets_preferButton (
#if NeedFunctionPrototypes
   Widget,	/* w      */
   int		/* prefer */
#endif
);

#endif /* _GV_WIDGETS_MISC_H_ */
