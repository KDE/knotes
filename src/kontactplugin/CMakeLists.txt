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
                      KF6::CalendarCore
                      KPim6::CalendarUtils
                      KPim6::KontactInterface
                      notesharedprivate
                      KF6::DNSSD
                      KPim6::Mime
                      KF6::KCMUtils
                      KPim6::AkonadiWidgets
                      KPim6::AkonadiNotes
                      KF6::ItemViews
                      KF6::Contacts
                      KF6::Completion
)

########### install files ###############

set(kcmknotessummary_PART_SRCS kcmknotessummary.cpp kcmknotessummary.h)
configure_file(knotesplugin.json.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotesplugin.json )
add_library(kcmknotessummary MODULE ${kcmknotessummary_PART_SRCS})
target_link_libraries(kcmknotessummary  KF6::KCMUtils KPim6::PimCommonAkonadi
    KPim6::AkonadiNotes KF6::I18n)


install(TARGETS kcmknotessummary DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim6/kcms/summary)


install(TARGETS kontact_knotesplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim6/kontact)
install(FILES knotes_part.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/knotes)
install(FILES org.kde.kontact.KNotes.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

