/*
**
** d_xtmem.h
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

#ifndef _GV_d_XtMem_H_
#define _GV_d_XtMem_H_
   
extern char *			d_XtMalloc (
#if NeedFunctionPrototypes
    Cardinal,
    int,
    char *,
    char *
#endif
);

extern char *			d_XtRealloc (
#if NeedFunctionPrototypes
    char *,
    Cardinal,
    int,
    char *,
    char *
#endif
);

extern char *			d_XtNewString (
#if NeedFunctionPrototypes
    String,
    int,
    char *,
    char *
#endif
);

void				d_XtFree (
#if NeedFunctionPrototypes
    char *,
    int,
    char *,
    char *
#endif
);

void				d_XtMemDump (
#if NeedFunctionPrototypes
#endif
);

#endif /* _GV_d_XtMem_H_ */



