INCLUDES        = -I$(top_srcdir) $(all_includes)

SUBDIRS         = . pics icons

bin_PROGRAMS    = knotes

include_HEADERS = KNotesIface.h
noinst_HEADERS  = knotesapp.h knote.h knoteedit.h knoteconfigdlg.h knotebutton.h knoteslegacy.h resourcenotes.h resourcemanager.h

noinst_LTLIBRARIES = libknotes.la
libknotes_la_SOURCES  = knotesapp.cpp knote.cpp knoteedit.cpp knotebutton.cpp knoteconfigdlg.cpp KNotesIface.skel knoteslegacy.cpp resourcemanager.cpp resourcenotes.cpp resourcelocal.cpp

knotes_SOURCES  = main.cpp
knotes_LDADD    = libknotes.la $(LIB_KIO) -lkresources -lkdeprint $(top_builddir)/libkcal/libkcal.la
knotes_LDFLAGS  = $(all_libraries) $(KDE_RPATH)

kde_module_LTLIBRARIES = knotes_local.la
knotes_local_la_SOURCES = resourcelocal_plugin.cpp
knotes_local_la_LDFLAGS = $(KDE_LDFLAGS) -module $(KDE_PLUGIN)
knotes_local_la_LIBADD  = libknotes.la $(top_builddir)/libkcal/libkcal.la -lkdeprint


METASOURCES     = AUTO

xdg_apps_DATA   = knotes.desktop

ui_DATA         = knotesui.rc
uidir           = $(kde_datadir)/knotes

rc_DATA         = knotesrc
rcdir           = $(kde_confdir)

servicedir = $(kde_servicesdir)/kresources/knotes
service_DATA = local.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/knotes.pot


include ../admin/Doxyfile.am
