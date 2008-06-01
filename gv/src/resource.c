/*
**
** resource.c
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
*/

#define _GV_RESOURCE_C_

/*
  #define MESSAGES
*/
#include "message.h"

#include <ac_config.h>
#include "config.h"

#include <stdlib.h>
#include <stdio.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)

#include "types.h"
#include "config.h"
#include "d_memdebug.h"
#include "file.h"
#include "main_resources.h"
#include "main_globals.h"
#include "resource.h"

# ifndef USER_DEFAULTS
#   define USER_DEFAULTS "~/.gv"
# endif

static String class_resources[] = {
#   include "gv_class.h"
  NULL
};

String intern_resources[] = {
#   include INTERN_RESOURCES_H
  NULL
};

#ifdef USE_FALLBACK_STYLES
#  ifdef FALLBACK_STYLE_1_H
static String fallback_style_1[] = {
#      include FALLBACK_STYLE_1_H
  NULL
};
#  endif
#  ifdef FALLBACK_STYLE_2_H
static String fallback_style_2[] = {
#      include FALLBACK_STYLE_2_H
  NULL
};
#  endif
#  ifdef FALLBACK_STYLE_3_H
static String fallback_style_3[] = {
#      include FALLBACK_STYLE_3_H
  NULL
};
#  endif
#endif

static String resource_system_file = NULL;
static String resource_user_file   = NULL;
static String resource_style_file  = NULL;
static String resource_ad_file     = NULL;
static char* resource_mergeFileIntoDatabase(XrmDatabase*,char*);

/*#######################################################
  resource_freeData
  #######################################################*/

void resource_freeData()
{
  BEGINMESSAGE(resource_freeData)
    if (resource_system_file) GV_XtFree(resource_system_file);
  if (resource_user_file)   GV_XtFree(resource_user_file);
  if (resource_style_file)  GV_XtFree(resource_style_file);
  if (resource_ad_file)     GV_XtFree(resource_ad_file);
  ENDMESSAGE(resource_freeData)
    }

/*#######################################################
  resource_buildDatabase
  #######################################################*/

XrmDatabase
resource_buildDatabase (
                        Display *display,
                        char *app_class,
                        char *app_name,
                        int *argcP,
                        char **argv)
{
  XrmDatabase gvdb = XtDatabase(display);
  XrmDatabase db = NULL;
  String *sP;
  String s,t, rpath;
  char tmp[GV_MAX_FILENAME_LENGTH];

  BEGINMESSAGE(resource_buildDatabase)

  /* ### class resources ################# */
  INFMESSAGE(merging class resources into database)
  sP = class_resources;
  while (*sP) XrmPutLineResource(&db,*sP++);

  /* ### system resources ################# */
  INFMESSAGE(checking for system resources)

  rpath = GV_XtMalloc (strlen (GV_LIBDIR) + strlen ("/gv_system.ad") + 1);
  rpath[0] = '\0';
  strcat (rpath, GV_LIBDIR);
  strcat (rpath, "/gv_system.ad");
  if (rpath) {
    INFSMESSAGE(merging system resource file into database,rpath)
      XrmCombineFileDatabase(rpath,&db,True);
    resource_system_file = rpath;
  }

  XrmCombineDatabase (gvdb, &db, True);
  /* CombineDatabase destroyed gvdb==XtDatabase(display), which
   * XtResolvePathname will access soon, so repair it: */
  XrmSetDatabase (display, db);

  /* ### user resources ################# */
  INFMESSAGE(checking for user resources)

  strcpy(tmp,USER_DEFAULTS);
  file_translateTildeInPath(tmp);
  if (!file_fileIsNotUseful(tmp)) {
    s = GV_XtNewString(tmp);
  } else {
    s = getenv("XUSERFILESEARCHPATH");
    if (s) s = XtResolvePathname(display,NULL,NULL,NULL,s,NULL,0,NULL);
  }
  if (s) {
    INFSMESSAGE(merging user resource file into database,s)
      XrmCombineFileDatabase(s,&db,True);
    resource_user_file = s;
  } else {
    resource_user_file = GV_XtNewString(tmp);
  }

  /* ### command line resources ################# */

  /*
   * When managing --[no]feature options, please put the
   * --nofeature management _after_ the --feature one.
   *
   * -jemarch
   */

  if (antialias_p)
    {
      resource_putResource (&db, app_name, ".antialias", "True");
    }
  if (noantialias_p)
    {
      resource_putResource (&db, app_name, ".antialias", "False");
    }
  if (safer_p)
    {
      resource_putResource (&db, app_name, ".gsSafer", "True");
    }
  if (nosafer_p)
    {
      resource_putResource (&db, app_name, ".gsSafer", "False");
    }
  if (pixmap_p)
    {
      resource_putResource (&db, app_name, ".useBackingPixmap", "True");
    }
  if (nopixmap_p)
    {
      resource_putResource (&db, app_name, ".useBackingPixmap", "False");
    }
  if (color_p)
    {
      resource_putResource (&db, app_name, "*Ghostview.palette", "Color");
    }
  if (grayscale_p)
    {
      resource_putResource (&db, app_name, "*Ghostview.palette", "Grayscale");
    }
  if (spartan_p)
    {
      char *spartan_filename = (char *)
	GV_XtMalloc (strlen(GV_LIBDIR) + strlen ("/gv_spartan.dat") + 1);
      spartan_filename[0] = '\0';
      strcat(spartan_filename, GV_LIBDIR);
      strcat(spartan_filename, "/gv_spartan.dat");
      resource_putResource (&db, app_name, ".style", spartan_filename);
      GV_XtFree (spartan_filename);
    }
  if (widgetless_p)
    {
      char *widgetless_filename = (char *)
	GV_XtMalloc (strlen(GV_LIBDIR) + strlen ("/gv_widgetless.dat") + 1);
      widgetless_filename[0] = '\0';
      strcat(widgetless_filename, GV_LIBDIR);
      strcat(widgetless_filename, "/gv_widgetless.dat");
      resource_putResource (&db, app_name, ".style", widgetless_filename);
      GV_XtFree (widgetless_filename);
    }
  if (quiet_p)
    {
      resource_putResource (&db, app_name, ".gsQuiet", "True");
    }
  if (monochrome_p)
    {
      resource_putResource (&db, app_name, "*Ghostview.palette", "Monochrome");
    }
  if (noquiet_p)
    {
      resource_putResource (&db, app_name, ".gsQuiet", "False");
    }
   if (media_p) 
     { 
       resource_putResource (&db, app_name, ".pageMedia", media_value); 
     } 
   if (orientation_p)
     {
       resource_putResource (&db, app_name, ".orientation", orientation_value);
     }
   if (page_p)
     {
       resource_putResource (&db, app_name, ".page", page_value);
     }

   if (center_p)
     {
       resource_putResource (&db, app_name, ".autoCenter", "True");
     }
   if (nocenter_p)
     {
       resource_putResource (&db, app_name, ".autoCenter", "False");
     }
   if (scale_p)
     {
       resource_putResource (&db, app_name, ".scale", scale_value);
     }
   if (magstep_p)
     {
       resource_putResource (&db, app_name, ".scale", magstep_value);
     }
   if (scalebase_p)
     {
       resource_putResource (&db, app_name, ".scaleBase", scalebase_value);
     }
   if (resize_p)
     {
       resource_putResource (&db, app_name, ".autoResize", "True");
     }
   if (noresize_p)
     {
       resource_putResource (&db, app_name, ".autoResize", "False");
     }
   if (swap_p)
     {
       resource_putResource (&db, app_name, ".swapLandscape", "True");
     }
   if (noswap_p)
     {
       resource_putResource (&db, app_name, ".swapLandscape", "False");
     }
   if (dsc_p)
     {
       resource_putResource (&db, app_name, ".respectDSC", "True");
     }
   if (nodsc_p)
     {
       resource_putResource (&db, app_name, ".respectDSC", "False");
     }
   if (eof_p)
     {
       resource_putResource (&db, app_name, ".ignoreEOF", "True");
     }
   if (noeof_p)
     {
       resource_putResource (&db, app_name, ".ignoreEOF", "False");
     }
   if (watch_p)
     {
       resource_putResource (&db, app_name, ".watchFile", "True");
     }
   if (nowatch_p)
     {
       resource_putResource (&db, app_name, ".watchFile", "False");
     }
   if (ad_p)
     {
       resource_putResource (&db, app_name, ".ad", ad_value);
     }
   if (style_p)
     {
       resource_putResource (&db, app_name, ".style", style_value);
     }
   if (arguments_p)
     {
       resource_putResource (&db, app_name, ".arguments", arguments_value);
     }
  
  INFMESSAGE(parsing command line)
    XrmParseCommand(&db,options,XtNumber(options),app_name,argcP,argv);

  /* ### style resources ######################## */
  INFMESSAGE(checking for style resources)
    s = resource_getResource(db,app_class,app_name,"style","Style");
  if (s) {
    t=NULL;
#   ifdef USE_FALLBACK_STYLES
    sP = NULL;
#     ifdef FALLBACK_STYLE_1_DAT
    if (!strcmp(GV_LIBDIR "/" FALLBACK_STYLE_1_DAT,s)) sP = fallback_style_1;
    if (!strcmp(FALLBACK_STYLE_1_DAT,s)) sP = fallback_style_1;
#     endif
#     ifdef FALLBACK_STYLE_2_DAT
    if (!strcmp(GV_LIBDIR "/" FALLBACK_STYLE_2_DAT,s)) sP = fallback_style_2;
    if (!strcmp(FALLBACK_STYLE_2_DAT,s)) sP = fallback_style_2;
#     endif
#     ifdef FALLBACK_STYLE_3_DAT
    if (!strcmp(GV_LIBDIR "/" FALLBACK_STYLE_3_DAT,s)) sP = fallback_style_3;
    if (!strcmp(FALLBACK_STYLE_3_DAT,s)) sP = fallback_style_3;
#     endif
    if (sP) {
      INFMESSAGE(merging fallback style resources into database)
	while (*sP) XrmPutLineResource(&db,*sP++);
      s = NULL;
    } else {
      INFSMESSAGE(merging style resource file into database,s)
        t = resource_mergeFileIntoDatabase(&db,s);
      if (!t) {
	fprintf(stderr,"%s: Style file '%s' not found - ignored\n",app_name,s);
	s=NULL;
      } else s = t;
    }
#   else
    
    /*
     * Do not use fallback styles
     *
     */

    INFSMESSAGE(merging style resource file into database,s)
      t = resource_mergeFileIntoDatabase(&db,s);
    if (!t) {
      fprintf(stderr,"%s: Style file '%s' not found - ignored\n",app_name,s);
      s=NULL;
    } else s = t;
#   endif
    if (s) {
      s = GV_XtNewString(s);
      resource_style_file = s;
    }
    if (t) GV_XtFree(t);
  }

  /* ### ad resources ######################## */
  INFMESSAGE(checking for ad resources)
    s = resource_getResource(db,app_class,app_name,"ad","Ad");
  if (s) {
    INFSMESSAGE(merging ad resource file into database,s)
      t = resource_mergeFileIntoDatabase(&db,s);
    if (!t) {
      fprintf(stderr,"%s: ad file '%s' not found - ignored\n",app_name,s);
      s=NULL;
    } else s = t;
    if (s) {
      s = GV_XtNewString(s);
      resource_ad_file = s;
    }
    if (t) GV_XtFree(t);
  }

  ENDMESSAGE(resource_buildDatabase)
  return (db);
}

/*#######################################################
  resource_putResource
  #######################################################*/

void 
resource_putResource (XrmDatabase *db,
		      char *app_name,
		      char *resource_class,
		      char *resource_value)
{
  char resource_name[GV_MAX_FILENAME_LENGTH];

  sprintf(resource_name, "%s%s", app_name, resource_class);
  XrmPutStringResource (db,
			resource_name,
			resource_value);

}

/*#######################################################
  resource_getResource
  #######################################################*/

char *resource_getResource(db,app_class,app_name,name_val,class_val)
     XrmDatabase db;
     char *app_class;
     char *app_name;
     char *name_val;
     char *class_val;
{
  char *result=NULL;
  XrmValue rm_value;
  String str_type;
  char str_name[GV_MAX_FILENAME_LENGTH],str_class[GV_MAX_FILENAME_LENGTH];
  Bool success=False;

  BEGINMESSAGE(resource_getResource)
    if (!class_val) class_val=name_val;
  sprintf(str_name, "%s.%s",app_name,name_val);
  sprintf(str_class,"%s.%s",app_class,class_val);
  INFSMESSAGE(looking for,str_class)
    success = XrmGetResource(db,str_name,str_class,&str_type,&rm_value);
  if  (success && rm_value.size>1) { result = (char*) rm_value.addr; INFSMESSAGE(found entry,result) }
  else                             { result = NULL;                  INFMESSAGE(no entry found)      }
  ENDMESSAGE(resource_getResource)
    return(result);
}

/*#######################################################
  resource_checkGeometryResource
  #######################################################*/

#ifdef max
#   undef max
#endif
#define max(a,b) ((a)>(b)?(a):(b))

int resource_checkGeometryResource(dbP,app_class,app_name)
     XrmDatabase *dbP;
     char *app_class;
     char *app_name;
{
  char tmp[GV_MAX_FILENAME_LENGTH];
  char *pos;
  int gx=0,gy=0;
  int flag;
  int dim_forced=0;
  char g[20];
  unsigned int gwidth=0,gheight=0;

  BEGINMESSAGE(resource_checkGeometryResource)
    pos = resource_getResource(*dbP,app_class,app_name,"geometry","Geometry");
  if (!pos) {
    ENDMESSAGE(resource_checkGeometryResource)
      return(dim_forced);
  }
  INFSMESSAGE(found geometry resource,pos)
    flag=XParseGeometry(pos,&gx,&gy,&gwidth,&gheight);
  IIMESSAGE(gx,gy) IIMESSAGE(gwidth,gheight)
    if (gwidth)  gwidth   = max(gwidth,GV_MINIMUM_SIZE);
  if (gheight) gheight  = max(gheight,GV_MINIMUM_SIZE);
  tmp[0]='\0';
  if (flag&WidthValue) {
    sprintf(g,"%d",(Dimension)gwidth); strcat(tmp,g);
    dim_forced=1;
  }
  if (flag&HeightValue) {
    sprintf(g,"x%d",(Dimension)gheight); strcat(tmp,g);
    dim_forced=1;
  }
  if (flag&XValue) {
    sprintf(g,"%s%d",((flag&XNegative) ? (gx ? "":"-") : "+"),gx);   strcat(tmp,g);
  }
  if (flag&YValue) {
    sprintf(g,"%s%d",((flag&YNegative) ? (gy ? "":"-") : "+"),gy);   strcat(tmp,g);
  }
  if (tmp[0]) {
    char s[50];
    INFSMESSAGE(corrected geometry,tmp)
      sprintf(s,"%s.geometry",app_name);
    SMESSAGE(s) SMESSAGE(tmp)
      XrmPutStringResource(dbP,s,tmp);
#   ifdef MESSAGES
    pos = resource_getResource(*dbP,app_class,app_name,"geometry","Geometry");
    if (pos) { INFSMESSAGE(check:,pos) }
#   endif
  }
  ENDMESSAGE(resource_checkGeometryResource)
    return(dim_forced);
}

/*#######################################################
  resource_checkResources
  #######################################################*/

int resource_checkResources(app_name,v,vc)
     char *app_name;
     char *v;
     char *vc;
{
  int n,nc,v1,v2,v3,v1c,v2c,v3c,r=1;

  BEGINMESSAGE(resource_checkResources)
    v1 = v2 = v3 = v1c = v2c = v3c = 0;
  n  = sscanf(v,"%*s %d.%d.%d",&v1,&v2,&v3);
  nc = sscanf(vc,"%*s %d.%d.%d",&v1c,&v2c,&v3c);
  if ((n < 2) || (nc < 2)
      || (v1 < v1c)
      || (v1 == v1c && v2 < v2c)
      || (v1 == v1c && v2 == v2c && v3 < v3c)) {
    fprintf(stderr,"%s Error: incompatible resources.\n",app_name);
    fprintf(stderr,"One of the files\n");
    if (resource_system_file) fprintf(stderr,"    %s\n",resource_system_file);
    if (resource_user_file)   fprintf(stderr,"    %s\n",resource_user_file);
    if (resource_style_file)  fprintf(stderr,"    %s\n",resource_style_file);
    if (resource_ad_file)     fprintf(stderr,"    %s\n",resource_ad_file);
    fprintf(stderr,"belongs to an older version of gv and can not be used.\n");
    fprintf(stderr,"Please remove or update the outdated file.\n");
    r=0;
  }
  ENDMESSAGE(resource_checkResources)
    return(r);
}


/*#######################################################
  resource_userResourceFile()
  #######################################################*/

char *resource_userDefaultsFile()
{
  char *s;
  BEGINMESSAGE(resource_userResourceFile)
    if      (resource_ad_file)   s = resource_ad_file;
    else if (resource_user_file) s = resource_user_file;
    else                         s = USER_DEFAULTS;
  s = GV_XtNewString(s);
  return(s);
  ENDMESSAGE(resource_userResourceFile)  
    }


/*---------------------------------------------------------------
  resource_mergeFileIntoDatabase
  ---------------------------------------------------------------*/

static char* resource_mergeFileIntoDatabase(dbP,name)
     XrmDatabase *dbP;
     char *name;
{
  char tmp[GV_MAX_FILENAME_LENGTH];
  int useful=0;

  BEGINMESSAGE(resource_mergeFileIntoDatabase)

    if (!name) {
      ENDMESSAGE(resource_mergeFileIntoDatabase)
	return(name);
    }

  strcpy(tmp,name);
  file_translateTildeInPath(tmp);

  if (file_fileIsNotUseful(tmp)) {
    INFSMESSAGE(not useful,tmp)
      if (name != file_locateFilename(name)) useful=-1;
      else {

	sprintf(tmp,"~/%s",name);
	file_translateTildeInPath(tmp);

      }
  } else useful=1;

  if (!useful) {

    if (!useful && file_fileIsNotUseful(tmp)) {
      INFSMESSAGE(not useful,tmp)
	sprintf(tmp,"%s%s",GV_LIBDIR,name);
    } else useful=1;

    if (!useful && file_fileIsNotUseful(tmp)) {
      INFSMESSAGE(not useful,tmp)
	INFMESSAGE(giving up)
	} else useful=1;
  }

  if (useful==1) {
    INFSMESSAGE(merging,tmp)
      XrmCombineFileDatabase(tmp,dbP,True);
    name = GV_XtNewString(tmp);
  }
  else name=NULL;
  ENDMESSAGE(resource_mergeFileIntoDatabase)
    return(name);
}
