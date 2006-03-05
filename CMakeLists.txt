kde4_header()

add_subdirectory( pics ) 
add_subdirectory( icons ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(knotes_SRCS main.cpp knotesapp.cpp )

kde4_automoc(${knotes_SRCS})

kde4_add_dcop_skels(knotes_SRCS KNotesAppIface.h )

kde4_add_executable(knotes ${knotes_SRCS})

target_link_libraries(knotes  ${QT_AND_KDECORE_LIBS} knote knoteseditor knotesnetwork knotesresources knoteslegacy knotesconfig kcal kdepim kio kresources kdeprint kutils )

install_targets(/bin knotes )


########### next target ###############


########### next target ###############


########### next target ###############


########### next target ###############


########### next target ###############


########### next target ###############


########### next target ###############

set(knotes_local_PART_SRCS resourcelocal_plugin.cpp )

kde4_automoc(${knotes_local_PART_SRCS})

kde4_add_plugin(knotes_local ${knotes_local_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} knotes_local )

target_link_libraries(knotes_local  ${QT_AND_KDECORE_LIBS} knotesresources knotesconfig kcal kdeprint )

install_targets(${LIB_INSTALL_DIR}/kde4 knotes_local )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES knotes.desktop )
install_files( ${KCFG_INSTALL_DIR} FILES knoteconfig.kcfg knotesglobalconfig.kcfg )
install_files( ${SERVICES_INSTALL_DIR}/kresources/knotes FILES local.desktop )
install_files( ${SERVICES_INSTALL_DIR}/kresources FILES knotes_manager.desktop )
install_files( /include FILES KNotesIface.h KNotesAppIface.h )
install_files( ${DATA_INSTALL_DIR}/knotes FILES knotesappui.rc knotesui.rc )

kde4_create_doxygen_docs(  )

kde4_footer()



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
