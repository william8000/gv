/*
 * Ghostview.c -- Ghostview widget.
 * Copyright (C) 1992  Timothy O. Theisen
 * Copyright (C) 2004  Jose E. Marchesi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU gv; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *   Author: Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
*/

/*
 * this code was modified by Johannes Plass,
 * plass@thep.physik.uni-mainz.de
 * February 1995, March 1996 ###jp###
 *
*/


/*
#define MESSAGES
*/
#include "message.h"

#include "config.h"

#include "paths.h"
#include INC_X11(IntrinsicP.h)
#include INC_X11(StringDefs.h)
#include INC_X11(Xatom.h)
#include INC_X11(Xproto.h)
#include INC_X11(Xos.h)
#include INC_X11(Xfuncs.h)
#include "GhostviewP.h"

#ifdef VMS
#   include <ssdef.h>
#   include<lib$routines.h>
#   include<starlet.h>
#else
#   include <sys/types.h>
#   include <unistd.h>
#   include <sys/wait.h>
#endif

#include <ctype.h>
#include <stdlib.h>
#include <signal.h>

#include "types.h"
#include "d_memdebug.h"
#include "main_resources.h"
#include "main_globals.h"

#ifndef XlibSpecificationRelease
typedef char *XPointer;
#endif

#include <gv_signal.h>
#ifdef SIGNALRETURNSINT
#define SIGVAL int
#else
#define SIGVAL void
#endif

#ifdef NON_BLOCKING_IO
#include <fcntl.h>
/* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined(O_NONBLOCK) && defined(O_NDELAY)
#define O_NONBLOCK O_NDELAY
#endif
#endif

#include <errno.h>
/* BSD 4.3 errno.h does not declare errno */
extern int errno;
/* Both error returns are checked for non-blocking I/O. */
/* Manufacture the other error code if only one exists. */
#if !defined(EWOULDBLOCK) && defined(EAGAIN)
#define EWOULDBLOCK EAGAIN
#endif
#if !defined(EAGAIN) && defined(EWOULDBLOCK)
#define EAGAIN EWOULDBLOCK
#endif

#ifndef VMS
/* GV_BUFSIZ is set to the minimum POSIX PIPE_BUF to ensure that
 * nonblocking writes to ghostscript will work properly.
 */
#define GV_BUFSIZ 512
#else /* VMS */
/*
** GV_BUFSIZ is the maximum length line we can handle, so we up it to 1024
*/
#define GV_BUFSIZ 1024
#endif /* VMS */

#define ALLOW_PDF /*###jp### 02/29/96 */

#define CURSOR_RESET   1
#define CURSOR_BUSY    2
#define CURSOR_SCROLL  3
#define CURSOR_NORMAL  4

static XtResource resources[] = {
#define offset(field) XtOffsetOf(GhostviewRec, ghostview.field)
    { XtNarguments, XtCArguments, XtRString, sizeof(String),
	  offset(arguments), XtRString, (XtPointer)NULL },
    { XtNbottomMargin, XtCMargin, XtRInt, sizeof(int),
	  offset(bottom_margin), XtRImmediate, (XtPointer)0 },
    { XtNbusyCursor, XtCCursor, XtRCursor, sizeof(XtPointer),
	  offset(busy_cursor), XtRString, "watch" },
    { XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	  offset(callback), XtRCallback, (XtPointer)NULL },
    { XtNcursor, XtCCursor, XtRCursor, sizeof(XtPointer),
	  offset(cursor), XtRString, "crosshair" },
    { XtNfilename, XtCFilename, XtRString, sizeof(String),
	  offset(filename), XtRString, (XtPointer)NULL },
    { XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	  offset(foreground), XtRString, XtDefaultForeground},
    { XtNleftMargin, XtCMargin, XtRInt, sizeof(int),
	  offset(left_margin), XtRImmediate, (XtPointer)0 },
    { XtNllx, XtCBoundingBox, XtRInt, sizeof(int),
	  offset(llx), XtRImmediate, (XtPointer)0 },
    { XtNlly, XtCBoundingBox, XtRInt, sizeof(int),
	  offset(lly), XtRImmediate, (XtPointer)0 },
    { XtNmessageCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	  offset(message_callback), XtRCallback, (XtPointer)NULL },
    { XtNorientation, XtCOrientation, XtRPageOrientation,
	  sizeof(XtPageOrientation), offset(orientation), XtRImmediate,
	  (XtPointer)XtPageOrientationPortrait },
    { XtNoutputCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	  offset(output_callback), XtRCallback, (XtPointer)NULL },
    { XtNpalette, XtCPalette, XtRPalette, sizeof(XtPalette),
	  offset(palette), XtRImmediate, (XtPointer)XtPaletteColor },
    { XtNquiet, XtCQuiet, XtRBoolean, sizeof(Boolean),
	  offset(quiet), XtRImmediate, (XtPointer)True },
    { XtNrightMargin, XtCMargin, XtRInt, sizeof(int),
	  offset(right_margin), XtRImmediate, (XtPointer)0 },
    { XtNscrollCursor, XtCCursor, XtRCursor, sizeof(XtPointer),
	  offset(scroll_cursor), XtRString, "plus" },
    { XtNtopMargin, XtCMargin, XtRInt, sizeof(int),
	  offset(top_margin), XtRImmediate, (XtPointer)0 },
    { XtNuseBackingPixmap, XtCUseBackingPixmap, XtRBoolean, sizeof(Boolean),
	  offset(use_bpixmap), XtRImmediate, (XtPointer)False },
    { XtNurx, XtCBoundingBox, XtRInt, sizeof(int),
	  offset(urx), XtRImmediate, (XtPointer)612 },
    { XtNury, XtCBoundingBox, XtRInt, sizeof(int),
	  offset(ury), XtRImmediate, (XtPointer)792 },
    { XtNlxdpi, XtCLResolution, XtRInt, sizeof(long),
	  offset(lxdpi), XtRImmediate, (XtPointer)0 },
    { XtNlydpi, XtCLResolution, XtRInt, sizeof(long),
	  offset(lydpi), XtRImmediate, (XtPointer)0 },
    {XtNpreferredWidth, XtCPreferredWidth, XtRDimension, sizeof(Dimension),
       offset(pref_width), XtRImmediate, (XtPointer)1},
    {XtNpreferredHeight, XtCPreferredHeight, XtRDimension, sizeof(Dimension),
       offset(pref_height), XtRImmediate, (XtPointer)1},
    {XtNsafer, XtCSafer, XtRBoolean, sizeof(Boolean),
          offset(safer), XtRImmediate, (XtPointer)True },
    {XtNinterpreter, XtCInterpreter, XtRString, sizeof(String),
          offset(interpreter), XtRString, "gs" },
#if 0
    {XtNhighlightPixel, XtCHighlightPixel, XtRPixel, sizeof(Pixel),
	offset(highlight_pixel), XtRString, XtDefaultForeground},
#endif
#undef offset
};

static void Message();
static void Notify();
static void action_changeCursor();
static void Input();
static void Output();

static void ClassInitialize();
static void ClassPartInitialize();
static void Initialize();
static void Realize();
static void Redisplay();
static void Destroy();
static void Resize();
static Boolean SetValues();
static XtGeometryResult QueryGeometry();

static void Layout();
static Boolean ComputeSize();
static void SetBackground();
static Boolean Setup();
static void StartInterpreter();
static void StopInterpreter();
static void InterpreterFailed();
static void ChangeCursor();

static XtActionsRec actions[] =
{
    {"message",	Message},
    {"notify",	Notify},
    {"cursor",	action_changeCursor},
};

static char translations[] =
"<Message>:message()\n\
";

GhostviewClassRec ghostviewClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &coreClassRec,
    /* class_name		*/	"Ghostview",
    /* widget_size		*/	sizeof(GhostviewRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* ghostview fields */
    /* ghostview		*/	NULL,
    /* gv_colors		*/	NULL,
    /* next			*/	NULL,
    /* page			*/	NULL,
    /* done			*/	NULL
  }
};


WidgetClass ghostviewWidgetClass = (WidgetClass)&ghostviewClassRec;


/*###################################################################################*/
/* 
 * Double pixmap routines
 */
/*###################################################################################*/

static Pixmap pix = 0;

static void Copy_pixmap(w)
    Widget w;
{
   GhostviewWidget gvw = (GhostviewWidget) w;
   GhostviewWidgetClass gvc = (GhostviewWidgetClass) XtClass(w);

  if(!gvw->ghostview.use_bpixmap){
    if(pix) {
      Display *dpy = XtDisplay(w);
      int scr = DefaultScreen(dpy);
      GC gc = DefaultGC(dpy, scr);
      Window win = XtWindow(w);
      int x, gwidth, gheight;
      Window r;
    
      XGetGeometry(dpy,win, &r,&x,&x,&gwidth, &gheight,&x,&x);
      XCopyArea(dpy, pix, win, gc, 0,0, gwidth, gheight, 0,0);
    }
  }      

}
static void Realize_pixmap(w)
    Widget w;
{
    if(!pix) {
        Display *dpy = XtDisplay(w);
        int scr = DefaultScreen(dpy);
        GC gc = DefaultGC(dpy, scr);
        Window win = XtWindow(w);
        int x, gwidth, gheight;
        Window r;

        XGetGeometry(dpy,win, &r,&x,&x,&gwidth, &gheight,&x,&x);
/*        printf("Realize_pixmap %d %d\n", gwidth, gheight);         */
        pix =  XCreatePixmap(dpy, win, gwidth, gheight, DefaultDepth(dpy,scr));
    } 
}

static void 
Redisplay(w, event, region)
Widget w;
XEvent *event;
Region region;
{

  BEGINMESSAGE(Redisplay)
  Copy_pixmap(w);
  ENDMESSAGE(Redisplay)
}

/*###################################################################################*/
/* Message action routine.
 * Passes ghostscript message events back to application via
 * the message callback.  It also marks the interpreter as
 * being not busy at the end of page, and stops the interpreter
 * when it send a "done" message.
 */
/*###################################################################################*/

static void
Message(w, event, params, num_params)
   Widget w;
   XEvent *event;
   String *params;		/* unused */
   Cardinal *num_params;	/* unused */
{
   GhostviewWidget gvw = (GhostviewWidget) w;
   GhostviewWidgetClass gvc = (GhostviewWidgetClass) XtClass(w);

   BEGINMESSAGE(Message)
   gvw->ghostview.mwin = event->xclient.data.l[0];
   if (event->xclient.message_type== XmuInternAtom(XtDisplay(w),gvc->ghostview_class.page)) {
      INFMESSAGE(interpreter completed page)
      gvw->ghostview.busy = False;
      ChangeCursor(gvw,CURSOR_RESET);
      XtCallCallbackList(w, gvw->ghostview.message_callback, "Page");
   } 
   else if (event->xclient.message_type==XmuInternAtom(XtDisplay(w),gvc->ghostview_class.done)) {
      INFMESSAGE(stopping interpreter NOW)
      StopInterpreter(w);  
      XtCallCallbackList(w, gvw->ghostview.message_callback, "Done");
   }
    
    if(!gvw->ghostview.use_bpixmap){
      if(!pix) {
        StopInterpreter(w);
        Realize_pixmap(w);
        show_page(REQUEST_REDISPLAY,NULL);
      } else {
        Copy_pixmap(w);
      }
    }      
   ENDMESSAGE(Message)
}

static void
action_changeCursor(w, event, params, num_params)
   Widget w;
   XEvent *event;
   String *params;
   Cardinal *num_params;
{
   int which=0;

   BEGINMESSAGE(action_changeCursor)
   if (*num_params == 1) {
      if      (*(params[0]) == 'n' || *(params[0]) == 'N') which = CURSOR_NORMAL;
      else if (*(params[0]) == 's' || *(params[0]) == 'S') which = CURSOR_SCROLL;
      if (which) ChangeCursor((GhostviewWidget)w, which);
   }
   ENDMESSAGE(action_changeCursor)
}

/*----------------------------------------------------------*/
/* ChangeCursor */
/*----------------------------------------------------------*/

static void
ChangeCursor(gvw, which)
   GhostviewWidget gvw;
   int which;
{
   Widget w = (Widget) gvw;
   Cursor cursor;

   BEGINMESSAGE(ChangeCursor)

   cursor = (Cursor) NULL;
   switch (which) {
      case CURSOR_RESET:
           if (!gvw->ghostview.busy) {
              if (gvw->ghostview.cursor_type == CURSOR_SCROLL)
                 cursor = gvw->ghostview.scroll_cursor;
              else if (gvw->ghostview.cursor_type == CURSOR_NORMAL)
                 cursor = gvw->ghostview.cursor;
           }
           break;
      case CURSOR_NORMAL:
           if (!gvw->ghostview.busy) cursor = gvw->ghostview.cursor;
           gvw->ghostview.cursor_type = CURSOR_NORMAL;
           break;
      case CURSOR_SCROLL:
           if (!gvw->ghostview.busy) cursor = gvw->ghostview.scroll_cursor;
           gvw->ghostview.cursor_type = CURSOR_SCROLL;
           break;
      case CURSOR_BUSY:
           cursor = gvw->ghostview.busy_cursor; 
           break;
      default:
           break;
   }
   if (cursor) XDefineCursor(XtDisplay(w), XtWindow(w),cursor);
    
   ENDMESSAGE(ChangeCursor)
}

/*###################################################################################*/
/* Notify action routine.
 * Calculates where the user clicked in the default user coordinate system.
 * Call the callbacks with the point of click.
 */
/*###################################################################################*/

static void
Notify(w, event, params, num_params)
  Widget w;
  XEvent *event;
  String *params;
  Cardinal *num_params;
{
  GhostviewWidget gvw = (GhostviewWidget) w;
  GhostviewReturnStruct ret_val;
  int psx,psy;

  BEGINMESSAGE1(Notify)
  GhostviewCoordsXtoPS(w,event->xbutton.x,event->xbutton.y,&psx,&psy);
#if 0
  {
    int wx,wy;
    GhostviewCoordsPStoX(w,psx,psy,&wx,&wy);
    printf("evx=%d evy=%d\n",event->xbutton.x,event->xbutton.y);
    printf("psx=%d psy=%d\n",psx,psy);
    printf("wx =%d wy =%d\n\n",wx,wy);
  }
#endif
  ret_val.width  = 0;
  ret_val.height = 0;
  ret_val.psx = psx;
  ret_val.psy = psy;
  XtCallCallbackList(w, gvw->ghostview.callback, (XtPointer) &ret_val);
  ENDMESSAGE1(Notify)
}

#ifndef SEEK_SET
#   define SEEK_SET 0
#endif

static Boolean broken_pipe = False;

static SIGVAL
CatchPipe(i)
    int i;
{
    broken_pipe = True;
#ifdef SIGNALRETURNSINT
    return 0;
#endif
}

#ifndef VMS

/*###################################################################################*/
/* Input - Feed data to ghostscript's stdin.
 * Write bytes to ghostscript using non-blocking I/O.
 * Also, pipe signals are caught during writing.  The return
 * values are checked and the appropriate action is taken.  I do
 * this at this low level, because it may not be appropriate for
 * SIGPIPE to be caught for the overall application.
 */
/*###################################################################################*/

static void
Input(client_data, source, id)
    XtPointer client_data;
    int *source;
    XtInputId *id;
{
    Widget w = (Widget) client_data;
    GhostviewWidget gvw = (GhostviewWidget) w;
    int bytes_written;
    SIGVAL (*oldsig)();

    BEGINMESSAGE(Input)

    oldsig = signal(SIGPIPE, CatchPipe);

#ifdef NON_BLOCKING_IO
    do {
#endif

	if (gvw->ghostview.buffer_bytes_left == 0) {

	    /* Get a new section if required */
	    if (gvw->ghostview.ps_input && gvw->ghostview.bytes_left == 0) {
		struct record_list *ps_old = gvw->ghostview.ps_input;
		gvw->ghostview.ps_input = ps_old->next;
		if (ps_old->close) fclose(ps_old->fp);
		GV_XtFree((char *)ps_old);
	    }

	    /* Have to seek at the beginning of each section */
	    if (gvw->ghostview.ps_input &&
		gvw->ghostview.ps_input->seek_needed) {
		if (gvw->ghostview.ps_input->len > 0)
		    fseek(gvw->ghostview.ps_input->fp,
			  gvw->ghostview.ps_input->begin, SEEK_SET);
		gvw->ghostview.ps_input->seek_needed = False;
		gvw->ghostview.bytes_left = gvw->ghostview.ps_input->len;
	    }

	    if (gvw->ghostview.bytes_left > GV_BUFSIZ) {
		gvw->ghostview.buffer_bytes_left =
			fread(gvw->ghostview.input_buffer,
			      sizeof (char), GV_BUFSIZ,
			      gvw->ghostview.ps_input->fp);
	    } else if (gvw->ghostview.bytes_left > 0) {
		gvw->ghostview.buffer_bytes_left =
			fread(gvw->ghostview.input_buffer,
			      sizeof (char), gvw->ghostview.bytes_left,
			      gvw->ghostview.ps_input->fp);
	    } else {
		gvw->ghostview.buffer_bytes_left = 0;
	    }
	    if (gvw->ghostview.bytes_left > 0 &&
		gvw->ghostview.buffer_bytes_left == 0) {
		InterpreterFailed(w);	/* Error occurred */
	    }
	    gvw->ghostview.input_buffer_ptr = gvw->ghostview.input_buffer;
	    gvw->ghostview.bytes_left -= gvw->ghostview.buffer_bytes_left;
	}

	if (gvw->ghostview.buffer_bytes_left > 0) {
	    bytes_written = write(gvw->ghostview.interpreter_input,
				  gvw->ghostview.input_buffer_ptr,
				  gvw->ghostview.buffer_bytes_left);

#if 0
{
 char tmp[5000];
 strncpy(tmp,gvw->ghostview.input_buffer_ptr,gvw->ghostview.buffer_bytes_left);
 tmp[gvw->ghostview.buffer_bytes_left] = '\0';
 strcat(tmp,"###END###\n");
 printf(tmp);
}
#endif
	    if (broken_pipe) {
		broken_pipe = False;
		InterpreterFailed(w);		/* Something bad happened */
	    } else if (bytes_written == -1) {
		if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
		    InterpreterFailed(w);	/* Something bad happened */
		}
	    } else {
	       gvw->ghostview.buffer_bytes_left -= bytes_written;
	       gvw->ghostview.input_buffer_ptr  += bytes_written;
/* 
   hack alert: if the last char of a section is not a <lf> we append a <lf>
               by hand. This we do since gs doesn't seem to finish a page properly if
               the file's records are terminated by <cr>. (###jp### 02.06.96)
*/
               IMESSAGE1(bytes_written)
               if (gvw->ghostview.bytes_left == 0 &&
                   gvw->ghostview.buffer_bytes_left == 0 &&
                   bytes_written > 0 &&
                   *(gvw->ghostview.input_buffer_ptr-1) != '\n') {
                  int b;
                  INFMESSAGE(################## appending artificial LF)
                  b=write(gvw->ghostview.interpreter_input,"\n",1);
                  if (broken_pipe) {
                     broken_pipe = False;
                     InterpreterFailed(w);		/* Something bad happened */
                  } else if (b == -1) {
                     if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
                        InterpreterFailed(w);	/* Something bad happened */
                     }
                  }
               }
	    }
	}
#ifdef NON_BLOCKING_IO
    } while(gvw->ghostview.ps_input &&
	    gvw->ghostview.buffer_bytes_left == 0);
#endif
    signal(SIGPIPE, oldsig);
    if (gvw->ghostview.ps_input == NULL &&
	gvw->ghostview.buffer_bytes_left == 0) {
	if (gvw->ghostview.interpreter_input_id != None) {
	    XtRemoveInput(gvw->ghostview.interpreter_input_id);
	    gvw->ghostview.interpreter_input_id = None;
	}
    }
    ENDMESSAGE(Input)

}

/*###################################################################################*/
/* Output - receive I/O from ghostscript's stdout and stderr.
 * Pass this to the application via the output_callback. */
/*###################################################################################*/

static void
Output(client_data, source, id)
    XtPointer client_data;
    int *source;
    XtInputId *id;
{
    Widget w = (Widget) client_data;
    GhostviewWidget gvw = (GhostviewWidget) w;
    char buf[GV_BUFSIZ+1];
    int bytes = 0;

    BEGINMESSAGE(Output)

    if (*source == gvw->ghostview.interpreter_output) {
	bytes = read(gvw->ghostview.interpreter_output, buf, GV_BUFSIZ);
	if (bytes == 0) { /* EOF occurred */
	    INFMESSAGE(EOF occured in interpreter_output)
	    close(gvw->ghostview.interpreter_output);
	    gvw->ghostview.interpreter_output = -1;
	    XtRemoveInput(gvw->ghostview.interpreter_output_id);
            ENDMESSAGE(Output)
	    return;
	} else if (bytes == -1) {
	    INFMESSAGE(something bad happened in interpreter_output)
	    InterpreterFailed(w);		/* Something bad happened */
            ENDMESSAGE(Output)
	    return;
	}
    } else if (*source == gvw->ghostview.interpreter_error) {
	bytes = read(gvw->ghostview.interpreter_error, buf, GV_BUFSIZ);
	if (bytes == 0) { /* EOF occurred */
	    INFMESSAGE(EOF occurred in interpreter_error)
	    close(gvw->ghostview.interpreter_error);
	    gvw->ghostview.interpreter_error = -1;
	    XtRemoveInput(gvw->ghostview.interpreter_error_id);
            ENDMESSAGE(Output)
	    return;
	} else if (bytes == -1) {
	    INFMESSAGE(something bad happened in interpreter_error)
	    InterpreterFailed(w);		/* Something bad happened */
            ENDMESSAGE(Output)
	    return;
	}
    }
    if (bytes > 0) {
	buf[bytes] = '\0';
	XtCallCallbackList(w, gvw->ghostview.output_callback, (XtPointer) buf);
    }
    ENDMESSAGE(Output)
}

#endif /* VMS */

/*###################################################################################*/
/* Register the type converter required for the PageOrientation. */
/* Register the type converter required for the Palette. */
/* This routine is called exactly once. */
/*###################################################################################*/

static void
ClassInitialize()
{
    BEGINMESSAGE(ClassInitialize)
#ifndef GV_CODE
    XtSetTypeConverter(XtRString, XtRPageOrientation,
		       XmuCvtStringToPageOrientation, NULL, 0,
		       XtCacheAll, NULL);
#endif
    XtSetTypeConverter(XtRString, XtRPalette,
		       XmuCvtStringToPalette, NULL, 0,
		       XtCacheAll, NULL);
    ENDMESSAGE(ClassInitialize)
}

/*###################################################################################*/
/* Get atoms needed to communicate with ghostscript. */
/* This routine is called once per display. */
/*###################################################################################*/

static void
ClassPartInitialize(class)
    WidgetClass class;
{
    GhostviewWidgetClass gvc = (GhostviewWidgetClass)class;
    BEGINMESSAGE(ClassPartInitialize)
    gvc->ghostview_class.ghostview = XmuMakeAtom("GHOSTVIEW");
    gvc->ghostview_class.gv_colors = XmuMakeAtom("GHOSTVIEW_COLORS");
    gvc->ghostview_class.next = XmuMakeAtom("NEXT");
    gvc->ghostview_class.page = XmuMakeAtom("PAGE");
    gvc->ghostview_class.done = XmuMakeAtom("DONE");
    ENDMESSAGE(ClassPartInitialize)
}

/*###################################################################################*/
/* Initialize private state. */
/*###################################################################################*/

static void
Initialize(request, new, args, num_args)
    Widget request, new;
    ArgList args;	/* unused */
    Cardinal *num_args;	/* unused */
{
    XGCValues	values;
    XtGCMask	mask;
    GhostviewWidget ngvw = (GhostviewWidget) new;
    GhostviewWidget rgvw = (GhostviewWidget) request;

    BEGINMESSAGE(Initialize)

    values.foreground = new->core.background_pixel;
    values.background = new->core.background_pixel;
    values.graphics_exposures = False;
    mask = GCGraphicsExposures|GCForeground|GCBackground;
    ngvw->ghostview.gc = XtGetGC(new, mask, &values);
    values.foreground   = new->core.background_pixel;
    values.background   = new->core.background_pixel;
    values.line_width   = 1;
    values.function     = GXxor;
    values.graphics_exposures = False;
    mask=(GCFunction|GCGraphicsExposures|GCBackground|GCForeground|GCLineWidth);
    ngvw->ghostview.highlight_gc = XtGetGC(new,mask,&values); 

    ngvw->ghostview.mwin = None;
    ngvw->ghostview.disable_start = False;
    ngvw->ghostview.interpreter_pid = -1;
    ngvw->ghostview.input_buffer = NULL;
    ngvw->ghostview.bytes_left = 0;
#ifndef VMS
    ngvw->ghostview.input_buffer_ptr = NULL;
    ngvw->ghostview.buffer_bytes_left = 0;
#endif
    ngvw->ghostview.ps_input = NULL;
    ngvw->ghostview.interpreter_input = -1;
    ngvw->ghostview.interpreter_output = -1;
#ifndef VMS
    ngvw->ghostview.interpreter_error = -1;
    ngvw->ghostview.interpreter_input_id = None;
    ngvw->ghostview.interpreter_output_id = None;
    ngvw->ghostview.interpreter_error_id = None;
#else /* VMS */
    memset(ngvw->ghostview.interpreter_input_iosb, 0, 8);
    memset(ngvw->ghostview.interpreter_output_iosb, 0, 8);
    ngvw->ghostview.output_buffer = NULL;
#endif /* VMS */
    ngvw->ghostview.gs_width = 0;
    ngvw->ghostview.gs_height = 0;
    ngvw->ghostview.changed = False;
    ngvw->ghostview.busy = False;
    ngvw->ghostview.cursor_type = CURSOR_NORMAL;
    ngvw->ghostview.background_cleared = 0;
    if (ngvw->ghostview.lxdpi) {
       ngvw->ghostview.xdpi  = (float)ngvw->ghostview.lxdpi/1000.0;
       ngvw->ghostview.lxdpi = 0;
       INFFMESSAGE(new xdpi,ngvw->ghostview.xdpi)
    }
    if (ngvw->ghostview.lydpi) {
       ngvw->ghostview.ydpi  = (float)ngvw->ghostview.lydpi/1000.0;
       ngvw->ghostview.lydpi = 0;
       INFFMESSAGE(new ydpi,ngvw->ghostview.ydpi)
    }

    /* Compute window size */
    Layout(new, (rgvw->core.width == 0), (rgvw->core.height == 0));
    ENDMESSAGE(Initialize)
}

/*###################################################################################*/
/* Create Window and start interpreter if needed */
/*###################################################################################*/

static void
Realize(w, valueMask, attributes)
    Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    GhostviewWidget gvw = (GhostviewWidget) w;

    BEGINMESSAGE(Realize)
    if (gvw->ghostview.cursor != None) {
	attributes->cursor = gvw->ghostview.cursor;
	*valueMask |= CWCursor;
    }

    XtCreateWindow(w,(unsigned int)InputOutput,(Visual *)CopyFromParent,*valueMask,attributes);
/*    Realize_pixmap(w);  */
    IMESSAGE(gvw->core.width) IMESSAGE(gvw->core.height) 

    ENDMESSAGE(Realize)
}

/*###################################################################################*/
/* Destroy routine: kill the interpreter and release the GC */
/*###################################################################################*/

static void
Destroy(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;

    BEGINMESSAGE(Destroy)
    StopInterpreter(w);
    XtReleaseGC(w, gvw->ghostview.gc);
    XtReleaseGC(w, gvw->ghostview.highlight_gc);
    if (gvw->ghostview.input_buffer) GV_XtFree(gvw->ghostview.input_buffer);
#ifdef VMS
    if (gvw->ghostview.output_buffer) GV_XtFree(gvw->ghostview.output_buffer);
#endif /* VMS */
    if (gvw->core.background_pixmap != XtUnspecifiedPixmap)
	XFreePixmap(XtDisplay(w), gvw->core.background_pixmap);
    ENDMESSAGE(Destroy)
}

/*###################################################################################*/
/* SetBackground */
/*###################################################################################*/

static void
SetBackground(w,clear)
    Widget w;
    Bool clear;
{
   GhostviewWidget gvw = (GhostviewWidget) w;

   BEGINMESSAGE(SetBackground)
   if (!XtIsRealized(w)) {
      INFMESSAGE(not realized) ENDMESSAGE(SetBackground)
      return;
   }
   if (clear) {
      if (!gvw->ghostview.background_cleared) {
         if ((gvw->core.background_pixmap != None) &&
            (gvw->core.background_pixmap != XtUnspecifiedPixmap)) {
            INFMESSAGE(filling background pixmap)
	    XFillRectangle(XtDisplay(w),gvw->core.background_pixmap,
		           gvw->ghostview.gc,
		           0,0,gvw->core.width,gvw->core.height);
         }
         INFMESSAGE(clearing window)
         XClearWindow(XtDisplay(w), XtWindow(w));
         gvw->ghostview.background_cleared = 1;
      }
#ifdef MESSAGES
      else { INFMESSAGE(background already cleared) }
#endif
   } else {
      if ((gvw->core.background_pixmap != XtUnspecifiedPixmap)) {
         INFMESSAGE(setting background pixmap)
         XSetWindowBackgroundPixmap(XtDisplay(w),XtWindow(w),gvw->core.background_pixmap);
      }
   }
   ENDMESSAGE(SetBackground)
}

/*###################################################################################*/
/* Resize */
/*###################################################################################*/

static void
Resize(w)
  Widget w;
{
  BEGINMESSAGE(Resize)
  ENDMESSAGE(Resize)
}

/*###################################################################################*/
/* SetValues routine.  Set new private state, based on changed values
 * in the widget.  Always returns False, because redisplay is never needed.
 */
/*###################################################################################*/

static Boolean
SetValues(current, request, new)
    Widget current, request, new;
{
    GhostviewWidget cgvw = (GhostviewWidget) current;
    GhostviewWidget rgvw = (GhostviewWidget) request;
    GhostviewWidget ngvw = (GhostviewWidget) new;
    String cfilename;
    String rfilename;
    String carguments;
    String rarguments;

    BEGINMESSAGE(SetValues)
    cfilename = cgvw->ghostview.filename;
    if (cfilename == NULL) cfilename = "(null)";
    rfilename = rgvw->ghostview.filename;
    if (rfilename == NULL) rfilename = "(null)";
    carguments = cgvw->ghostview.arguments;
    if (carguments == NULL) carguments = "(null)";
    rarguments = rgvw->ghostview.arguments;
    if (rarguments == NULL) rarguments = "(null)";

    if (ngvw->ghostview.lxdpi) {
       ngvw->ghostview.xdpi  = (float)ngvw->ghostview.lxdpi/1000.0;
       ngvw->ghostview.lxdpi = 0;
       INFFMESSAGE(new xdpi,ngvw->ghostview.xdpi)
    }
    if (ngvw->ghostview.lydpi) {
       ngvw->ghostview.ydpi  = (float)ngvw->ghostview.lydpi/1000.0;
       ngvw->ghostview.lydpi = 0;
       INFFMESSAGE(new ydpi,ngvw->ghostview.ydpi)
    }

    if (XtIsRealized(new) && !ngvw->ghostview.busy &&
	(cgvw->ghostview.cursor != ngvw->ghostview.cursor)) {
        ChangeCursor(ngvw,CURSOR_RESET);
    }
    if (XtIsRealized(new) && ngvw->ghostview.busy &&
	(cgvw->ghostview.busy_cursor != ngvw->ghostview.busy_cursor)) {
        ChangeCursor(ngvw,CURSOR_BUSY);
    }
    if (cgvw->core.background_pixel != rgvw->core.background_pixel) {
	XGCValues	values;
	XtGCMask	mask;

	XtReleaseGC(current, cgvw->ghostview.gc);
	values.foreground = new->core.background_pixel;
	values.background = new->core.background_pixel;
	values.graphics_exposures = False;
	mask = GCGraphicsExposures|GCForeground|GCBackground;
	ngvw->ghostview.gc = XtGetGC(new, mask, &values);
    }
    IIMESSAGE(ngvw->core.width,ngvw->core.height)
    if ( ( (cgvw->core.width == ngvw->core.width)		&&
	   (cgvw->core.height == ngvw->core.height)		&&
           (cgvw->core.border_width == ngvw->core.border_width)
         ) &&
         ( (cgvw->core.background_pixel != ngvw->core.background_pixel)		||
	   (cgvw->ghostview.foreground != ngvw->ghostview.foreground)		||
	   (cgvw->ghostview.palette != ngvw->ghostview.palette)			||
	   strcmp(cgvw->ghostview.interpreter, ngvw->ghostview.interpreter)	||
	   strcmp(carguments, rarguments)					||
	   (cgvw->ghostview.quiet != ngvw->ghostview.quiet)			||
	   (cgvw->ghostview.safer != ngvw->ghostview.safer)			||
	   strcmp(cfilename, rfilename)						||
           (cgvw->ghostview.orientation != ngvw->ghostview.orientation)		||
	   (cgvw->ghostview.use_bpixmap != ngvw->ghostview.use_bpixmap)		||  
           (cgvw->ghostview.xdpi != ngvw->ghostview.xdpi)			||
	   (cgvw->ghostview.ydpi != ngvw->ghostview.ydpi)			||
	   (cgvw->ghostview.bottom_margin != ngvw->ghostview.bottom_margin)	||
	   (cgvw->ghostview.left_margin != ngvw->ghostview.left_margin)		||
	   (cgvw->ghostview.right_margin != ngvw->ghostview.right_margin)	||
	   (cgvw->ghostview.top_margin != ngvw->ghostview.top_margin)		||
           (cgvw->ghostview.llx != ngvw->ghostview.llx)				||
	   (cgvw->ghostview.lly != ngvw->ghostview.lly)				||
	   (cgvw->ghostview.urx != ngvw->ghostview.urx)				||
	   (cgvw->ghostview.ury != ngvw->ghostview.ury)
       ) ) {	
          INFMESSAGE(change detected - redisplay unnecessary)
          ngvw->ghostview.changed = True;
          ComputeSize(new, True, True, &(ngvw->ghostview.pref_width), &(ngvw->ghostview.pref_height));
#         ifdef MESSAGES
            {
              Dimension w=ngvw->ghostview.pref_width,h=ngvw->ghostview.pref_height;
              INFIIMESSAGE(preferred size:,w,h)
            }
#         endif
    }

    ENDMESSAGE(SetValues)
    return(False);
}

/*###################################################################################*/
/*	Function Name: QueryGeometry
 *	Description: This tells the parent what size we would like to be
 *                   given certain constraints.
 *	Arguments: w - the widget.
 *                 intended - what the parent intends to do with us.
 *                 requested - what we want to happen.
 */
/*###################################################################################*/

static XtGeometryResult 
QueryGeometry(w, intended, requested)
Widget w;
XtWidgetGeometry *intended, *requested;
{
    Dimension new_width, new_height;
    Boolean change, width_req, height_req;
    
    BEGINMESSAGE(QueryGeometry)
    INFIMESSAGE(received request:,intended->request_mode)

    width_req  = (Boolean) (intended->request_mode & CWWidth);
    height_req = (Boolean) (intended->request_mode & CWHeight);

    if (!width_req && !height_req) {
       INFMESSAGE(not a width or height request) ENDMESSAGE(QueryGeometry) 
       return(XtGeometryYes);
    }    

    if (width_req) new_width = intended->width;
    else new_width = w->core.width;
    if (height_req) new_height = intended->height;
    else new_height = w->core.height;

    requested->request_mode = 0;
    change = ComputeSize(w, !width_req, !height_req, &new_width, &new_height);

    requested->request_mode |= CWWidth;
    requested->width = new_width;
    requested->request_mode |= CWHeight;
    requested->height = new_height;

    if (change) {INFMESSAGE(agree almost) ENDMESSAGE(QueryGeometry) return(XtGeometryAlmost);}
    ENDMESSAGE(QueryGeometry)
    return(XtGeometryYes);
}

/*###################################################################################*/
/* Layout */
/* Layout the widget. */
/*###################################################################################*/

static void
Layout(w, xfree, yfree)
    Widget w;
    Boolean xfree, yfree;
{
    Dimension width = w->core.width;
    Dimension height = w->core.height;

    BEGINMESSAGE(Layout)
    if (ComputeSize(w, xfree, yfree, &width, &height)) {
       XtWidgetGeometry request, reply;
       Boolean changed = False;

       INFMESSAGE(geometry has changed; requesting size change)
       request.request_mode = CWWidth | CWHeight;
       request.width = width;
       request.height = height;
    
       switch ( XtMakeGeometryRequest(w, &request, &reply) ) {
          case XtGeometryYes:
             changed = True;
             break;
          case XtGeometryNo:
             break;
	  case XtGeometryDone:
	     /* should not be reached */
             break;
          case XtGeometryAlmost:
	      ComputeSize(w, (request.height != reply.height),
                             (request.width != reply.width),
                             &(reply.width), &(reply.height));
	      request = reply;
	      switch (XtMakeGeometryRequest(w, &request, &reply) ) {
	         case XtGeometryYes:
	            changed = True;
	            break;
	         case XtGeometryNo:
	            break;
	         case XtGeometryDone:
		    /* should not be reached */
                    break;
	         case XtGeometryAlmost:
	            request = reply;
	            ComputeSize(w, FALSE, FALSE, &(request.width), &(request.height));
	            request.request_mode = CWWidth | CWHeight;
	            XtMakeGeometryRequest(w, &request, &reply);
	            changed = True;
	            break;
	      }
	      break;
       }
    }
    ENDMESSAGE(Layout)
}

/*###################################################################################*/
/* ComputeSize */
/* Compute new size of window, sets xdpi and ydpi if necessary.
 * returns True if new window size is different */
/*###################################################################################*/


static Boolean SetProperty(Widget w, Pixmap bpixmap);

static Boolean
ComputeSize(w, xfree, yfree, width, height)
  Widget w;
  Boolean xfree, yfree;	/* Am I allowed to change width or height */
  Dimension *width, *height;
{
  GhostviewWidget gvw = (GhostviewWidget) w;
  Dimension new_width = *width;
  Dimension new_height = *height;
  float newxdpi=72, newydpi=72,xdpi,ydpi;
  Boolean change;
  int dpsw,dpsh;

  BEGINMESSAGE(ComputeSize)

  switch (gvw->ghostview.orientation) {
    case XtPageOrientationLandscape:
    case XtPageOrientationSeascape:
      dpsw = gvw->ghostview.ury-gvw->ghostview.lly+1;
      dpsh = gvw->ghostview.urx-gvw->ghostview.llx+1;
      xdpi = gvw->ghostview.ydpi;
      ydpi = gvw->ghostview.xdpi;
      break;
    case XtPageOrientationPortrait:
    case XtPageOrientationUpsideDown:
    default:
      dpsw = gvw->ghostview.urx-gvw->ghostview.llx+1;
      dpsh = gvw->ghostview.ury-gvw->ghostview.lly+1;
      xdpi = gvw->ghostview.xdpi;
      ydpi = gvw->ghostview.ydpi;
      break;
  }
  if (dpsw<1) dpsw = 1;
  if (dpsh<1) dpsh = 1;
  xdpi = gvw->ghostview.xdpi;
  ydpi = gvw->ghostview.ydpi;
  if (xfree && yfree) {
    INFMESSAGE(x and y are free)
    new_width  = (((float)dpsw*72.0)/xdpi + 0.5);
    new_height = (((float)dpsh*72.0)/ydpi + 0.5);
  } else {
    if (!xfree) newxdpi = (float)dpsw * 72.0 / (float)gvw->core.width;
    if (!yfree) newydpi = (float)dpsh * 72.0 / (float)gvw->core.height;
    if (xfree) {
      INFMESSAGE(x is free)
      newxdpi = newydpi * xdpi / ydpi;
      new_width = (((float)dpsw*72.0)/newxdpi+0.5);
    } else if (yfree) {
      INFMESSAGE(y is free)
      newydpi = newxdpi * ydpi / xdpi;
      new_height = (((float)(dpsh)*72.0)/newydpi+0.5);
    }
    gvw->ghostview.xdpi = newxdpi;
    gvw->ghostview.ydpi = newydpi;
  }

  FMESSAGE(gvw->ghostview.xdpi)
  FMESSAGE(gvw->ghostview.ydpi)
  change = (new_width != *width) || (new_height != *height);
  *width = new_width; 
  *height = new_height;
  gvw->ghostview.pref_width  = new_width; 
  gvw->ghostview.pref_height = new_height;
#if 0
    printf("--------------\n");
    if (xfree) printf(" x is free\n");
    else       printf(" x is not free\n");
    if (yfree) printf(" y is free\n");
    else       printf(" y is not free\n");
    printf(" xdpi=%f ydpi=%f\n",gvw->ghostview.xdpi,gvw->ghostview.ydpi);
    printf(" width=%d  height=%d\n",new_width,new_height);
    printf(" llx=%d lly=%d  urx=%d ury=%d\n",
       gvw->ghostview.llx, gvw->ghostview.lly, gvw->ghostview.urx,  gvw->ghostview.ury
    );
    printf("--------------\n");
#endif
    ENDMESSAGE(ComputeSize)
    return (change);
}

/*###################################################################################*/
/* Catch the alloc error when there is not enough resources for the
 * backing pixmap.  Automatically shut off backing pixmap and let the
 * user know when this happens.
 */
/*###################################################################################*/

static Boolean alloc_error;
static XErrorHandler oldhandler;

static int
catch_alloc (dpy, err)
Display *dpy;
XErrorEvent *err;
{
    if (err->error_code == BadAlloc) alloc_error = True;
    if (alloc_error) return 0;
    return oldhandler(dpy, err);
}

/*###################################################################################*/
/* Setup - sets up the backing pixmap, and GHOSTVIEW property and
 * starts interpreter if needed.
 * NOTE: the widget must be realized before calling Setup().
 * Returns True if a new interpreter was started, False otherwise.
 */
/*###################################################################################*/
 
static Boolean
Setup(w)
   Widget w;
{
   GhostviewWidget gvw = (GhostviewWidget) w;
   GhostviewWidgetClass gvc = (GhostviewWidgetClass) XtClass(w);
   Pixmap bpixmap;

   BEGINMESSAGE(Setup)

   if (!gvw->ghostview.changed &&
      (gvw->core.width == gvw->ghostview.gs_width) &&
      (gvw->core.height == gvw->ghostview.gs_height)) {
      INFMESSAGE(nothing changed) ENDMESSAGE(Setup)
      return False;
   }

   StopInterpreter(w);

   if ((gvw->core.width != gvw->ghostview.gs_width) ||
      (gvw->core.height != gvw->ghostview.gs_height) ||
      (!gvw->ghostview.use_bpixmap)) {
      INFMESSAGE(geometry has changed or backingPixmap no longer used)
      if (gvw->core.background_pixmap != XtUnspecifiedPixmap) {
         INFMESSAGE(freeing previous background pixmap)
         XFreePixmap(XtDisplay(w), gvw->core.background_pixmap);
         gvw->core.background_pixmap = XtUnspecifiedPixmap;
         INFMESSAGE(setting background pixmap to 'None')
         XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), None);
      }
   }

   if (gvw->ghostview.use_bpixmap) {
      INFMESSAGE(setting up backing pixmap)
      if (gvw->core.background_pixmap == XtUnspecifiedPixmap) {
         INFMESSAGE(backing pixmap will be allocated)
         /* Get a Backing Pixmap, but be ready for the BadAlloc. */
         XSync(XtDisplay(w), False);  /* Get to known state */
         oldhandler = XSetErrorHandler(catch_alloc);
         alloc_error = False;
         bpixmap = XCreatePixmap(XtDisplay(w), XtWindow(w),
	     		         gvw->core.width, gvw->core.height,
			         gvw->core.depth);
	 XSync(XtDisplay(w), False);  /* Force the error */
	 if (alloc_error) {
            INFMESSAGE(allocation error for backing pixmap)
	    XtCallCallbackList(w, gvw->ghostview.message_callback,"BadAlloc");
	    if (bpixmap != None) {
	       XFreePixmap(XtDisplay(w), bpixmap);
	       XSync(XtDisplay(w), False);  /* Force the error */
	       bpixmap = None;
	    }
	 }
	 oldhandler = XSetErrorHandler(oldhandler);
	 if (bpixmap != None) {
            INFMESSAGE(setting background pixmap)
	    gvw->core.background_pixmap = bpixmap;
            XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), bpixmap);
         }
      } else {
	 bpixmap = gvw->core.background_pixmap;
      }
   } else {
      bpixmap = None;
   }

   {
      XSetWindowAttributes xswa;
      if (bpixmap != None) xswa.backing_store = NotUseful;
      else                 xswa.backing_store = Always;
      XChangeWindowAttributes(XtDisplay(w), XtWindow(w), CWBackingStore, &xswa);
   }

   return SetProperty(w,bpixmap);
}


 
static Boolean
SetProperty(w, bpixmap)
   Widget w; Pixmap bpixmap;
{
   GhostviewWidget gvw = (GhostviewWidget) w;
   GhostviewWidgetClass gvc = (GhostviewWidgetClass) XtClass(w);
   char buf[GV_BUFSIZ];
   float xdpi,ydpi;

   gvw->ghostview.gs_width = gvw->core.width;
   gvw->ghostview.gs_height = gvw->core.height;

   { 
     int dpsw,dpsh;
     switch (gvw->ghostview.orientation) {
       case XtPageOrientationLandscape:
       case XtPageOrientationSeascape:
	 dpsw = gvw->ghostview.ury-gvw->ghostview.lly+1;
	 dpsh = gvw->ghostview.urx-gvw->ghostview.llx+1;
	 break;
       case XtPageOrientationPortrait:
       case XtPageOrientationUpsideDown:
       default:
	 dpsw = gvw->ghostview.urx-gvw->ghostview.llx+1;
	 dpsh = gvw->ghostview.ury-gvw->ghostview.lly+1;
	 break;
     }
     if (dpsw<1) dpsw=1; if (dpsh<1) dpsh = 1;
     xdpi =  (float)gvw->core.width * 72.0 / (float)dpsw;
     ydpi =  (float)gvw->core.height* 72.0 / (float)dpsh;
  }

   IIMESSAGE(gvw->ghostview.gs_width,gvw->ghostview.gs_height)
   sprintf(buf, "%ld %d %d %d %d %d %g %g %d %d %d %d",
	   bpixmap, gvw->ghostview.orientation,
	   (int)gvw->ghostview.llx, (int)gvw->ghostview.lly,
	   gvw->ghostview.urx, gvw->ghostview.ury,
	   xdpi, ydpi,
	   gvw->ghostview.left_margin, gvw->ghostview.bottom_margin,
	   gvw->ghostview.right_margin, gvw->ghostview.top_margin);

   XChangeProperty(XtDisplay(w), XtWindow(w),
		   XmuInternAtom(XtDisplay(w), gvc->ghostview_class.ghostview),
		   XA_STRING, 8, PropModeReplace,
		   (unsigned char *)buf, strlen(buf));
   INFSMESSAGE(atom 1:, buf)
   sprintf(buf, "%s %d %d",
	   gvw->ghostview.palette == XtPaletteMonochrome ? "Monochrome" :
	   gvw->ghostview.palette == XtPaletteGrayscale  ? "Grayscale" :
	   gvw->ghostview.palette == XtPaletteColor      ? "Color" : "?",
	   (int)gvw->ghostview.foreground, (int)gvw->core.background_pixel);
   XChangeProperty(XtDisplay(w), XtWindow(w),
		   XmuInternAtom(XtDisplay(w), gvc->ghostview_class.gv_colors),
		   XA_STRING, 8, PropModeReplace,
		   (unsigned char *)buf, strlen(buf));
   INFSMESSAGE(atom 2:, buf)

  /* Be sure to update properties */

   XSync(XtDisplay(w), False);

   gvw->ghostview.background_cleared = 0;
   if(!pix || w != page) SetBackground(w,True);

   ENDMESSAGE(Setup)
   return True;
}

#ifndef VMS

/* This routine starts the interpreter.  It sets the DISPLAY and 
 * GHOSTVIEW environment variables.  The GHOSTVIEW environment variable
 * contains the Window that ghostscript should write on.
 *
 * This routine also opens pipes for stdout and stderr and initializes
 * application input events for them.  If input to ghostscript is not
 * from a file, a pipe for stdin is created.  This pipe is setup for
 * non-blocking I/O so that the user interface never "hangs" because of
 * a write to ghostscript.
 */

static void
StartInterpreter(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    int	std_in[2];
    int	std_out[2];
    int	std_err[2];
    char buf[GV_BUFSIZ];
#define NUM_ARGS 100
    char *argv[NUM_ARGS];
    char *arguments = NULL;
    char *cptr;
    char *device = NULL;
    char *dptr;
    int argc = 0;
    int ret;

    BEGINMESSAGE(StartInterpreter)

  if(w == page && !gvw->ghostview.use_bpixmap) {
/*    printf("StartInterpreter on page\n"); */
    SetProperty(w,None);
    if (!pix) SetBackground(w,True); 
  }
    else {
/*    printf("StartInterpreter old\n"); */
    SetBackground(w,True);
  }

    StopInterpreter(w);

    /* Clear the window before starting a new interpreter. */
    if (gvw->ghostview.disable_start) return;

    argv[argc++] = gvw->ghostview.interpreter;
    if (app_res.antialias) dptr = device = GV_XtNewString(gv_gs_x11_alpha_device);
    else                   dptr = device = GV_XtNewString(gv_gs_x11_device);
    while (isspace(*dptr)) dptr++;
    while (*dptr) {
	 argv[argc++] = dptr;
	 while (*dptr && !isspace(*dptr)) dptr++;
	 if (*dptr) *dptr++ = '\0';
	 if (argc + 2 >= NUM_ARGS) {
	    fprintf(stderr, "Too many arguments to interpreter.\n");
	    exit(EXIT_STATUS_ERROR);
	 }
	 while (isspace(*dptr)) dptr++;
    }
    argv[argc++] = "-dNOPAUSE";
    argv[argc++] = "-g2x2"; // Avoid unwanted rotation of landscape pdf files
    if (gvw->ghostview.quiet) argv[argc++] = "-dQUIET";
    if (gvw->ghostview.safer) 
      {
	argv[argc++] = "-dSAFER";
      }

    if (gvw->ghostview.arguments) {
	cptr = arguments = GV_XtNewString(gvw->ghostview.arguments);
	while (isspace(*cptr)) cptr++;
	while (*cptr) {
	    argv[argc++] = cptr;
	    while (*cptr && !isspace(*cptr)) cptr++;
	    if (*cptr) *cptr++ = '\0';
	    if (argc + 2 >= NUM_ARGS) {
		fprintf(stderr, "Too many arguments to interpreter.\n");
		exit(EXIT_STATUS_ERROR);
	    }
	    while (isspace(*cptr)) cptr++;
	}
    }
#   ifdef ALLOW_PDF
    if (gvw->ghostview.filename && strcmp(gvw->ghostview.filename,"-")) {
          argv[argc++] = gvw->ghostview.filename;
          argv[argc++] = "-c";
          argv[argc++] = "quit";
    } else {
       argv[argc++] = "-";
    }
#   else
       argv[argc++] = "-";
#   endif
    argv[argc++] = NULL;

    if (gvw->ghostview.filename == NULL) {
	ret = pipe(std_in);
	if (ret == -1) {
	    perror("Could not create pipe");
	    exit(EXIT_STATUS_ERROR);
	}
    } else if (strcmp(gvw->ghostview.filename, "-")) {
#      ifdef ALLOW_PDF
          ret = pipe(std_in);
          if (ret == -1) {
             perror("Could not create pipe");
             exit(EXIT_STATUS_ERROR);
          }
#      else
          std_in[0] = open(gvw->ghostview.filename, O_RDONLY, 0);
#      endif
    }
    ret = pipe(std_out);
    if (ret == -1) {
	perror("Could not create pipe");
	exit(EXIT_STATUS_ERROR);
    }
    ret = pipe(std_err);
    if (ret == -1) {
	perror("Could not create pipe");
	exit(EXIT_STATUS_ERROR);
    }

    gvw->ghostview.changed = False;
    gvw->ghostview.busy = True;
    ChangeCursor(gvw,CURSOR_BUSY);
#if defined(VMS)
#   define fork vfork
#endif
    gvw->ghostview.interpreter_pid = fork();

    if (gvw->ghostview.interpreter_pid == 0) { /* child */
	close(std_out[0]);
	close(std_err[0]);
	dup2(std_out[1], 1);
	close(std_out[1]);
	dup2(std_err[1], 2);
	close(std_err[1]);
         if(pix && w == page){
           sprintf(buf, "%ld %ld", XtWindow(w), pix );
         } else {
           sprintf(buf, "%ld", XtWindow(w));
         }
	setenv("GHOSTVIEW", buf, True);
	setenv("DISPLAY", XDisplayString(XtDisplay(w)), True);
	if (gvw->ghostview.filename == NULL) {
	    close(std_in[1]);
	    dup2(std_in[0], 0);
	    close(std_in[0]);
	} else if (strcmp(gvw->ghostview.filename, "-")) {
#          ifdef ALLOW_PDF
	      close(std_in[1]);
	      dup2(std_in[0], 0);
	      close(std_in[0]);
#          else
	      dup2(std_in[0], 0);
	      close(std_in[0]);
#          endif
	}
	execvp(argv[0], argv);
	sprintf(buf, "Exec of %s failed", argv[0]);
	perror(buf);
	_exit(EXIT_STATUS_ERROR);
    } else {
	if (gvw->ghostview.filename == NULL) {
#ifdef NON_BLOCKING_IO
	    int result;
#endif
	    close(std_in[0]);

#ifdef NON_BLOCKING_IO
	    result = fcntl(std_in[1], F_GETFL, 0);
	    result = result | O_NONBLOCK;
	    result = fcntl(std_in[1], F_SETFL, result);
#endif
	    gvw->ghostview.interpreter_input = std_in[1];
	    gvw->ghostview.interpreter_input_id = None;
	} else if (strcmp(gvw->ghostview.filename, "-")) {
	    close(std_in[0]);
	}
	close(std_out[1]);
	gvw->ghostview.interpreter_output = std_out[0];
	gvw->ghostview.interpreter_output_id = 
	    XtAppAddInput(XtWidgetToApplicationContext(w), std_out[0],
			  (XtPointer)XtInputReadMask, Output, (XtPointer)w);
	close(std_err[1]);
	gvw->ghostview.interpreter_error = std_err[0];
	gvw->ghostview.interpreter_error_id = 
	    XtAppAddInput(XtWidgetToApplicationContext(w), std_err[0],
			  (XtPointer)XtInputReadMask, Output, (XtPointer)w);
    }
    if (arguments) GV_XtFree(arguments);
    if (device)    GV_XtFree(device);
    gvw->ghostview.background_cleared=0;

    ENDMESSAGE(StartInterpreter)
}

/* Stop the interperter, if present, and remove any Input sources. */
/* Also reset the busy state. */
int restarted = 0;

static void
StopInterpreter(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(StopInterpreter)
    if (gvw->ghostview.interpreter_pid >= 0) {
        INFMESSAGE(killing process)
	kill(gvw->ghostview.interpreter_pid, SIGTERM);
	gvw->ghostview.interpreter_pid = -1;
	wait(0);
    }
    if (gvw->ghostview.interpreter_input >= 0) {
	close(gvw->ghostview.interpreter_input);
	gvw->ghostview.interpreter_input = -1;
	if (gvw->ghostview.interpreter_input_id != None) {
	    XtRemoveInput(gvw->ghostview.interpreter_input_id);
	    gvw->ghostview.interpreter_input_id = None;
	}
	while (gvw->ghostview.ps_input) {
	    struct record_list *ps_old = gvw->ghostview.ps_input;
	    gvw->ghostview.ps_input = ps_old->next;
	    if (ps_old->close) fclose(ps_old->fp);
	    GV_XtFree((char *)ps_old);
	}
    }
    if (gvw->ghostview.interpreter_output >= 0) {
	close(gvw->ghostview.interpreter_output);
	gvw->ghostview.interpreter_output = -1;
	XtRemoveInput(gvw->ghostview.interpreter_output_id);
    }
    if (gvw->ghostview.interpreter_error >= 0) {
	close(gvw->ghostview.interpreter_error);
	gvw->ghostview.interpreter_error = -1;
	XtRemoveInput(gvw->ghostview.interpreter_error_id);
    }
    gvw->ghostview.busy = False;
    ChangeCursor(gvw,CURSOR_RESET);
    ENDMESSAGE(StopInterpreter)
}

#endif /* VMS */

/*###################################################################################*/
/* InterpreterFailed */
/* The interpeter failed, Stop what's left and notify application */
/*###################################################################################*/

static void
InterpreterFailed(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(InterpreterFailed)
    StopInterpreter(w);
    XtCallCallbackList(w, gvw->ghostview.message_callback, "Failed");
    ENDMESSAGE(InterpreterFailed)
}

/*###################################################################################*/
/*###################################################################################*/
/*
 *	Public Routines
 */
/*###################################################################################*/
/*###################################################################################*/

void
GhostviewDrawRectangle(w,x,y,width,height)
  Widget w;
  int x,y,width,height;
{
  GhostviewWidget gvw = (GhostviewWidget) w;

  BEGINMESSAGE(GhostviewDrawRectangle)
  XDrawRectangle(XtDisplay(w),XtWindow(w),gvw->ghostview.highlight_gc,x,y,width,height);
  if (gvw->core.background_pixmap != XtUnspecifiedPixmap)
     XDrawRectangle(XtDisplay(w),gvw->core.background_pixmap,gvw->ghostview.highlight_gc,x,y,width,height);
  ENDMESSAGE(GhostviewDrawRectangle)
}

/*------------------------------------------------------------*/
/* GhostviewOrientCoords */
/*------------------------------------------------------------*/

#define GV_O_P (1<<0)
#define GV_O_U (1<<1)
#define GV_O_L (1<<2)
#define GV_O_S (1<<3)
#define GV_O_H ((GV_O_L) | (GV_O_S))
#define GV_O_V ((GV_O_P) | (GV_O_U))

static void GhostviewOrientCoords(gvw,x,y,xP,yP,widthP,heightP,orientP)
  GhostviewWidget gvw;
  int x;
  int y;
  int *xP;
  int *yP;
  int *widthP;
  int *heightP;
  int *orientP;
{
  BEGINMESSAGE(GhostviewOrientCoords)
  switch (gvw->ghostview.orientation) {
    case XtPageOrientationPortrait:   *orientP = GV_O_P; break;
    case XtPageOrientationUpsideDown: *orientP = GV_O_U; break;
    case XtPageOrientationLandscape:  *orientP = GV_O_L; break;
    case XtPageOrientationSeascape:   *orientP = GV_O_S; break;
    default:                          *orientP = GV_O_P; break;
  }
  if (*orientP & GV_O_V) {
    *widthP     = gvw->core.width-1;
    *heightP    = gvw->core.height-1;
    if (xP) *xP = x;
    if (yP) *yP = y;
  } else {
    *widthP     = gvw->core.height-1;
    *heightP    = gvw->core.width-1;
    if (xP) *xP = y;
    if (yP) *yP = x;
  }
  if (*widthP  < 1) *widthP=1;
  if (*heightP < 1) *heightP=1;
  ENDMESSAGE(GhostviewOrientCoords)
}

/*------------------------------------------------------------*/
/* GhostviewCornersPS */
/*------------------------------------------------------------*/

static void GhostviewCornersPS(gvw,o,pslxP,psrxP,pslyP,psuyP)
  GhostviewWidget gvw;
  int o;
  int *pslxP;
  int *psrxP;
  int *pslyP;
  int *psuyP;
{
  if (o & GV_O_P) {
    *pslxP=gvw->ghostview.llx; *psrxP=gvw->ghostview.urx;
    *pslyP=gvw->ghostview.ury; *psuyP=gvw->ghostview.lly;
  } else if (o & GV_O_L) {
    *pslxP=gvw->ghostview.llx; *psrxP=gvw->ghostview.urx;
    *pslyP=gvw->ghostview.lly; *psuyP=gvw->ghostview.ury;
  } else if (o & GV_O_U) {
    *pslxP=gvw->ghostview.urx; *psrxP=gvw->ghostview.llx;
    *pslyP=gvw->ghostview.lly; *psuyP=gvw->ghostview.ury;
  } else {
    *pslxP=gvw->ghostview.urx; *psrxP=gvw->ghostview.llx;
    *pslyP=gvw->ghostview.ury; *psuyP=gvw->ghostview.lly;
  }
}

/*############################################################*/
/* GhostviewCoordsXtoPS */
/*############################################################*/

void
#if NeedFunctionPrototypes
GhostviewCoordsXtoPS(
  Widget w,
  int wx,
  int wy,
  int *psxP,
  int *psyP
)
#else
GhostviewCoordsXtoPS(w,wx,wy,psxP,psyP)
  Widget w;
  int wx;
  int wy;
  int *psxP;
  int *psyP;
#endif
{
  GhostviewWidget gvw = (GhostviewWidget) w;
  int o,x,y,pslx,psrx,psly,psuy,wrx,wly;

  BEGINMESSAGE(GhostviewCoordsXtoPS)
  GhostviewOrientCoords(gvw,wx,wy,&x,&y,&wrx,&wly,&o);
  GhostviewCornersPS(gvw,o,&pslx,&psrx,&psly,&psuy);
  *psxP = (pslx + ((float)x*(float)(psrx-pslx))/(float)wrx + 0.5);
  *psyP = (psly + ((float)y*(float)(psuy-psly))/(float)wly + 0.5);
#if 1
  {
    int x,y;
    IIMESSAGE(wx,wy)
    IIMESSAGE(*psxP,*psyP)
    GhostviewCoordsPStoX(w,*psxP,*psyP,&x,&y);
    IIMESSAGE(x,y)
  }
#endif
  ENDMESSAGE(GhostviewCoordsXtoPS)
}

/*############################################################*/
/* GhostviewCoordsPStoX */
/*############################################################*/

void
#if NeedFunctionPrototypes
GhostviewCoordsPStoX(
  Widget w,
  int psx,
  int psy,
  int *wxP,
  int *wyP
)
#else
GhostviewCoordsPStoX(w,psx,psy,wxP,wyP)
  Widget w;
  int psx;
  int psy;
  int *wxP;
  int *wyP;
#endif
{
  GhostviewWidget gvw = (GhostviewWidget) w;
  int o,pslx,psrx,psly,psuy,wrx,wly,*xP,*yP;

  BEGINMESSAGE(GhostviewCoordsPStoX)
  GhostviewOrientCoords(gvw,0,0,NULL,NULL,&wrx,&wly,&o);
  GhostviewCornersPS(gvw,o,&pslx,&psrx,&psly,&psuy);
  if (o & GV_O_V) { xP = wxP; yP=wyP; }
  else            { xP = wyP; yP=wxP; }
  *xP = (((float)(psx-pslx)*(float)wrx)/(float)(psrx-pslx)+0.5);
  *yP = (((float)(psy-psly)*(float)wly)/(float)(psuy-psly)+0.5);
  IIMESSAGE(*xP,*yP)
  ENDMESSAGE(GhostviewCoordsPStoX)
}

/*############################################################*/
/* GhostviewGetBBOfArea */
/*############################################################*/

void
#if NeedFunctionPrototypes
GhostviewGetBBofArea (
  Widget   w,
  int x1,
  int y1,
  int x2,
  int y2,
  GhostviewReturnStruct* p
)
#else
GhostviewGetBBofArea (w,x1,y1,x2,y2,p)
  Widget w;
  int x1;
  int y1;
  int x2;
  int y2;
  GhostviewReturnStruct* p;
#endif
{
  GhostviewWidget gvw = (GhostviewWidget) w;
  int psx1,psy1,psx2,psy2;

  BEGINMESSAGE(GhostviewGetBBOfArea) 
  GhostviewCoordsXtoPS(w,x1,y1,&psx1,&psy1);
  GhostviewCoordsXtoPS(w,x2,y2,&psx2,&psy2);
  p->psx    = psx1 < psx2 ? psx1 : psx2;
  p->psy    = psy1 < psy2 ? psy1 : psy2;
  p->width  = abs(psx1-psx2)+1;
  p->height = abs(psy1-psy2)+1;
  p->xdpi   = gvw->ghostview.xdpi;
  p->ydpi   = gvw->ghostview.ydpi;
  IIMESSAGE(p->psx,p->psy)
  IIMESSAGE(p->width,p->height)
  ENDMESSAGE(GhostviewGetBBOfArea) 
}

/*############################################################*/
/* GhostviewGetAreaOfBB */
/*############################################################*/

void
#if NeedFunctionPrototypes
GhostviewGetAreaOfBB (
  Widget   w,
  int psx1,
  int psy1,
  int psx2,
  int psy2,
  GhostviewReturnStruct* p
) 
#else
GhostviewGetAreaOfBB (w,psx1,psy1,psx2,psy2,p)
  Widget w;
  int psx1;
  int psy1;
  int psx2;
  int psy2;
  GhostviewReturnStruct* p;
#endif
{
  int x1,y1,x2,y2;

  BEGINMESSAGE(GhostviewGetAreaOfBB) 
  GhostviewCoordsPStoX(w,psx1,psy1,&x1,&y1);
  GhostviewCoordsPStoX(w,psx2,psy2,&x2,&y2);
  p->psx    = x1 < x2 ? x1 : x2;
  p->psy    = y1 < y2 ? y1 : y2;
  p->width  = abs(x1-x2)+1;
  p->height = abs(y1-y2)+1;
  IIMESSAGE(p->psx,p->psy)
  IIMESSAGE(p->width,p->height)
  ENDMESSAGE(GhostviewGetAreaOfBB) 
}


/*###################################################################################*/
/* GhostviewClearBackground */
/*###################################################################################*/

void
GhostviewClearBackground(w)
    Widget w;
{
    BEGINMESSAGE(GhostviewClearBackground)
      if (XtIsRealized(w)) { if(!pix || w != page) SetBackground(w,True); }
    ENDMESSAGE(GhostviewClearBackground)
}

/*###################################################################################*/
/* GhostviewSetup */
/*###################################################################################*/

void
GhostviewSetup(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(GhostviewSetup)
    gvw->ghostview.disable_start = True;
    if (XtIsRealized(w)) Setup(w);
    ENDMESSAGE(GhostviewSetup)
}

/*###################################################################################*/
/* GhostviewDisableInterpreter:
 * Stop any interpreter and disable new ones from starting.
 */
/*###################################################################################*/
void
GhostviewDisableInterpreter(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(GhostviewDisableInterpreter)
    gvw->ghostview.disable_start = True;
    if (XtIsRealized(w)) StopInterpreter(w);
    ENDMESSAGE(GhostviewDisableInterpreter)
}

/*###################################################################################*/
/* GhostviewEnableInterpreter:
 * Allow an interpreter to start and start one if the widget is
 * currently realized.
 */
/*###################################################################################*/

void
GhostviewEnableInterpreter(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(GhostviewEnableInterpreter)
    gvw->ghostview.disable_start = False;
    if (XtIsRealized(w)) StartInterpreter(w);
    ENDMESSAGE(GhostviewEnableInterpreter)
}

/*###################################################################################*/
/* GhostviewState
 */
/*###################################################################################*/

void
#if NeedFunctionPrototypes
GhostviewState(
    Widget w,
    Boolean *processflag_p,
    Boolean *busyflag_p,
    Boolean *inputflag_p
)
#else
GhostviewState(w,processflag_p,busyflag_p,inputflag_p)
    Widget w;
    Boolean *processflag_p;
    Boolean *busyflag_p;
    Boolean *inputflag_p;
#endif
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(GhostviewState)
    if (processflag_p) *processflag_p = (gvw->ghostview.interpreter_pid != -1) ? True: False;
    if (busyflag_p)    *busyflag_p    = (!(gvw->ghostview.busy))               ? True: False;
    if (inputflag_p)   *inputflag_p   = (gvw->ghostview.ps_input == NULL)      ? True: False;
#   ifdef MESSAGES
       if (gvw->ghostview.interpreter_pid != -1) {INFMESSAGE(have interpreter)}
       else                                      {INFMESSAGE(have no interpreter)}
       if (!(gvw->ghostview.busy))               {INFMESSAGE(widget idle)}
       else                                      {INFMESSAGE(widget busy)}
       if (gvw->ghostview.ps_input == NULL)      {INFMESSAGE(no pending input)}
       else                                      {INFMESSAGE(have pending input)}
#   endif
    ENDMESSAGE(GhostviewState)
}

/*###################################################################################*/
/* GhostviewIsInterpreterReady:
 * Returns True if the interpreter is ready for new input.
 */
/*###################################################################################*/

Boolean
GhostviewIsInterpreterReady(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(GhostviewIsInterpreterReady)
#   ifdef MESSAGES
       if (gvw->ghostview.interpreter_pid == -1) {INFMESSAGE(no interpreter process)}
       if (gvw->ghostview.busy)                  {INFMESSAGE(interpreter busy)}
       if (gvw->ghostview.ps_input != NULL)      {INFMESSAGE(interpreter still has input)}
#   endif
    ENDMESSAGE(GhostviewIsInterpreterReady)
    return gvw->ghostview.interpreter_pid != -1 && 
           !gvw->ghostview.busy &&
	   gvw->ghostview.ps_input == NULL;
}

/*###################################################################################*/
/* GhostviewIsBusy:
 * Returns True if the interpreter is running but the widget is busy
 */
/*###################################################################################*/

Boolean
GhostviewIsBusy(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(GhostviewIsBusy)
    ENDMESSAGE(GhostviewIsBusy)
    return (((gvw->ghostview.interpreter_pid != -1)&&(gvw->ghostview.busy)));
}

/*###################################################################################*/
/* GhostviewIsInterpreterRunning:
 * Returns True if the interpreter is running.
 */
/*###################################################################################*/

Boolean
GhostviewIsInterpreterRunning(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    BEGINMESSAGE(GhostviewIsInterpreterRunning)
    ENDMESSAGE(GhostviewIsInterpreterRunning)
    return gvw->ghostview.interpreter_pid != -1;
}

#ifndef VMS

/*###################################################################################*/
/* GhostviewSendPS:
 *   Queue a portion of a PostScript file for output to ghostscript.
 *   fp: FILE * of the file in question.  NOTE: if you have several
 *   Ghostview widgets reading from the same file.  You must open
 *   a unique FILE * for each widget.
 *   SendPS does not actually send the PostScript, it merely queues it
 *   for output.
 *   begin: position in file (returned from ftell()) to start.
 *   len:   number of bytes to write.
 *
 *   If an interpreter is not running, nothing is queued and
 *   False is returned.
 */
/*###################################################################################*/

Boolean
GhostviewSendPS(w, fp, begin, len, close)
    Widget w;
    FILE *fp;
    long begin;
    unsigned int len;
    Bool close;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    struct record_list *ps_new;

    if (gvw->ghostview.interpreter_input < 0) return False;
    ps_new = (struct record_list *) GV_XtMalloc(sizeof (struct record_list));
    ps_new->fp = fp;
    ps_new->begin = begin;
    ps_new->len = len;
    ps_new->seek_needed = True;
    ps_new->close = close;
    ps_new->next = NULL;

    if (gvw->ghostview.input_buffer == NULL) {
	gvw->ghostview.input_buffer = GV_XtMalloc(GV_BUFSIZ);
    }

    if (gvw->ghostview.ps_input == NULL) {
	gvw->ghostview.input_buffer_ptr = gvw->ghostview.input_buffer;
	gvw->ghostview.bytes_left = len;
	gvw->ghostview.buffer_bytes_left = 0;
	gvw->ghostview.ps_input = ps_new;
	gvw->ghostview.interpreter_input_id =
		XtAppAddInput(XtWidgetToApplicationContext(w),
			      gvw->ghostview.interpreter_input,
			      (XtPointer)XtInputWriteMask, Input, (XtPointer)w);
    } else {
	struct record_list *p = gvw->ghostview.ps_input;
	while (p->next != NULL) {
	    p = p->next;
	}
	p->next = ps_new;
    }
    return True;
}

#endif /* VMS */

/*###################################################################################*/
/* GhostviewNextPage:
 *   Tell ghostscript to start the next page.
 *   Returns False if ghostscript is not running, or not ready to start
 *   another page.
 *   If another page is started.  Sets the busy flag and cursor.
 */
/*###################################################################################*/

Boolean
GhostviewNextPage(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    GhostviewWidgetClass gvc = (GhostviewWidgetClass) XtClass(w);
    XEvent event;

    BEGINMESSAGE(GhostviewNextPage)
    if (gvw->ghostview.interpreter_pid < 0) {
       INFMESSAGE(no interpreter active) ENDMESSAGE(GhostviewNextPage) return False;
    }
    if (gvw->ghostview.mwin == None) {
       INFMESSAGE(no window) ENDMESSAGE(GhostviewNextPage) return False;
    }

    if (!gvw->ghostview.busy) {
	gvw->ghostview.busy = True;
        ChangeCursor(gvw,CURSOR_BUSY);
	event.xclient.type = ClientMessage;
	event.xclient.display = XtDisplay(w);
	event.xclient.window = gvw->ghostview.mwin;
	event.xclient.message_type = XmuInternAtom(XtDisplay(w), gvc->ghostview_class.next);
	event.xclient.format = 32;
	XSendEvent(XtDisplay(w), gvw->ghostview.mwin, False, 0, &event);
	XFlush(XtDisplay(w));	/* And push it out */
        INFMESSAGE(ok) ENDMESSAGE(GhostviewNextPage) return True;
    } else {
        INFMESSAGE(ghostview busy) ENDMESSAGE(GhostviewNextPage) return False;
    }
}

/*###################################################################################*/
/* Palette Conversion Routine.
 * Returns True if Conversion is successful.
 */
/*###################################################################################*/

#define	done(type, value) \
	{							\
	    if (toVal->addr != NULL) {				\
		if (toVal->size < sizeof(type)) {		\
		    toVal->size = sizeof(type);			\
		    return False;				\
		}						\
		*(type*)(toVal->addr) = (value);		\
	    }							\
	    else {						\
		static type static_val;				\
		static_val = (value);				\
		toVal->addr = (XPointer)&static_val;		\
	    }							\
	    toVal->size = sizeof(type);				\
	    return True;					\
	}

Boolean
XmuCvtStringToPalette(dpy, args, num_args, fromVal, toVal, data)
    Display	*dpy;
    XrmValue	*args;		/* unused */
    Cardinal	*num_args;	/* unused */
    XrmValue	*fromVal;
    XrmValue	*toVal;
    XtPointer	*data;		/* unused */
{
    static XrmQuark		XrmQEmonochrome;
    static XrmQuark		XrmQEgrayscale;
    static XrmQuark		XrmQEcolor;
    static int			haveQuarks;
    XrmQuark    q;
    char	*str = (XPointer) fromVal->addr;
    char        lowerName[100];

    if (str == NULL) return False;

    if (!haveQuarks) {
	XrmQEmonochrome = XrmStringToQuark(XtEmonochrome);
	XrmQEgrayscale  = XrmStringToQuark(XtEgrayscale);
	XrmQEcolor      = XrmStringToQuark(XtEcolor);
	haveQuarks = 1;
    }

    XmuCopyISOLatin1Lowered(lowerName, str);

    q = XrmStringToQuark(lowerName);

    if (q == XrmQEmonochrome)
	done(XtPalette, XtPaletteMonochrome);
    if (q == XrmQEgrayscale)
	done(XtPalette, XtPaletteGrayscale);
    if (q == XrmQEcolor)
	done(XtPalette, XtPaletteColor);

    XtDisplayStringConversionWarning(dpy, str, XtRPalette);
    return False;
}

/*###################################################################################*/
/*###################################################################################*/
/*###################################################################################*/
/* VMS */
/*###################################################################################*/
/*###################################################################################*/
/*###################################################################################*/

#ifdef VMS

#include <descrip.h>
#include <clidef.h>
#include <lnmdef.h>
#include <iodef.h>
#include <dvidef.h>
#include "vms_types.h"

#define ERR_SIGNAL(s) if(!((s) & 1))lib$signal((s), 0, 0)

static void CancelInput();
static void CancelOutput();
static void StartInput();
static void StartOutput();
static void OutputComplete();
static void InputComplete();


/*###################################################################################*/
/*  StartInput  */
/*###################################################################################*/

static void
StartInput(gvw)
    GhostviewWidget gvw;
{
   int stat;

   BEGINMESSAGE(StartInput)
   if (gvw->ghostview.filename == NULL) {
      stat = gvw->ghostview.interpreter_input_iosb[0];
      if (stat != SS$_NORMAL) {
         INFIMESSAGE(### strange input pipe encountered:,stat)
         InterpreterFailed((Widget)gvw);
         return;
      }
      INFMESSAGE(activating input pipe NOW)
      Input(gvw);
   }
   ENDMESSAGE(StartInput)
}

/*###################################################################################*/
/*  Input  */
/*###################################################################################*/

#define BUFFER_CHUNK_SIZE  1024
#define SEND_CHUNK_SIZE    1000

#define BYTES_LEFT_IN_FILE gvw->ghostview.bytes_left
#define OVERHEAD_BYTES     gvw->ghostview.input_overhead_bytes
#define OVERHEAD_BUFFER    gvw->ghostview.input_overhead_buffer
#define INPUT_BUFFER(aaa)  gvw->ghostview.input_buffer[(aaa)]
#define INPUT_BUFFER_PTR   gvw->ghostview.input_buffer

static void
Input(gvw)
    GhostviewWidget gvw;
{
    int stat;
    size_t br   ; /* bytes_read             */
    size_t btr  ; /* bytes_to_read          */
    size_t bts  ; /* bytes_to_send          */
    size_t bib  ; /* bytes_in_buffer        */

    
    BEGINMESSAGE(Input)

    /* Get a new section if required */
    if (gvw->ghostview.ps_input && BYTES_LEFT_IN_FILE == 0 && OVERHEAD_BYTES == 0) {
       struct record_list *ps_old = gvw->ghostview.ps_input;
       INFMESSAGE1(getting a new section)
       gvw->ghostview.ps_input = ps_old->next;
       if (ps_old->close) fclose(ps_old->fp);
       GV_XtFree((char *)ps_old);
    }
 
    if (gvw->ghostview.ps_input){
       /* Have to seek at the beginning of each section */
       if (gvw->ghostview.ps_input->seek_needed) {
          INFMESSAGE1(seek needed)
          if (gvw->ghostview.ps_input->len > 0)
	     fseek(gvw->ghostview.ps_input->fp,
	           gvw->ghostview.ps_input->begin, SEEK_SET);
	  gvw->ghostview.ps_input->seek_needed = False;
          gvw->ghostview.bytes_left = gvw->ghostview.ps_input->len;
          OVERHEAD_BYTES  = 0;
          OVERHEAD_BUFFER = NULL;
       }
  
       btr = BUFFER_CHUNK_SIZE - OVERHEAD_BYTES;
       if (btr>BYTES_LEFT_IN_FILE)  btr = BYTES_LEFT_IN_FILE; 
       IMESSAGE(BYTES_LEFT_IN_FILE)
       IIMESSAGE(OVERHEAD_BYTES,btr)

       if (OVERHEAD_BYTES) {
          INFMESSAGE(moving overhead buffer to start of input buffer)
          memmove(INPUT_BUFFER_PTR,OVERHEAD_BUFFER,OVERHEAD_BYTES);
       }
       if (btr>0) br = fread(&(INPUT_BUFFER(OVERHEAD_BYTES)),
                             sizeof(char), btr,
                             gvw->ghostview.ps_input->fp);
       else       br = 0;
       INFIMESSAGE(bytes read,br)
       BYTES_LEFT_IN_FILE -= br;
       bib = br + OVERHEAD_BYTES;
       bts = bib; if (bts>SEND_CHUNK_SIZE) bts = SEND_CHUNK_SIZE; 
       IIMESSAGE(bib,bts)

       if (btr>0 && br==0) InterpreterFailed(gvw);
       else if (bts>0) {
          char *c;
          size_t s = bts-1;
          if (s==SEND_CHUNK_SIZE-1) {
             c = (char*) (&(INPUT_BUFFER(s)));
             while (s != 0 && *c != '\n') { c--; s--; }
          }
          if (s==0) {
             s = bts-1;
             c = (char*) (&(INPUT_BUFFER(s)));
             while (s != 0 && (*c != '\t' && *c != ' ' && *c != '\r')) { c--; s--; }
          }
          if (s==0) {
             INFMESSAGE(Cannot split record)
             fprintf(stderr,"Ghostview Widget: Fatal error: Cannot split record.\n");
             InterpreterFailed(gvw);
          } else {
#if 0
             {
                char tmp[2*GV_BUFSIZ];
                strncpy(tmp, INPUT_BUFFER_PTR ,s+1);
                tmp[s+1]='\0';
                fprintf(stdout,"%s####### SENT #############\n",tmp);
             }
             {
                char tmp[2*GV_BUFSIZ];
                strncpy(tmp, &(INPUT_BUFFER(s+1)) ,bib-(s+1));
                tmp[bib-(s+1)]='\0';
                fprintf(stdout,"%s******* OVERHEAD *********\n",tmp);
             }
#endif
             OVERHEAD_BYTES  = bib - (s+1);
             OVERHEAD_BUFFER = &(INPUT_BUFFER(s+1));
	     stat = sys$qio( 0, (short)gvw->ghostview.interpreter_input,
                             IO$_WRITEVBLK, &gvw->ghostview.interpreter_input_iosb,
                             InputComplete,gvw,
                             INPUT_BUFFER_PTR, s+1,
                             0,0,0,0 );
             ERR_SIGNAL(stat);
          }
       }
    }
    ENDMESSAGE(Input)
    return;
}

/*###################################################################################*/
/*  InputComplete  */
/*###################################################################################*/

static void
InputComplete(gvw)
   GhostviewWidget gvw;
{
   int stat;

   BEGINMESSAGE1(InputComplete)
   stat = gvw->ghostview.interpreter_input_iosb[0];
   if (stat != SS$_NORMAL) {
      if (stat == SS$_CANCEL || stat == SS$_ABORT) {
         INFIMESSAGE(### input pipe aborted:,stat)
         ENDMESSAGE1(InputComplete)
         return;
      } else {
         INFIMESSAGE(### input pipe broken:,stat)
         InterpreterFailed((Widget)gvw);
         ENDMESSAGE1(InputComplete)
         return;
      }
   }
   Input(gvw); /* queue the next write */
   ENDMESSAGE1(InputComplete)
}

/*###################################################################################*/
/*  CancelInput  */
/*###################################################################################*/

static void
CancelInput(gvw)
   GhostviewWidget gvw;
{
   BEGINMESSAGE(CancelInput)
   if (gvw->ghostview.interpreter_input >= 0) {
      INFMESSAGE(aborting possible write requests)
      sys$cancel((short)gvw->ghostview.interpreter_input);
   }
   ENDMESSAGE(CancelInput)
}

/*###################################################################################*/
/*  StartOutput  */
/*###################################################################################*/

static void
StartOutput(gvw)
    GhostviewWidget gvw;
{
   int stat;

   BEGINMESSAGE(StartOutput)
   stat = gvw->ghostview.interpreter_output_iosb[0];
   if (stat != SS$_NORMAL) {
       INFIMESSAGE(### strange output pipe encountered:,stat)
       InterpreterFailed((Widget)gvw);
       return;
   }
   INFMESSAGE(activating output pipe NOW)
   Output(gvw);
   ENDMESSAGE(StartOutput)
}

/*###################################################################################*/
/*  Output  */
/*  queue a read to the output mailbox. */
/*###################################################################################*/

static void
Output(gvw)
   GhostviewWidget gvw;
{
   int stat;
   unsigned long gvw_number=0;

   BEGINMESSAGE1(Output)
   stat = sys$qio(
   	     0,
	     (short)gvw->ghostview.interpreter_output,
	     IO$_READVBLK, &gvw->ghostview.interpreter_output_iosb,
	     OutputComplete,gvw,
	     gvw->ghostview.output_buffer, GV_BUFSIZ,
	     0,0,0,0
	  );
   ENDMESSAGE1(Output)
}

/*###################################################################################*/
/*  OutputComplete */
/*
 * Note: the use of  'static int received'
 * should be cleaned sometimes (remember, this is part of a widget) ###jp###
 * Most Error Messages from ghostscript come 'per byte', without line termination.
 * OutputComplete therefore also tries to give these messages some 'structure'.
*/
/*###################################################################################*/

static void
OutputComplete(gvw)
   GhostviewWidget gvw;
{
   static char buf[GV_BUFSIZ+1];
   static int received =0;
   int bytes, stat;

   BEGINMESSAGE1(OutputComplete)

   stat = gvw->ghostview.interpreter_output_iosb[0];
   bytes = gvw->ghostview.interpreter_output_iosb[1];
   IIMESSAGE1(stat,bytes)

   if (stat != SS$_NORMAL) {
      if (stat == SS$_CANCEL || stat == SS$_ABORT) {
         INFIMESSAGE(### output pipe aborted:,stat)
         ENDMESSAGE1(OutputComplete)
         return;
      } else if (stat == SS$_ENDOFFILE) {
         INFIMESSAGE(### end of file,stat)
         StopInterpreter((Widget)gvw);
         ENDMESSAGE1(OutputComplete)
         return;
      } else {
         INFIMESSAGE(### output pipe broken:,stat)
         InterpreterFailed((Widget)gvw);
         ENDMESSAGE1(OutputComplete)
         return;
      }
   }

   if (bytes == 0) { 
      if (received) {
         buf[received++]= '\n';  buf[received++]= '\0';
         received = -1;
      }
   } else if (bytes == 1) {
      buf[received++] = gvw->ghostview.output_buffer[0];
      if (received>77) {
         buf[received++]= '\n';  buf[received++]= '\0';
         received = -1;
      }
   } else if (bytes > 1) {
      memcpy(&(buf[received]), gvw->ghostview.output_buffer, bytes);
      buf[bytes] = '\n'; buf[bytes+1] = '\0';
      received = -1;
   }

   if (received==-1) {
      INFSMESSAGE1(got message:,buf)
      XtCallCallbackList((Widget)gvw,gvw->ghostview.output_callback,(XtPointer)buf);
      received=0;
   }

   Output(gvw);

   ENDMESSAGE1(OutputComplete)
   return;
}

/*###################################################################################*/
/*  CancelOutput */
/*###################################################################################*/

static void
CancelOutput(gvw)
   GhostviewWidget gvw;
{
   BEGINMESSAGE(CancelOutput)
   if (gvw->ghostview.interpreter_output >= 0) {
      INFMESSAGE(aborting possible read requests)
      sys$cancel((short)gvw->ghostview.interpreter_output);
   }
   ENDMESSAGE(CancelOutput)
}

/*###################################################################################*/
/*  SendCommand */
/*###################################################################################*/

static void
SendCommand(gvw)
   GhostviewWidget gvw;
{
   int ret;
   char cmd[GV_BUFSIZ];

   BEGINMESSAGE(SendCommand)

   switch (gvw->ghostview.interpreter_command_number) {
      case 0:
         sprintf(cmd,"$ set noverify");
         break;
      case 1:
         sprintf(cmd,"$ set on");
         break;
      case 2:
         sprintf(cmd,"$ on warning then exit");
         break;
      case 3:
         sprintf(cmd,"$ define__/nolog/proc/user GHOSTVIEW %d",XtWindow((Widget)gvw));
         break;
      case 4:
         sprintf(cmd,"$ define__/nolog/proc/user DECW$DISPLAY %s",XDisplayString(XtDisplay((Widget)gvw)));
         break;
      case 5: {
         char in_mbx_name[256];
         if (gvw->ghostview.filename == NULL) {
            ITEM_LIST_3_T(dvi_list, 1) = {{{64, DVI$_DEVNAM, NULL, NULL}}, 0};
            IOSB_GET_T dvi_iosb;
            dvi_list.item[0].buffer_p = in_mbx_name;
	    ret = sys$getdvi(0,gvw->ghostview.interpreter_input, 0, &dvi_list, &dvi_iosb, 0, 0, 0);
	    ERR_SIGNAL(ret); ERR_SIGNAL(dvi_iosb.status);
	    in_mbx_name[64] = '\0';
         } else {
           if (!strcmp(gvw->ghostview.filename,"-")) {
              char translation_buffer[256];
              unsigned short translation_length;
              long attr=LNM$M_CASE_BLIND;
              ITEM_LIST_3_T(sys_list, 1) = {{{256, LNM$_STRING, "", 0}}, 0};
              $DESCRIPTOR(lnt_desc, "LNM$PROCESS");
              $DESCRIPTOR(sys_desc, "SYS$INPUT");
              sys_list.item[0].buffer_p = translation_buffer;
              sys_list.item[0].buffer_length_p = &translation_length;
              ret = sys$trnlnm(&attr, &lnt_desc, &sys_desc, 0, &sys_list);
              ERR_SIGNAL(ret);
              translation_buffer[translation_length]='\0';
              if (translation_buffer[0]==27) strcpy(in_mbx_name,&(translation_buffer[4]));
              else                           strcpy(in_mbx_name,translation_buffer);
           } else {
              strcpy(in_mbx_name,"NL:");
           }
         }
         sprintf(cmd,"$ define__/nolog/proc/user SYS$INPUT %s",in_mbx_name);
         break;
      }
      case 6:
         sprintf(cmd,"$ %s ",gvw->ghostview.interpreter);
         strcat(cmd," ");
         if (app_res.antialias) strcat(cmd,gv_gs_x11_alpha_device);
         else                   strcat(cmd,gv_gs_x11_device);
         strcat(cmd," \"-dNOPAUSE\"");
         if (gvw->ghostview.safer) strcat(cmd, " \"-dSAFER\"");
         if (gvw->ghostview.quiet) strcat(cmd, " \"-dQUIET\"");
         if (gvw->ghostview.arguments) {strcat(cmd, " "); strcat(cmd,gvw->ghostview.arguments);}
         if (gvw->ghostview.filename != NULL && strcmp(gvw->ghostview.filename,"-")) {
            strcat(cmd," ");
            strcat(cmd,gvw->ghostview.filename);
         }
         strcat(cmd," \"-\"");
         break;
      case 7:
         INFMESSAGE(sending end of file)
         ret = sys$qio(0,(short)gvw->ghostview.interpreter_command,
	         IO$_WRITEOF, &gvw->ghostview.interpreter_command_iosb,
	         NULL,0,
	         NULL, NULL,
	         0,0,0,0
  	      );
         ENDMESSAGE(SendCommand) return;
   }
   (gvw->ghostview.interpreter_command_number)++;

   INFSMESSAGE(sending command:,cmd)
   ret = sys$qio(0,(short)gvw->ghostview.interpreter_command,
	    IO$_WRITEVBLK, &gvw->ghostview.interpreter_command_iosb,
	    SendCommand,gvw,
	    cmd, strlen(cmd),
	    0,0,0,0
  	 );
   ERR_SIGNAL(ret);
   ENDMESSAGE(SendCommand)
}

/*###################################################################################*/
/*  StartInterpreter  */
/*###################################################################################*/

static void
StartInterpreter(w)
    Widget w;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    char buf[GV_BUFSIZ];
    char cmd[512];
    int ret;
    short ch1,ch2,ch3;
    char in_mbx_name[65], out_mbx_name[65],cmd_mbx_name[65];
    long pid, nowait = CLI$M_NOWAIT;
    $DESCRIPTOR(out_desc, "");
    $DESCRIPTOR(cmd_desc, "");
    ITEM_LIST_3_T(dvi_list, 1) = {{{64, DVI$_DEVNAM, NULL, NULL}}, 0};
    IOSB_GET_T dvi_iosb;

    BEGINMESSAGE(StartInterpreter)

    StopInterpreter(w);
    SetBackground(w,True);
    if (gvw->ghostview.disable_start) {
       INFMESSAGE(interpreter start was disabled) ENDMESSAGE(StartInterpreter)
       return;
    }

    INFMESSAGE(switching to busy)
    gvw->ghostview.busy = True;
    ChangeCursor(gvw,CURSOR_BUSY);
    INFMESSAGE(creating output mailbox)
    ret = sys$crembx(0, &ch2, GV_BUFSIZ, GV_BUFSIZ, 0, 0, 0, 0);
    ERR_SIGNAL(ret);
    dvi_list.item[0].buffer_p = out_mbx_name;
    ret = sys$getdvi(0, ch2, 0, &dvi_list, &dvi_iosb, 0, 0, 0);
    ERR_SIGNAL(ret); ERR_SIGNAL(dvi_iosb.status);
    gvw->ghostview.interpreter_output = ch2;
    out_mbx_name[64] = '\0';
    out_desc.dsc$a_pointer = out_mbx_name;
    out_desc.dsc$w_length = strlen(out_mbx_name);

    if (gvw->ghostview.filename == NULL) {
        INFMESSAGE(creating input mailbox)
	ret = sys$crembx(0, &ch1, GV_BUFSIZ, GV_BUFSIZ, 0, 0, 0, 0);
	ERR_SIGNAL(ret);
	dvi_list.item[0].buffer_p = in_mbx_name;
	ret = sys$getdvi(0, ch1, 0, &dvi_list, &dvi_iosb, 0, 0, 0);
	ERR_SIGNAL(ret); ERR_SIGNAL(dvi_iosb.status);
	in_mbx_name[64] = '\0';
	gvw->ghostview.interpreter_input = ch1;
    } else {
        if (strcmp(gvw->ghostview.filename,"-")) in_mbx_name[0] = '\0';
        else strcpy(in_mbx_name,getenv("SYS$INPUT"));
	gvw->ghostview.interpreter_input = 0;
    }

    INFMESSAGE(creating command mailbox)
    ret = sys$crembx(0, &ch3, GV_BUFSIZ, GV_BUFSIZ, 0, 0, 0, 0);
    ERR_SIGNAL(ret);
    dvi_list.item[0].buffer_p = cmd_mbx_name;
    ret = sys$getdvi(0, ch3, 0, &dvi_list, &dvi_iosb, 0, 0, 0);
    ERR_SIGNAL(ret); ERR_SIGNAL(dvi_iosb.status);
    gvw->ghostview.interpreter_command = ch3;
    cmd_mbx_name[64] = '\0';
    cmd_desc.dsc$a_pointer = cmd_mbx_name;
    cmd_desc.dsc$w_length = strlen(cmd_mbx_name);

    gvw->ghostview.interpreter_command_number = 1;
    SendCommand(gvw);

    INFMESSAGE(spawning interpreter process)
    ret = lib$spawn(NULL,&cmd_desc,&out_desc,&nowait,0,&pid,0,0,0,0,0,0,0);
    ERR_SIGNAL(ret);

    /* Everything worked, initialize IOSBs and save info about interpreter. */
    gvw->ghostview.interpreter_pid = pid;
    if (gvw->ghostview.output_buffer == NULL) {
	gvw->ghostview.output_buffer = GV_XtMalloc(GV_BUFSIZ);
    }
    gvw->ghostview.interpreter_input_iosb[0] = SS$_NORMAL;
    gvw->ghostview.interpreter_output_iosb[0] = SS$_NORMAL;

    if (gvw->ghostview.filename != NULL) StartInput(gvw);
    StartOutput(gvw);
    gvw->ghostview.changed = False;
    gvw->ghostview.background_cleared=0;

    ENDMESSAGE(StartInterpreter)
}

/*###################################################################################*/
/*  StopInterpreter  */
/*  Stop the interperter, if present, and remove any Input sources. */
/*  Also reset the busy state. */
/*###################################################################################*/

static void
StopInterpreter(w)
    Widget w;
{
    int ret;
    GhostviewWidget gvw = (GhostviewWidget) w;

    BEGINMESSAGE(StopInterpreter)

    CancelInput(gvw);
    CancelOutput(gvw);
    if (gvw->ghostview.interpreter_pid >= 0) {
       INFMESSAGE(deleting process)
	ret = sys$delprc(&gvw->ghostview.interpreter_pid, 0);
	if(ret != SS$_NORMAL && ret != SS$_NONEXPR)lib$signal(ret, 0, 0);
	gvw->ghostview.interpreter_pid = -1;
    }
    if (gvw->ghostview.interpreter_input >= 0) {
       INFMESSAGE(cleaning input mailbox)
	(void) sys$dassgn(gvw->ghostview.interpreter_input);
	gvw->ghostview.interpreter_input = -1;
	while (gvw->ghostview.ps_input) {
	    struct record_list *ps_old = gvw->ghostview.ps_input;
	    gvw->ghostview.ps_input = ps_old->next;
	    if (ps_old->close) fclose(ps_old->fp);
	    GV_XtFree((char *)ps_old);
	}
    }
    if (gvw->ghostview.interpreter_output >= 0) {
       INFMESSAGE(cleaning output mailbox)
	(void) sys$dassgn(gvw->ghostview.interpreter_output);
	gvw->ghostview.interpreter_output = -1;
    }
    if (gvw->ghostview.interpreter_command >= 0) {
       INFMESSAGE(cleaning command mailbox)
	(void) sys$dassgn(gvw->ghostview.interpreter_command);
    }
    gvw->ghostview.busy = False;
    ChangeCursor(gvw,CURSOR_RESET);

    ENDMESSAGE(StopInterpreter)
}

/*###################################################################################*/
/*  GhostviewSendPS */
/*
 *   Queue a portion of a PostScript file for output to ghostscript.
 *   fp: FILE * of the file in question.  NOTE: if you have several
 *   Ghostview widgets reading from the same file.  You must open
 *   a unique FILE * for each widget.
 *   SendPS does not actually send the PostScript, it merely queues it
 *   for output.
 *   begin: position in file (returned from ftell()) to start.
 *   len:   number of bytes to write.
 *
 *   If an interpreter is not running, nothing is queued and
 *   False is returned.
*/
/*###################################################################################*/

Boolean
GhostviewSendPS(w, fp, begin, len, close)
    Widget w;
    FILE *fp;
    long begin;
    unsigned int len;
    Bool close;
{
    GhostviewWidget gvw = (GhostviewWidget) w;
    struct record_list *ps_new;

    BEGINMESSAGE(GhostviewSendPS)
    if (gvw->ghostview.interpreter_input < 0) {
       INFMESSAGE(no interpreter running) ENDMESSAGE(GhostviewSendPS)
       return False;
    }
    if(len != 0){
	ps_new = (struct record_list *) GV_XtMalloc(sizeof (struct record_list));
	ps_new->fp = fp;
	ps_new->begin = begin;
	ps_new->len = len;
	ps_new->seek_needed = True;
	ps_new->close = close;
	ps_new->next = NULL;

	if (gvw->ghostview.input_buffer == NULL) {
	    gvw->ghostview.input_buffer = GV_XtMalloc(GV_BUFSIZ);
	}

	if (gvw->ghostview.ps_input == NULL) {
	    gvw->ghostview.bytes_left = len;
	    gvw->ghostview.ps_input = ps_new;
	    StartInput(gvw);
	} else {
	    struct record_list *p = gvw->ghostview.ps_input;
	    while (p->next != NULL) {
		p = p->next;
	    }
	    p->next = ps_new;
	}
    }
    ENDMESSAGE(GhostviewSendPS)
    return True;
}
#endif /* VMS */
