# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_notes_agent\")
add_executable(akonadi_notes_agent)


kconfig_add_kcfg_files(notesagent_SRCS
    settings/notesagentsettings.kcfgc
    )


qt_add_dbus_adaptor(notesagent_SRCS org.freedesktop.Akonadi.NotesAgent.xml notesagent.h NotesAgent)
ecm_qt_declare_logging_category(akonadi_notes_agent HEADER notesagent_debug.h IDENTIFIER NOTESAGENT_LOG CATEGORY_NAME org.kde.pim.notesagentagent
        DESCRIPTION "knotes (note agent)"
        OLD_CATEGORY_NAMES log_notesagentagent
        EXPORT KNOTES
    )
target_sources(akonadi_notes_agent PRIVATE
    ${notesagent_SRCS}
    notesagent.cpp
    notesmanager.cpp
    notesagentsettingsdialog.cpp
    notesagentalarmdialog.cpp
    notesagentnotedialog.cpp
    notesagent.h
    notesmanager.h
    notesagentsettingsdialog.h
    notesagentalarmdialog.h
    notesagentnotedialog.h
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(akonadi_notes_agent PROPERTIES UNITY_BUILD ON)
endif()
target_link_libraries(akonadi_notes_agent
    KPim${KF_MAJOR_VERSION}::AkonadiCore
    KPim${KF_MAJOR_VERSION}::Mime
    KPim${KF_MAJOR_VERSION}::AkonadiAgentBase
    KF${KF_MAJOR_VERSION}::NotifyConfig
    notesharedprivate
    KF5::PimCommon
    KPim${KF_MAJOR_VERSION}::PimTextEdit
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::WindowSystem
    KF${KF_MAJOR_VERSION}::Notifications
    KF${KF_MAJOR_VERSION}::IconThemes
    KF${KF_MAJOR_VERSION}::I18n
    KPim${KF_MAJOR_VERSION}::AkonadiNotes
    )

if( APPLE )
    set_target_properties(akonadi_notes_agent PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${knotes_SOURCE_DIR}/notesagent/Info.plist.template)
    set_target_properties(akonadi_notes_agent PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.Akonadi.archivemail")
    set_target_properties(akonadi_notes_agent PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KDE Akonadi Notes")
endif ()

install(TARGETS akonadi_notes_agent ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES settings/notesagentsettings.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(FILES notesagent.desktop DESTINATION "${KDE_INSTALL_DATAROOTDIR}/akonadi/agents")
install(FILES akonadi_notes_agent.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR} )


