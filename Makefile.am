

###################################################################

LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
LDADD   =       $(LIB_KSYCOCA) -lm
INCLUDES =	$(all_includes)

####### Files

bin_PROGRAMS = knotes

noinst_HEADERS = knotes.h optiondialog.h mail.h alarm.h \
		bwdatetime.h spin.h timer.h renamedlg.h docking.h

knotes_SOURCES	= knotes.cpp optiondialog.cpp mail.cpp alarm.cpp \
		bwdatetime.cpp timer.cpp savealarm.cpp renamedlg.cpp \
		docking.cpp 

METASOURCES = knotes.moc optiondialog.moc mail.moc alarm.moc \
	spin.moc renamedlg.moc docking.moc

KDE_ICON = knotes

pix_DATA = knoteslogo.png knotesclose.png
pixdir = $(kde_datadir)/knotes/pics/

apps_DATA = knotes.desktop
appsdir = $(kde_appsdir)/Utilities

messages:
	$(XGETTEXT) $(knotes_SOURCES) -o $(podir)/knotes.pot

