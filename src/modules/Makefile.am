libqfv = $(top_srcdir)/src/libqfv/libqfv.la

AM_CFLAGS = -I$(top_srcdir)/src $(GTK_CFLAGS)

modulesdir = $(libdir)/qfv/modules
modules_LTLIBRARIES = libmd5sumplugin.la

libmd5sumplugin_la_SOURCES = qfv-md5sum-plugin.c
libmd5sumplugin_la_LIBADD = $(GTK_LIBS) $(libqfv)
libmd5sumplugin_la_LDFLAGS = -avoid-version -module
