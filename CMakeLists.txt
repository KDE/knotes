set(KDEPIM_VERSION_NUMBER "5.8.90")
cmake_minimum_required(VERSION 3.1)
project(knotes VERSION ${KDEPIM_VERSION_NUMBER})


set(KF5_VERSION "5.47.0")

find_package(ECM ${KF5_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${knotes_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
include(ECMInstallIcons)

include(ECMOptionalAddSubdirectory)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMMarkNonGuiExecutable)
include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(CheckFunctionExists)
include(ECMGeneratePriFile)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)
include(ECMCoverageOption)

# Do NOT add quote
set(KDEPIM_DEV_VERSION rc)

# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()



set(KDEPIM_VERSION "${KDEPIM_VERSION_NUMBER}${KDEPIM_DEV_VERSION}")
set(KDEPIM_LIB_VERSION "${KDEPIM_VERSION_NUMBER}")
set(KDEPIM_LIB_SOVERSION "5")


set(AKONADINOTES_LIB_VERSION "5.8.90")
set(AKONADI_VERSION "5.8.90")
set(CALENDARUTILS_LIB_VERSION "5.8.90")
set(KDEPIM_APPS_LIB_VERSION "5.8.90")
set(KONTACTINTERFACE_LIB_VERSION "5.8.90")
set(KPIMTEXTEDIT_LIB_VERSION "5.8.90")
set(LIBKDEPIM_LIB_VERSION "5.8.90")
set(KMIME_LIB_VERSION "5.8.90")
set(PIMCOMMON_LIB_VERSION "5.8.90")


set(QT_REQUIRED_VERSION "5.9.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED DBus Network PrintSupport Test Widgets Xml)
find_package(Qt5X11Extras NO_MODULE)
find_package(Grantlee5 "5.1" CONFIG REQUIRED)
# Find KF5 package
find_package(KF5Completion ${KF5_VERSION} REQUIRED)
find_package(KF5Config ${KF5_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_VERSION} REQUIRED)
find_package(KF5CoreAddons ${KF5_VERSION} REQUIRED)
find_package(KF5Crash ${KF5_VERSION} REQUIRED)
find_package(KF5DBusAddons ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DNSSD ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools ${KF5_VERSION} REQUIRED)
find_package(KF5GlobalAccel ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5IconThemes ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5ItemModels ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5ItemViews ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5KCMUtils ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5NewStuff ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Notifications ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5NotifyConfig ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Parts ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5TextWidgets ${KF5_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF5_VERSION} REQUIRED)
find_package(KF5WindowSystem ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5XmlGui ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Notifications ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5KDELibs4Support ${KF5_VERSION} CONFIG REQUIRED)


# Find KdepimLibs Package
find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiNotes ${AKONADINOTES_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5CalendarUtils ${CALENDARUTILS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5KontactInterface ${KONTACTINTERFACE_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimCommonAkonadi ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimTextEdit ${KPIMTEXTEDIT_LIB_VERSION} CONFIG REQUIRED)


find_package(KF5AkonadiSearch "5.8.90" CONFIG REQUIRED)
set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "http://www.kde.org" TYPE REQUIRED PURPOSE "Provides search capabilities in KMail and Akonadi")

if (NOT APPLE)
    find_package(X11)
endif()

set(KDEPIM_HAVE_X11 ${X11_FOUND})

add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DQT_USE_QSTRINGBUILDER)
set(CMAKE_MODULE_PATH ${knotes_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
find_package(Xsltproc)
set_package_properties(Xsltproc PROPERTIES DESCRIPTION "XSLT processor from libxslt" TYPE REQUIRED PURPOSE "Required to generate D-Bus interfaces for all Akonadi resources.")

include_directories(${knotes_SOURCE_DIR} ${knotes_BINARY_DIR})
add_definitions(-DQT_NO_CAST_FROM_ASCII)
add_definitions(-DQT_NO_CAST_TO_ASCII)

configure_file(knotes-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotes-version.h @ONLY)

add_subdirectory(noteshared)
add_subdirectory(src)
add_subdirectory(notesagent)

install( FILES knotes.categories knotes.renamecategories DESTINATION ${KDE_INSTALL_CONFDIR} )

add_subdirectory(doc)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
