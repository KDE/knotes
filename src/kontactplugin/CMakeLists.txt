# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause



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

  knotes_plugin.h
  knotes_part.h
  summarywidget.h
  knotesiconview.h
  knoteslistwidgetsearchline.h
  knoteswidget.h
  knoteseditdialog.h
  knotesselectdeletenotesdialog.h
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
                      KF${KF_MAJOR_VERSION}::CalendarCore
                      KF5::CalendarUtils
                      KF5::KontactInterface
                      notesharedprivate
                      KF${KF_MAJOR_VERSION}::DNSSD
                      KPim${KF_MAJOR_VERSION}::Mime
                      KF${KF_MAJOR_VERSION}::KCMUtils
                      KF5::AkonadiWidgets
                      KF5::AkonadiNotes
                      KF${KF_MAJOR_VERSION}::ItemViews
                      KF${KF_MAJOR_VERSION}::Contacts
                      KF${KF_MAJOR_VERSION}::Completion
)

########### install files ###############

set(kcmknotessummary_PART_SRCS kcmknotessummary.cpp kcmknotessummary.h)
configure_file(knotesplugin.json.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotesplugin.json )
add_library(kcmknotessummary MODULE ${kcmknotessummary_PART_SRCS})
target_link_libraries(kcmknotessummary  KF5::PimCommonAkonadi KF5::AkonadiNotes KF${KF_MAJOR_VERSION}::ConfigWidgets KF${KF_MAJOR_VERSION}::I18n)


install(TARGETS kcmknotessummary DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim${QT_MAJOR_VERSION}/kcms/summary)


install(TARGETS kontact_knotesplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim${QT_MAJOR_VERSION}/kontact)
install(FILES knotes_part.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/knotes)
install(FILES org.kde.kontact.KNotes.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

