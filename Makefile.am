

###################################################################

LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       $(LIB_KFM) $(LIB_KDEUI) -lm
INCLUDES =	$(all_includes)

####### Files

bin_PROGRAMS = knotes

noinst_HEADERS = knotes.h configdlg.h fontdlg.h mail.h alarm.h \
		bwdatetime.h spin.h timer.h renamedlg.h docking.h

knotes_SOURCES	= knotes.cpp  configdlg.cpp fontdlg.cpp mail.cpp alarm.cpp \
		bwdatetime.cpp spin.cpp timer.cpp savealarm.cpp renamedlg.cpp \
		docking.cpp 

METASOURCES = knotes.moc configdlg.moc fontdlg.moc mail.moc alarm.moc \
	spin.moc renamedlg.moc docking.moc

SUBDIRS = docs

icon_DATA = knotes.xpm
icondir = $(kde_icondir)

pix_DATA = knoteslogo.xpm knotesclose.xpm
pixdir = $(kde_datadir)/knotes/pics/

apps_DATA = knotes.kdelnk
appsdir = $(kde_appsdir)/Utilities

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-knotes.xpm $(kde_minidir)/knotes.xpm
	
uninstall-local:
	rm -f $(kde_minidir)/knotes.xpm

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(knotes_SOURCES) && mv messages.po ../po/knotes.pot

