/*
**
** dialog.h
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
*/


#ifndef	_GV_DIALOG_H_
#define	_GV_DIALOG_H_

#define DIALOG_BUTTON_DONE   (1<<0)
#define DIALOG_BUTTON_CANCEL (1<<1)

extern void cb_popdownDialogPopup (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void cb_popupDialogPopup (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void action_preferDialogPopupButton (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void makeDialogPopup (
#if NeedFunctionPrototypes
void
#endif
);

extern void DialogPopupSetButton (
#if NeedFunctionPrototypes
   int,
   String,
   XtCallbackProc
#endif
);

extern void DialogPopupClearText (
#if NeedFunctionPrototypes
void
#endif
);

extern void DialogPopupSetText (
#if NeedFunctionPrototypes
    String
#endif
);

extern String DialogPopupGetText (
#if NeedFunctionPrototypes
void
#endif
);

extern void DialogPopupSetPrompt (
#if NeedFunctionPrototypes
    String
#endif
);

extern void DialogPopupSetMessage (
#if NeedFunctionPrototypes
    String
#endif
);

#endif	/* _GV_DIALOG_H_ */
