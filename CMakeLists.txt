
add_subdirectory( pics ) 
add_subdirectory( icons ) 




########### next target ###############


set(libknotesnetwork_SRCS knotesnetrecv.cpp knotesnetsend.cpp)


set(libknoteslegacy_SRCS  knoteslegacy.cpp )

set(libknote_SRCS knote.cpp knotebutton.cpp knoteconfigdlg.cpp knotehostdlg.cpp knotealarmdlg.cpp knotesalarm.cpp )


set(knotes_SRCS main.cpp knotesapp.cpp ${libknoteseditor_SRCS} ${libknotesnetwork_SRCS} ${libknotesresources_SRCS} ${libknoteslegacy_SRCS} ${libknote_SRCS})
qt4_add_dbus_adaptor(knotes_SRCS org.kde.KNotes.xml knotesapp.h KNotesApp )

set( libknotesconfig_SRCS knoteconfig.kcfgc knotesglobalconfig.kcfgc )

kde4_add_kcfg_files(knotes_SRCS ${libknotesconfig_SRCS})

kde4_automoc(${knotes_SRCS})

kde4_add_executable(knotes ${knotes_SRCS})

target_link_libraries(knotes  ${KDE4_KIO_LIBS} kcal kdepim kresources kdeprint kutils )

install(TARGETS knotes  DESTINATION ${BIN_INSTALL_DIR} )


########### next target ###############

set(knotes_local_PART_SRCS resourcelocal_plugin.cpp ${libknotesresources_SRCS})

kde4_automoc(${knotes_local_PART_SRCS})

kde4_add_kcfg_files(knotes_local_PART_SRCS ${libknotesconfig_SRCS})

kde4_add_plugin(knotes_local ${knotes_local_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} knotes_local )

target_link_libraries(knotes_local  ${KDE4_KIO_LIBS}  kcal kdeprint )

install(TARGETS knotes_local  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES knotes.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES knoteconfig.kcfg knotesglobalconfig.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES local.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kresources/knotes)
install( FILES knotes_manager.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kresources)
install( FILES KNotesIface.h KNotesAppIface.h  DESTINATION ${INCLUDE_INSTALL_DIR} )
install( FILES knotesappui.rc knotesui.rc  DESTINATION ${DATA_INSTALL_DIR}/knotes)




#original Makefile.am contents follow:

#INCLUDES        = -I$(top_srcdir) $(all_includes)
#
#KDE_CXXFLAGS    = -DQT3_SUPPORT_WARNINGS
##KDE_CXXFLAGS    = -UQT3_SUPPORT
#
#SUBDIRS         = . pics icons
#
#bin_PROGRAMS    = knotes
#
#include_HEADERS = KNotesIface.h KNotesAppIface.h
#noinst_HEADERS  = knotesapp.h knote.h knoteedit.h knotebutton.h \
#                  knoteconfigdlg.h knoteslegacy.h resourcemanager.h \
#                  resourcenotes.h resourcelocal.h resourcelocalconfig.h \
#                  knotesnetrecv.h knotesnetsend.h knotehostdlg.h \
#                  knotealarmdlg.h knotesalarm.h
#
#noinst_LTLIBRARIES = libknote.la libknoteseditor.la libknoteslegacy.la libknotesconfig.la \
#                     libknotesresources.la libknotesnetwork.la
#
#libknoteslegacy_la_SOURCES = knoteslegacy.cpp
#libknotesconfig_la_SOURCES = knoteconfig.kcfgc knotesglobalconfig.kcfgc
#libknoteseditor_la_SOURCES = knoteedit.cpp
#libknotesnetwork_la_SOURCES = knotesnetrecv.cpp knotesnetsend.cpp
#libknotesresources_la_SOURCES = resourcemanager.cpp resourcenotes.cpp \
#                                resourcelocal.cpp resourcelocalconfig.cpp
#libknote_la_SOURCES = knote.cpp knotebutton.cpp knoteconfigdlg.cpp knotehostdlg.cpp \
#							 knotealarmdlg.cpp knotesalarm.cpp
#
#knotes_SOURCES  = main.cpp knotesapp.cpp KNotesAppIface.skel
#knotes_LDADD    = libknote.la libknoteseditor.la libknotesnetwork.la libknotesresources.la \
#                  libknoteslegacy.la libknotesconfig.la  $(top_builddir)/libkcal/libkcal.la \
#                  $(top_builddir)/libkdepim/libkdepim.la $(LIB_KIO) -lkresources -lkdeprint -lkutils
#knotes_LDFLAGS  = $(all_libraries) $(KDE_RPATH)
#knotes_COMPILE_FIRST = libknotesconfig.la
#
#kde_module_LTLIBRARIES  = knotes_local.la
#knotes_local_la_SOURCES = resourcelocal_plugin.cpp
#knotes_local_la_LDFLAGS = $(KDE_LDFLAGS) $(all_libraries) -module $(KDE_PLUGIN)
#knotes_local_la_LIBADD  = libknotesresources.la libknotesconfig.la \
#                          $(top_builddir)/libkcal/libkcal.la -lkdeprint
#
#METASOURCES     = AUTO
#
#xdg_apps_DATA   = knotes.desktop
#kde_kcfg_DATA   = knoteconfig.kcfg knotesglobalconfig.kcfg
#
#ui_DATA         = knotesappui.rc knotesui.rc
#uidir           = $(kde_datadir)/knotes
#
#service_DATA    = local.desktop
#servicedir      = $(kde_servicesdir)/kresources/knotes
#
#manager_DATA    = knotes_manager.desktop
#managerdir      = $(kde_servicesdir)/kresources
#
#KDE_OPTIONS = nofinal
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/knotes.pot
#
#include $(top_srcdir)/admin/Doxyfile.am
