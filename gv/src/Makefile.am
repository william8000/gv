# Makefile.am for gv/src
#
# Copyright (C) 2004 José E. Marchesi

# Process this file with automake to get Makefile.in

# File distribution
bin_PROGRAMS = gv

# Compilation flags
AM_CPPFLAGS = -DNON_BLOCKING_IO -DGV_CODE
AM_CFLAGS = -g
AM_LDFLAGS = -lm $(XAW3D_LIB)


gv_SOURCES = gv_spartan.h \
             gv_intern_res_unix.h \
             gv_class.h \
	     Aaa.c \
             Aaa_bison.c \
             Aaa_lex.c \
             Button.c \
             Clip.c \
             FileSel.c \
             Frame.c \
             Ghostview.c \
             MButton.c \
             Switch.c \
             Vlist.c \
             actions.c \
             callbacks.c \
             confirm.c \
             dialog.c \
             doc_misc.c \
             error.c \
             file.c \
             info.c \
             main.c \
             media.c \
             misc.c \
             miscmenu.c \
             misc_private.c \
             note.c \
             options.c \
             options_fs.c \
             options_gs.c \
             options_gv.c \
             options_setup.c \
             process.c \
             popup.c \
             ps.c \
             resource.c \
             save.c \
             scale.c \
             gv_signal.c \
             version.c \
             widgets_misc.c \
             zoom.c


# Additional rules for generating styles

gv_spartan.h : gv_spartan_dat
	sh ./ad2c gv_spartan_dat > gv_spartan.h

gv_intern_res_unix.h : gv_intern_res_unix.dat
	echo "creating gv_intern_res_unix.h"
	sh ./ad2c gv_intern_res_unix.h gv_intern_res_unix.dat

gv_class.h : gv_class.ad
	echo "creating gv_class.h"
	sh ./ad2c gv_class.ad > gv_class.h

gv_class.ad : gv_font_res.dat gv_layout_res.dat gv_make_res.dat gv_misc_res.dat gv_user_res.dat gv_intern_res_unix.dat
	echo "creating gv_class.ad"
	cp gv_copyright.dat gv_class.ad
	echo "!" >> gv_class.ad
	echo "!  gv_class.ad" >> gv_class.ad
	echo "!  Application class defaults for gv" >> gv_class.ad
	echo "!  Copyright (C) 1995, 1996, 1997  Johannes Plass" >> gv_class.ad
	echo "!  Copyright (C) 2004 José E. Marchesi" >> 
	echo "!" >> gv_class.ad
	echo ""  >> gv_class.ad
	@cat gv_user_res.dat >> gv_class.ad
	@cat gv_intern_res_unix.dat >> gv_class.ad
	@cat gv_make_res.dat >> gv_class.ad
	@cat gv_font_res.dat >> gv_class.ad
	@cat gv_misc_res.dat >> gv_class.ad
	@cat gv_layout_res.dat >> gv_class.ad



# Conditional source files

if USE_SCROLLBAR_CODE
   gv_SOURCES += Scrollbar.c
endif

if USE_SETENV_CODE
   gv_SOURCES += setenv.c getenv.c
endif

if USE_MEMDEBUG_CODE
   gv_SOURCES += d_mem.c d_xtmem.c
endif