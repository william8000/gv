/*
 * d_gv_mem.h
 *
 * Copyright (C) 1995, 1996, 1997  Johannes Plass
 * Copyright (C) Jose E. Marchesi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU gv; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *   Author: Johannes Plass
 *           Department of Physics
 *           Johannes Gutenberg University, Mainz, Germany
 *
 *           Jose E. Marchesi (jemarch@gnu.org)
 *           GNU Project
 *
 */ 

#ifndef _GV_D_GV_MEM_H_
#define _GV_D_GV_MEM_H_

#ifdef GV_GV_MEM_DEBUG

#   include "d_mem.h"
#   define GV_malloc(sss)       d_malloc  ((size_t)(sss)		,__LINE__,__FILE__,"GV")
#   define GV_realloc(ppp,sss)  d_realloc ((void*) (ppp),(size_t)(sss)	,__LINE__,__FILE__,"GV")
#   define GV_free(ppp)         d_free    ((void*) (ppp)		,__LINE__,__FILE__,"GV")
#   define DUMP_MEM

#else  /* GV_GV_MEM_DEBUG */

#   define GV_malloc(sss)       malloc    ((size_t)(sss)               )
#   define GV_realloc(ppp,sss)  realloc   ((void*) (ppp),(size_t)(sss) )
#   define GV_free(ppp)         free      ((void*) (ppp)               )

#endif /* GV_GV_MEM_DEBUG */

#endif /* _GV_GV_MEM_H_ */

