!XCOMM
!XCOMM  descrip.mms
!XCOMM
!XCOMM ** Copyright (C) 1995, 1996, 1997 Johannes Plass
!XCOMM ** Copyright (C) 2004 Jose E. Marchesi
!XCOMM 
!XCOMM  This program is free software; you can redistribute it and/or modify
!XCOMM  it under the terms of the GNU General Public License as published by
!XCOMM  the Free Software Foundation; either version 3 of the License, or
!XCOMM  (at your option) any later version.
!XCOMM 
!XCOMM  This program is distributed in the hope that it will be useful,
!XCOMM  but WITHOUT ANY WARRANTY; without even the implied warranty of
!XCOMM  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!XCOMM  GNU General Public License for more details.
!XCOMM 
!XCOMM  You should have received a copy of the GNU General Public License
!XCOMM  along with GNU gv; see the file COPYING.  If not, write to
!XCOMM  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
!XCOMM  Boston, MA 02111-1307, USA.
!XCOMM 
!XCOMM  Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
!XCOMM            Department of Physics
!XCOMM            Johannes Gutenberg-University
!XCOMM            Mainz, Germany


TARGET = GV
TARGET_TYPE = executable

.include X11_ROOT:[GV]CONFIG.VMS

CC_DEFS = VMS,GV_CODE,GV_LIBDIR="""$(GV_LIBDIR)"""
!MEMDEBUG = 1

.ifdef STATIC
.ifdef USE_FALLBACK_STYLES
.else
USE_FALLBACK_STYLES    = 1
.endif
XMU_LIBRARY   = XMULIB
XAW3D_LIBRARY = XAW3DLIB
.else
XMU_LIBRARY   = XMULIBSHR
XAW3D_LIBRARY = XAW3DLIBSHR
.endif

.ifdef USE_SIGNAL_HANDLER
CC_DEF_USE_SIGNAL_HANDLER = ,USE_SIGNAL_HANDLER
.endif 
.ifdef USE_FALLBACK_STYLES
CC_DEF_USE_FALLBACK_STYLES = ,USE_FALLBACK_STYLES
.endif 

CC_DEFINES = /DEF=($(CC_DEFS)$(CC_DEF_USE_SIGNAL_HANDLER)$(CC_DEF_USE_FALLBACK_STYLES))

.ifdef __DECC__
CC_QUALIFIER = /NODEB/EXT=STRICT/PREF=ALL
.endif

.ifdef __VAXC__
CC_QUALIFIER = /NODEB/OPT=(D,I)
.endif

.include X11_ROOT:[COMMAND]X11_RULES.MMS

!### Objects #################################################################

.ifdef MEMDEBUG
DEBUG_OBJS =\
D_MEM.$(OBJ_EXT),\
D_XTMEM.$(OBJ_EXT),
.endif

.ifdef USE_SIGNAL_HANDLER
SIGNAL_OBJ = SIGNAL.$(OBJ_EXT),
.endif 

OBJS =\
AAA.$(OBJ_EXT),\
AAA_BISON.$(OBJ_EXT),\
AAA_LEX.$(OBJ_EXT),\
ACTIONS.$(OBJ_EXT),\
BUTTON.$(OBJ_EXT),\
CALLBACKS.$(OBJ_EXT),\
CLIP.$(OBJ_EXT),\
CONFIRM.$(OBJ_EXT),\
$(DEBUG_OBJS)\
DIALOG.$(OBJ_EXT),\
DOC_MISC.$(OBJ_EXT),\
ERROR.$(OBJ_EXT),\
FILE.$(OBJ_EXT),\
FILESEL.$(OBJ_EXT),\
FRAME.$(OBJ_EXT),\
GHOSTVIEW.$(OBJ_EXT),\
INFO.$(OBJ_EXT),\
MAGMENU.$(OBJ_EXT),\
MAIN.$(OBJ_EXT),\
MBUTTON.$(OBJ_EXT),\
MEDIA.$(OBJ_EXT),\
MISC.$(OBJ_EXT),\
MISC_PRIVATE.$(OBJ_EXT),\
MISCMENU.$(OBJ_EXT),\
NOTE.$(OBJ_EXT),\
OPTIONS.$(OBJ_EXT),\
OPTIONS_FS.$(OBJ_EXT),\
OPTIONS_GV.$(OBJ_EXT),\
OPTIONS_GS.$(OBJ_EXT),\
OPTIONS_SETUP.$(OBJ_EXT),\
PROCESS.$(OBJ_EXT),\
POPUP.$(OBJ_EXT),\
PS.$(OBJ_EXT),\
RESOURCE.$(OBJ_EXT),\
SAVE.$(OBJ_EXT),\
SCALE.$(OBJ_EXT),\
$(SIGNAL_OBJ)\
STRCASECMP.$(OBJ_EXT),\
SWITCH.$(OBJ_EXT),\
VERSION.$(OBJ_EXT),\
VLIST.$(OBJ_EXT),\
VMS_DIR.$(OBJ_EXT),\
WIDGETS_MISC.$(OBJ_EXT),\
ZOOM.$(OBJ_EXT)

GV_CONFIG_FILE      = [-]config.vms
GV_FONT_RES_DAT     = gv_font_res.dat
GV_MAKE_RES_DAT     = gv_make_res.dat
GV_MISC_RES_DAT     = gv_misc_res.dat
GV_USER_RES_DAT     = gv_user_res.dat
GV_INTERN_RES_DAT   = gv_intern_res_vms.dat
GV_INTERN_RES_H     = gv_intern_res_vms.h
GV_STYLE_1_DAT      = gv_spartan.dat
GV_STYLE_1_H        = gv_spartan.h
GV_LAYOUT_RES_DAT   = gv_layout_res.dat
GV_COPYRIGHT_DAT    = gv_copyright.dat
GV_DUMMY_DAT        = gv_dummy.dat
GV_CLASS_NAME       = GV
GV_CLASS_DAT        = gv_class.dat
GV_CLASS_H          = gv_class.h
GV_SYSTEM_DAT       = gv_system.dat
GV_USER_DAT         = gv_user.dat
GV_SOURCE_IMAKEFILE = descrip.mms

.ifdef USE_FALLBACK_STYLES
GV_STYLE_1_H_TARGET = $(GV_STYLE_1_H)
.else
GV_STYLE_1_H_TARGET =  
.endif

!### Targets ##########################################################

DEFAULT :  $(TARGET).$(EXE_EXT)
 @  write_ sys$output ""
 @  write_ sys$output "  $(TARGET).$(EXE_EXT) available"
 @  write_ sys$output ""

$(TARGET).$(EXE_EXT) :	$(OBJS) $(XMU_LIBRARY) $(XAW3D_LIBRARY)
 @ write_ sys$output "  linking $(TARGET).$(EXE_EXT) ..." 
 @ LINK_/NODEB/NOTRACE -
       /EXE=$(TARGET).$(EXE_EXT) -
       $(OBJS),X11_LIBRARY:XAW3D_CLIENT.OPT/OPT
 @ write_ sys$output "  copying $(GV_SYSTEM_DAT)		   -> X11_ROOT:[DEFAULTS]$(GV_SYSTEM_DAT)"
 @ COPY_/NOLOG/NOCONF $(GV_SYSTEM_DAT) X11_ROOT:[DEFAULTS]$(GV_SYSTEM_DAT);
.ifdef USE_FALLBACK_STYLES
.else
 @ write_ sys$output "  copying $(GV_STYLE_1_DAT)    -> $(GV_LIBDIR)$(GV_STYLE_1_DAT)"
 @ COPY_/NOLOG/NOCONF $(GV_STYLE_1_DAT) $(GV_LIBDIR)$(GV_STYLE_1_DAT);
.endif
 @ write_ sys$output "  copying $(GV_CLASS_DAT)	   -> $(GV_LIBDIR)$(GV_CLASS_DAT)"
 @ COPY_/NOLOG/NOCONF $(GV_CLASS_DAT) $(GV_LIBDIR)$(GV_CLASS_DAT);
 @ write_ sys$output "  copying $(GV_USER_DAT)	   -> $(GV_LIBDIR)$(GV_USER_DAT)"
 @ COPY_/NOLOG/NOCONF $(GV_USER_DAT) $(GV_LIBDIR)$(GV_USER_DAT);
 @ write_ sys$output "  copying $(GV_SYSTEM_DAT)	   -> $(GV_LIBDIR)$(GV_SYSTEM_DAT)"
 @ COPY_/NOLOG/NOCONF $(GV_SYSTEM_DAT) $(GV_LIBDIR)$(GV_SYSTEM_DAT);

resource.$(obj_ext) :  $(GV_CLASS_H) $(GV_INTERN_RES_H)

.ifdef USE_FALLBACK_STYLES
resource.$(obj_ext) :  $(GV_STYLE_1_H)
.endif

$(GV_CLASS_H) : AD2C.$(EXE_EXT) $(GV_CLASS_DAT)
 @ write_ sys$output "  generating $(GV_CLASS_H) ..." 
 @ ad2c__ := $X11_ROOT:[GV.SOURCE]AD2C.$(EXE_EXT)
 @ ad2c__ $(GV_CLASS_DAT) $(GV_CLASS_H)

$(GV_CLASS_DAT) : $(GV_FONT_RES_DAT) $(GV_LAYOUT_RES_DAT)\
		 $(GV_MAKE_RES_DAT) $(GV_MISC_RES_DAT)   $(GV_USER_RES_DAT)\
		$(GV_INTERN_RES_DAT) $(GV_SYSTEM_DAT)
 @ write_ sys$output "  generating $(GV_CLASS_DAT) ..." 
 @ COPY_/NOLOG $(GV_COPYRIGHT_DAT)		$(GV_CLASS_DAT)
 @ OPEN_/APPEND file $(GV_CLASS_DAT);
 @ WRITE_ file "!"
 @ WRITE_ file "!  gv_class.dat"
 @ WRITE_ file "!  Application class defaults for gv."
 @ WRITE_ file "!  Copyright (C) 1995, 1996, 1997  Johannes Plass"
 @ WRITE_ file "!"
 @ WRITE_ file ""
 @ CLOSE_ file
 @ APPEND_ $(GV_USER_RES_DAT)		$(GV_CLASS_DAT);
 @ APPEND_ $(GV_INTERN_RES_DAT)		$(GV_CLASS_DAT);
 @ APPEND_ $(GV_MAKE_RES_DAT)		$(GV_CLASS_DAT);
 @ APPEND_ $(GV_FONT_RES_DAT)		$(GV_CLASS_DAT);
 @ APPEND_ $(GV_MISC_RES_DAT)		$(GV_CLASS_DAT);
 @ APPEND_ $(GV_LAYOUT_RES_DAT)		$(GV_CLASS_DAT);

$(GV_SYSTEM_DAT) : $(GV_USER_DAT)
 @ write_ sys$output "  generating $(GV_SYSTEM_DAT) ..." 
 @ COPY_/NOLOG $(GV_USER_DAT)	$(GV_SYSTEM_DAT);

$(GV_USER_DAT) : 
 @ write_ sys$output "  generating $(GV_USER_DAT) ..." 
 @ COPY_/NOLOG $(GV_DUMMY_DAT)	$(GV_USER_DAT);
 @ OPEN_/APPEND file $(GV_USER_DAT);
 @ WRITE_ file "!"
 @ WRITE_ file "!  gv_user.dat"
 @ WRITE_ file "!  User specific application defaults for gv."
 @ WRITE_ file "!  Copyright (C) 1995, 1996, 1997  Johannes Plass"
 @ WRITE_ file "!"
 @ WRITE_ file ""
 @ CLOSE_ file
 @ APPEND_ $(GV_USER_RES_DAT)		$(GV_USER_DAT);
 @ APPEND_ $(GV_INTERN_RES_DAT)		$(GV_USER_DAT);
 @ APPEND_ $(GV_MAKE_RES_DAT)		$(GV_USER_DAT);

$(GV_MAKE_RES_DAT) : $(GV_CONFIG_FILE) $(GV_SOURCE_IMAKEFILE)
 @ write_ sys$output "  generating $(GV_MAKE_RES_DAT) ..." 
 @ COPY_/NOLOG $(GV_DUMMY_DAT)	$(GV_MAKE_RES_DAT);
 @ OPEN_/APPEND file $(GV_MAKE_RES_DAT);
 @ WRITE_ file ""
 @ WRITE_  file "!########## gv_make_res.dat (generated by makefile)"
 @ WRITE_ file ""
 @ WRITE_ file "GV.scratchDir:		$(SCRATCH_DIR)"
 @ WRITE_ file "GV.defaultSaveDir:	$(SAVE_DIR)"
 @ WRITE_ file "GV.fallbackPageMedia:	$(PAPERSIZE)"
 @ WRITE_ file "GV.useBackingPixmap:	$(USE_BACKING_PIXMAP)"
 @ WRITE_ file "GV*fileSelPopup*fileSel*pathtext*font:	-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1"
 @ WRITE_ file "GV*fileSelPopup*fileSel.minimumWidth:	480"
 @ WRITE_ file "GV*dirs:		Home\n\"
 @ WRITE_ file "			Tmp"
 @ WRITE_ file "GV*filter:		"
 @ WRITE_ file "GV*filters:		None\n\"
 @ WRITE_ file "			*.*ps* *.pdf*\n\"
 @ WRITE_ file "			*.*ps*\n\"
 @ WRITE_ file "			*.pdf*"
 @ WRITE_ file ""
 @ CLOSE_ file

.ifdef PRODUCE_PS_LEVEL_ONE
PS_LEVEL = ""-dPSLevel1""
.endif

$(GV_INTERN_RES_H) : AD2C.$(EXE_EXT) 
 @ write_ sys$output "  generating $(GV_INTERN_RES_H) ..." 
 @ ad2c__ := $X11_ROOT:[GV.SOURCE]AD2C.$(EXE_EXT)
 @ ad2c__ $(GV_INTERN_RES_DAT) $(GV_INTERN_RES_H)

$(GV_INTERN_RES_DAT) : $(GV_CONFIG_FILE) $(GV_SOURCE_IMAKEFILE)
 @ write_ sys$output "  generating $(GV_INTERN_RES_DAT) ..." 
 @ COPY_/NOLOG $(GV_DUMMY_DAT)	$(GV_INTERN_RES_DAT);
 @ OPEN_/APPEND file $(GV_INTERN_RES_DAT);
 @ WRITE_ file ""
 @ WRITE_ file "!########## gv_intern_res.dat (generated by makefile)"
 @ WRITE_ file ""
 @ WRITE_ file "GV.gsInterpreter:	gs"
 @ WRITE_ file "GV.gsCmdScanPDF:	gs ""-dNODISPLAY"" ""-dQUIET"" ""-sPDFname""=%s ""-sDSCname""=%s pdf2dsc.ps -c quit"
 @ WRITE_ file "GV.gsCmdConvPDF:	gs ""-dNODISPLAY"" ""-dQUIET"" $(PS_LEVEL) ""-dNOPAUSE"" ""-sPSFile""=%s %s -c quit"
 @ WRITE_ file "GV.gsX11Device:	""-sDEVICE=x11"""
 @ WRITE_ file "GV.gsX11AlphaDevice:""-dNOPLATFONTS"" ""-sDEVICE=x11alpha"""
 @ WRITE_ file "GV.gsSafer:	True"
 @ WRITE_ file "GV.gsQuiet:	True"
 @ WRITE_ file "GV.gsArguments:"
 @ WRITE_ file "GV.printCommand:	$(PRINT_COMMAND)"
 @ WRITE_ file "GV.uncompressCommand:gzip -d -c %s > %s"
 @ CLOSE_ file

$(GV_STYLE_1_H) : AD2C.$(EXE_EXT) $(GV_STYLE_1_DAT)
 @ write_ sys$output "  generating $(GV_STYLE_1_H) ..." 
 @ ad2c__ := $X11_ROOT:[GV.SOURCE]AD2C.$(EXE_EXT)
 @ ad2c__ $(GV_STYLE_1_DAT) $(GV_STYLE_1_H)

AD2C.$(EXE_EXT) :
 @ write_ sys$output "  compiling: AD2C.C		   -> AD2C.$(OBJ_EXT)" 
 @ CC/NODEB AD2C.C/OBJ=AD2C.$(OBJ_EXT)
 @ write_ sys$output "  linking AD2C.$(EXE_EXT) ..."
 @ LINK/NOTRACE/NOMAP AD2C.$(OBJ_EXT)/EXE=AD2C.$(EXE_EXT)
 @ write_ sys$output "  AD2C.$(EXE_EXT) available."




