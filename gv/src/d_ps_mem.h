/*
**
** d_ps_mem.h.h
**
** Copyright (C) 1996-1997 Johannes Plass
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

#ifndef _GV_D_PS_MEM_H_
#define _GV_D_PS_MEM_H_

#ifdef GV_PS_MEM_DEBUG

#   include "d_mem.h"
#   define PS_malloc(sss)       d_malloc  ((size_t)(sss)		,__LINE__,__FILE__,"PS")
#   define PS_calloc(ccc,sss)   d_calloc  ((size_t)(ccc),(size_t)(sss)	,__LINE__,__FILE__,"PS")
#   define PS_realloc(ppp,sss)  d_realloc ((void*) (ppp),(size_t)(sss)	,__LINE__,__FILE__,"PS")
#   define PS_free(ppp)         d_free    ((void*) (ppp)		,__LINE__,__FILE__,"PS")
#   define PS_cfree(ppp)	d_cfree   ((void*) (ppp)		,__LINE__,__FILE__,"PS")
#   define DUMP_MEM

#else  /* GV_PS_MEM_DEBUG */

#   define PS_malloc(sss)       malloc    ((size_t)(sss)               )
#   define PS_calloc(ccc,sss)   calloc    ((size_t)(ccc),(size_t)(sss) )
#   define PS_realloc(ppp,sss)  realloc   ((void*) (ppp),(size_t)(sss) )
#   define PS_free(ppp)         free      ((void*) (ppp)               )
#   define PS_cfree(ppp)        cfree     ((void*) (ppp)               )

#endif /* GV_PS_MEM_DEBUG */

#endif /* _GV_PS_MEM_H_ */

