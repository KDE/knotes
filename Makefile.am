INCLUDES        = -I$(top_srcdir) $(all_includes)

SUBDIRS         = . pics icons

bin_PROGRAMS    = knotes

include_HEADERS = KNotesIface.h
noinst_HEADERS  = knotesapp.h knote.h knoteedit.h knotebutton.h knoteconfigdlg.h \
						knoteslegacy.h resourcemanager.h resourcenotes.h resourcelocal.h \
						knotesnetrecv.h knotesnetsend.h knotehostdlg.h

noinst_LTLIBRARIES = libknotes.la
libknotes_la_SOURCES = knotesapp.cpp knote.cpp knoteedit.cpp knotebutton.cpp \
							  knoteconfigdlg.cpp knoteconfig.kcfgc knotesglobalconfig.kcfgc \
							  KNotesIface.skel knoteslegacy.cpp \
							  resourcemanager.cpp resourcenotes.cpp resourcelocal.cpp \
							  knotesnetrecv.cpp knotesnetsend.cpp knotehostdlg.cpp

knotes_SOURCES  = main.cpp
knotes_LDADD    = libknotes.la $(LIB_KIO) -lkresources -lkdeprint $(top_builddir)/libkcal/libkcal.la
knotes_LDFLAGS  = $(all_libraries) $(KDE_RPATH)

kde_module_LTLIBRARIES = knotes_local.la
knotes_local_la_SOURCES = resourcelocal_plugin.cpp
knotes_local_la_LDFLAGS = $(KDE_LDFLAGS) -module $(KDE_PLUGIN)
knotes_local_la_LIBADD  = libknotes.la $(top_builddir)/libkcal/libkcal.la -lkdeprint


METASOURCES     = AUTO

xdg_apps_DATA   = knotes.desktop
kde_kcfg_DATA   = knotes.kcfg knotesglobalconfig.kcfg

ui_DATA         = knotesui.rc
uidir           = $(kde_datadir)/knotes

service_DATA    = local.desktop
servicedir      = $(kde_servicesdir)/kresources/knotes

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/knotes.pot

include ../admin/Doxyfile.am
