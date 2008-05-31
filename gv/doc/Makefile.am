# Makefile.am for gv/doc/
# 
# Time-stamp: "2004-12-19 20:16:44 jemarch"
#
# Copyright (C) 2004 José E. Marchesi

# File distribution

EXTRA_DIST= gv.1

info_TEXINFOS = gv.texi
gv_TEXINFOS = gpl.texi version.texi

man_MANS = gv.1

gv.1: ../src/versionp.h ../src/gv_message.h
	help2man ../src/gv > gv.1
