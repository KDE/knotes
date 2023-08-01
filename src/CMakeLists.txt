# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_definitions(-DTRANSLATION_DOMAIN=\"knotes\")

configure_file(config-knotes.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-knotes.h )

add_subdirectory(icons)

if (BUILD_TESTING)
    add_subdirectory(print/autotests)
endif()

add_subdirectory(kontactplugin)

########### next target ###############

add_library(knotesprivate)

target_sources(knotesprivate PRIVATE
    knoteedit.cpp
    notes/knotedisplaysettings.cpp
    utils/knoteutils.cpp
    notes/knoteinterface.cpp
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

    knoteedit.h
    notes/knotedisplaysettings.h
    utils/knoteutils.h
    notes/knoteinterface.h
    finddialog/knotefinddialog.h
    configdialog/knotesimpleconfigdialog.h
    configdialog/knoteconfigdialog.h
    configdialog/knotedisplayconfigwidget.h
    configdialog/knoteeditorconfigwidget.h
    print/knoteprintselectthemedialog.h
    print/knoteprintselectthemecombobox.h
    print/knoteprinter.h
    print/knoteprintobject.h
    print/knotegrantleeprint.h
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
    KF6::IconThemes
    PRIVATE
    KPim6::PimCommon
    KF6::XmlGui
    KF6::KCMUtils KF6::NewStuffCore
    KF6::WindowSystem Qt::PrintSupport
    KF6::TextWidgets KF6::I18n notesharedprivate
    KPim6::AkonadiSearchPIM KPim6::AkonadiNotes KPim6::PimTextEdit
    KPim6::AkonadiWidgets KPim6::GrantleeTheme
    KF6::TextUtils
    )

if (TARGET KF6::TextAddonsWidgets)
    target_link_libraries(knotesprivate
        PRIVATE
        KF6::TextAddonsWidgets)
endif()

target_link_libraries(knotesprivate PRIVATE KF6::TextTemplate)

set_target_properties(knotesprivate
    PROPERTIES VERSION ${KDEPIM_LIB_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION}
    )

install(TARGETS knotesprivate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)


set(knotes_SRCS
    knotes_debug.cpp
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


    apps/application.h
    apps/knotesapp.h
    apps/knotesakonaditray.h
    dialog/knoteskeydialog.h
    dialog/knotedeleteselectednotesdialog.h
    notes/knotebutton.h
    print/knoteprintselectednotesdialog.h
    notes/knote.h
    dialog/knoteselectednotesdialog.h
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
    KF6::KCMUtils
    KF6::I18n
    KPim6::PimCommon
    KF6::DNSSD
    KPim6::KontactInterface
    notesharedprivate
    KPim6::AkonadiCore
    KPim6::Mime
    KF6::GlobalAccel
    KF6::Notifications
    KF6::WindowSystem
    KF6::KIOFileWidgets
    KPim6::AkonadiSearchDebug
    KPim6::AkonadiNotes
    KF6::Crash
    KF6::XmlGui
    KPim6::AkonadiWidgets
    )
install(TARGETS knotes  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############
set(kcm_knotes_PART_SRCS knoteconfigmodule.cpp knoteconfigmodule.h configdialog/knotecollectionconfigwidget.cpp configdialog/knotecollectionconfigwidget.h)
ecm_qt_declare_logging_category(kcm_knotes_PART_SRCS HEADER knotes_kcm_debug.h IDENTIFIER KNOTES_MODULES_LOG CATEGORY_NAME org.kde.pim.knotes.module
	DESCRIPTION "knotes (kcm knotes)"
        OLD_CATEGORY_NAMES log_knotes_modules
        EXPORT KNOTES
    )

add_library(kcm_knote_static STATIC ${kcm_knotes_PART_SRCS} )
set_property(TARGET kcm_knote_static PROPERTY POSITION_INDEPENDENT_CODE ON)
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(kcm_knote_static PROPERTIES UNITY_BUILD ON)
endif()
target_link_libraries(kcm_knote_static knotesprivate KF6::NewStuffWidgets KPim6::AkonadiCore  KF6::KCMUtils notesharedprivate KPim6::AkonadiNotes KPim6::PimCommon KF6::I18n KPim6::AkonadiWidgets)

# Create the individual KCMs
kcoreaddons_add_plugin(kcm_knote_action SOURCES kcm_knote_action.cpp INSTALL_NAMESPACE "pim6/kcms/knotes")
target_link_libraries(kcm_knote_action kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_display SOURCES kcm_knote_display.cpp INSTALL_NAMESPACE "pim6/kcms/knotes")
target_link_libraries(kcm_knote_display kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_editor SOURCES kcm_knote_editor.cpp INSTALL_NAMESPACE "pim6/kcms/knotes")
target_link_libraries(kcm_knote_editor kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_network SOURCES kcm_knote_network.cpp INSTALL_NAMESPACE "pim6/kcms/knotes")
target_link_libraries(kcm_knote_network kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_print SOURCES kcm_knote_print.cpp INSTALL_NAMESPACE "pim6/kcms/knotes")
target_link_libraries(kcm_knote_print kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_collection SOURCES kcm_knote_collection.cpp INSTALL_NAMESPACE "pim6/kcms/knotes")
target_link_libraries(kcm_knote_collection kcm_knote_static)

kcoreaddons_add_plugin(kcm_knote_misc SOURCES kcm_knote_misc.cpp INSTALL_NAMESPACE "pim6/kcms/knotes")
target_link_libraries(kcm_knote_misc kcm_knote_static)

########### install files ###############

install( PROGRAMS data/org.kde.knotes.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install( FILES data/org.kde.knotes.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/knotesglobalconfig.kcfg  DESTINATION ${KDE_INSTALL_KCFGDIR})
install( FILES data/knotesappui.rc data/knotesui.rc  DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/knotes)
install( FILES apps/org.kde.KNotes.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR}) 
install( FILES data/knotes_printing_theme.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR} )

install(DIRECTORY print/themes/ DESTINATION ${KDE_INSTALL_DATADIR}/knotes/print/themes )
