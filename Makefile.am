INCLUDES        = -I$(top_srcdir) $(all_includes)

SUBDIRS         = . pics icons

lib_LTLIBRARIES = libknotes.la

libknotes_la_SOURCES = knote.cpp knotesapp.cpp knoteedit.cpp knotebutton.cpp knoteconfigdlg.cpp knoteslegacy.cpp KNotesIface.skel
libknotes_la_LIBADD = $(top_builddir)/libkcal/libkcal.la -lkdeprint
libknotes_la_LDFLAGS = -module -avoid-version

kde_module_LTLIBRARIES = libknotespart.la

libknotespart_la_SOURCES = knotes_part.cpp
libknotespart_la_LIBADD = $(LIB_KPARTS) libknotes.la
libknotespart_la_LDFLAGS = $(KDE_PLUGIN)

bin_PROGRAMS    = knotes

include_HEADERS = KNotesIface.h
noinst_HEADERS  = knotesapp.h knote.h knoteedit.h knoteconfigdlg.h knotebutton.h knoteslegacy.h
knotes_SOURCES  = main.cpp
knotes_LDADD    = $(LIB_KIO) libknotes.la
knotes_LDFLAGS  = $(all_libraries) $(KDE_RPATH)

METASOURCES     = AUTO

apps_DATA       = knotes.desktop
appsdir         = $(kde_appsdir)/Utilities

ui_DATA         = knotesui.rc
uidir           = $(kde_datadir)/knotes

rc_DATA         = knotesrc
rcdir           = $(kde_confdir)


messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/knotes.pot


include ../admin/Doxyfile.am
