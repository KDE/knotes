INCLUDES        = -I$(top_srcdir) $(all_includes)

SUBDIRS         = . pics icons

bin_PROGRAMS    = knotes

include_HEADERS = KNotesIface.h KNotesAppIface.h
noinst_HEADERS  = knotesapp.h knote.h knoteedit.h knotebutton.h knoteconfigdlg.h \
                  knoteslegacy.h resourcemanager.h resourcenotes.h resourcelocal.h \
                  knotesnetrecv.h knotesnetsend.h knotehostdlg.h knotealarmdlg.h knotesalarm.h

noinst_LTLIBRARIES = libknote.la libknoteseditor.la libknoteslegacy.la libknotesconfig.la \
                     libknotesresources.la libknotesnetwork.la

libknoteslegacy_la_SOURCES = knoteslegacy.cpp
libknotesconfig_la_SOURCES = knoteconfig.kcfgc knotesglobalconfig.kcfgc
libknoteseditor_la_SOURCES = knoteedit.cpp
libknotesnetwork_la_SOURCES = knotesnetrecv.cpp knotesnetsend.cpp
libknotesresources_la_SOURCES = resourcemanager.cpp resourcenotes.cpp resourcelocal.cpp
libknote_la_SOURCES = knote.cpp knotebutton.cpp knoteconfigdlg.cpp knotehostdlg.cpp \
							 knotealarmdlg.cpp knotesalarm.cpp

knotes_SOURCES  = main.cpp knotesapp.cpp KNotesAppIface.skel
knotes_LDADD    = libknote.la libknoteseditor.la libknotesnetwork.la libknotesresources.la \
                  libknoteslegacy.la libknotesconfig.la  $(top_builddir)/libkcal/libkcal.la \
                  $(top_builddir)/libkdepim/libkdepim.la $(LIB_KIO) -lkresources -lkdeprint -lkutils
knotes_LDFLAGS  = $(all_libraries) $(KDE_RPATH)
knotes_COMPILE_FIRST = libknotesconfig.la

kde_module_LTLIBRARIES  = knotes_local.la
knotes_local_la_SOURCES = resourcelocal_plugin.cpp
knotes_local_la_LDFLAGS = $(KDE_LDFLAGS) -module $(KDE_PLUGIN)
knotes_local_la_LIBADD  = libknotesresources.la libknotesconfig.la \
                          $(top_builddir)/libkcal/libkcal.la -lkdeprint

METASOURCES     = AUTO

xdg_apps_DATA   = knotes.desktop
kde_kcfg_DATA   = knoteconfig.kcfg knotesglobalconfig.kcfg

ui_DATA         = knotesappui.rc knotesui.rc
uidir           = $(kde_datadir)/knotes

service_DATA    = local.desktop
servicedir      = $(kde_servicesdir)/kresources/knotes

manager_DATA    = knotes_manager.desktop
managerdir      = $(kde_servicesdir)/kresources

KDE_OPTIONS = nofinal

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/knotes.pot

include $(top_srcdir)/doc/api/Doxyfile.am
