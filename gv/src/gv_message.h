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
Usage: gv [OPTION]... [FILE]\n\
PostScript and PDF viewer.\n\
  --monochrome               display document using only black and white\n\
  --grayscale                display document without colors\n\
  --color                    display document as usual\n\
  --safer                    start ghostscript in safe mode\n\
  --nosafer                  do not start ghostscript in safe mode\n\
  --quiet                    start ghostscript with the -dQUIET option\n\
  --noquiet                  do not start ghostscript with the -dQUIET option\n\
  --arguments=ARGS           start ghostscript with additional options as specified\n\
                             by the string ARGS\n\
  --page=LABEL               display the page with label LABEL first\n\
  --center                   the page should be centered automatically\n\
  --nocenter                 the page should not be centered automatically\n\
  --media=MEDIA              selects the paper size to be used\n\
  --orientation=ORIENTATION  sets the orientation of the page\n\
  --scale=N                  selects the scale N\n\
  --scalebase=N              selects the scale base N\n\
  --swap                     interchange the meaning of the orientations landscape\n\
                             and seascape\n\
  --noswap                   do not interchange the meaning of the orientation\n\
                             landscape and seascape\n\
  --antialias                use antialiasing\n\
  --noantialias              do not use antialiasing\n\
  --dsc                      dsc comments are respected\n\
  --nodsc                    dsc comments are not respected\n\
  --eof                      ignore the postscript EOF comment while scanning\n\
                             documents\n\
  --noeof                    do not ignore the postscript EOF comment while\n\
                             scanning documents\n\
  --pixmap                   use backing pixmap\n\
  --nopixmap                 do not use backing pixmap\n\
  --watch                    watch the document file for changes\n\
  --nowatch                  do not watch the document file for changes\n\
  --help                     print a help message and exit\n\
  --usage                    print a usage message and exit\n\
  --resize                   fit the size of the window to the size of the page\n\
  --noresize                 do not fit the size of the window to the size of the page\n\
  -geometry [<width>][x<height>][{+-}<xoffset>{+-}<yoffset>]\n\
  --ad=FILE                  read and use additional resources from FILE\n\
  --style=FILE               read and use additional resources from FILE. These resources\n\
                             have lower priority than those provided on the context of --ad\n\
  --spartan                  shortcut for --style=gv_spartan.dat\n\
  --version                  show gv version and exit\n\
";

char *message_help = "\
gv help:\n\
   \n\
   # Document to be displayed\n\
     [<name of file to display>[.ps][.pdf]] ! The extension may be omitted\n\
   # Color environment:\n\
     [-monochrome] [-grayscale] [-color]\n\
   # Ghostscript Options:\n\
     [-[no]safer]             ! [Do not] pass -dSAFER to Ghostscript\n\
     [-[no]quiet]             ! [Do not] pass -dQUIET to Ghostscript\n\
     [-arguments <arguments>] ! Start Ghostscript with <arguments>\n\
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

