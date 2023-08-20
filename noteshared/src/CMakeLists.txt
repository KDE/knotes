# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"libnoteshared\")

set(libnoteshared_SRCS
    akonadi/notesakonaditreemodel.cpp
    akonadi/noteschangerecorder.cpp
    attributes/notealarmattribute.cpp
    attributes/notedisplayattribute.cpp
    attributes/notelockattribute.cpp
    attributes/showfoldernotesattribute.cpp
    attributes/attributeregistrar.cpp
    noteutils.cpp
    network/notesnetworksender.cpp
    network/notesnetworkreceiver.cpp
    network/notehostdialog.cpp
    editor/noteeditorutils.cpp
    editor/noteeditor.cpp
    config/noteactionconfig.cpp
    config/notenetworkconfig.cpp
    resources/localresourcecreator.cpp
    job/createnewnotejob.cpp
    dialog/selectednotefolderdialog.cpp
    widget/notelistwidget.cpp
    alarms/notealarmdialog.cpp

    akonadi/notesakonaditreemodel.h
    akonadi/noteschangerecorder.h
    attributes/notealarmattribute.h
    attributes/notedisplayattribute.h
    attributes/notelockattribute.h
    attributes/showfoldernotesattribute.h
    noteutils.h
    network/notesnetworksender.h
    network/notesnetworkreceiver.h
    network/notehostdialog.h
    editor/noteeditorutils.h
    editor/noteeditor.h
    config/noteactionconfig.h
    config/notenetworkconfig.h
    resources/localresourcecreator.h
    job/createnewnotejob.h
    dialog/selectednotefolderdialog.h
    widget/notelistwidget.h
    alarms/notealarmdialog.h

    widget/richtexteditwithautocorrection.cpp
    widget/richtexteditwithautocorrection.h
)

ecm_qt_declare_logging_category(libnoteshared_SRCS HEADER noteshared_debug.h IDENTIFIER NOTESHARED_LOG CATEGORY_NAME org.kde.pim.noteshared
        DESCRIPTION "knotes (noteshared)"
        OLD_CATEGORY_NAMES log_noteshared
        EXPORT KNOTES
    )


macro(add_resource_iface _kcfgFile _ifaceName _className)
    kcfg_generate_dbus_interface(${_kcfgFile} ${_ifaceName})
    set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/${_ifaceName}.xml PROPERTIES INCLUDE "resources/metatype.h")
    qt_add_dbus_interface(libnoteshared_SRCS
        ${CMAKE_CURRENT_BINARY_DIR}/${_ifaceName}.xml ${_className}
        )
endmacro()

add_resource_iface( ${CMAKE_CURRENT_SOURCE_DIR}/resources/maildirresource.kcfg
    org.kde.Akonadi.Maildir.Settings maildirsettings )

add_custom_target(noteshared_settings_xml ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.Maildir.Settings.xml
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/maildirresource.kcfg
    )



set( libnoteshared_kcfg_lib_SRCS settings/notesharedglobalconfig.kcfgc )

kconfig_add_kcfg_files(libnoteshared_kcfg_lib_SRCS ${libnoteshared_kcfg_lib_SRCS})


add_library(notesharedprivate ${libnoteshared_SRCS} ${libnoteshared_kcfg_lib_SRCS} )
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(notesharedprivate PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(notesharedprivate BASE_NAME noteshared)

target_link_libraries(notesharedprivate
    PUBLIC
    KPim6::AkonadiCore
    PRIVATE
    KF6::KCMUtils
    KF6::DNSSD KPim6::PimCommon  KPim6::Mime KPim6::AkonadiNotes
     KF6::Completion Qt::DBus KF6::I18n
    KPim6::AkonadiWidgets
    )
    target_link_libraries(notesharedprivate PRIVATE KF6::TextAutoCorrectionWidgets)

set_target_properties(notesharedprivate
    PROPERTIES VERSION ${KDEPIM_LIB_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION}
    )

install(TARGETS notesharedprivate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
