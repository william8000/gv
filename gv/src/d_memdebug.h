/*
**
** d_memdebug.h
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
**           GNU Project
**
*/

#ifndef _GV_D_MEMDEBUG_H_
#define _GV_D_MEMDEBUG_H_

/*=====================================================*/
/*=== Define an item below to trace memory usage for   */
/*=== the corresponding group of malloc/free calls.    */
/*=====================================================*/

/*
#define GV_FS_XTMEM_DEBUG
#define GV_PS_MEM_DEBUG
#define GV_PS_XTMEM_DEBUG
#define GV_AAA_MEM_DEBUG
#define GV_GV_MEM_DEBUG
#define GV_GV_XTMEM_DEBUG
#define GV_PROC_XTMEM_DEBUG
*/

/*=====================================================*/

#include "d_gv_mem.h"
#include "d_ps_mem.h"
#if defined(XtNumber) /* Intrinsic.h loaded */
#   include "d_aaa_xtmem.h"
#   include "d_fs_xtmem.h"
#   include "d_gv_xtmem.h"
#   include "d_proc_xtmem.h"
#   include "d_ps_xtmem.h"
#endif

#ifdef DUMP_XTMEM
#   define GV_XtMemoryDUMP           d_XtMemDump();
#else
#   define GV_XtMemoryDUMP
#endif

#ifdef DUMP_MEM
#   define GV_MemoryDUMP             d_MemDump();
#else
#   define GV_MemoryDUMP
#endif

#endif /* _GV_D_MEMDEBUG_H_ */





