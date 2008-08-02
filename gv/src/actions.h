/*
**
** actions.h
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
** Authors:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**            Department of Physics
**            Johannes Gutenberg-University
**            Mainz, Germany
**
**            Jose E. Marchesi (jemarch@gnu.org)
**            GNU Project
**
*/

#ifndef	_GV_ACTIONS_H_
#define	_GV_ACTIONS_H_

extern void 			action_shellConfigureNotify (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_page (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_menuPopdown (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_toc (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_otherPage (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_movePage (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_panner (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_antialias (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_quit (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_handleDSC (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_handleEOF (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_open (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_reopen (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_savepos (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_save (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_print (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_print_pos (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_center (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_setPageMark (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_autoResize (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_setScale (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_setOrientation (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_setPagemedia (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_dismissPopup (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_deleteWindow (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_eraseLocator (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_checkFile (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void 			action_watchFile (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

#endif	/* _GV_ACTIONS_H_ */
