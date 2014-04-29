project(knotes)
add_subdirectory( icons ) 
add_subdirectory( tests )
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
kde_enable_exceptions()

include_directories(
  ${CMAKE_SOURCE_DIR}/pimcommon
  ${Grantlee_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/noteshared
  ${CMAKE_BINARY_DIR}/noteshared
  ${BALOO_INCLUDE_DIR}
)

########### next target ###############

set(libknotesprivate_config_lib_SRCS
    configdialog/knotesimpleconfigdialog.cpp
    configdialog/knoteconfigdialog.cpp
    configdialog/knotedisplayconfigwidget.cpp
    configdialog/knoteeditorconfigwidget.cpp
    configdialog/knotecollectionconfigwidget.cpp
)

set(libknotesprivate_print_lib_SRCS
    print/knoteprintselectthemedialog.cpp
    print/knoteprintselectthemecombobox.cpp
    print/knoteprinter.cpp
    print/knoteprintobject.cpp
)

set(libknotesprivate_lib_SRCS
    ${libknotesprivate_config_lib_SRCS}
    ${libknotesprivate_print_lib_SRCS}
    knoteedit.cpp
    notes/knotedisplaysettings.cpp
    utils/knoteutils.cpp
    notes/knoteinterface.cpp
    finddialog/knotefinddialog.cpp
)

configure_file(settings/knotesglobalconfig.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg )

set( libknotesprivate_kcfg_lib_SRCS settings/knotesglobalconfig.kcfgc )

kconfig_add_kcfg_files(libknotesprivate_kcfg_lib_SRCS ${libknotesprivate_kcfg_lib_SRCS})

add_library(knotesprivate ${LIBRARY_TYPE} ${libknotesprivate_lib_SRCS} ${libknotesprivate_kcfg_lib_SRCS} )

target_link_libraries(knotesprivate kdepim KF5::KIOCore pimcommon KF5::KCMUtils Grantlee::Templates KF5::NewStuff noteshared akonadi_next baloopim)

set_target_properties(knotesprivate
  PROPERTIES VERSION ${KDEPIM_LIB_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION}
)

install(TARGETS knotesprivate ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)


set(knotes_SRCS 
                apps/main.cpp 
                apps/application.cpp 
                apps/knotesapp.cpp 
                apps/knotesakonaditray.cpp
                dialog/knoteskeydialog.cpp 
                notes/knotebutton.cpp 
                print/knoteprintselectednotesdialog.cpp
                notes/knote.cpp
                dialog/knoteselectednotesdialog.cpp
)


qt5_add_dbus_adaptor(knotes_SRCS  apps/org.kde.KNotes.xml apps/knotesapp.h KNotesApp )

kde4_add_app_icon(knotes_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-apps-knotes.png")

add_executable(knotes ${knotes_SRCS})

target_link_libraries(knotes knotesprivate pimcommon kdepim KF5::KIOCore
                             KF5::KCMUtils KF5::KontactInterface noteshared
  KF5::AkonadiCore
  KF5::Mime
  akonadi_next)
install(TARGETS knotes  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############
set(kcm_knotes_PART_SRCS configdialog/knoteconfigdialog.cpp configdialog/knotedisplayconfigwidget.cpp configdialog/knoteeditorconfigwidget.cpp configdialog/knotecollectionconfigwidget.cpp)

add_library(kcm_knote MODULE ${kcm_knotes_PART_SRCS} )

target_link_libraries(kcm_knote knotesprivate KF5::AkonadiCore  KF5::KCMUtils KF5::NewStuff noteshared akonadi_next)

install(TARGETS kcm_knote DESTINATION ${PLUGIN_INSTALL_DIR})


install(FILES 
	data/knote_config_action.desktop
	data/knote_config_display.desktop
	data/knote_config_editor.desktop
	data/knote_config_network.desktop
        data/knote_config_print.desktop
        data/knote_config_collection.desktop
        data/knote_config_misc.desktop
	DESTINATION ${SERVICES_INSTALL_DIR})

########### install files ###############

install( PROGRAMS data/knotes.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES data/knotesappui.rc data/knotesui.rc  DESTINATION ${DATA_INSTALL_DIR}/knotes)
install( FILES apps/org.kde.KNotes.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}) 
install( FILES data/knotes_printing_theme.knsrc DESTINATION ${CONFIG_INSTALL_DIR} )

install(DIRECTORY print/themes/ DESTINATION ${DATA_INSTALL_DIR}/knotes/print/themes )


