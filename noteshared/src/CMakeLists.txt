
add_definitions(-DTRANSLATION_DOMAIN=\"libnoteshared\")

set(libnoteshared_akonadi_SRCS
    akonadi/notesakonaditreemodel.cpp
    akonadi/noteschangerecorder.cpp
    )

set(libnoteshared_SRCS
    noteshared_debug.cpp
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
    ${libnoteshared_akonadi_SRCS}
    )

ecm_qt_declare_logging_category(libnoteshared_SRCS HEADER noteshared_debug.h IDENTIFIER NOTESHARED_LOG CATEGORY_NAME log_noteshared)

macro(add_resource_iface _kcfgFile _ifaceName _className)
    kcfg_generate_dbus_interface(${_kcfgFile} ${_ifaceName})
    string(TOLOWER ${_className} _codeFile)
    set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/${_ifaceName}.xml PROPERTIES INCLUDE "resources/metatype.h")
    qt5_add_dbus_interface(libnoteshared_SRCS
        ${CMAKE_CURRENT_BINARY_DIR}/${_ifaceName}.xml ${_codeFile} ${_className}
        )
endmacro()

add_resource_iface( ${CMAKE_CURRENT_SOURCE_DIR}/resources/maildirresource.kcfg
    org.kde.Akonadi.Maildir.Settings MaildirSettings )

add_custom_target(noteshared_settings_xml ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.Maildir.Settings.xml
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/maildirresource.kcfg
    )



set( libnoteshared_kcfg_lib_SRCS settings/notesharedglobalconfig.kcfgc )

kconfig_add_kcfg_files(libnoteshared_kcfg_lib_SRCS ${libnoteshared_kcfg_lib_SRCS})


add_library(notesharedprivate ${libnoteshared_SRCS} ${libnoteshared_kcfg_lib_SRCS} )
generate_export_header(notesharedprivate BASE_NAME noteshared)

target_link_libraries(notesharedprivate
    PUBLIC
    KF5::AkonadiCore
    PRIVATE
    KF5::DNSSD KF5::PimCommon  KF5::Mime KF5::AkonadiNotes KF5::PimTextEdit KF5::Completion Qt5::DBus KF5::I18n
    )


set_target_properties(notesharedprivate
    PROPERTIES VERSION ${KDEPIM_LIB_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION}
    )

install(TARGETS notesharedprivate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
