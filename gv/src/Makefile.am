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


gv_SOURCES = Aaa.c \
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


# Additional actions for resource.o



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