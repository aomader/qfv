SUBDIRS = libqfv modules

libqfv = $(top_srcdir)/src/libqfv/libqfv.la

AM_CFLAGS = $(GTK_CFLAGS) -DMODULEDIR=\""$(libdir)/qfv/modules"\"

bin_PROGRAMS = qfv
qfv_SOURCES = qfv.c qfv-module-manager.c qfv-window.c
qfv_LDADD = $(libqfv) $(GTK_LIBS) $(GTHREAD_LIBS)
