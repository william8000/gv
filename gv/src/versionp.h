/*
**
** versionp.h
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
**           Department of Physicss
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

#ifndef _GV_VERSIONP_H_
#define _GV_VERSIONP_H_

#include "version.h"

#define  VERSION_SHELL_TITLE	"gv Copyright Information"
#define  VERSION_TOPLEVEL	toplevel       			/* the Application Shell   */
#define  VERSION_APPLIC_CONTEXT	app_con  			/* the Application Context */

char *versionIdentification[] = {
        "gv 3.5.8",
        "June 1997",
	0
};

/*
   declare it writable due to problems with sscanf() in main_versionIsCompatible() 
   on HP/UX 9.07
   [suggested by Christian Illinger (illinger@lepsi.in2p3.fr)]
*/
char versionCompatibility[] = "gv 3.5.2";
                                            
char * copyright = "\
 gv -- An X11 user interface for GhostScript. \n\
 \n\
 Copyright (C) 1992-1997  Johannes Plass, Timothy O. Theisen  \n\
 \n\
 This program is free software; you can redistribute it and/or modify \n\
 it under the terms of the GNU General Public License as published by \n\
 the Free Software Foundation; either version 2 of the License, or \n\
 (at your option) any later version. \n\
 \n\
 This program is distributed in the hope that it will be useful, \n\
 but WITHOUT ANY WARRANTY; without even the implied warranty of \n\
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n\
 GNU General Public License for more details. \n\
 \n\
 You should have received a copy of the GNU General Public License \n\
 along with this program; if not, write to the Free Software \n\
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.";

char *author[] = {
" Authors: ",
"\
           Johannes Plass       plass@thep.physik.uni-mainz.de \n\
                                Department of Physics \n\
                                Johannes Gutenberg University \n\
                                Mainz, Germany\
",
"\
 gv is derived from GhostView 1.5, created by\n\
\n\
           Tim Theisen          Systems Programmer \n\
 Internet: tim@cs.wisc.edu      Department of Computer Sciences  \n\
     UUCP: uwvax!tim            University of Wisconsin-Madison  \n\
    Phone: (608)262-0438        1210 West Dayton Street \n\
      FAX: (608)262-9777        Madison, WI   53706\
",
"\
 Please send error reports to:  plass@thep.physik.uni-mainz.de \n\
",
0 
};

#endif /* _GV_VERSIONP_H_ */
