project(knotes)

add_definitions(-DTRANSLATION_DOMAIN=\"knotes\")

add_subdirectory(icons)

if (BUILD_TESTING)
   add_subdirectory(print/autotests)
endif()

add_subdirectory(kontactplugin)
add_subdirectory(kconf_update)

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
    print/knotegrantleeprint.cpp
)

set(libknotesprivate_lib_SRCS
    ${libknotesprivate_config_lib_SRCS}
    ${libknotesprivate_print_lib_SRCS}
    knoteedit.cpp
    knotes_debug.cpp
    notes/knotedisplaysettings.cpp
    utils/knoteutils.cpp
    notes/knoteinterface.cpp
    notes/knotesmigrateapplication.cpp
    finddialog/knotefinddialog.cpp
)

configure_file(settings/knotesglobalconfig.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg )

set( libknotesprivate_kcfg_lib_SRCS settings/knotesglobalconfig.kcfgc )

kconfig_add_kcfg_files(libknotesprivate_kcfg_lib_SRCS ${libknotesprivate_kcfg_lib_SRCS})
ecm_qt_declare_logging_category(libknotesprivate_lib_SRCS HEADER knotes_debug.h IDENTIFIER KNOTES_LOG CATEGORY_NAME log_knotes)

add_library(knotesprivate ${libknotesprivate_lib_SRCS} ${libknotesprivate_kcfg_lib_SRCS} )
generate_export_header(knotesprivate BASE_NAME knotes)

target_link_libraries(knotesprivate
PUBLIC
  KF5::IconThemes
PRIVATE
  KF5::PimCommon
  KF5::KCMUtils Grantlee5::Templates KF5::NewStuff KF5::WindowSystem Qt5::PrintSupport KF5::TextWidgets KF5::I18n notesharedprivate KF5::AkonadiSearchPIM KF5::AkonadiNotes KF5::PimTextEdit
)

set_target_properties(knotesprivate
  PROPERTIES VERSION ${KDEPIM_LIB_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION}
)

install(TARGETS knotesprivate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)


set(knotes_SRCS knotes_debug.cpp 
                apps/main.cpp 
                apps/application.cpp 
                apps/knotesapp.cpp 
                apps/knotesakonaditray.cpp
                dialog/knoteskeydialog.cpp 
                dialog/knotedeleteselectednotesdialog.cpp
                notes/knotebutton.cpp 
                print/knoteprintselectednotesdialog.cpp
                notes/knote.cpp
                dialog/knoteselectednotesdialog.cpp
)


qt5_add_dbus_adaptor(knotes_SRCS  apps/org.kde.KNotes.xml apps/knotesapp.h KNotesApp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-knotes.png")
ecm_add_app_icon(knotes_SRCS ICONS ${ICONS_SRCS})

add_executable(knotes ${knotes_SRCS})

target_link_libraries(knotes 
   knotesprivate
   KF5::KCMUtils
   KF5::PimCommon
   KF5::DNSSD
   KF5::KontactInterface
   notesharedprivate
  KF5::AkonadiCore
  KF5::Mime
  KF5::GlobalAccel
  KF5::KDELibs4Support
  KF5::AkonadiSearchDebug
)
if (Qt5X11Extras_FOUND)
  target_link_libraries(knotes Qt5::X11Extras ${X11_LIBRARIES})
endif()
install(TARGETS knotes  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############
set(kcm_knotes_PART_SRCS configdialog/knoteconfigdialog.cpp configdialog/knotedisplayconfigwidget.cpp configdialog/knoteeditorconfigwidget.cpp configdialog/knotecollectionconfigwidget.cpp knotes_debug.cpp)

add_library(kcm_knote MODULE ${kcm_knotes_PART_SRCS} )

target_link_libraries(kcm_knote knotesprivate KF5::AkonadiCore  KF5::KCMUtils KF5::NewStuff KF5::WindowSystem notesharedprivate KF5::AkonadiNotes KF5::PimCommon KF5::I18n)

install(TARGETS kcm_knote DESTINATION ${KDE_INSTALL_PLUGINDIR})


install(FILES 
	data/knote_config_action.desktop
	data/knote_config_display.desktop
	data/knote_config_editor.desktop
	data/knote_config_network.desktop
        data/knote_config_print.desktop
        data/knote_config_collection.desktop
        data/knote_config_misc.desktop
	DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

########### install files ###############

install( PROGRAMS data/org.kde.knotes.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install( FILES data/org.kde.knotes.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg  DESTINATION ${KDE_INSTALL_KCFGDIR})
install( FILES data/knotesappui.rc data/knotesui.rc  DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/knotes)
install( FILES apps/org.kde.KNotes.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR}) 
install( FILES data/knotes_printing_theme.knsrc DESTINATION ${KDE_INSTALL_CONFDIR} )

install(DIRECTORY print/themes/ DESTINATION ${KDE_INSTALL_DATADIR}/knotes/print/themes )
