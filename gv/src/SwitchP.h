/*
**
** SwitchP.c
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

#ifndef _SwitchP_h_
#define _SwitchP_h_

#include "paths.h"
#include "Switch.h"
#include "ButtonP.h"

typedef struct _SwitchClass 
{
  int makes_compiler_happy;  /* not used */
} SwitchClassPart;

typedef struct _MenuButtonClassRec {
  CoreClassPart	     core_class;
  SimpleClassPart    simple_class;
  ThreeDClassPart    threeD_class;
  LabelClassPart     label_class;
  CommandClassPart   command_class;
  ButtonClassPart    button_class;
  SwitchClassPart    switch_class;
} SwitchClassRec;

extern SwitchClassRec switchClassRec;

typedef struct {
  int makes_compiler_happy;  /* not used */
} SwitchPart;

typedef struct _SwitchRec {
    CorePart         core;
    SimplePart	     simple;
    ThreeDPart       threeD;
    LabelPart	     label;
    CommandPart	     command;
    ButtonPart	     button;
  /* switch won't work :-) */
    SwitchPart       swidch;
} SwitchRec;

#endif /* _SwitchP_h_ */


