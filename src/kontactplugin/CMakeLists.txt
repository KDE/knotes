


########### next target ###############
set(kontact_knotesplugin_PART_SRCS
  knotes_plugin.cpp
  knotes_part.cpp
  summarywidget.cpp
  knotesiconview.cpp
  knoteslistwidgetsearchline.cpp
  knoteswidget.cpp
  knoteseditdialog.cpp
  knotesselectdeletenotesdialog.cpp
)

qt5_add_dbus_adaptor(kontact_knotesplugin_PART_SRCS org.kde.kontact.KNotes.xml knotes_part.h KNotesPart)
qt5_add_dbus_interfaces(kontact_knotesplugin_interface_SRCS org.kde.kontact.KNotes.xml)

ecm_qt_declare_logging_category(kontact_knotesplugin_PART_SRCS HEADER knotes_kontact_plugin_debug.h IDENTIFIER KNOTES_KONTACT_PLUGIN_LOG CATEGORY_NAME log_knoteskontactplugin)


add_library(kontact_knotesplugin MODULE ${kontact_knotesplugin_PART_SRCS} ${kontact_knotesplugin_interface_SRCS} )

target_link_libraries(kontact_knotesplugin
                      knotesprivate
                      KF5::CalendarCore
                      KF5::CalendarUtils
                      KF5::KontactInterface
                      KF5::Libkdepim 
                      notesharedprivate
                      KF5::DNSSD
                      KF5::Mime
                      KF5::KCMUtils
                      KF5::AkonadiWidgets
                      KF5::AkonadiNotes
)

########### install files ###############

set(kcm_knotessummary_PART_SRCS kcmknotessummary.cpp )

add_library(kcm_knotessummary MODULE ${kcm_knotessummary_PART_SRCS})

target_link_libraries(kcm_knotessummary  KF5::PimCommon KF5::AkonadiNotes KF5::ConfigWidgets KF5::I18n)


install(TARGETS kcm_knotessummary DESTINATION ${KDE_INSTALL_PLUGINDIR})

install(FILES kcmknotessummary.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})


install(TARGETS kontact_knotesplugin DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES knotes_part.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/knotes)
install(FILES knotesplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR}/kontact)
install(FILES org.kde.kontact.KNotes.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

install(FILES knotes.setdlg DESTINATION ${KDE_INSTALL_DATADIR}/kontact/ksettingsdialog)
