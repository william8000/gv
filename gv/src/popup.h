/*
**
** popup.h
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

#ifndef _GV_POPUP_H_

extern void cb_popupPopup(
#if NeedFunctionPrototypes
    Widget,
    XtPointer,
    XtPointer
#endif
);

extern void cb_popdownPopup(
#if NeedFunctionPrototypes
    Widget,
    XtPointer,
    XtPointer
#endif
);

extern void			popup_positionPopup (
#if NeedFunctionPrototypes
    Widget,
    Widget,
    int,
    int,
    int
#endif
);

#define POPUP_POSITION_POS		1
#define POPUP_POSITION_CENTER		2
#define POPUP_POSITION_POS_CENTER	3
#define POPUP_POSITION_POS_NW		4

#define _GV_POPUP_H_
#endif  /* _GV_POPUP_H_ */
