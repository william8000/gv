/*
**
** ad2c.c
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unixlib.h>
#include <unixio.h>
#include <ctype.h>

/*
#define MESSAGES
*/
#include"message.h"

FILE  *in;
FILE  *out;
#define EOS		'\0'
#define LINELENGTH	2048
#define SUCCESS		0
#define FAILED		-1

/* ####################################################################
  f$replace
#####################################################################*/

int
f$replace(OUT,FIND,REPLACE,IN)
   char 	*OUT;
   char 	*FIND;
   char 	*REPLACE;    
   char 	*IN;
{  
   int		locat = 0;
   int		findlength;
   char		*intemp;
   char		*temp;

   BEGINMESSAGE(f$replace)

   findlength = strlen(FIND);
   if (!(*IN) || !(*FIND)) { ENDMESSAGE(f$replace) return(FAILED); };
   if (!(strstr(IN,FIND))) { if (OUT != IN) strcpy(OUT,IN); ENDMESSAGE(f$replace) return(FAILED); };

   intemp = malloc(strlen(IN)+1);
   strcpy(intemp,IN);

   temp=intemp;
   while ((temp=strstr(temp,FIND))) { *temp='\0'; temp += findlength; ++locat; }

   temp=intemp; *OUT = '\0';
   while ((locat--) > 0) {
      strcat(OUT,temp); strcat(OUT,REPLACE);
      temp = strchr(temp,'\0') + findlength;
   }
   strcat(OUT,temp);
   free(intemp);
   ENDMESSAGE(f$replace)
   return(SUCCESS);
}

/* ####################################################################
  usage
#####################################################################*/

void 
usage()
{  
   BEGINMESSAGE(usage)
   fprintf(stdout,"  Usage:  ad2c <infile> [outfile]\n");
   fprintf(stdout,"          with  default [outfile] = app-defaults.h\n");
   ENDMESSAGE(usage)
   MAINENDMESSAGE(main)
   exit(1);
}

/* ####################################################################
  main
#####################################################################*/

int
main(argc,argv) 
   int		argc;
   char		*argv[];
{
   char		line[LINELENGTH];
   char		in_file[255];
   char		out_file[255];
   char         *line_start;
   char         *line_end;
   int          new_line;
   char         tmp[LINELENGTH];
   char         *tempfile = "ad2c.tmp";
   char 	*temp;

   MAINBEGINMESSAGE(main)
    if (argc < 2) usage();

    strcpy(in_file,*(argv+1));
    if (argc >2) strcpy(out_file,*(argv+2));
    else strcpy(out_file,"app-defaults.h");
    SMESSAGE(in_file)
    SMESSAGE(out_file)

    if (!(strcmp(in_file,"-H"))) usage();
    if (!(in = fopen(in_file,"r"))) {
       fprintf(stderr,"  Error: unable to open %s for reading\n",in_file);
       MAINENDMESSAGE(main)
       exit(2);
    }
    if (!(out = fopen(tempfile,"w"))) {
       fprintf(stderr,"  Error: unable to open %s for writing\n",tempfile);
       MAINENDMESSAGE(main)
       exit(2);
    }

    new_line = TRUE;
    while (!(feof(in))) {
       fgets(line,LINELENGTH,in);
       if      (*line=='!')    *line = '\0';
       else if (temp = strstr(line,"\n\0")) *temp = '\0';
       else temp = strrchr(line,'\0');
       strcpy(tmp,line);
       if (tmp[0] == '\0') line[0] = '\0';
       if (line[0]) {
          SMESSAGE(line)
          if (new_line) line_start = "\"";
          else          line_start = "";
          --temp;
          if (*temp == '\\') { *temp = '\0'; line_end = "\\";  new_line = FALSE;}
          else               {               line_end = "\","; new_line = TRUE; }
          f$replace(line,"\\","\\\\",line);
          f$replace(line,"\"","\\\"",line);
          strcpy(tmp,line_start);
          strcat(tmp,line);
          strcat(tmp,line_end);
          SMESSAGE(tmp)
          strcat(tmp,"\n");
          fputs(tmp,out);
       }
    }
    fclose(in);
    fclose(out);
    rename(tempfile,out_file);
    MAINENDMESSAGE(main)
}
