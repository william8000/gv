/*
**
** options.h
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** Copyright (C) 2004 Jose E. Marchesi
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
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
**           Jose E. Marchesi (jemarch@gnu.org)
**           GNU Project
**
*/

#ifndef	_GV_OPTIONS_H_
#define	_GV_OPTIONS_H_

extern void options_cb_popup (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void options_cb_popdown (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void options_cb_changeMenuLabel (
#if NeedFunctionPrototypes
  Widget,
  XtPointer,
  XtPointer
#endif
);

extern void options_textApply (
#if NeedFunctionPrototypes
  Widget,
  Boolean*,
  String*
#endif
);

extern void options_createLabeledMenu (
#if NeedFunctionPrototypes
  String,
  Widget,
  Widget*,
  Widget*,
  Widget*
#endif
);

extern void options_realize (
#if NeedFunctionPrototypes
  Widget,
  Widget
#endif
);

extern void options_setArg (
#if NeedFunctionPrototypes
  String *,
  String *,
  String,
  String,
  String
#endif
);

extern String options_squeezeMultiline (
#if NeedFunctionPrototypes
  String
#endif
);

extern String options_squeeze (
#if NeedFunctionPrototypes
  String
#endif
);

extern void options_save (
#if NeedFunctionPrototypes
  int,
  String *,
  String *
#endif
);

#endif	/* _GV_OPTIONS_H_ */
