/*
**
** callbacks.h
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** Copyright (C) 2004 Jose E. Marchesi
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

#ifndef	_GV_CALLBACKS_H_
#define	_GV_CALLBACKS_H_

extern void			cb_showTitle (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_newtocScrollbar (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_newtocClipAdjust (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_adjustSlider (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_antialias (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_useBackingPixmap (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_handleDSC (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_handleEOF (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_stopInterpreter (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_pageAdjustNotify (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_checkFile (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_watchFile (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_print (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_doPrint (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_cancelPrint (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_save (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_doSave (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_openFile (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_doOpenFile (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_reopen (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_redisplay (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_page (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_positionPage (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_setPageMark ( 
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_autoResize (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_setScale (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_setOrientation (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_setPagemedia (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_track (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_message (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_destroy (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_destroyGhost (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_quitGhostview (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_doQuit (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void			cb_cancelQuit (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

#endif /* _GV_CALLBACKS_H_ */
