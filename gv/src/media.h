/*
**
** media.h
**
** Copyright (C) 1997 Johannes Plass
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

#ifndef _GV_MEDIA_H_
#define _GV_MEDIA_H_

extern void			media_freeMedias (
#if NeedFunctionPrototypes
   Media*
#endif
);

extern Media*			media_parseMedias (
#if NeedFunctionPrototypes
   char*
#endif
);

extern int			media_numMedias (
#if NeedFunctionPrototypes
   Media *
#endif
);

#endif /* _GV_MEDIA_H_ */ 

