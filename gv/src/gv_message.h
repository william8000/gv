/*
**
** gv_messages.h
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
** along with GNU gv; see the file COPYING.  If not, write to
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA 02111-1307, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

char *message_usage = "\
gv usage (use \"gv -help\" for more details):\n\
   gv	[<name of file to display>[.ps]]\n\
	[-monochrome] [-grayscale] [-color]\n\
	[-[no]safer] [-[no]quiet] [-arguments <arguments>]\n\
	[-page <label>] [-[no]center]\n\
	[-media <media>]\n\
	[-portrait] [-landscape] [-upsidedown] [-seascape]\n\
	[-scale <n>] [-scalebase <n>]\n\
	[-swap] [-noswap]\n\
	[-antialias] [-noantialias]\n\
	[-dsc] [-nodsc]\n\
	[-eof] [-noeof]\n\
	[-pixmap] [-nopixmap]\n\
	[-watch] [-nowatch]\n\
	[-?] [-h] [-help] [-v]\n\
	[-resize] [-noresize]\n\
	[-geometry [<width>][x<height>][{+-}<xoffset>{+-}<yoffset>]\n\
	[-ad <resource file>]\n\
	[-style <resource file>]\n\
	[-spartan]\n\
	[<other toolkit options>]\
";

char *message_help = "\
gv help:\n\
   \n\
   # Document to be displayed\n\
     [<name of file to display>[.ps][.pdf]] ! The extension may be omitted\n\
   # Color environment:\n\
     [-monochrome] [-grayscale] [-color]\n\
   # GhostScript Options:\n\
     [-[no]safer]             ! [Do not] pass -dSAFER to GhostScript\n\
     [-[no]quiet]             ! [Do not] pass -dQUIET to GhostScript\n\
     [-arguments <arguments>] ! Start GhostScript with <arguments>\n\
   # Page specific \n\
     [-page <label>]          ! Display the page with label <label>\n\
     [-[no]center]            ! Center the displayed page\n\
   # Page Media\n\
     [-media <media>]\n\
   # Page Orientation\n\
     [-portrait] [-landscape] [-upsidedown] [-seascape]\n\
     [-[no]swap]              ! Swap landscape/seascape\n\
   # Scales\n\
     [-scalebase <n>]\n\
     [-scale <n>]\n\
   # Antialiasing\n\
     [-[no]antialias] \n\
   # Updating the file automatically\n\
     [-[no]watch]             ! Whether to check the file periodically\n\
   # How to handle 'End of File' Comments\n\
     [-[no]eof]               ! Whether to respect 'End of File' comments\n\
   # How to handle Document Structuring Comments (DSC)\n\
     [-[no]dsc]               ! Whether to respect structuring comments\n\
   # How to handle obscured window regions\n\
     [-[no]pixmap]            ! Whether to maintain a backing pixmap\n\
   # Resize Behaviour of the Main Window \n\
     [-[no]resize]            ! Adjust size of main window automatically\n\
   # Geometry of Main Window\n\
     [-geometry [<width>][x<height>][{+-}<xoffset>{+-}<yoffset>]\n\
        where {+-} means either + or -\n\
   # Load additional Resources at startup\n\
     [-ad <resource file>]\n\
   # Load additional resource style file at startup\n\
     [-style <resource file>]\n\
   # Shortcut for '-style gv_spartan.dat'\n\
     [-spartan]\n\
   # Toolkit Options\n\
     [<other toolkit options>]\n\
   # Show help\n\
     [-v]                     ! Version information\n\
     [-?] [-h]\n\
     [-help]                  ! More explicit than -h and -?\n\
   \n\
   More information about gv may be found on its Web page\n\
     http://wwwthep.physik.uni-mainz.de/~plass/gv/\
";

