/*
**
** file.c
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** Copyright (C) 2004 Jose E. Marchesi
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
**           Jose E. Marchesi (jemarch@gnu.org)
**           GNU Project
**
** Changes submitted by Maurizio Loreti distributed on the public
** domain:
**
**       - Code for handle bzip2 compressed files.
**
*/

/*
#define MESSAGES
*/
#include "message.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef VMS
#   include <stat.h>
#else  
#   include <sys/types.h>
#   include <sys/stat.h>
#endif

#include "paths.h"
#include INC_X11(Intrinsic.h)

#include "types.h"
#include "config.h"
#include "d_memdebug.h"
#include "file.h"
#include "main_resources.h"
#include "main_globals.h"

#ifdef VMS
#   include "strcasecmp.h"
#else
#   include <string.h>
#endif


/*############################################################*/
/* file_getDirOfPath */
/*############################################################*/

char *
file_getDirOfPath(path)
   char *path;
{
   char *dir=NULL;
   char *pos;

   BEGINMESSAGE(file_getDirOfPath)
#ifdef VMS
   if (path) {
      dir=GV_XtNewString(path);
      pos = strrchr(dir,']');
      if (!pos) pos=strrchr(dir,':');
      if (pos) { pos++; *pos='\0'; }
   }
#else
   if (path) {
      dir=GV_XtNewString(path);
      pos = strrchr(dir,'/');
      if (pos) { pos++; *pos='\0'; }
   }
#endif
#ifdef MESSAGES
   if (!dir) { INFMESSAGE (### Warning: returning NULL) }
   else      { INFSMESSAGE(returning, dir) }
#endif
   ENDMESSAGE(file_getDirOfPath)
   return(dir);
}

/*############################################################*/
/* file_stripVersionNumber */
/*############################################################*/

int
file_stripVersionNumber(filename)
   char *filename;
{
   int strippedVersionNumber=0;

   BEGINMESSAGE(file_stripVersionNumber)
#ifdef VMS
   if (filename) {
      char *pos;
      pos = strrchr(filename,';');
      if (pos) {
         strippedVersionNumber=1;
         *pos='\0';
      }
   }
#endif
   ENDMESSAGE(file_stripVersionNumber)
   return(strippedVersionNumber);
}

/*############################################################*/
/* file_locateFilename */
/*############################################################*/

char *
file_locateFilename(path)
   char *path;
{
   char *tmp=NULL;
   BEGINMESSAGE(file_locateFileName)
   if (path) {
#     ifdef VMS
         tmp = strrchr(path,']');
         if (!tmp) tmp = strrchr(path,':');
#     else
         tmp = strrchr(path,'/');
#     endif
      if (!tmp) tmp=path;
      else tmp++;
      INFSMESSAGE(found,tmp)
   }
   ENDMESSAGE(file_locateFilename)
   return(tmp);
}

/*############################################################*/
/* file_getTmpFilename */
/* provide some temporary file name */
/*############################################################*/

char *
file_getTmpFilename(baseDirectory,baseFilename)
   char *baseDirectory;
   char *baseFilename;
{
   char tempFilename[256];
   char *tempFilenameP;
   char tmpNameBuf[256];
   char tmpDirBuf[256];
   char *tmpName;
   char *tmpExt;
   char *pos;

   BEGINMESSAGE(file_getTmpFilename)

   if (!baseDirectory) baseDirectory = app_res.scratch_dir;
   strcpy(tmpDirBuf,baseDirectory);
   pos = file_locateFilename(tmpDirBuf);
   if (pos) { ++pos; *pos='\0'; }
   else strcpy(tmpDirBuf,app_res.scratch_dir);

   if (!baseFilename) baseFilename= ".";
   strcpy(tmpNameBuf,baseFilename);
   file_stripVersionNumber(tmpNameBuf);
   pos = file_locateFilename(tmpNameBuf);
   if (pos) tmpName = pos;
   else     tmpName = tmpNameBuf;

   pos = strrchr(tmpName,'.');
   if (pos) { *pos='\0'; tmpExt = pos+1; }
   else tmpExt = "";

/* Limit filename to 39 characters (excluding dir and .tmp).
   This is required for VMS, but is also reasonable for Unix. */
   if (strlen(tmpName)+strlen(tmpExt)>23) {
      if (strlen(tmpExt)>11) tmpExt[11] = '\0';     /* allow .ps_page_nnn */
      if (strlen(tmpName)+strlen(tmpExt)>23) tmpName[23-strlen(tmpExt)] = '\0';
   }
   {
      struct stat s;
      int no_such_file;
      int i=1;
      do {
#ifdef VMS
         sprintf(tempFilename,"%sgv_%lx_%x_%s_%s.tmp",tmpDirBuf,time(NULL),i,tmpName,tmpExt);
#else
         sprintf(tempFilename,"%sgv_%lx_%x_%s.%s.tmp",tmpDirBuf,time(NULL),i,tmpName,tmpExt);
#endif
         file_translateTildeInPath(tempFilename);
         no_such_file = stat(tempFilename,&s);
         i++;
      } while (!no_such_file);
   } 
   SMESSAGE(tempFilename)
   tempFilenameP = GV_XtNewString(tempFilename);
   ENDMESSAGE(file_getTmpFilename)
   return(tempFilenameP);
}

/*############################################################*/
/* file_translateTildeInPath */
/* Replaces tilde in string by user's home directory. */
/*############################################################*/

void
file_translateTildeInPath(path)
   char *path;
{
   char *pos;

   BEGINMESSAGE(file_translateTildeInPath)
#ifndef VMS
   if ((pos=strchr(path,'~'))) {
      char *home;
      char tmp[GV_MAX_FILENAME_LENGTH];
      home=getenv("HOME");
      if (strlen(home)+strlen(path)-1 < GV_MAX_FILENAME_LENGTH-1) {
         *pos='\0'; pos++;
         strcpy(tmp,path);
         strcat(tmp,home);
         strcat(tmp,pos);
         strcpy(path,tmp);
      }
   }
#endif /* end of not VMS */
   ENDMESSAGE(file_translateTildeInPath)
}

/*############################################################*/
/* file_fileIsDir */
/*############################################################*/

int file_fileIsDir(fn)
  char *fn;
{
  struct stat s;
  int r=0;
  char *c;
  BEGINMESSAGE(file_fileIsNotUseful)
  if (fn) {
#ifdef VMS
     c = strrchr(fn,']');
     if (c && (!*(c+1) || isspace(*(c+1)))) r = 1;
     if (!r) {
       c = strrchr(fn,':');
       if (c && (!*(c+1) || isspace(*(c+1)))) r = 1;
     }
#else
     c = strrchr(fn,'/');
     if (c && (!*(c+1) || isspace(*(c+1)))) r = 1;
#endif
     if (!r && !stat(fn,&s)  && (S_ISDIR(s.st_mode))) r=1;
  }
  IMESSAGE(r)
  ENDMESSAGE(file_fileIsDir)
  return(r);
}

/*############################################################*/
/* file_fileIsNotUseful */
/*############################################################*/

int
file_fileIsNotUseful(fn)
  char *fn;
{
  struct stat s;
  int r=0;
  BEGINMESSAGE(file_fileIsNotUseful)
  if (!fn || stat(fn,&s)  || (S_ISDIR(s.st_mode)) || s.st_size==0) r=1;
  IMESSAGE(r)
  ENDMESSAGE(file_fileIsNotUseful)
  return(r);
}

/*############################################################*/
/* file_pdfname2psname */
/* If the file ends in .pdf, change this to .ps.*/
/* Return pointer to temp copy if changed, else to input string. */
/*############################################################*/
 
char *
file_pdfname2psname(name)
  char *name;
{
  char *e;

  BEGINMESSAGE(file_pdfname2psname)
  if (!name) {
     ENDMESSAGE(file_pdfname2psname)
     return(name);
  }
  INFSMESSAGE(in,name)
# ifdef VMS
    e = strrchr(name,';');
    if (!e) e = name+strlen(name);
    else *e = '\0';
# else
    e = name+strlen(name);
# endif
  if ((e-name)-4 >= 0) {
    e -= 4;
    if (!strcasecmp(e,".pdf")) {
#     ifdef VMS
        strcpy(e,".PS");
#     else
        strcpy(e,".ps");
#     endif
    }
  }
  INFSMESSAGE(out,name)
  ENDMESSAGE(file_pdfname2psname)
  return(name);
}


/*############################################################*/
/* file_getUsefulName */
/*############################################################*/
 
char *
file_getUsefulName(name)
  char *name;
{
  char *e,*c;
  String mext,ext;

  BEGINMESSAGE(file_getUsefulName)
  if (!name) {
    ENDMESSAGE(file_getUsefulName)
    return(name);
  }
  INFSMESSAGE(in,name)
# ifdef VMS
    e = strrchr(name,';');
    if (e) *e = '\0';
# endif
  c = e = strrchr(name,'.');
  if (!e) {
     ENDMESSAGE(file_getUsefulName)
     return(name);
  }
  mext = ext = GV_XtNewString(e);
  while (*ext) { *ext = tolower(*ext); ext++; }
  if      (!strncmp(mext,".gz",3))  ext = "";
  else if (!strncmp(mext,".bz2",4)) ext = "";
  else if (!strncmp(mext,".z",2))   ext = "";
  else if (!strncmp(mext,".ps",3))  ext = ".ps";
  else if (!strncmp(mext,".pdf",4)) ext = ".pdf";
  else                              ext = e;
  if (e == name && !(*ext)) {
    ENDMESSAGE(file_getUsefulName)
    return(name);
  }
  strcpy(mext,ext);
  ext=mext;
  while (*ext && *e) *ext++=*e++;
  strcpy(c,mext);
  GV_XtFree(mext);
  INFSMESSAGE(out,name)
  ENDMESSAGE(file_getUsefulName)
  return(name);
}

