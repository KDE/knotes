project(knotes)
add_subdirectory( icons ) 
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )

include_directories(
  ${CMAKE_SOURCE_DIR}/pimcommon
  ${Grantlee_INCLUDE_DIRS}
)

########### next target ###############

set(libknotesprivate_lib_SRCS
    resource/resourcemanager.cpp
    resource/resourcenotes.cpp
    resource/resourcelocal.cpp
    resource/resourcelocalconfig.cpp
    knoteedit.cpp
    print/knoteprinter.cpp
    print/knoteprintobject.cpp
    print/knoteprintselectthemedialog.cpp
    print/knoteprintselectthemecombobox.cpp
    configdialog/knotesimpleconfigdialog.cpp
    configdialog/knoteconfigdialog.cpp
    knoteutils.cpp
    migrations/knoteslegacy.cpp
    alarms/knotesalarm.cpp
    alarms/knotealarmdialog.cpp
    network/knotesnetrecv.cpp
    network/knotesnetsend.cpp
    network/knotehostdialog.cpp
)

configure_file(settings/knotesglobalconfig.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg )

set( libknotesprivate_kcfg_lib_SRCS settings/knoteconfig.kcfgc settings/knotesglobalconfig.kcfgc )

kde4_add_kcfg_files(libknotesprivate_kcfg_lib_SRCS ${libknotesprivate_kcfg_lib_SRCS})

kde4_add_library(knotesprivate ${LIBRARY_TYPE} ${libknotesprivate_lib_SRCS} ${libknotesprivate_kcfg_lib_SRCS} )

target_link_libraries(knotesprivate ${KDEPIMLIBS_KCAL_LIBS} kdepim ${KDEPIMLIBS_KRESOURCES_LIBS} ${KDE4_KIO_LIBS} pimcommon ${KDE4_KPRINTUTILS_LIBS} ${KDE4_KCMUTILS_LIBS} ${Grantlee_CORE_LIBRARIES} ${KDE4_KDNSSD_LIBS})

set_target_properties(knotesprivate
  PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(TARGETS knotesprivate ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)


set(knotes_SRCS main.cpp 
                knoteskeydialog.cpp 
                knotestray.cpp 
                application.cpp 
                knotesapp.cpp 
                print/knoteprintselectednotesdialog.cpp 
                notes/knote.cpp 
                notes/knotebutton.cpp 
)


qt4_add_dbus_adaptor(knotes_SRCS org.kde.KNotes.xml knotesapp.h KNotesApp )

kde4_add_app_icon(knotes_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-apps-knotes.png")

kde4_add_executable(knotes ${knotes_SRCS})

target_link_libraries(knotes knotesprivate pimcommon ${KDEPIMLIBS_KCAL_LIBS} kdepim ${KDEPIMLIBS_KRESOURCES_LIBS} ${KDE4_KIO_LIBS}
                             ${KDE4_KCMUTILS_LIBS} ${X11_X11_LIB} ${KDEPIMLIBS_KONTACTINTERFACE_LIBS} ${KDE4_KPRINTUTILS_LIBS} ${KDE4_KDNSSD_LIBS})

install(TARGETS knotes  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(knotes_local_PART_SRCS resource/resourcelocal_plugin.cpp )


kde4_add_plugin(knotes_local ${knotes_local_PART_SRCS})



target_link_libraries(knotes_local  knotesprivate ${KDE4_KIO_LIBS}  ${KDEPIMLIBS_KRESOURCES_LIBS} ${KDEPIMLIBS_KCAL_LIBS} )

install(TARGETS knotes_local  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############
set(kcm_knotes_PART_SRCS configdialog/knoteconfigdialog.cpp )

kde4_add_plugin(kcm_knote ${kcm_knotes_PART_SRCS} ${libknotesconfig_SRCS})

target_link_libraries(kcm_knote knotesprivate ${KDE4_KDEUI_LIBS} ${KDE4_KCMUTILS_LIBS})

install(TARGETS kcm_knote DESTINATION ${PLUGIN_INSTALL_DIR})


install(FILES 
	data/knote_config_action.desktop
	data/knote_config_display.desktop
	data/knote_config_editor.desktop
	data/knote_config_network.desktop
        data/knote_config_print.desktop
	DESTINATION ${SERVICES_INSTALL_DIR})

########### install files ###############

install( PROGRAMS data/knotes.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES settings/knoteconfig.kcfg ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES data/local.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kresources/knotes)
install( FILES data/knotes_manager.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kresources)
install( FILES data/knotesappui.rc data/knotesui.rc  DESTINATION ${DATA_INSTALL_DIR}/knotes)
install( FILES org.kde.KNotes.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}) 

install(DIRECTORY print/themes/ DESTINATION ${DATA_INSTALL_DIR}/knotes/print/themes )


