INCLUDES = $(all_includes)


####### Files
bin_PROGRAMS = knotes

noinst_HEADERS   = knotesapp.h knote.h knoteedit.h knoteconfigdlg.h knotebutton.h
knotes_SOURCES	 = knotesapp.cpp knote.cpp knoteedit.cpp knoteconfigdlg.cpp \
			main.cpp 
knotes_LDADD    = $(LIB_KDEUI) $(LIB_KDECORE)
knotes_LDFLAGS  = $(all_libraries) $(KDE_RPATH)
METASOURCES      = AUTO

KDE_ICON = knotes

pix_DATA = knoteslogo.png knotesclose.png
pixdir   = $(kde_datadir)/knotes/pics/

apps_DATA = knotes.desktop
appsdir   = $(kde_appsdir)/Utilities

rcdir     = $(kde_confdir)
rc_DATA   = knotesrc

messages:
	$(XGETTEXT) $(knotes_SOURCES) -o $(podir)/knotes.pot

