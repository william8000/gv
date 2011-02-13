/*
**
** string.c
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
#include "ac_config.h"


/*
#define MESSAGES
*/
#define MESSAGES
#include "message.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)

#include "types.h"

char* mprintf(char* fmt,...)
{
  va_list a;
  size_t l=1;
  char *p;

  BEGINMESSAGE(mprintf)
  va_start(a,fmt);
  for (p=fmt;*p; p++) {
    if (*p != '%') {
      l++;
      continue;
    }
    switch (*++p) {
      case 's':
        l += strlen(va_arg(a,char*));
        break;
      default:
        l++;
        break;
    }
  }
  va_end(a);
  p = XtMalloc(l*sizeof(char));
  va_start(a,fmt);
  vsprintf(p,fmt,a);
  va_end(a);
  SMESSAGE(p)
  ENDMESSAGE(mprintf)
  return(p);
}

