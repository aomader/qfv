SUBDIRS = libqfv modules

libqfv = $(top_srcdir)/src/libqfv/libqfv.la

AM_CFLAGS += $(GTK_CFLAGS) -DMODULEDIR=\""$(libdir)/qfv/modules"\"
LIBS += $(GTK_LIBS)

bin_PROGRAMS = qfv
qfv_SOURCES = qfv.c qfv-module-manager.c
qfv_LDADD = $(libqfv)
