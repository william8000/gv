# Makefile.am for gv/src
#
# Copyright (C) 2004 JosÃ© E. Marchesi

# Process this file with automake to get Makefile.in

# File distribution
bin_PROGRAMS = gv
bin_SCRIPTS = gv-update-userconfig
dist_pkgdata_DATA = gv_system.ad gv_user.ad gv_class.ad gv_spartan.dat gv_user_res.dat gv_copyright.dat gv_widgetless.dat

appdefaultsdir = $(pkgdatadir)

appdefaults_DATA = ../GV

EXTRA_DIST=ad2c gv_font_res.dat gv_layout_res.dat gv_misc_res.dat Aaa_bison.yacc \
           gv_current.xbm gv_doc.xbm gv_empty.xbm gv_even.xbm gv_icon.xbm gv_odd.xbm \
           gv_selected.xbm gv_unmark.xbm

# Clean files
CLEANFILES = ../GV \
             $(srcdir)/GV.ad \
             $(srcdir)/gv_system.ad \
             $(srcdir)/gv_user.ad \
             $(srcdir)/gv_spartan.h \
             $(srcdir)/gv_widgetless.h \
             $(srcdir)/gv_intern_res_unix.h \
             $(srcdir)/gv_class.ad \
             $(srcdir)/gv_class.h \
             $(srcdir)/gv_make_res.dat \
	     $(srcdir)/gv_intern_res_unix.dat

# Compilation flags
AM_CPPFLAGS = -DNON_BLOCKING_IO -DGV_CODE -DGV_CODE -DGV_LIBDIR=\"$(pkgdatadir)\" \
              -I$(top_srcdir)/lib -I$(top_builddir)/lib -g


AM_CFLAGS = @X_CFLAGS@

LDADD = ../lib/libgnu.a

gv_LDFLAGS = @X_LIBS@

gv_SOURCES = Aaa.c \
             Aaa_bison.c \
             Aaa_bison.h \
             Aaa_lex.c \
             Button.c \
             Button.h \
             ButtonP.h \
             Clip.c \
             Clip.h \
             ClipP.h \
             FileSel.c \
             FileSel.h \
             FileSelP.h \
             Frame.c \
             Frame.h \
             FrameP.h \
             Ghostview.c \
             Ghostview.h \
             GhostviewP.h \
             MButton.c \
             MButton.h \
             MButtonP.h \
             Switch.c \
             Switch.h \
             SwitchP.h \
             Vlist.c \
             Vlist.h \
             VlistP.h \
             actions.c \
             actions.h \
             callbacks.c \
             callbacks.h \
             confirm.c \
             confirm.h \
             config.h \
             dialog.c \
             dialog.h \
             doc_misc.c \
             doc_misc.h \
             error.c \
             error.h \
             file.c \
             file.h \
             info.c \
             info.h \
             magmenu.c \
             magmenu.h \
             main.c \
             main_globals.h \
             main_resources.h \
             media.c \
             media.h \
             message.h \
             misc.c \
             misc.h \
             miscmenu.c \
             miscmenu.h \
             misc_private.c \
             misc_private.h \
             note.c \
             note.h \
             options.c \
             options.h \
             options_fs.c \
             options_fs.h \
             options_gs.c \
             options_gs.h \
             options_gv.c \
             options_gv.h \
             options_setup.c \
             options_setup.h \
             paths.h \
             process.c \
             process.h \
             popup.c \
             popup.h \
             ps.c \
             ps.h \
             resource.c \
             resource.h \
             save.c \
             save.h \
             scale.c \
             scale.h \
             gv_signal.c \
             gv_signal.h \
             version.c \
             version.h \
             versionp.h \
             widgets_misc.c \
             widgets_misc.h \
             zoom.c \
             zoom.h \
             message.h \
             Aaa_intern.h \
             Aaa.h \
             AaaP.h \
             d_memdebug.h \
             d_aaa_xtmem.h \
             d_fs_xtmem.h \
             d_gv_mem.h \
             d_gv_xtmem.h \
             d_proc_xtmem.h \
             d_ps_mem.h \
             d_ps_xtmem.h \
             gv_message.h \
             types.h \
             gv-update-userconfig

BUILT_SOURCES = gv_spartan.h gv_widgetless.h gv_intern_res_unix.h gv_class.h


# Additional rules for generating styles

../GV : $(srcdir)/GV.ad
	@echo "creating GV"
	sed -e 's/^[^!]/!&/' $(srcdir)/GV.ad > ../GV

$(srcdir)/GV.ad : $(srcdir)/gv_system.ad
	@echo "creating GV.ad"
	@cp $(srcdir)/gv_system.ad $(srcdir)/GV.ad

$(srcdir)/gv_system.ad : $(srcdir)/gv_user.ad
	@echo "creating gv_system.ad"
	@cp $(srcdir)/gv_user.ad $(srcdir)/gv_system.ad

$(srcdir)/gv_user.ad : $(srcdir)/gv_user_res.dat $(srcdir)/gv_intern_res_unix.dat $(srcdir)/gv_make_res.dat
	@echo "creating gv_user.ad"
	@echo "!"						>  $(srcdir)/gv_user.ad
	@echo "!  gv_user.ad"				>> $(srcdir)/gv_user.ad
	@echo "!  User specific application defaults for gv"	>> $(srcdir)/gv_user.ad
	@echo "!  Copyright (C) 1995, 1996, 1997  Johannes Plass"	>> $(srcdir)/gv_user.ad
	@echo "!  Copyright (C) 2004,2005,2006,2007 José E. Marchesi"	>> $(srcdir)/gv_user.ad
	@echo "!"						>> $(srcdir)/gv_user.ad
	@cat $(srcdir)/gv_user_res.dat					>> $(srcdir)/gv_user.ad
	@cat $(srcdir)/gv_intern_res_unix.dat				>> $(srcdir)/gv_user.ad
	@cat $(srcdir)/gv_make_res.dat					>> $(srcdir)/gv_user.ad



$(srcdir)/gv_spartan.h : $(srcdir)/gv_spartan.dat
	@sh $(srcdir)/ad2c $(srcdir)/gv_spartan.dat > $(srcdir)/gv_spartan.h

$(srcdir)/gv_widgetless.h : $(srcdir)/gv_widgetless.dat
	@sh $(srcdir)/ad2c $(srcdir)/gv_widgetless.dat > $(srcdir)/gv_widgetless.h

$(srcdir)/gv_intern_res_unix.h : $(srcdir)/gv_intern_res_unix.dat
	@echo "creating gv_intern_res_unix.h"
	@sh $(srcdir)/ad2c $(srcdir)/gv_intern_res_unix.dat > $(srcdir)/gv_intern_res_unix.h

$(srcdir)/gv_intern_res_unix.dat :
	@echo "creating gv_intern_res_unix.dat"
	@echo "" 							 >  $(srcdir)/gv_intern_res_unix.dat
	@echo "!########## gv_intern_res.dat (generated by makefile)" >> $(srcdir)/gv_intern_res_unix.dat
	@echo ""							>> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.gsInterpreter:	gs" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.gsCmdScanPDF:	gs -dNODISPLAY -dQUIET -sPDFname=%s -sDSCname=%s pdf2dsc.ps -c quit" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.gsCmdConvPDF:	gs -dNOPAUSE -dQUIET -dBATCH -sDEVICE=pswrite -sOutputFile=%s -f %s -c save pop quit" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.gsX11Device:		-sDEVICE=x11" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.gsX11AlphaDevice:	-sDEVICE=x11 -dTextAlphaBits=4 -dGraphicsAlphaBits=2 -dMaxBitmap=10000000 -dNOPLATFONTS" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.gsSafer:		True" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.gsQuiet:		True" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.infoVerbose:		Warnings" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.xinerama:		Auto" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.gsArguments:		-dFIXEDMEDIA" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.uncompressCommand:	gzip -d -c %s > %s" >> $(srcdir)/gv_intern_res_unix.dat
	@echo "GV.printCommand:	$(PRINT_COMMAND)" >> $(srcdir)/gv_intern_res_unix.dat


$(srcdir)/gv_class.h : $(srcdir)/gv_class.ad
	@echo "creating gv_class.h"
	@sh $(srcdir)/ad2c $(srcdir)/gv_class.ad > $(srcdir)/gv_class.h

$(srcdir)/gv_class.ad : $(srcdir)/gv_font_res.dat $(srcdir)/gv_layout_res.dat $(srcdir)/gv_make_res.dat $(srcdir)/gv_misc_res.dat $(srcdir)/gv_user_res.dat $(srcdir)/gv_intern_res_unix.dat
	@echo "creating gv_class.ad"
	cp $(srcdir)/gv_copyright.dat $(srcdir)/gv_class.ad
	@echo "!" >> $(srcdir)/gv_class.ad
	@echo "!  gv_class.ad" >> $(srcdir)/gv_class.ad
	@echo "!  Application class defaults for gv" >> $(srcdir)/gv_class.ad
	@echo "!  Copyright (C) 1995, 1996, 1997  Johannes Plass" >> $(srcdir)/gv_class.ad
	@echo "!  Copyright (C) 2004,2005,2006,2007 José E. Marchesi" >> $(srcdir)/gv_class.ad
	@echo "!" >> $(srcdir)/gv_class.ad
	@echo ""  >> $(srcdir)/gv_class.ad
	@cat $(srcdir)/gv_user_res.dat >> $(srcdir)/gv_class.ad
	@cat $(srcdir)/gv_intern_res_unix.dat >> $(srcdir)/gv_class.ad
	@cat $(srcdir)/gv_make_res.dat >> $(srcdir)/gv_class.ad
	@cat $(srcdir)/gv_font_res.dat >> $(srcdir)/gv_class.ad
	@cat $(srcdir)/gv_misc_res.dat >> $(srcdir)/gv_class.ad
	@cat $(srcdir)/gv_layout_res.dat >> $(srcdir)/gv_class.ad


$(srcdir)/gv_make_res.dat :
	@echo "creating gv_make_res.dat"
	@echo ""							>  $(srcdir)/gv_make_res.dat
	@echo  "!########## gv_make_res.dat (generated by makefile)"	>> $(srcdir)/gv_make_res.dat
	@echo ""							>> $(srcdir)/gv_make_res.dat
	@echo "GV.scratchDir:		$(SCRATCH_DIR)"			>> $(srcdir)/gv_make_res.dat
	@echo "GV.defaultSaveDir:	$(SAVE_DIR)"			>> $(srcdir)/gv_make_res.dat
	@echo "GV.fallbackPageMedia:	$(PAPERSIZE)"			>> $(srcdir)/gv_make_res.dat
	@echo "GV.useBackingPixmap:	$(USE_BACKING_PIXMAP)"		>> $(srcdir)/gv_make_res.dat
	@echo "GV*dirs:		Home\n\\"				>> $(srcdir)/gv_make_res.dat
	@echo "			Tmp\n\\"				>> $(srcdir)/gv_make_res.dat
	@echo "			/usr/doc\n\\"				>> $(srcdir)/gv_make_res.dat
	@echo "			/usr/local/doc"				>> $(srcdir)/gv_make_res.dat
	@echo "GV*filter:		no .*"				>> $(srcdir)/gv_make_res.dat
	@echo "GV*filters:		None\n\\"			>> $(srcdir)/gv_make_res.dat
	@echo "			*.*ps* *.pdf*  no  .*\n\\"		>> $(srcdir)/gv_make_res.dat
	@echo "			*.*ps*   no  .*\n\\"			>> $(srcdir)/gv_make_res.dat
	@echo "			*.pdf*  no  .*\n\\"			>> $(srcdir)/gv_make_res.dat
	@echo "			no  .*"					>> $(srcdir)/gv_make_res.dat
	@echo ""							>> $(srcdir)/gv_make_res.dat

# Conditional source files

if USE_SCROLLBAR_CODE
   gv_SOURCES += Scrollbar.c Scrollbar.h ScrollbarP.h
endif

if USE_SETENV_CODE
   gv_SOURCES += setenv.c setenv.h
   AM_CFLAGS += -DUSE_SETENV_CODE
endif

if USE_MEMDEBUG_CODE
   gv_SOURCES += d_mem.c d_mem.h d_xtmem.c d_xtmem.h
endif
