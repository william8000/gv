/*
**
** Vlist.c
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

#include <stdio.h>
#include <stdlib.h>
/*
#define MESSAGES
*/
#include "message.h"

#include "paths.h"
#include INC_X11(IntrinsicP.h)
#include INC_X11(StringDefs.h)
#include INC_XMU(Misc.h)
#include INC_XAW(XawInit.h)
#include "VlistP.h"
#include "Frame.h"
#include INC_XMU(Converters.h)

#include "d_memdebug.h"

#define VLIST_MARK_LEFT_INDENT  4
#define VLIST_MARK_LABEL_INTERNAL_WIDTH 11
#define VLIST_MARK_LABEL_INDENT 4
#define VLIST_MARK_VERTICAL_INDENT 3
#define VLIST_MARK_WIDTH 5

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char defaultTranslations[] = "";

#define offset(field) XtOffsetOf(VlistRec, field)
static XtResource resources[] = { 
   {XtNselectedShadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
	offset(vlist.selected_shadow_width), XtRImmediate, (XtPointer) 1},
   {XtNmarkShadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
	offset(vlist.mark_shadow_width), XtRImmediate, (XtPointer) 1},
   {XtNhighlightedShadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
	offset(vlist.highlighted_shadow_width), XtRImmediate, (XtPointer) 1},
   {XtNhighlightedGeometry, XtCHighlightedGeometry, XtRString, sizeof(String),
      offset(vlist.highlighted_geometry), XtRImmediate, (XtPointer)"2 0 -2 1"},
   {XtNselectedGeometry, XtCSelectedGeometry, XtRString, sizeof(String),
      offset(vlist.selected_geometry), XtRImmediate, (XtPointer)"12 0 -2 1"},
   {XtNvlist,  XtCVlist, XtRString, sizeof(String),
      offset(vlist.vlist), XtRString, NULL},
   {XtNmarkBackground, XtCMarkBackground, XtRPixel, sizeof(Pixel),
	offset(vlist.mark_background), XtRString, XtDefaultBackground},
   {XtNselectedBackground, XtCSelectedBackground, XtRPixel, sizeof(Pixel),
	offset(vlist.selected_background), XtRString, XtDefaultBackground},
   {XtNhighlightedBackground, XtCHighlightedBackground, XtRPixel, sizeof(Pixel),
	offset(vlist.highlighted_background), XtRString, XtDefaultBackground},
   {XtNshadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
      offset(threeD.shadow_width), XtRImmediate, (XtPointer) 2},
   {XtNallowMarks, XtCAllowMarks, XtRBoolean, sizeof(Boolean),
      offset(vlist.allow_marks), XtRImmediate, (XtPointer)True},
   {XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
	offset(label.internal_width), XtRImmediate, (XtPointer)VLIST_MARK_LABEL_INTERNAL_WIDTH},
   {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      XtOffsetOf(RectObjRec,rectangle.border_width), XtRImmediate,
      (XtPointer)0}
};
#undef offset

static Boolean SetValues();
static void Initialize(), Redisplay();
static void Destroy(), PaintVlistWidget();
static void ClassInitialize();
static void Resize();
static void PaintMarkOfEntry();
static void PaintMarkMarkOfEntry();


#define SuperClass ((LabelWidgetClass)&labelClassRec)

VlistClassRec vlistClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "Vlist",				/* class_name		  */
    sizeof(VlistRec),			/* size			  */
    ClassInitialize,			/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    NULL,				/* actions		  */
    0,					/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    TRUE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    Resize,				/* resize		  */
    Redisplay,				/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    defaultTranslations,		/* tm_table		  */
    XtInheritQueryGeometry,		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },  /* CoreClass fields initialization */
  {
    XtInheritChangeSensitive		/* change_sensitive	*/
  },  /* SimpleClass fields initialization */
  {
    XtInheritXaw3dShadowDraw,           /* shadowdraw           */
  },  /* ThreeD Class fields initialization */
  {
    0,                                     /* field not used    */
  },  /* LabelClass fields initialization */
  {
    0,                                     /* field not used    */
  },  /* VlistClass fields initialization */
};

  /* for public consumption */
WidgetClass vlistWidgetClass = (WidgetClass) &vlistClassRec;

/*---------------------------------------------------*/
/* ClassInitialize */
/*---------------------------------------------------*/

static void ClassInitialize()
{
  BEGINMESSAGE(ClassInitialize)
  XawInitializeWidgetSet();
  ENDMESSAGE(ClassInitialize)
}

/*---------------------------------------------------*/
/* Initialize */
/*---------------------------------------------------*/

static void 
Initialize(request, new, args, num_args)
Widget request, new;
ArgList args;			/* unused */
Cardinal *num_args;		/* unused */
{
  VlistWidget vw = (VlistWidget) new;
  String s="";
  char *c;
  XGCValues values;

  BEGINMESSAGE(Initialize)
  if (vw->vlist.vlist) s = vw->vlist.vlist;
  vw->vlist.vlist = GV_XtNewString(s);
  c = vw->vlist.vlist;
  vw->vlist.selected = -1;
  vw->vlist.highlighted = -1;
  vw->vlist.entries = (int)strlen(vw->vlist.vlist);
  sscanf(vw->vlist.highlighted_geometry,"%d %d %d %d",
        &(vw->vlist.hulx),&(vw->vlist.huly),&(vw->vlist.hlrx),&(vw->vlist.hlry));
  sscanf(vw->vlist.selected_geometry,"%d %d %d %d",
        &(vw->vlist.sulx),&(vw->vlist.suly),&(vw->vlist.slrx),&(vw->vlist.slry));
  /* mark background GC */
  values.foreground	= vw->vlist.mark_background;
  vw->vlist.mark_background_GC = XtGetGC((Widget)vw,(unsigned) GCForeground,&values);
  /* selected background GC */
  values.foreground = vw->vlist.selected_background;
  vw->vlist.selected_background_GC = XtGetGC((Widget)vw,(unsigned) GCForeground,&values);
  /* highlighted background GC */
  values.foreground = vw->vlist.highlighted_background;
  vw->vlist.highlighted_background_GC = XtGetGC((Widget)vw,(unsigned) GCForeground,&values);
  /* background GC */
  values.foreground	= vw->core.background_pixel;
  values.graphics_exposures = False;
  vw->vlist.background_GC = XtGetGC((Widget)vw,(unsigned) GCForeground | GCGraphicsExposures,&values);

  ENDMESSAGE(Initialize)
}

/*---------------------------------------------------*/
/* Redisplay */
/*---------------------------------------------------*/

static void 
Redisplay(w, event, region)
Widget w;
XEvent *event;
Region region;
{
  BEGINMESSAGE(Redisplay)
  PaintVlistWidget(w, event, region);
  ENDMESSAGE(Redisplay)
}

/*---------------------------------------------------*/
/* Resize */
/*---------------------------------------------------*/

static void shiftLabel(vw)
  VlistWidget vw;
{
  BEGINMESSAGE(shiftLabel)
  if (vw->vlist.allow_marks) {
    vw->label.label_x = vw->label.internal_width +
                        vw->threeD.shadow_width  +
                        VLIST_MARK_LABEL_INDENT;
  }
  ENDMESSAGE(shiftLabel)
}

static void Resize(w)
  Widget w;
{
  VlistWidget vw = (VlistWidget) w;
  BEGINMESSAGE(Resize)
  (*vlistWidgetClass->core_class.superclass->core_class.resize)(w);
  shiftLabel(vw);
  ENDMESSAGE(Resize)
}

/*---------------------------------------------------*/
/* SetValues */
/*---------------------------------------------------*/

static Boolean 
SetValues (current, request, new, args, num_args)
Widget current, request, new;
ArgList args;
Cardinal *num_args;
{
  VlistWidget cvw = (VlistWidget) current;
  VlistWidget nvw = (VlistWidget) new;
  Boolean changed = False;

  BEGINMESSAGE(SetValues)
  if (nvw->vlist.vlist == NULL) nvw->vlist.vlist = "";
  if (cvw->vlist.vlist != nvw->vlist.vlist) {
    GV_XtFree((char *)cvw->vlist.vlist);
    nvw->vlist.vlist = GV_XtNewString(nvw->vlist.vlist);
    nvw->vlist.entries = (int)strlen(nvw->vlist.vlist);
    if (!nvw->vlist.entries) nvw->vlist.entries =- 1;
    SMESSAGE(nvw->vlist.vlist)
    IMESSAGE(nvw->vlist.entries)
    changed = True;
  }
  ENDMESSAGE(SetValues)
  return (changed);
}

/*---------------------------------------------------*/
/* Destroy */
/*---------------------------------------------------*/

static void 
Destroy(w)
Widget w;
{
  VlistWidget vw = (VlistWidget)w;

  BEGINMESSAGE(Destroy)
  GV_XtFree(vw->vlist.vlist);
  XtReleaseGC(w,vw->vlist.background_GC);
  XtReleaseGC(w,vw->vlist.mark_background_GC);
  XtReleaseGC(w,vw->vlist.selected_background_GC);
  XtReleaseGC(w,vw->vlist.highlighted_background_GC);
  ENDMESSAGE(Destroy)
}

/*---------------------------------------------------*/
/* PaintEntryString */
/*---------------------------------------------------*/

static void
PaintEntryString(w, entry)
  Widget w;
  int entry;
{
  VlistWidget vw = (VlistWidget)w;
  char * s;
  int i;

  BEGINMESSAGE1(PaintEntryString)
  s = vw->label.label;
  i = entry;
  if (s) while (i > 0 && (s = strchr(s,'\n'))) { s++; i--; }
  if (s) {
    char *nl = strchr(s,'\n');
    if (nl) *nl = '\0';
    XDrawString(XtDisplay(w), XtWindow(w), vw->label.normal_GC,
		vw->label.label_x, 
		vw->label.label_y+entry*(vw->label.font->max_bounds.ascent +
					 vw->label.font->max_bounds.descent) +
		                         vw->label.font->max_bounds.ascent,
		s, (int)strlen(s));
    if (nl) *nl = '\n';
  }
  ENDMESSAGE1(PaintEntryString)
}

/*---------------------------------------------------*/
/* PaintMarkOfEntry */
/*---------------------------------------------------*/

static int
PaintMark(w, region, entry, style,erase)
  Widget w;
  Region region;
  int entry;
  int style;
  Boolean erase;
{
  VlistWidget vw = (VlistWidget)w;
  int ss,ulx,uly,lrx,lry,ret=0;
  Position x,y;
  Dimension width,height;
  Dimension sw;
  GC gc;

  BEGINMESSAGE(PaintMark)
  if (entry < 0 || entry >= vw->vlist.entries) {
    INFMESSAGE(invalid entry)
    ENDMESSAGE(PaintMark)
    return(ret);
  }
  if (style<0) { /* highlighted */
    INFMESSAGE(highlighted entry)
    ulx = vw->vlist.hulx;
    uly = vw->vlist.huly;
    lrx = vw->vlist.hlrx;
    lry = vw->vlist.hlry;
    sw  = vw->vlist.highlighted_shadow_width;
    gc  = vw->vlist.highlighted_background_GC;
  } else { /* selected */
    INFMESSAGE(selected entry)
    ulx = vw->vlist.sulx;
    uly = vw->vlist.suly;
    lrx = vw->vlist.slrx;
    lry = vw->vlist.slry;
    sw  = vw->vlist.selected_shadow_width;
    gc  = vw->vlist.selected_background_GC;
  }
  x = (Position) ulx;
  y = (Position) (((int) vw->label.label_y) +
		  (entry*(vw->label.label_height))/vw->vlist.entries +
		  uly);
  width = (Dimension)((int) vw->core.width - ulx + lrx);
  height= (Dimension)(((int) vw->label.label_height)/vw->vlist.entries +
		      - uly + lry + .5);
  ss = XawSUNKEN;
  if (region == NULL || XRectInRegion(region,x,y,width,height) != RectangleOut) {
    if (erase) {
      INFMESSAGE(clearing entry)
      XFillRectangle(XtDisplayOfObject(w), XtWindowOfObject(w),vw->vlist.background_GC,x,y,width,height);
    } else {
      INFMESSAGE(drawing entry)
      XFillRectangle(XtDisplayOfObject(w), XtWindowOfObject(w),gc,x,y,width,height);
      FrameDrawFrame(w,x,y,width,height,ss,sw,
		vw->threeD.top_shadow_GC,
		vw->threeD.bot_shadow_GC);
    }
    ret=1;
  }
  ENDMESSAGE(PaintMark)
  return ret;
}

static void
PaintMarkOfEntry(w, region, entry, style,erase)
  Widget w;
  Region region;
  int entry;
  int style;
  Boolean erase;
{
  VlistWidget vw = (VlistWidget)w;
  int pstyle,dstyle,d,p;
  int drawn=0,pdrawn=0;
  int order=0;

  BEGINMESSAGE(PaintMarkOfEntry)
  if (entry < 0 || entry >= vw->vlist.entries) {
    INFMESSAGE(invalid entry)
    ENDMESSAGE(PaintMarkOfEntry)
    return;
  }
  d = entry; dstyle=style;
  if (style<0) { p=vw->vlist.selected; pstyle=1; }
  else         { p=vw->vlist.highlighted; pstyle=-1; }
  if (d+1 == p)      order = 10;    
  else if (d-1 == p) { if (erase) order=10; else order=01; }
  else if (d==p)     { if (dstyle<0) order=10; else order=01; }
  else order = 999;
  IIMESSAGE(d,p)
  IMESSAGE(order)
  if (order==10 || order == 999) {
    drawn = PaintMark(w, region, d, dstyle,erase);
    if (order==10) pdrawn = PaintMark(w, region, p, pstyle,False);
  } else if (order==01) {
    pdrawn = PaintMark(w, region, p, pstyle,False);
    drawn = PaintMark(w, region, d, dstyle,erase);
  }
  if (pdrawn) {
    PaintEntryString(w,p);
    PaintMarkMarkOfEntry(w,region,p,False);
  }
  if (drawn && p!=d) {
    PaintEntryString(w,d);
    PaintMarkMarkOfEntry(w,region,d,False);
  }
  ENDMESSAGE(PaintMarkOfEntry)
}

/*---------------------------------------------------*/
/* PaintMarkMarkOfEntry */
/*---------------------------------------------------*/

static void 
PaintMarkMarkOfEntry(w, region, entry, erase)
  Widget w;
  Region region;
  int entry;
  Boolean erase;
{
  VlistWidget vw = (VlistWidget)w;

  BEGINMESSAGE1(PaintMarkMarkOfEntry)
  if (vw->vlist.allow_marks) {
    int ss;
    Position x,y;
    Dimension width,height;
    char *vlist = vw->vlist.vlist;
    Boolean paint=False;

    if (vlist[entry] == '*') paint = True;
    if (paint || erase) {
      x = (Position) (VLIST_MARK_LEFT_INDENT);
      y = (Position) (((int) vw->label.label_y) +
		      VLIST_MARK_VERTICAL_INDENT +
		      (entry*(vw->label.label_height))/vw->vlist.entries);
      width = (Dimension) (VLIST_MARK_WIDTH);
      height= (Dimension)(((int) vw->label.label_height)/vw->vlist.entries + 0.5 - 2*VLIST_MARK_VERTICAL_INDENT);
      ss = XawSUNKEN;
      if (region == NULL || XRectInRegion(region,x,y,width,height) != RectangleOut) {
	if (paint) {
	  INFMESSAGE(drawing mark)
	    XFillRectangle(XtDisplayOfObject(w), XtWindowOfObject(w),vw->vlist.mark_background_GC,x,y,width,height);
	  FrameDrawFrame(w,x,y,width,height,ss,vw->vlist.mark_shadow_width,
			 vw->threeD.top_shadow_GC,
			 vw->threeD.bot_shadow_GC);
	} else {
	  INFMESSAGE(clearing mark)
	    XFillRectangle(XtDisplayOfObject(w), XtWindowOfObject(w),
			   (entry == vw->vlist.highlighted) ? vw->vlist.highlighted_background_GC : vw->vlist.background_GC,
			   x,y,width,height);
	}
      }
    }
  }
  ENDMESSAGE1(PaintMarkMarkOfEntry)
}

/*---------------------------------------------------*/
/* PaintMarksOfEntries */
/*---------------------------------------------------*/

static void 
PaintMarksOfEntries(w, event, region)
Widget w;
XEvent *event;
Region region;
{
  VlistWidget vw = (VlistWidget)w;

  BEGINMESSAGE(PaintMarksOfEntries)
  PaintMarkOfEntry(w, region, vw->vlist.highlighted,-1, False);
  if (vw->vlist.allow_marks) {
    int i=0;
    while (i < vw->vlist.entries) {
      if (i != vw->vlist.highlighted)
	PaintMarkMarkOfEntry(w, region, i, False);
      i++;
    }
  }
  if (vw->vlist.selected != vw->vlist.highlighted && vw->vlist.entries >= 0)
    PaintMarkOfEntry(w, region, vw->vlist.selected,1, False);
  ENDMESSAGE(PaintMarksOfEntries)
}

/*---------------------------------------------------*/
/* PaintVlistWidget */
/*---------------------------------------------------*/

static void 
PaintVlistWidget(w, event, region)
Widget w;
XEvent *event;
Region region;
{
  Dimension width;
  VlistWidget vw = (VlistWidget)w;
  BEGINMESSAGE(PaintVlistWidget)
  shiftLabel(vw);
  PaintMarksOfEntries(w, event, region);
  width = vw->threeD.shadow_width;
  vw->threeD.shadow_width = 0;
  (*SuperClass->core_class.expose) (w, event, region);
  vw->threeD.shadow_width =  width; 
  ENDMESSAGE(PaintVlistWidget)
}

/*####################################################################*/
/*####################################################################*/
/* Public Routines */
/*####################################################################*/
/*####################################################################*/

/*###################################################*/
/* VlistSelected */
/*###################################################*/

int
VlistSelected(w)
  Widget w;
{
  VlistWidget vw = (VlistWidget)w;

  BEGINMESSAGE(VlistSelected)
  ENDMESSAGE(VlistSelected)
  return(vw->vlist.selected);
}

/*###################################################*/
/* VlistHighlighted */
/*###################################################*/

int
VlistHighlighted(w)
  Widget w;
{
  VlistWidget vw = (VlistWidget)w;

  BEGINMESSAGE(VlistHighlighted)
  ENDMESSAGE(VlistHighlighted)
  return(vw->vlist.highlighted);
}

/*###################################################*/
/* VlistEntries */
/*###################################################*/

int
VlistEntries(w)
  Widget w;
{
  VlistWidget vw = (VlistWidget)w;

  BEGINMESSAGE(VlistEntries)
  ENDMESSAGE(VlistEntries)
  return(vw->vlist.entries);
}

/*###################################################*/
/* VlistVlist */
/*###################################################*/

char*
VlistVlist(w)
  Widget w;
{
  VlistWidget vw = (VlistWidget)w;

  BEGINMESSAGE(VlistVlist)
  ENDMESSAGE(VlistVlist)
  return(vw->vlist.vlist);
}

/*###################################################*/
/* VlistEntryOfPosition */
/*###################################################*/

int
VlistEntryOfPosition(w,y)
  Widget w;
  int y;
{
  VlistWidget vw = (VlistWidget)w;
  int entry = -1;

  BEGINMESSAGE(VlistEntryOfPosition)
  y = y - (int) vw->label.label_y;
  if (vw->label.label_height > 0) {
    if (y < 0) entry = -1;
    else       entry = (vw->vlist.entries*y)/(int)vw->label.label_height;
  }
  IMESSAGE(entry)
  ENDMESSAGE(VlistEntryOfPosition)
  return(entry);
}

/*###################################################*/
/* VlistPositionOfEntry */
/*###################################################*/

void
VlistPositionOfEntry(w,e,yuP,ylP)
  Widget w;
  int e;
  int *yuP;
  int *ylP;
{
  VlistWidget vw = (VlistWidget)w;
  float h;
  BEGINMESSAGE(VlistPositionOfEntry)
  *yuP = *ylP = (int) vw->label.label_y;
  if (e>=0 && vw->vlist.entries > 0) {
    h = (float)vw->label.label_height/(float)vw->vlist.entries;
    *yuP += (int)((float)e*h);
    *ylP += (int)((float)(e+1)*h);
  }
  ENDMESSAGE(VlistPositionOfEntry)
}

/*###################################################*/
/* VlistChangeMark */
/*###################################################*/

void
VlistChangeMark(w,entry,change)
  Widget w;
  int entry;
  int change;
{
  VlistWidget vw = (VlistWidget)w;
  char *vlist=vw->vlist.vlist;
  Boolean paint;
  int lb,ub,st;

  BEGINMESSAGE(VlistChangeMark)
  switch (entry) {
    case XawVlistAll:
      lb=0;
      ub=vw->vlist.entries-1;
      st=1;
      break;
    case XawVlistEven:
      lb=1;
      ub=vw->vlist.entries-1;
      st=2;
      break;
    case XawVlistOdd:
      lb=0;
      ub=vw->vlist.entries-1;
      st=2;
      break;
    case XawVlistCurrent:
      lb=vw->vlist.selected;
      ub=vw->vlist.selected;
      st=1;
      break;
    default:
      lb=entry;
      ub=entry;
      st=1;
      break;
  }

  if (ub < 0 || ub >= vw->vlist.entries) {
    INFMESSAGE(invalid setup)
    ENDMESSAGE(VlistChangeMark)
    return;
  }

  entry=lb;
  while (entry <= ub) {
    paint=False;
    switch (change) {
      case XawVlistToggle:
	switch (vlist[entry]) {
          case '*':
	    vlist[entry]=' ';
	    paint=True;
	    break;
          default:
	    vlist[entry]='*';
	    paint=True;
	    break;
        }
	break;
      case XawVlistSet:
	vlist[entry]='*';
	paint=True;
	break;
      case XawVlistUnset:
	vlist[entry]=' ';
	paint=True;
	break;
      default:
	INFMESSAGE(invalid action)
	break;
    }
    if (paint) PaintMarkMarkOfEntry(w,NULL,entry,True);
    entry += st;
  }
  ENDMESSAGE(VlistChangeMark)
}

/*---------------------------------------------------*/
/* vlist_change_mark */
/*---------------------------------------------------*/

static void 
vlist_change_mark(w,e,change,kind)
  Widget w;
  int e;
  int change;
  int kind;
{
  VlistWidget vw = (VlistWidget)w;
  int *eP;

  BEGINMESSAGE(vlist_change_mark)
  if (kind < 0) eP = &(vw->vlist.highlighted);
  else eP = &(vw->vlist.selected);
  switch (change) {
    case XawVlistSet:
      if (*eP >= 0 && *eP < vw->vlist.entries) PaintMarkOfEntry(w,NULL,*eP,kind,True);
      *eP = -1;
      if (e >= 0 && e < vw->vlist.entries) {
	PaintMarkOfEntry(w,NULL,e,kind,False);
	*eP = e;
      }
      break;
    case XawVlistUnset:
      if (*eP >= 0 && *eP < vw->vlist.entries) PaintMarkOfEntry(w,NULL,*eP,kind,True);
      *eP = -1;
      break;
    default:
      INFMESSAGE(invalid action)
      break;
  }
  ENDMESSAGE(vlist_change_mark)
}

/*###################################################*/
/* VlistChangeSelected */
/*###################################################*/

void 
VlistChangeSelected(w,entry,change)
  Widget w;
  int entry;
  int change;
{
  BEGINMESSAGE(VlistChangeSelected)
  vlist_change_mark(w,entry,change,1);
  ENDMESSAGE(VlistChangeSelected)
}

/*###################################################*/
/* VlistChangeHighlighted */
/*###################################################*/

void 
VlistChangeHighlighted(w,entry,change)
  Widget w;
  int entry;
  int change;
{
  BEGINMESSAGE(VlistChangeHighlighted)
  vlist_change_mark(w,entry,change,-1);
  ENDMESSAGE(VlistChangeHighlighted)
}
