/*
**
** options_gv.c
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

/*
#define MESSAGES
*/
#include "message.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_X11(Shell.h)
#include INC_XAW(SimpleMenu.h)
#include INC_XAW(SmeBSB.h)
#include "Aaa.h"
#include "Button.h"
#include "FileSel.h"
#include "Ghostview.h"
#include "Frame.h"
#include "MButton.h"
#include "Switch.h"

#include "types.h"
#include "callbacks.h"
#include "d_memdebug.h"
#include "file.h"
#include "widgets_misc.h"
#include "main_resources.h"
#include "main_globals.h"
#include "misc.h"
#include "note.h"
#include "options.h"
#include "options_gv.h"
#include "ps.h"
#include "doc_misc.h"

#ifdef VMS
#   include "strcasecmp.h"
#else
#   include <string.h>
#endif

static Widget   popup=NULL,optionControl;
static Widget   antialiasToggle;
static Widget   dscToggle,eofToggle,autoResizeToggle;
static Widget   swapLandscapeToggle,watchToggle;
static Widget   mediaLabel,mediaButton,mediaMenu=NULL;
static Widget   fmediaLabel,fmediaButton,fmediaMenu=NULL;
static Widget   orientLabel,orientButton,orientMenu;
static Widget   forientLabel,forientButton,forientMenu;
static Widget   scalesLabel,scalesButton,scalesMenu=NULL;
static Widget   scaleBaseLabel,scaleBaseButton,scaleBaseMenu=NULL;

static String orientations[5] = { "Portrait","Landscape","Upside-Down","Seascape", NULL };
static int opt_orientation;
static int opt_pagemedia;

static void options_gv_create();
static void options_gv_setOptionsAtEntry();

/*------------------------------------------------------
  OptionPopupStruct
------------------------------------------------------*/

OptionPopupStruct gv_options_gv_struct = {
  NULL,
  options_gv_create,
  options_gv_setOptionsAtEntry,
  False
};

OptionPopup gv_options_gv = (OptionPopup) &gv_options_gv_struct;

/*------------------------------------------------------
   options_gv_setOptionsAtEntry
------------------------------------------------------*/

static void options_gv_setOptionsAtEntry()
{
  String s;
  Arg args[2];
  Cardinal n;
  int i;

  BEGINMESSAGE(options_gv_setOptionsAtEntry)

  widgets_setToggle(antialiasToggle, (app_res.antialias ? 1 : 0));
  widgets_setToggle(dscToggle, ((gv_scanstyle & SCANSTYLE_IGNORE_DSC) ? 0 : 1));
  widgets_setToggle(eofToggle,        ((gv_scanstyle & SCANSTYLE_IGNORE_EOF) ? 1 : 0));
  widgets_setToggle(autoResizeToggle, (app_res.auto_resize ? 1 : 0));
  widgets_setToggle(swapLandscapeToggle, (gv_swap_landscape ? 1 : 0));
  widgets_setToggle(watchToggle, (app_res.watch_file ? 1 : 0));

  s=NULL;
  for (i = 0; orientations[i]; i++) {
    if (!strcasecmp(app_res.default_orientation,orientations[i]))
      s = orientations[i];
  }
  if (!s) s = "Automatic";
                                   n=0;
  XtSetArg(args[n], XtNlabel, s);  n++;
  XtSetValues(orientButton, args, n);
  opt_orientation = doc_convStringToDocOrient(s);

  s=NULL;
  for (i = 0; orientations[i]; i++) {
    if (!strcasecmp(app_res.fallback_orientation,orientations[i]))
      s = orientations[i];
  }
  if (!s) s = orientations[0];
                                   n=0;
  XtSetArg(args[n], XtNlabel, s);  n++;
  XtSetValues(forientButton, args, n);

  ENDMESSAGE(options_gv_setOptionsAtEntry)
}

/*------------------------------------------------------
   options_gv_getScales
------------------------------------------------------*/

static void options_gv_getScales(bP,sP)
  int *sP;
  int *bP;
{
  Arg args[1];
  Cardinal n;
  String bl,sl;
  int i,si=0,bi=0;
  BEGINMESSAGE(options_gv_getScales)
  *sP=*bP=-1;
		      		    n=0;
  XtSetArg(args[n], XtNlabel, &bl); n++;
  XtGetValues(scaleBaseButton, args, n);
		      		    n=0;
  XtSetArg(args[n], XtNlabel, &sl); n++;
  XtGetValues(scalesButton, args, n);
  for (i=0; gv_scales[i];i++) {
    if (gv_scales[i]->is_base) {
      if (!strcmp(gv_scales[i]->name,bl)) *bP=bi;
      si++;
      bi++;
    } else {
      if (!strcmp(gv_scales[i]->name,sl)) *sP=si;
      si++;
    }
  }
  ENDMESSAGE(options_gv_getScales)
}

/*------------------------------------------------------
   options_gv_cb_apply
------------------------------------------------------*/

static void options_gv_cb_apply(w, client_data, call_data)
   Widget	w;
   XtPointer	client_data, call_data;
{
   Arg args[5];
   Cardinal n;
   static Boolean s_media = False;
   static Boolean s_fmedia = False;
   static Boolean s_orient = False;
   static Boolean s_forient = False;
   Boolean b;
   int i,j,k;
   Boolean redisplay=False;
   Boolean reopen=False;

   BEGINMESSAGE(options_gv_cb_apply)

   b = app_res.antialias;
   app_res.antialias = SwitchIsSet(antialiasToggle) ? True : False;
   cb_antialias(NULL,NULL,NULL); /* only change the menu entry */
   if (b != app_res.antialias) reopen=True;

   i = gv_swap_landscape;
   gv_swap_landscape = SwitchIsSet(swapLandscapeToggle) ? 1 : 0;
   if (i != gv_swap_landscape) redisplay=True;

   b = SwitchIsSet(autoResizeToggle) ? True : False;
   if (b != app_res.auto_resize) {
     cb_autoResize(NULL,(XtPointer)1,NULL);
   }

   i = (gv_scanstyle & SCANSTYLE_IGNORE_DSC) ? 0 : 1;
   j = SwitchIsSet(dscToggle) ? 1 : 0;
   if (i != j) {
     gv_scanstyle = (gv_scanstyle & SCANSTYLE_IGNORE_DSC) ?
       (gv_scanstyle & ~SCANSTYLE_IGNORE_DSC) :
       (gv_scanstyle |  SCANSTYLE_IGNORE_DSC);
     cb_handleDSC(NULL,NULL,NULL);
     reopen=True;
   }

   b = SwitchIsSet(watchToggle) ? True : False;
   if (b != app_res.watch_file) {
     cb_watchFile(NULL,(XtPointer)1,NULL);
   }

   i = (gv_scanstyle & SCANSTYLE_IGNORE_EOF ? 1 : 0);
   j = SwitchIsSet(eofToggle) ? 1 : 0;
   if (i != j) {
     gv_scanstyle = (gv_scanstyle & SCANSTYLE_IGNORE_EOF) ?
       (gv_scanstyle & ~SCANSTYLE_IGNORE_EOF) :
       (gv_scanstyle |  SCANSTYLE_IGNORE_EOF);
     cb_handleEOF(NULL,NULL,NULL);
     reopen=True;
   }

   if (s_media) GV_XtFree(app_res.default_pagemedia);
							n=0;
   XtSetArg(args[n], XtNlabel, &app_res.default_pagemedia);n++;
   XtGetValues(mediaButton, args, n);
   app_res.default_pagemedia = GV_XtNewString(app_res.default_pagemedia);
   i = doc_convStringToPageMedia(NULL,app_res.default_pagemedia);
   if (i != opt_pagemedia) {
     INFMESSAGE(pagemedia changed)
     opt_pagemedia = i;
     j = gv_pagemedia_auto;
     k = gv_pagemedia;
     gv_pagemedia_auto = 0;
     i = doc_convStringToPageMedia(doc,app_res.default_pagemedia);
     cb_setPagemedia(NULL,(XtPointer)i,(XtPointer)1);
     if ((j != gv_pagemedia_auto) || (k != gv_pagemedia)) redisplay = True;
   }
   s_media = True;

   if (s_fmedia) GV_XtFree(app_res.fallback_pagemedia);
							n=0;
   XtSetArg(args[n], XtNlabel, &app_res.fallback_pagemedia);n++;
   XtGetValues(fmediaButton, args, n);
   app_res.fallback_pagemedia = GV_XtNewString(app_res.fallback_pagemedia);
   gv_fallback_pagemedia = doc_convStringToPageMedia(NULL,app_res.fallback_pagemedia);
   s_fmedia = True;

   if (s_orient) GV_XtFree(app_res.default_orientation);
							n=0;
   XtSetArg(args[n], XtNlabel, &app_res.default_orientation);n++;
   XtGetValues(orientButton, args, n);
   app_res.default_orientation = GV_XtNewString(app_res.default_orientation);
   i = doc_convStringToDocOrient(app_res.default_orientation);
   if (i != opt_orientation) {
     INFMESSAGE(orientation changed)
     opt_orientation = i;
     j = gv_orientation_auto;
     k = gv_orientation;
     gv_orientation_auto = 0;
     cb_setOrientation(NULL,(XtPointer)i,(XtPointer)1);
     if ((j != gv_orientation_auto) || (k != gv_orientation)) redisplay = True;
   }
   s_orient = True;

   if (s_forient) GV_XtFree(app_res.fallback_orientation);
							n=0;
   XtSetArg(args[n], XtNlabel, &app_res.fallback_orientation);n++;
   XtGetValues(forientButton, args, n);
   app_res.fallback_orientation = GV_XtNewString(app_res.fallback_orientation);
   gv_fallback_orientation = doc_convStringToDocOrient(app_res.fallback_orientation);
   s_forient = True;

   options_gv_getScales(&i,&j);
   if (i>=0 && i != gv_scale_base_current) {
      gv_scale_base = i;
      redisplay = True;
   }
   if (j>=0 && j != gv_scale_current) {
      gv_scale = j;
      redisplay = True;
   }

   if (reopen && gv_filename) {
     cb_stopInterpreter(page,NULL,NULL);
     show_page(REQUEST_REOPEN,NULL);
   } else if (redisplay) show_page(REQUEST_OPTION_CHANGE,NULL);

   ENDMESSAGE(options_gv_cb_apply)
}

/*------------------------------------------------------
   options_gv_cb_save
------------------------------------------------------*/

static
void options_gv_cb_save(w, client_data, call_data)
  Widget	w;
  XtPointer	client_data, call_data;
{
  int    argn = 0;
  String argi[30];
  String argv[30];
  String l;
  Arg args[1];
  Cardinal n;
  String t = "True";
  String f = "False";
  int i,s,b;
  char tmp[20];

  BEGINMESSAGE(options_gv_cb_save)

  options_setArg(&(argi[argn]),&(argv[argn]),s_respectDSC          ,gv_class       ,SwitchIsSet(dscToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_ignoreEOF           ,gv_class       ,SwitchIsSet(eofToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_watchFile           ,gv_class       ,SwitchIsSet(watchToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_antialias           ,gv_class       ,SwitchIsSet(antialiasToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_autoResize          ,gv_class       ,SwitchIsSet(autoResizeToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_swapLandscape       ,gv_class       ,SwitchIsSet(swapLandscapeToggle) ? t : f);
       ++argn;
                                    n=0;
  XtSetArg(args[n], XtNlabel, &l);  n++;
  XtGetValues(mediaButton, args, n);
  options_setArg(&(argi[argn]),&(argv[argn]),s_pageMedia           ,gv_class       ,l);
       ++argn;
  XtGetValues(fmediaButton, args, n);
  options_setArg(&(argi[argn]),&(argv[argn]),s_fallbackPageMedia   ,gv_class       ,l);
       ++argn;
  XtGetValues(orientButton, args, n);
  options_setArg(&(argi[argn]),&(argv[argn]),s_orientation         ,gv_class       ,l);
       ++argn;
  XtGetValues(forientButton, args, n);
  options_setArg(&(argi[argn]),&(argv[argn]),s_fallbackOrientation,gv_class        ,l);
       ++argn;

  options_gv_getScales(&b,&s);
  if (b>=0) {
     sprintf(tmp,"%d",b+1);
     options_setArg(&(argi[argn]),&(argv[argn]),s_scaleBase,       gv_class        ,tmp);
       ++argn;
  }
  if (s>=0) {
    for (i=0; gv_scales[i];i++) if (gv_scales[i]->is_center) break;
    sprintf(tmp,"%d",s-i);
     options_setArg(&(argi[argn]),&(argv[argn]),s_scale,           gv_class        ,tmp);
       ++argn;
  }

  options_save(argn,argi,argv);
  while (--argn >=0) {
    GV_XtFree(argi[argn]); 
    GV_XtFree(argv[argn]);
  }

  ENDMESSAGE(options_gv_cb_save)
}

/*######################################################
   options_gv_createScaleMenus
######################################################*/

void options_gv_createScaleMenus()
{
  int i,sn=0,bn=0;
  Boolean change;
  Widget p,w;

  BEGINMESSAGE(options_gv_createScaleMenus)
  if (!popup) {
    INFMESSAGE(options not created)
    ENDMESSAGE(options_gv_createScaleMenus)
    return;
  }
  if (scalesMenu) {
    XtDestroyWidget(scalesMenu);
    scalesMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass,scalesButton,NULL,(Cardinal)0);
  } else {
    options_createLabeledMenu("scales",optionControl,&scalesLabel,&scalesButton,&scalesMenu);
  }
  if (scaleBaseMenu) {
    XtDestroyWidget(scaleBaseMenu);
    scaleBaseMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass,scaleBaseButton,NULL,(Cardinal)0);
  } else {
    options_createLabeledMenu("scaleBase",optionControl,&scaleBaseLabel,&scaleBaseButton,&scaleBaseMenu);
  }
  for (i = 0; gv_scales[i]; i++) {
    change=False;
    if (gv_scales[i]->is_base) {
      p = scaleBaseMenu;
      if (gv_scale_base_current==bn) change=True;
      sn++;
      bn++;
    } else {
      p = scalesMenu;
      if (gv_scale_current==sn) change=True;
      sn++;
    }
    w = XtCreateManagedWidget(gv_scales[i]->name,smeBSBObjectClass,p,NULL,(Cardinal)0);
    XtAddCallback(w, XtNcallback,options_cb_changeMenuLabel,NULL); 
    if (change) options_cb_changeMenuLabel(w,NULL,NULL);
  }
  ENDMESSAGE(options_gv_createScaleMenus)
}

/*######################################################
   options_gv_createMediaMenus
######################################################*/

void options_gv_createMediaMenus()
{
  int i;
  Widget w;
  char *s;
  Arg args[1];
  Cardinal n;

  BEGINMESSAGE(options_gv_createMediaMenus)
  if (!popup) {
    INFMESSAGE(options not created)
    ENDMESSAGE(options_gv_createMediaMenus)
    return;
  }
  if (mediaMenu) {
    XtDestroyWidget(mediaMenu);
    mediaMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass,mediaButton,NULL,(Cardinal)0);
  } else {
    options_createLabeledMenu("media",optionControl,&mediaLabel,&mediaButton,&mediaMenu);
  }
  if (fmediaMenu) {
    XtDestroyWidget(fmediaMenu);
    fmediaMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass,fmediaButton,NULL,(Cardinal)0);
  } else {
    options_createLabeledMenu("fmedia",optionControl,&fmediaLabel,&fmediaButton,&fmediaMenu);
  }
  w = XtCreateManagedWidget("Automatic",smeBSBObjectClass,mediaMenu,NULL,(Cardinal)0);
  XtAddCallback(w, XtNcallback,options_cb_changeMenuLabel,NULL); 
  for (i = 0; gv_medias[i]; i++) {
    if (gv_medias[i]->used) {
      w = XtCreateManagedWidget(gv_medias[i]->name,smeBSBObjectClass, mediaMenu,NULL,(Cardinal)0);
      XtAddCallback(w, XtNcallback,options_cb_changeMenuLabel,NULL); 
      if (i>0) { /* skip the bounding box */
	w = XtCreateManagedWidget(gv_medias[i]->name,smeBSBObjectClass, fmediaMenu,NULL,(Cardinal)0);
	XtAddCallback(w, XtNcallback,options_cb_changeMenuLabel,NULL); 
      }
    }
  }

  s=NULL;
  for (i = 0; gv_medias[i]; i++) {
    if (gv_medias[i]->used && !strcasecmp(app_res.default_pagemedia,gv_medias[i]->name))
      s = gv_medias[i]->name;
  }
  if (!s) s = "Automatic";
                                   n=0;
  XtSetArg(args[n], XtNlabel, s);  n++;
  XtSetValues(mediaButton, args, n);
  opt_pagemedia = doc_convStringToPageMedia(NULL,s);

  s=NULL;
  for (i = 0; gv_medias[i]; i++) {
    if (gv_medias[i]->used && !strcasecmp(app_res.fallback_pagemedia,gv_medias[i]->name))
      s = gv_medias[i]->name;
  }
  if (!s) {
    i=1; while (!gv_medias[i]->used) i++;
    s = gv_medias[1]->name;
  }
                                   n=0;
  XtSetArg(args[n], XtNlabel, s);  n++;
  XtSetValues(fmediaButton, args, n);

  ENDMESSAGE(options_gv_createMediaMenus)
}

/*------------------------------------------------------
   options_gv_create
------------------------------------------------------*/

void options_gv_create()
{
   Arg          args[10];
   Cardinal     n;
   Widget       w;
   int i;

   BEGINMESSAGE(options_gv_create)

   if (popup) {
     INFMESSAGE(popup exists)
     ENDMESSAGE(options_gv_create)
     return;
   }

        						n=0;
        XtSetArg(args[n], XtNallowShellResize, True);	n++;
   popup = XtCreatePopupShell("optiongvPopup",transientShellWidgetClass,toplevel, args, n);
   gv_options_gv->popup = popup;

         						n=0;
   optionControl = XtCreateManagedWidget("optionControl",aaaWidgetClass,popup,args,n);

   antialiasToggle        = XtCreateManagedWidget("antialias",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   autoResizeToggle       = XtCreateManagedWidget("autoResize",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   swapLandscapeToggle    = XtCreateManagedWidget("swapLandscape",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   dscToggle              = XtCreateManagedWidget("respectDSC",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   eofToggle              = XtCreateManagedWidget("eof",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   watchToggle            = XtCreateManagedWidget("watch",switchWidgetClass,optionControl,NULL,(Cardinal)0);

   options_gv_createScaleMenus();
   options_gv_createMediaMenus();
   options_createLabeledMenu("orient",optionControl,&orientLabel,&orientButton,&orientMenu);
     w = XtCreateManagedWidget("Automatic",smeBSBObjectClass,orientMenu,args,n);
     XtAddCallback(w, XtNcallback,options_cb_changeMenuLabel,NULL); 
   options_createLabeledMenu("forient",optionControl,&forientLabel,&forientButton,&forientMenu);
     for (i = 0; orientations[i]; i++) {
       w = XtCreateManagedWidget(orientations[i],smeBSBObjectClass, orientMenu,NULL,(Cardinal)0);
       XtAddCallback(w, XtNcallback,options_cb_changeMenuLabel,NULL); 
       w = XtCreateManagedWidget(orientations[i],smeBSBObjectClass, forientMenu,NULL,(Cardinal)0);
       XtAddCallback(w, XtNcallback,options_cb_changeMenuLabel,NULL); 
     }
        						n=0;
   w = XtCreateManagedWidget("apply", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_gv_cb_apply,NULL); 
   w = XtCreateManagedWidget("save", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_gv_cb_save,NULL);
   w = XtCreateManagedWidget("dismiss", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_cb_popdown, (XtPointer)gv_options_gv);

   options_gv_setOptionsAtEntry();
   options_realize(popup,optionControl);
                                
   ENDMESSAGE(options_gv_create)
}

