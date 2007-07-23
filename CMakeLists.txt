project(knotes)
add_subdirectory( pics ) 
add_subdirectory( icons ) 



add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

########### next target ###############


set(libknotesnetwork_SRCS knotesnetrecv.cpp knotesnetsend.cpp)


set(libknoteslegacy_SRCS  knoteslegacy.cpp )

set(libknote_SRCS knote.cpp knotebutton.cpp knoteconfigdlg.cpp knotehostdlg.cpp knotealarmdlg.cpp knotesalarm.cpp )


set(knotes_SRCS main.cpp knotesapp.cpp ${libknoteseditor_SRCS} ${libknotesnetwork_SRCS} ${libknotesresources_SRCS} ${libknoteslegacy_SRCS} ${libknote_SRCS})
qt4_add_dbus_adaptor(knotes_SRCS org.kde.KNotes.xml knotesapp.h KNotesApp )

set( libknotesconfig_SRCS knoteconfig.kcfgc knotesglobalconfig.kcfgc )

kde4_add_kcfg_files(knotes_SRCS ${libknotesconfig_SRCS})


kde4_add_executable(knotes ${knotes_SRCS})

target_link_libraries(knotes  ${KDE4_KCAL_LIBS} kdepim ${KDE4_KRESOURCES_LIBS} ${KDE4_KDEPRINT_LIBS} ${KDE4_KUTILS_LIBS} )

install(TARGETS knotes  DESTINATION ${BIN_INSTALL_DIR} )


########### next target ###############

set(knotes_local_PART_SRCS resourcelocal_plugin.cpp ${libknotesresources_SRCS})


kde4_add_kcfg_files(knotes_local_PART_SRCS ${libknotesconfig_SRCS})

kde4_add_plugin(knotes_local ${knotes_local_PART_SRCS})



target_link_libraries(knotes_local  ${KDE4_KIO_LIBS}  ${KDE4_KRESOURCES_LIBS} ${KDE4_KCAL_LIBS} ${KDE4_KDEPRINT_LIBS} ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS knotes_local  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES knotes.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES knoteconfig.kcfg knotesglobalconfig.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES local.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kresources/knotes)
install( FILES knotes_manager.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kresources)
install( FILES knotesappui.rc knotesui.rc  DESTINATION ${DATA_INSTALL_DIR}/knotes)
install( FILES org.kde.KNotes.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}) 

