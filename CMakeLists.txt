project(knotes)
add_subdirectory( icons ) 
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )

include_directories(
  ${CMAKE_SOURCE_DIR}/pimcommon
)

########### next target ###############


set(libknotesnetwork_SRCS network/knotesnetrecv.cpp network/knotesnetsend.cpp)


set(libknoteslegacy_SRCS  migrations/knoteslegacy.cpp )

set(libknote_SRCS knoteconfigdlg.cpp knote.cpp knotebutton.cpp network/knotehostdlg.cpp knotealarmdlg.cpp knotesalarm.cpp knoteprinter.cpp )


set(knotes_SRCS main.cpp knoteskeydialog.cpp knotestray.cpp application.cpp knotesapp.cpp ${libknoteseditor_SRCS} ${libknotesnetwork_SRCS}
                ${libknotesresources_SRCS} ${libknoteslegacy_SRCS} ${libknote_SRCS})

set( libknotesconfig_SRCS settings/knoteconfig.kcfgc settings/knotesglobalconfig.kcfgc )

kde4_add_kcfg_files(knotes_SRCS ${libknotesconfig_SRCS})


qt4_add_dbus_adaptor(knotes_SRCS org.kde.KNotes.xml knotesapp.h KNotesApp )

kde4_add_app_icon(knotes_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-apps-knotes.png")

kde4_add_executable(knotes ${knotes_SRCS})

target_link_libraries(knotes pimcommon ${KDEPIMLIBS_KCAL_LIBS} kdepim ${KDEPIMLIBS_KRESOURCES_LIBS} ${KDE4_KIO_LIBS}  ${KDE4_KDNSSD_LIBS}
                             ${KDE4_KCMUTILS_LIBS} ${X11_X11_LIB} ${KDEPIMLIBS_KONTACTINTERFACE_LIBS} ${KDE4_KPRINTUTILS_LIBS})

install(TARGETS knotes  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(knotes_local_PART_SRCS resource/resourcelocal_plugin.cpp ${libknotesresources_SRCS})


kde4_add_plugin(knotes_local ${knotes_local_PART_SRCS})



target_link_libraries(knotes_local  ${KDE4_KIO_LIBS}  ${KDEPIMLIBS_KRESOURCES_LIBS} ${KDEPIMLIBS_KCAL_LIBS} )

install(TARGETS knotes_local  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############
set(kcm_knotes_PART_SRCS knoteconfigdlg.cpp )

set( libknotesconfig_SRCS settings/knoteconfig.kcfgc settings/knotesglobalconfig.kcfgc )

kde4_add_kcfg_files(libknotesconfig_SRCS ${libknotesconfig_SRCS})

kde4_add_plugin(kcm_knote ${kcm_knotes_PART_SRCS} ${libknotesconfig_SRCS})

target_link_libraries(kcm_knote  ${KDE4_KDEUI_LIBS} ${KDE4_KCMUTILS_LIBS})

install(TARGETS kcm_knote DESTINATION ${PLUGIN_INSTALL_DIR})


install(FILES 
	data/knote_config_action.desktop
	data/knote_config_display.desktop
	data/knote_config_editor.desktop
	data/knote_config_network.desktop
	data/knote_config_style.desktop
	DESTINATION ${SERVICES_INSTALL_DIR})

########### install files ###############

install( PROGRAMS data/knotes.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES settings/knoteconfig.kcfg settings/knotesglobalconfig.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES data/local.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kresources/knotes)
install( FILES data/knotes_manager.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kresources)
install( FILES knotesappui.rc knotesui.rc  DESTINATION ${DATA_INSTALL_DIR}/knotes)
install( FILES org.kde.KNotes.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}) 

