/*
**
** vms_dir.h
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

#ifndef	_VMS_DIR_H_
#define	_VMS_DIR_H_

#ifdef VMS

#include <types.h>

#define MAXNAMLEN 255

struct  dirent {
   char d_name[MAXNAMLEN+1];            /* name (up to MAXNAMLEN + 1)            */
};

typedef struct {
  unsigned long dd_fd;                  /* file descriptor for lib$find_file     */
  unsigned long dd_loc;                 /* lib$find_file stores the context here */
  struct dirent *dd_buf;                /* -> directory buffer                   */
} DIR;

extern DIR *			opendir (
#if NeedFunctionPrototypes
   char *dirname
#endif
);

extern struct dirent *		readdir (
#if NeedFunctionPrototypes
   DIR *
#endif
);

extern int			closedir (
#if NeedFunctionPrototypes
   DIR *
#endif
);

extern char *			getwd (
#if NeedFunctionPrototypes
#endif
);

#endif /* VMS */

#endif	/* _VMS_DIR_H_ */

