/*
**
** error.c
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


/*
#define MESSAGES
*/
#include "message.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "paths.h"
#include INC_X11(Xos.h)
#include INC_X11(Intrinsic.h)

#include "types.h"
#include "error.h"
#include "main_resources.h"
#include "main_globals.h"
#include "d_memdebug.h"

char* 
open_fail_error(errornumber,error_str,file_name,show)
  int errornumber;
  char *error_str;
  char *file_name;
  int show;
{
  char *m;

  BEGINMESSAGE(open_fail_error_message)
#if defined (__DECC) && defined (VMS)
  if (errornumber < __ERRNO_MAX) m=strerror(errornumber);
  else m = "Unknown error";
#else
  if (!(m = strerror(errornumber))) m = "Unknown error";
#endif

  if (show) {
    fprintf(stderr,"%s: %s %s (%s)\n",gv_name,error_str,file_name,m);
    m = NULL;
  } else {
    char buf[1024];
    sprintf(buf,"%s %s: %s",error_str,file_name,m);
    m = GV_XtNewString(buf);
  }
  ENDMESSAGE(open_fail_error_message)
  return(m);
}

