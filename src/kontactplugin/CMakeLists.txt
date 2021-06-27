


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

qt_add_dbus_adaptor(kontact_knotesplugin_PART_SRCS org.kde.kontact.KNotes.xml knotes_part.h KNotesPart)
qt_add_dbus_interfaces(kontact_knotesplugin_interface_SRCS org.kde.kontact.KNotes.xml)

ecm_qt_declare_logging_category(kontact_knotesplugin_PART_SRCS HEADER knotes_kontact_plugin_debug.h IDENTIFIER KNOTES_KONTACT_PLUGIN_LOG CATEGORY_NAME org.kde.pim.knoteskontactplugin
        DESCRIPTION "knotes (knotes kontact plugin)"
        OLD_CATEGORY_NAMES log_knoteskontactplugin
        EXPORT KNOTES
    )



add_library(kontact_knotesplugin MODULE ${kontact_knotesplugin_PART_SRCS} ${kontact_knotesplugin_interface_SRCS} )
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(kontact_knotesplugin PROPERTIES UNITY_BUILD ON)
endif()
target_link_libraries(kontact_knotesplugin
                      knotesprivate
                      KF5::CalendarCore
                      KF5::CalendarUtils
                      KF5::KontactInterface
                      notesharedprivate
                      KF5::DNSSD
                      KF5::Mime
                      KF5::KCMUtils
                      KF5::AkonadiWidgets
                      KF5::AkonadiNotes
                      KF5::ItemViews
                      KF5::Contacts
)

kcoreaddons_desktop_to_json(kontact_knotesplugin knotesplugin.desktop SERVICE_TYPES kcmodule.desktop)

########### install files ###############

set(kcmknotessummary_PART_SRCS kcmknotessummary.cpp )

add_library(kcmknotessummary MODULE ${kcmknotessummary_PART_SRCS})
kcoreaddons_desktop_to_json(kcmknotessummary "kcmknotessummary.desktop" SERVICE_TYPES kcmodule.desktop)
target_link_libraries(kcmknotessummary  KF5::PimCommonAkonadi KF5::AkonadiNotes KF5::ConfigWidgets KF5::I18n)


install(TARGETS kcmknotessummary DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim/kcms/summary)

install(FILES kcmknotessummary.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})


install(TARGETS kontact_knotesplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/kontact5)
install(FILES knotes_part.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/knotes)
install(FILES knotesplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR}/kontact)
install(FILES org.kde.kontact.KNotes.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

install(FILES knotes.setdlg DESTINATION ${KDE_INSTALL_DATADIR}/kontact/ksettingsdialog)
