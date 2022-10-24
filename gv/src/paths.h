/*
**
** paths.h
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

#ifndef _PATHS_H_
#define _PATHS_H_

#include "config.h"
#if HAVE_GCC_VERSION(2,95)
#define INC_X11(aaa) <X11/aaa>
#define INC_XMU(aaa) <X11/Xmu/aaa>
#if HAVE_XAW3DXFT
#define INC_XAW(aaa) <X11/Xaw3dxft/aaa>
#else
#define INC_XAW(aaa) <X11/Xaw3d/aaa>
#endif
#define INC_EXT(aaa) <X11/extensions/aaa>
#else
#define INC_X11(aaa) <X11/##aaa##>
#define INC_XMU(aaa) <X11/Xmu/##aaa##>
#if HAVE_XAW3DXFT
#define INC_XAW(aaa) <X11/Xaw3dxft/##aaa##>
#else
#define INC_XAW(aaa) <X11/Xaw3d/##aaa##>
#endif
#define INC_EXT(aaa) <X11/extensions/##aaa##>
#endif

#endif /* _PATHS_H_ */

