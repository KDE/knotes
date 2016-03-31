project(notesagent)

add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_notes_agent\")

set(notesagent_SRCS
    notesagent.cpp
    notesmanager.cpp
    notesagentsettingsdialog.cpp
    notesagentalarmdialog.cpp
    notesagentnotedialog.cpp
)

kconfig_add_kcfg_files(notesagent_SRCS
    settings/notesagentsettings.kcfgc
)


qt5_add_dbus_adaptor(notesagent_SRCS org.freedesktop.Akonadi.NotesAgent.xml notesagent.h NotesAgent)
ecm_qt_declare_logging_category(notesagent_SRCS HEADER notesagent_debug.h IDENTIFIER NOTESAGENT_LOG CATEGORY_NAME log_notesagentagent)

add_executable(akonadi_notes_agent ${notesagent_SRCS})

target_link_libraries(akonadi_notes_agent
  KF5::AkonadiCore
  KF5::Mime
  KF5::AkonadiAgentBase
  KF5::NotifyConfig
  KF5::DBusAddons
  notesharedprivate
  KF5::PimCommon
  KF5::PimTextEdit
  KF5::XmlGui
  KF5::WindowSystem
  KF5::Notifications
  KF5::IconThemes
)

if( APPLE )
  set_target_properties(akonadi_notes_agent PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist.template)
  set_target_properties(akonadi_notes_agent PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.Akonadi.archivemail")
  set_target_properties(akonadi_notes_agent PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KDE Akonadi Notes")
endif ()

install(TARGETS akonadi_notes_agent ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES settings/notesagentsettings.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(FILES notesagent.desktop DESTINATION "${KDE_INSTALL_DATAROOTDIR}/akonadi/agents")
install(FILES akonadi_notes_agent.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR} )

add_subdirectory(kconf_update)

