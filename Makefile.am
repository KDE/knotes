INCLUDES = -I$(top_srcdir)/knotes/qt3stuff $(all_includes)

####### Files

SUBDIRS         = qt3stuff .

bin_PROGRAMS    = knotes

include_HEADERS = KNotesIface.h
noinst_HEADERS  = knotesapp.h knote.h knoteedit.h knoteconfigdlg.h knotebutton.h
knotes_SOURCES  = knotesapp.cpp knote.cpp knoteedit.cpp knoteconfigdlg.cpp main.cpp KNotesIface.skel
knotes_LDADD    = $(LIB_KIO) -lkdeprint qt3stuff/libqt3stuff.la
knotes_LDFLAGS  = $(all_libraries) $(KDE_RPATH)

METASOURCES     = AUTO

KDE_ICON  = knotes

pix_DATA  = knoteslogo.png knotesclose.png
pixdir    = $(kde_datadir)/knotes/pics

apps_DATA = knotes.desktop
appsdir   = $(kde_appsdir)/Utilities

ui_DATA   = knotesui.rc
uidir     = $(kde_datadir)/knotes

rc_DATA   = knotesrc
rcdir     = $(kde_confdir)


messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/knotes.pot

