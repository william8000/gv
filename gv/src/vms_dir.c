/*
**
** vms_dir.c
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
*/

/*
#define MESSAGES
#define MESSAGE_NO_ESC
*/
#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unixlib.h>
#include <rmsdef.h>
#include <descrip.h>
#include <lib$routines.h>

/*  --- INCLUDE BEGIN -------- */
#   include "vms_dir.h"
#   include "d_memdebug.h"
/*  --- INCLUDE END ---------- */

#define	NOWILD		0x00000001
#define MULTIPLE	0x00000002

/*#################################################################
   Check_Directory
#################################################################*/

static int 
Check_Directory(path)
   char	*path;
{
   char *cp;
   char dirfile[MAXNAMLEN+15];
   FILE *fp;
   int valid = 0;

   BEGINMESSAGE(Check_Directory)
   SMESSAGE(path);

   if (strlen(path) >= MAXNAMLEN) {
      INFMESSAGE(path exceeds maximum length) ENDMESSAGE(Check_Directory)
      return(valid);
   }
   strcpy(dirfile,path);

   cp = strrchr(dirfile,'.');
   if (cp) {
      *cp = ']';
      cp = strrchr(dirfile, ']');
      *cp = '\0';
      strcat(cp, ".DIR");
      valid = 1;
      SMESSAGE(dirfile)
   }
   else {
      char *dl, *dr;
      dr = strrchr(dirfile, ']');
      dl = strchr(dirfile, '[');
      if ((dl) && (dr)) {
         char tmp[MAXNAMLEN+15];
         *dr = '\0';
         strcpy(tmp,dl+1); *dl='\0';
         strcat(dirfile, "[000000]");
         strcat(dirfile, tmp);
         strcat(dirfile, ".DIR");
         valid = 1;
         SMESSAGE(dirfile)
      }
   }
   if (valid) {
      fp = fopen(dirfile, "r");
      if (!fp) {INFMESSAGE(directory does not exist) valid = 0; }
      else {INFMESSAGE(directory exists) fclose(fp); valid = 1; }
   }
   else {INFMESSAGE(invalid directory specification) valid = 0; }
   ENDMESSAGE(Check_Directory)
   return (valid);
}

/*#################################################################
   opendir
#################################################################*/

DIR *opendir( dirname )
   char *dirname;
{
   DIR *retdir = (DIR *)NULL;
   struct dsc$descriptor_s filedescriptor;
   char *filepathname;
   char path[MAXNAMLEN];
   struct dsc$descriptor_s *retdescrip;

   BEGINMESSAGE(opendir)

   if (!dirname) {
      INFMESSAGE(no dirname) ENDMESSAGE(opendir) 
      return (retdir);
   }

   retdir = (DIR *) GV_malloc(sizeof(DIR));
   if (!retdir) {
      INFMESSAGE(cannot GV_malloc retdir) ENDMESSAGE(opendir) 
      return (retdir);
   }

   strcpy(path,dirname);
   if (!strcmp(path,".")) getwd(path);
   SMESSAGE(path)

   if (!Check_Directory(path)) {
      INFMESSAGE(cannot open) ENDMESSAGE(opendir)
      GV_free(retdir);
      return ((DIR *)NULL);
   }

   filepathname = (char *) GV_malloc((MAXNAMLEN+1)*sizeof(char));
   if (!filepathname) {
      INFMESSAGE(cannot malloc filepathname) ENDMESSAGE(opendir)
      GV_free(retdir);
      return ((DIR *)NULL);
   }
   strcpy(filepathname, path);
   strcat(filepathname, "*.*.*");

   retdescrip = (struct dsc$descriptor_s *) GV_malloc(sizeof(struct dsc$descriptor_s));
   if (!retdescrip) {
      INFMESSAGE(cannot malloc retdescrip) ENDMESSAGE(opendir)
      GV_free(retdir);
      GV_free(filepathname);
      return ((DIR *)NULL);
   }
   retdescrip->dsc$b_dtype	= DSC$K_DTYPE_T;
   retdescrip->dsc$b_class	= DSC$K_CLASS_S;
   retdescrip->dsc$w_length	= strlen(filepathname);
   retdescrip->dsc$a_pointer	= filepathname;

   retdir->dd_fd  = (unsigned long) retdescrip;
   retdir->dd_loc = 0;
   retdir->dd_buf = (struct dirent *) GV_malloc(sizeof(struct dirent));
   if (!(retdir->dd_buf)) {
      INFMESSAGE(cannot malloc dd_buf) ENDMESSAGE(opendir)
      GV_free(retdir);
      GV_free(filepathname);
      GV_free(retdescrip);
      return ((DIR *)NULL);
   }

   INFSMESSAGE(valid:,path)
   ENDMESSAGE(opendir)
   return (retdir);
}

/*#################################################################
   readdir
#################################################################*/

struct dirent *readdir( dirp )
   DIR *dirp;
{
   struct dsc$descriptor_s retfilenamedesc;
   char retfilename[MAXNAMLEN+1];
   char *sp;
   unsigned long istatus;
   unsigned long rms_status;
   unsigned long flags;

   BEGINMESSAGE(readdir)

   flags = MULTIPLE;

   retfilenamedesc.dsc$b_dtype	= DSC$K_DTYPE_T;
   retfilenamedesc.dsc$b_class	= DSC$K_CLASS_S;
   retfilenamedesc.dsc$w_length	= MAXNAMLEN;
   retfilenamedesc.dsc$a_pointer= retfilename;

   istatus = lib$find_file ((struct dsc$descriptor_s*)dirp->dd_fd,
                            &retfilenamedesc,
                            &dirp->dd_loc,
                            0, 0,
                            &rms_status,
                            &flags);

   if (!(istatus & 1) && (istatus != RMS$_NMF) && (istatus != RMS$_FNF)) {
      lib$signal (istatus);
      ENDMESSAGE(readdir)
      return((struct dirent *)NULL);
   } else if ((istatus == RMS$_NMF) || (istatus == RMS$_FNF)) {
      ENDMESSAGE(readdir)
      return((struct dirent *)NULL);
   }

   retfilename[retfilenamedesc.dsc$w_length] = '\0';

   sp = strchr(retfilename, ' ');
   if (sp != NULL) *sp = '\0';

            sp = strrchr(retfilename, ']');
   if (!sp) sp = strrchr(retfilename, ':');
   if (sp)  sp++;
   else     sp = retfilename;

   strcpy(dirp->dd_buf->d_name, sp);

   ENDMESSAGE(readdir)
   return (dirp->dd_buf);
}

/*#################################################################
   closedir
#################################################################*/

int closedir(dirp)
   DIR *dirp;
{
   BEGINMESSAGE(closedir)
   lib$find_file_end (&dirp->dd_loc);
   GV_free (((struct dsc$descriptor_s*)dirp->dd_fd)->dsc$a_pointer);
   GV_free ((void*)dirp->dd_fd);
   GV_free ((void*)dirp->dd_buf);
   GV_free ((void*)dirp);
   ENDMESSAGE(closedir)
   return(0);
}

/*#################################################################
   getwd
#################################################################*/

char *getwd(p)
   char 	*p;
{
   BEGINMESSAGE(getwd)
   getcwd(p,MAXNAMLEN);
   SMESSAGE(p)
   ENDMESSAGE(getwd)
   return(p);
}
