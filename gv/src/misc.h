/*
**
** misc.h
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
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
**           Jose E. Marchesi (jemarch@gnu.org)
**
*/

#ifndef	_GV_MISC_H_
#define	_GV_MISC_H_

extern void			misc_drawEyeGuide (
#if NeedFunctionPrototypes
  Widget,
  int,
  int,
  int
#endif
);

extern void			misc_savePagePosition (
#if NeedFunctionPrototypes
#endif
);

extern int			misc_restorePagePosition (
#if NeedFunctionPrototypes
    int *,  /* xP */
    int *   /* yP */
#endif
);

extern void			misc_resetPagePosition (
#if NeedFunctionPrototypes
#endif
);

extern void			misc_setPageMarker (
#if NeedFunctionPrototypes
    int, /* entry */
    int, /* kind */
    XEvent*,
    Boolean
#endif
);

extern String			misc_testFile (
#if NeedFunctionPrototypes
    String	/* name */
#endif
);

extern String			misc_changeFile (
#if NeedFunctionPrototypes
    String	/* name */
#endif
);

extern String			close_file (
#if NeedFunctionPrototypes
    FILE *,	/* file */
    String	/* name */
#endif
);

extern int			check_file (
#if NeedFunctionPrototypes
    int
#endif
);

extern void			show_page (
#if NeedFunctionPrototypes
    int,
    XtPointer
#endif
);

extern Boolean			setup_ghostview (
#if NeedFunctionPrototypes
#endif
); 

extern void			setup_layout_ghostview (
#if NeedFunctionPrototypes
#endif
); 

extern void			misc_buildPagemediaMenu (
#if NeedFunctionPrototypes
#endif
); 

extern int			catch_Xerror (
#if NeedFunctionPrototypes
    Display*,
    XErrorEvent*
#endif
);
 
extern char *                   quote_filename (
#if NeedFunctionPrototypes
    char*
#endif
);

#endif	/* _GV_MISC_H_ */











