/*
**
** error.h
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

#ifndef	_GV_ERROR_H_
#define	_GV_ERROR_H_

#ifdef VMS
#   include <errno.h>
#   include <string.h>
#   ifdef __DECC
#      ifndef __ERRNO_MAX /*###jp### __ERRNO_MAX is undefined in lower versions of DEC C */
#         define __ERRNO_MAX 85
#      endif
#   else
#      include <perror.h>
#   endif
#else
#if 0 /* unused ###jp###, 02.06.97 */
#   ifndef CSRG_BASED /* for __FreeBSD__ */
       extern int sys_nerr;
       extern char *sys_errlist[];
#   endif
#endif 
#   include <errno.h>
    /* BSD 4.3 errno.h does not declare errno */
    extern int errno;
#endif

extern char*			open_fail_error (
#if NeedFunctionPrototypes
   int,
   char *,
   char *,
   int
#endif
);

#endif /*_GV_ERROR_H_*/








