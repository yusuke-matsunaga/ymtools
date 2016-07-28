## Process this file with automake to produce Makefile.in

SUBDIRS = \
	ym_networks \
	blif \
	iscas89 \
	tgnet \
	.

MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libym_networks.la

libym_networks_la_SOURCES =

libym_networks_la_LIBADD = \
	blif/libym_blif.la \
	iscas89/libym_iscas89.la \
	tgnet/libym_tgnet.la \
	tgnet/blif/libym_tgblifreader.la \
	tgnet/iscas89/libym_tgiscas89reader.la

libym_networks_la_LDFLAGS =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libym_networks_la_SOURCES = dummy.cc
