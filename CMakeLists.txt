project(knotes)
add_subdirectory( icons ) 
add_subdirectory( tests )
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

include_directories(
  ${CMAKE_SOURCE_DIR}/pimcommon
  ${Grantlee_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/noteshared
  ${CMAKE_BINARY_DIR}/noteshared
)

########### next target ###############

set(libknotesprivate_lib_SRCS
    knoteedit.cpp
    configdialog/knotesimpleconfigdialog.cpp
    configdialog/knoteconfigdialog.cpp
    configdialog/knotedisplayconfigwidget.cpp
    configdialog/knoteeditorconfigwidget.cpp
    configdialog/knotecollectionconfigwidget.cpp
    print/knoteprintselectthemedialog.cpp
    print/knoteprintselectthemecombobox.cpp
    print/knoteprinter.cpp
    alarms/knotesalarm.cpp
    alarms/knotealarmdialog.cpp
    print/knoteprintobject.cpp
    utils/knoteutils.cpp
    finddialog/knotefinddialog.cpp
)

configure_file(settings/knotesglobalconfig.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg )

set( libknotesprivate_kcfg_lib_SRCS settings/knotesglobalconfig.kcfgc )

kde4_add_kcfg_files(libknotesprivate_kcfg_lib_SRCS ${libknotesprivate_kcfg_lib_SRCS})

kde4_add_library(knotesprivate ${LIBRARY_TYPE} ${libknotesprivate_lib_SRCS} ${libknotesprivate_kcfg_lib_SRCS} )

target_link_libraries(knotesprivate kdepim ${KDE4_KIO_LIBS} pimcommon ${KDE4_KPRINTUTILS_LIBS} ${KDE4_KCMUTILS_LIBS} ${Grantlee_CORE_LIBRARIES} ${KDE4_KDNSSD_LIBS} ${KDE4_KNEWSTUFF3_LIBS} noteshared akonadi_next)

set_target_properties(knotesprivate
  PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(TARGETS knotesprivate ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)


set(knotes_SRCS 
                apps/main.cpp apps/application.cpp apps/knotesapp.cpp apps/knotesakonaditray.cpp
                dialog/knoteskeydialog.cpp 
                notes/knotebutton.cpp 
                print/knoteprintselectednotesdialog.cpp
                notes/knote.cpp
                notes/knotedisplaysettings.cpp
                dialog/selectednotefolderdialog.cpp
                dialog/knoteselectednotesdialog.cpp
                debug/knotesnepomukdebugdialog.cpp
)


qt4_add_dbus_adaptor(knotes_SRCS  apps/org.kde.KNotes.xml apps/knotesapp.h KNotesApp )

kde4_add_app_icon(knotes_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-apps-knotes.png")

kde4_add_executable(knotes ${knotes_SRCS})

target_link_libraries(knotes knotes2private pimcommon kdepim ${KDE4_KIO_LIBS}
                             ${KDE4_KCMUTILS_LIBS} ${X11_X11_LIB} ${KDEPIMLIBS_KONTACTINTERFACE_LIBS} ${KDE4_KPRINTUTILS_LIBS} ${KDE4_KDNSSD_LIBS} noteshared
  ${KDEPIMLIBS_AKONADI_LIBS}
  ${KDEPIMLIBS_KMIME_LIBS}
  akonadi_next)
install(TARGETS knotes  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############
set(kcm_knotes_PART_SRCS configdialog/knoteconfigdialog.cpp configdialog/knotedisplayconfigwidget.cpp configdialog/knoteeditorconfigwidget.cpp configdialog/knotecollectionconfigwidget.cpp)

kde4_add_plugin(kcm_knote ${kcm_knotes_PART_SRCS} )

target_link_libraries(kcm_knote knotesprivate ${KDEPIMLIBS_AKONADI_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KCMUTILS_LIBS} ${KDE4_KNEWSTUFF3_LIBS} noteshared akonadi_next)

install(TARGETS kcm_knote DESTINATION ${PLUGIN_INSTALL_DIR})


install(FILES 
	data/knote_config_action.desktop
	data/knote_config_display.desktop
	data/knote_config_editor.desktop
	data/knote_config_network.desktop
        data/knote_config_print.desktop
        data/knote_config_collection.desktop
	DESTINATION ${SERVICES_INSTALL_DIR})

########### install files ###############

install( PROGRAMS data/knotes.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES data/knotesappui.rc data/knotesui.rc  DESTINATION ${DATA_INSTALL_DIR}/knotes)
install( FILES apps/org.kde.KNotes.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}) 
install( FILES data/knotes_printing_theme.knsrc DESTINATION ${CONFIG_INSTALL_DIR} )

install(DIRECTORY print/themes/ DESTINATION ${DATA_INSTALL_DIR}/knotes/print/themes )


