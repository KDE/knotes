
ICONSDIR  = @kde_icondir@
PIXDIR  = @kde_datadir@/knotes/pics/
APPSDIR = @kde_appsdir@/Utilities

###################################################################

LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       -lkfm -lkdeui -lkdecore -lqt -lm -lXext -lX11 $(LIBSOCKET)
INCLUDES =	$(all_includes)

####### Files

bin_PROGRAMS = knotes

noinst_HEADERS = knotes.h configdlg.h fontdlg.h mail.h alarm.h\
		bwdatetime.h spin.h timer.h renamedlg.h

knotes_SOURCES	= knotes.cpp  configdlg.cpp fontdlg.cpp mail.cpp alarm.cpp\
		bwdatetime.cpp spin.cpp timer.cpp savealarm.cpp renamedlg.cpp

BUILTSOURCES = knotes.moc configdlg.moc fontdlg.moc mail.moc alarm.moc \
	spin.moc renamedlg.moc

SUBDIRS = docs

icon_DATA = knotes.xpm
icondir = $(kde_icondir)

pix_DATA = knoteslogo.xpm knotesclose.xpm
pixdir = $(kde_datadir)/knotes/pics/

apps_DATA = knotes.kdelnk
appsdir = $(kde_appsdir)/Utilities

messages:
	@XGETTEXT@ -C -ktranslate -x../../kdelibs/po/kde.pot *.cpp && mv messages.po ../po/knotes.pot

####### Explicit dependencies
knotes.cpp: knotes.moc
knotes.moc: knotes.h
	$(MOC) knotes.h -o knotes.moc

configdlg.cpp: configdlg.moc
configdlg.moc: configdlg.h
	$(MOC) configdlg.h -o configdlg.moc

fontdlg.cpp: fontdlg.moc
fontdlg.moc: fontdlg.h
	$(MOC) fontdlg.h -o fontdlg.moc

mail.cpp: mail.moc
mail.moc: mail.h
	$(MOC) mail.h -o mail.moc

alarm.cpp: alarm.moc
alarm.moc: alarm.h
	$(MOC) alarm.h -o alarm.moc

spin.cpp: spin.moc
spin.moc: spin.h
	$(MOC) spin.h -o spin.moc

renamedlg.cpp: renamedlg.moc
renamedlg.moc: renamedlg.h
	$(MOC) renamedlg.h -o renamedlg.moc








