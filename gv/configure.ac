dnl configure.ac for gv

dnl Time-stamp: "2004-12-19 19:12:32 jemarch"

dnl Copyright (C) 2004 José E. Marchesi

dnl Please process this file with `autoconf' to get a configure script


AC_INIT(src/ps.c)
AM_INIT_AUTOMAKE(gv,3.6.1)
AM_CONFIG_HEADER(src/ac_config.h)
dnl AM_CONFIG_HEADER(src/ac_config.h)

AM_MAINTAINER_MODE

dnl gv version
VERSION=3.6.1
dnl manual edition
MANUALEDITION=First

AC_SUBST(VERSION)
AC_SUBST(MANUALEDITION)


dnl DEFUNS

AC_DEFUN([AC_XT_VERSION6],
[AC_MSG_CHECKING([For Xt Revision > 4])
AC_CACHE_VAL(XT_cv_version,[
  AC_TRY_LINK([#include <X11/Intrinsic.h>],
[#if XtSpecificationRelease < 5
fail;
#endif
], XT_cv_version="yes",
   [AC_MSG_RESULT(no)])
])
])


dnl Check for programs
AC_PROG_CC

dnl Search for libraries

AC_CHECK_LIB(m, sqrt)
AC_CHECK_LIB(fl, yywrap)

AC_PATH_XTRA()
AC_CHECK_LIB(X11, XOpenDisplay, , , $X_LIBS)
AC_CHECK_LIB(ICE, main, , , $X_LIBS)
AC_CHECK_LIB(SM, main, , , $X_LIBS)
AC_CHECK_LIB(Xext, main, , , $X_LIBS)
AC_CHECK_LIB(Xt, XtOpenDisplay, , , $X_LIBS)
AC_CHECK_LIB(Xmu, main, , , $X_LIBS)
AC_CHECK_LIB(Xpm, main, , , $X_LIBS)
AC_CHECK_LIB([Xaw3d], XawInitializeWidgetSet,,
   AC_ERROR([Unable to find a valid Xaw3d library. Please install it and rerun configure.]),
   $X_LIBS)



dnl WITH options

AC_ARG_WITH(default-papersize, AC_HELP_STRING([--with-default-papersize=PSIZE],[The default papersize (can be changed at runtime). Defaults to `a4']), opt_default_papersize=$withval, opt_default_papersize=a4)

test "$opt_default_papersize" = "yes" && {

     opt_default_papersize=a4

}

PAPERSIZE=$opt_default_papersize
AC_SUBST(PAPERSIZE)

AC_ARG_WITH(print-command, AC_HELP_STRING([--with-print-command=COMMAND],[Command for spool a print job. Defaults for `lpr']), opt_print_command=$withval, opt_print_command=lpr)

test "$opt_print_command" = "yes" && {

     opt_print_command=\"lpr\"

}

PRINT_COMMAND=$opt_print_command
AC_SUBST(PRINT_COMMAND)

AC_ARG_WITH(user-defaults-file, AC_HELP_STRING([--with-user-defaults-file=PATH],[The file containing your preferences. It defaults to `~/.gv'.]), opt_user_defaults_file=$withval, opt_user_defaults_file="~/.gv")

test "$opt_user_defaults_file" = "yes" && {

     opt_user_defaults_file="~/.gv"

}

AC_DEFINE_UNQUOTED(USER_DEFAULTS,"$opt_user_defaults_file", "Define USER_DEFAULTS")

AC_ARG_WITH(scratch-dir,AC_HELP_STRING([--with-scratch-dir=PATH],[Temporary directory. Must be terminated with a trailing `/'. Defaults to `/tmp/']), opt_scratch_dir=$withval, opt_scratch_dir="/tmp/")

test "$opt_scratch_dir" = "yes" && {

     opt_scratch_dir="/tmp/"

}

SCRATCH_DIR=$opt_scratch_dir
AC_SUBST(SCRATCH_DIR)

AC_ARG_WITH(save-dir,AC_HELP_STRING([--with-save-dir=PATH],[The default directory for saving. Must be terminated with a trailing `/'. Defaults to `~/']), opt_save_dir=$withval, opt_save_dir="~/")

test "$opt_save_dir" = "yes" && {

     opt_save_dir="~/"

}

SAVE_DIR=$opt_save_dir
AC_SUBST(SAVE_DIR)


dnl ENABLE options

AC_ARG_ENABLE(ps-level-one, AC_HELP_STRING([--enable-ps-level-one],[Produce Level 1 PostScript]), opt_ps_level_one=$enableval, opt_ps_level_one=no)

test "$opt_ps_level_one" = "yes" && {

     AC_DEFINE(PSLevel1,[], [Define PRODUCE_PS_LEVEL_ONE])     

}

dnl TESTED
AC_ARG_ENABLE(signal-handle, AC_HELP_STRING([--enable-signal-handle],[Use a signal handler intercepting SIGTERM,SIGQUIT, SIGHUP snd SIGINT signals. It requires X11R6. Defaults to `yes']), opt_signal_handle=$enableval, opt_signal_handle=no)

test "x$opt_signal_handle" = "xyes" && {

     AC_DEFINE(USE_SIGNAL_HANDLER,[],[Define USE_SIGNAL_HANDLER])

}

AC_ARG_ENABLE(backing-pixmap, AC_HELP_STRING([--enable-backing-pixmap],[Use a backing pixmap and not rely on the XServer to maintain backing store (can be changed at runtime). Defaults to `yes']), opt_backing_pixmap=$enableval, opt_backing_pixmap=yes)

test "$opt_backing_pixmap" = "yes" && {

dnl     AC_DEFINE(USE_BACKING_PIXMAP, True, [Define USE_BACKING_PIXMAP])
	USE_BACKING_PIXMAP=True

}

test "$opt_backing_pixmap" = "no" && {

     USE_BACKING_PIXMAP=False

}

AC_SUBST(USE_BACKING_PIXMAP)

AC_ARG_ENABLE(scrollbar-code, AC_HELP_STRING([--enable-scrollbar-code],[Use gv's own Motif like scrollbar. It defaults to `yes']), 
[case "${enableval}" in
   yes) opt_scrollbar_code=true ;;
   no)  opt_scrollbar_code=false ;;
   *) AC_MSG_ERROR([bad value ${enableval} for --enable-scrollbar-code]) ;;
 esac], [opt_scrollbar_code=false])
 
AM_CONDITIONAL(USE_SCROLLBAR_CODE, test x$debug = xtrue)

AC_ARG_ENABLE(memmove-code, AC_HELP_STRING([--enable-memmove-code],[Use gv's own memmove. It defaults to `yes']), opt_memmove_code=$enableval, opt_memmove_code=yes)

test "$opt_memmove_code" = "yes" && {

     AC_DEFINE(USE_MEMMOVE_CODE,[],[Define USE_MEMMOVE_CODE])

}

AC_ARG_ENABLE(runtime-messages, AC_HELP_STRING([--enable-runtime-messages],[Generate informative messages at runtime. It defaults to `no']), opt_runtime_messages=$enableval, opt_runtime_messages=no)

test "$opt_runtime_messages" = "yes" && {

     AC_DEFINE(MESSAGES,[],[Define MESSAGES])
     AC_DEFINE(MESSAGE_NO_ESC,[],[Define MESSAGE_NO_ESC])

}

AC_ARG_ENABLE(fallback-styles, AC_HELP_STRING([--enable-fallback-styles],[Compile X resource files into the executable. Defaults to `yes']), opt_fallback_styles=$enableval, opt_fallback_styles=yes)

test "$opt_fallback_styles" = "yes" && {

dnl     USE_FALLBACK_STYLES=-DUSE_FALLBACK_STYLES
     AC_DEFINE(USE_FALLBACK_STYLES,[],[Define USE_FALLBACK_STYLES])

}

AC_ARG_ENABLE(memmove-code, AC_HELP_STRING([--enable-memmove-code],[Use gv's own memmove. It defaults to `yes']), opt_memmove_code=$enableval, opt_memmove_code=yes)

test "$opt_memmove_code" = "yes" && {

     AC_DEFINE(USE_MEMMOVE_CODE,[],[Define USE_MEMMOVE_CODE])

}
AC_ARG_ENABLE(setenv-code, AC_HELP_STRING([--enable-setenv-code],[Use extra code for setenv() and getenv(). Defaults to `yes']),
[case "${enableval}" in
   yes) opt_setenv_code=true ;;
   no)  opt_setenv_code=false ;;
   *) AC_MSG_ERROR(bad value ${enableval} for --enable-setenv-code) ;;
 esac],[opt_setenv_code=false])

AM_CONDITIONAL(USE_SETENV_CODE, test x$opt_setenv_code = xtrue)

AC_ARG_ENABLE(memdebug-code, AC_HELP_STRING([--enable-memdebug-code],[Use extra code for debug memory operations. It defaults to `no']), 
[case "${enableval}" in
   yes) opt_memdebug_code=true ;;
   no)  opt_memdebug_code=false ;;
   *) AC_MSG_ERROR(bad value ${enableval} for --enable-memdebug-code) ;;
 esac],[opt_memdebug_code=false])

test "$opt_memdebug_code" = "true" && {

     AC_DEFINE(GV_FS_XTMEM_DEBUG,[],[Define GV_FS_XTMEM_DEBUG])
     AC_DEFINE(GV_PS_MEM_DEBUG,[],[Define GV_PS_MEM_DEBUG])
     AC_DEFINE(GV_PS_XTMEM_DEBUG,[],[Define GV_PS_XTMEM_DEBUG])
     AC_DEFINE(GV_AAA_MEM_DEBUG,[],[Define GV_AAA_MEM_DEBUG])
     AC_DEFINE(GV_GV_MEM_DEBUG,[],[Define GV_GV_MEM_DEBUG])
     AC_DEFINE(GV_GV_XTMEM_DEBUG,[],[Define GV_GV_XTMEM_DEBUG])
     AC_DEFINE(GV_PROC_XTMEM_DEBUG,[],[Define GV_PROC_XTMEM_DEBUG])
}

AM_CONDITIONAL(USE_MEMDEBUG_CODE, test x$opt_memdebug_code = xtrue)


dnl Generate output files
AC_OUTPUT(Makefile doc/version.texi src/Makefile doc/Makefile)