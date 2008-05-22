/*
**
** info.c
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

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_XAW(Form.h)
#include INC_XAW(AsciiText.h)
#include INC_X11(Shell.h)
#include "Button.h"

/*### Application dependencies ##############################################*/

#define  INFO_TOPLEVEL			toplevel
#define  INFO_APPLIC_CONTEXT		app_con
#define  INFO_POPUP			infopopup
#define  INFO_POPUP_NAME		"infoPopup"

#include "types.h"
#include "popup.h"
#include "info.h"
#include "main_resources.h"
#include "main_globals.h"

/*### Global Variables ######################################################*/

static Widget   infoform;
static Widget   infodismiss;
static Widget   infotext;
static Bool	infoPopupCreated = False;
static Bool	infoPopupVisible = False;
static int	info_length;

/*###############################################################################
   cb_popupInfoPopup
###############################################################################*/

void cb_popupInfoPopup(w, client_data, call_data)
   Widget	w;
   XtPointer	client_data, call_data;
{
   if (getenv("GV_COMPLETLY_SILENT")) return; /* This feature will be replaced 
                                           in rel. 3.6.5 by a real solution */
   BEGINMESSAGE(cb_popupInfoPopup)
   if (!infoPopupCreated) { makeInfoPopup(); }
   if (infoPopupVisible==True) {INFMESSAGE(InfoPopup already up) ENDMESSAGE(popupInfoPopup) return; }
/* Strange, the whole application gets stuck if we do the following.
   Precisely, the XtSetArg of rootX,rootY in positionPopup
   never comes back ....

   positionPopup(infopopup,2,INFO_TOPLEVEL,50,50, 1,1);
*/
   XtPopup(infopopup, XtGrabNone);
   infoPopupVisible=True;
   ENDMESSAGE(cb_popupInfoPopup)
}         

/*###############################################################################
   cb_popdownInfoPopup
###############################################################################*/

void cb_popdownInfoPopup(w, client_data, call_data)
   Widget	w;
   XtPointer	client_data, call_data;  
{
   BEGINMESSAGE(cb_popdownInfoPopup)
   if (infoPopupCreated && infoPopupVisible) {
      XtPopdown(infopopup);
      infoPopupVisible = False;
   }
   ENDMESSAGE(cb_popdownInfoPopup)
}

/*###############################################################################
   cb_appendInfoPopup
###############################################################################*/

void cb_appendInfoPopup(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    Arg args[5];
    Cardinal n;
    XawTextBlock message_block;

    BEGINMESSAGE(cb_appendInfoPopup)
    SMESSAGE((char*)call_data)
    if (!infoPopupCreated) { makeInfoPopup(); }
    message_block.firstPos = 0;
    message_block.length = strlen(call_data);
    message_block.ptr = call_data;
    message_block.format = FMT8BIT;

    XawTextDisableRedisplay(infotext);
							n=0;
    XtSetArg(args[n], XtNeditType, XawtextAppend);      n++;
    XtSetValues(infotext,args,n);
    XawTextReplace(infotext, info_length, info_length, &message_block);
    info_length = info_length + message_block.length;
							n=0;
    XtSetArg(args[0], XtNeditType, XawtextRead);	n++;
    XtSetArg(args[1], XtNinsertPosition, info_length);	n++;
    XtSetValues(infotext,args,n);
    XawTextEnableRedisplay(infotext);
    if (!infoPopupVisible) cb_popupInfoPopup((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
    ENDMESSAGE(cb_appendInfoPopup)
}

/*###############################################################################
   cb_resetInfoPopup
###############################################################################*/

void cb_resetInfoPopup(w, client_data, call_data)
   Widget	w;
   XtPointer	client_data, call_data;  
{
   Arg args[5];
   Cardinal n;
   XawTextBlock message_block;

   BEGINMESSAGE(resetInfoPopup)
   if ((!infoPopupCreated) || (info_length == 0)) {
      INFMESSAGE(nothing to do) ENDMESSAGE(resetInfoPopup) return;
   }

							n=0;
   XtSetArg(args[n], XtNeditType, XawtextEdit);		n++;
   XtSetArg(args[n], XtNinsertPosition,1);		n++;
   XtSetValues(infotext, args,n);
   message_block.length = 0;
   XawTextReplace(infotext, 0, info_length, &message_block);
   info_length = 0;
							n=0;
   XtSetArg(args[0], XtNeditType, XawtextRead);		n++;
   XtSetValues(infotext,args,n);

   ENDMESSAGE(resetInfoPopup)
}

/*###############################################################################
   makeInfoPopup
###############################################################################*/

void makeInfoPopup()
{
   Arg          args[12];
   Cardinal     n;
   Dimension	bottomMargin, leftMargin, rightMargin, topMargin;
   Dimension	width, height;
   XFontStruct	*font;

   BEGINMESSAGE(makeInfoPopup)

										n=0;
    INFO_POPUP = XtCreatePopupShell(INFO_POPUP_NAME,topLevelShellWidgetClass,INFO_TOPLEVEL,args,n);

    infoform = XtCreateManagedWidget("form", formWidgetClass,INFO_POPUP,args,n);
										n=0;
            XtSetArg(args[n], XtNfromVert,NULL);				n++;
            XtSetArg(args[n], XtNfromHoriz,NULL);				n++;
            XtSetArg(args[n], XtNtop, XtChainTop);				n++;
            XtSetArg(args[n], XtNbottom, XtChainBottom);			n++;
            XtSetArg(args[n], XtNleft, XtChainLeft);				n++;
            XtSetArg(args[n], XtNright, XtChainRight);				n++;
            XtSetArg(args[n], XtNscrollHorizontal, XawtextScrollWhenNeeded);	n++;
            XtSetArg(args[n], XtNscrollVertical, XawtextScrollWhenNeeded);	n++;
            XtSetArg(args[n], XtNdisplayCaret, False);				n++;
    infotext = XtCreateManagedWidget("text", asciiTextWidgetClass,infoform,args,n);

										n=0;
            XtSetArg(args[n], XtNfromVert, infotext);				n++;
            XtSetArg(args[n], XtNfromHoriz,NULL);				n++;
            XtSetArg(args[n], XtNtop, XtChainBottom);				n++;
            XtSetArg(args[n], XtNbottom, XtChainBottom);			n++;
            XtSetArg(args[n], XtNleft, XtChainLeft);				n++;
            XtSetArg(args[n], XtNright, XtChainRight);				n++;
    infodismiss = XtCreateManagedWidget("dismiss", buttonWidgetClass,infoform,args,n);
            XtAddCallback(infodismiss, XtNcallback, cb_popdownInfoPopup,NULL);
            XtInstallAccelerators(infoform, infodismiss);
            XtInstallAccelerators(infotext, infodismiss);
   
										n=0;
            XtSetArg(args[n], XtNfont, &font);					n++;
            XtSetArg(args[n], XtNbottomMargin, &bottomMargin);			n++;
            XtSetArg(args[n], XtNleftMargin, &leftMargin);			n++;
            XtSetArg(args[n], XtNrightMargin, &rightMargin);			n++;
            XtSetArg(args[n], XtNtopMargin, &topMargin);			n++;
    XtGetValues(infotext,args,n);

    width = font->max_bounds.width * 80 + leftMargin + rightMargin;
    height = (font->ascent + font->descent) * 22 + topMargin + bottomMargin;

										n=0;
            XtSetArg(args[0], XtNwidth, width);					n++;
    XtSetValues(infodismiss, args,n);
            XtSetArg(args[1], XtNheight, height);				n++;
    XtSetValues(infotext,args,n);
    
    XtRealizeWidget(INFO_POPUP);
    info_length=0;

   XSetWMProtocols(XtDisplay(INFO_POPUP),XtWindow(INFO_POPUP),&wm_delete_window,1);

   infoPopupCreated=True;
   infoPopupVisible=False;
   ENDMESSAGE(makeInfoPopup)
}
