add_definitions(-DTRANSLATION_DOMAIN=\"knotes\")

configure_file(config-knotes.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-knotes.h )

add_subdirectory(icons)

if (BUILD_TESTING)
    add_subdirectory(print/autotests)
endif()

add_subdirectory(kontactplugin)
add_subdirectory(kconf_update)

########### next target ###############

add_library(knotesprivate)

target_sources(knotesprivate PRIVATE
    knoteedit.cpp
    knotes_debug.cpp
    notes/knotedisplaysettings.cpp
    utils/knoteutils.cpp
    notes/knoteinterface.cpp
    notes/knotesmigrateapplication.cpp
    finddialog/knotefinddialog.cpp
    configdialog/knotesimpleconfigdialog.cpp
    configdialog/knoteconfigdialog.cpp
    configdialog/knotedisplayconfigwidget.cpp
    configdialog/knoteeditorconfigwidget.cpp
    print/knoteprintselectthemedialog.cpp
    print/knoteprintselectthemecombobox.cpp
    print/knoteprinter.cpp
    print/knoteprintobject.cpp
    print/knotegrantleeprint.cpp
    )
configure_file(settings/knotesglobalconfig.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg )

set( libknotesprivate_kcfg_lib_SRCS settings/knotesglobalconfig.kcfgc )

kconfig_add_kcfg_files(libknotesprivate_kcfg_lib_SRCS ${libknotesprivate_kcfg_lib_SRCS})
ecm_qt_declare_logging_category(knotesprivate HEADER knotes_debug.h IDENTIFIER KNOTES_LOG CATEGORY_NAME org.kde.pim.knotes
        DESCRIPTION "knotes (knotes)"
        OLD_CATEGORY_NAMES log_knotes
        EXPORT KNOTES
    )

target_sources(knotesprivate PRIVATE ${libknotesprivate_kcfg_lib_SRCS})

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(knotesprivate PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(knotesprivate BASE_NAME knotes)

target_link_libraries(knotesprivate
    PUBLIC
    KF5::IconThemes
    PRIVATE
    KF5::PimCommon
    KF5::KCMUtils Grantlee5::Templates KF5::NewStuff KF5::WindowSystem Qt::PrintSupport KF5::TextWidgets KF5::I18n notesharedprivate KF5::AkonadiSearchPIM KF5::AkonadiNotes KF5::PimTextEdit KF5::AkonadiWidgets KF5::GrantleeTheme
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


qt_add_dbus_adaptor(knotes_SRCS  apps/org.kde.KNotes.xml apps/knotesapp.h KNotesApp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-knotes.png")
ecm_add_app_icon(knotes_SRCS ICONS ${ICONS_SRCS})

add_executable(knotes ${knotes_SRCS})
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(knotes PROPERTIES UNITY_BUILD ON)
endif()

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
    KF5::Notifications
    KF5::WindowSystem
    KF5::KIOFileWidgets
    KF5::AkonadiSearchDebug
    KF5::AkonadiNotes
    KF5::Crash
    KF5::AkonadiWidgets
    )
if (Qt5X11Extras_FOUND)
    target_link_libraries(knotes Qt::X11Extras ${X11_LIBRARIES})
endif()
install(TARGETS knotes  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############
set(kcm_knotes_PART_SRCS knoteconfigmodule.cpp configdialog/knotecollectionconfigwidget.cpp)
ecm_qt_declare_logging_category(kcm_knotes_PART_SRCS HEADER knotes_kcm_debug.h IDENTIFIER KNOTES_MODULES_LOG CATEGORY_NAME org.kde.pim.knotes.module
        DESCRIPTION "kcm knotes (knotes)"
        OLD_CATEGORY_NAMES log_knotes_modules
        EXPORT KNOTES
    )

add_library(kcm_knote_static STATIC ${kcm_knotes_PART_SRCS} )
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(kcm_knote_static PROPERTIES UNITY_BUILD ON)
endif()
target_link_libraries(kcm_knote_static knotesprivate KF5::AkonadiCore  KF5::KCMUtils KF5::NewStuff notesharedprivate KF5::AkonadiNotes KF5::PimCommon KF5::I18n KF5::AkonadiWidgets)

install(FILES 
    data/kcm_knote_action.desktop
    data/kcm_knote_display.desktop
    data/kcm_knote_editor.desktop
    data/kcm_knote_network.desktop
    data/kcm_knote_print.desktop
    data/kcm_knote_collection.desktop
    data/kcm_knote_misc.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

kcoreaddons_desktop_to_json(kcm_knote_static "data/kcm_knote_action.desktop" SERVICE_TYPES kcmodule.desktop)
kcoreaddons_desktop_to_json(kcm_knote_static "data/kcm_knote_display.desktop" SERVICE_TYPES kcmodule.desktop)
kcoreaddons_desktop_to_json(kcm_knote_static "data/kcm_knote_editor.desktop" SERVICE_TYPES kcmodule.desktop)
kcoreaddons_desktop_to_json(kcm_knote_static "data/kcm_knote_network.desktop" SERVICE_TYPES kcmodule.desktop)
kcoreaddons_desktop_to_json(kcm_knote_static "data/kcm_knote_print.desktop" SERVICE_TYPES kcmodule.desktop)
kcoreaddons_desktop_to_json(kcm_knote_static "data/kcm_knote_collection.desktop" SERVICE_TYPES kcmodule.desktop)
kcoreaddons_desktop_to_json(kcm_knote_static "data/kcm_knote_misc.desktop" SERVICE_TYPES kcmodule.desktop)


# Create the individual KCMs
kcoreaddons_add_plugin(kcm_knote_action SOURCES kcm_knote_action.cpp INSTALL_NAMESPACE "pim/kcms/knotes")
target_link_libraries(kcm_knote_action kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_display SOURCES kcm_knote_display.cpp INSTALL_NAMESPACE "pim/kcms/knotes")
target_link_libraries(kcm_knote_display kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_editor SOURCES kcm_knote_editor.cpp INSTALL_NAMESPACE "pim/kcms/knotes")
target_link_libraries(kcm_knote_editor kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_network SOURCES kcm_knote_network.cpp INSTALL_NAMESPACE "pim/kcms/knotes")
target_link_libraries(kcm_knote_network kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_print SOURCES kcm_knote_print.cpp INSTALL_NAMESPACE "pim/kcms/knotes")
target_link_libraries(kcm_knote_print kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_collection SOURCES kcm_knote_collection.cpp INSTALL_NAMESPACE "pim/kcms/knotes")
target_link_libraries(kcm_knote_collection kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_misc SOURCES kcm_knote_misc.cpp INSTALL_NAMESPACE "pim/kcms/knotes")
target_link_libraries(kcm_knote_misc kcm_knote_static)

########### install files ###############

install( PROGRAMS data/org.kde.knotes.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install( FILES data/org.kde.knotes.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg  DESTINATION ${KDE_INSTALL_KCFGDIR})
install( FILES data/knotesappui.rc data/knotesui.rc  DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/knotes)
install( FILES apps/org.kde.KNotes.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR}) 
install( FILES data/knotes_printing_theme.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR} )

install(DIRECTORY print/themes/ DESTINATION ${KDE_INSTALL_DATADIR}/knotes/print/themes )
