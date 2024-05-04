# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
set(PIM_VERSION "6.1.40")
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(knotes VERSION ${PIM_VERSION})
set(RELEASE_SERVICE_VERSION "24.08.40")

set(KF_MIN_VERSION "6.1.0")
set(QT_REQUIRED_VERSION "6.6.0")
find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${knotes_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
include(ECMInstallIcons)


include(ECMSetupVersion)
include(ECMAddTests)

include(GenerateExportHeader)
include(ECMGenerateHeaders)

include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
include(CheckFunctionExists)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMFeatureSummary)

# Do NOT add quote
set(KDEPIM_DEV_VERSION alpha)

# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()



set(KDEPIM_VERSION "${PIM_VERSION}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")
set(KDEPIM_LIB_VERSION "${PIM_VERSION}")
set(KDEPIM_LIB_SOVERSION "6")

set(KTEXTADDONS_MIN_VERSION "1.5.4")
set(AKONADINOTES_LIB_VERSION "6.1.40")
set(AKONADI_VERSION "6.1.40")
set(CALENDARUTILS_LIB_VERSION "6.1.40")
set(KONTACTINTERFACE_LIB_VERSION "6.1.40")
set(KMIME_LIB_VERSION "6.1.40")
set(PIMCOMMON_LIB_VERSION "6.1.40")
set(GRANTLEETHEME_LIB_VERSION "6.1.40")
set(AKONADI_SEARCH_VERSION "6.1.40")

find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED DBus Network PrintSupport Widgets Xml)
# Find KF6 package
find_package(KF6Completion ${KF_MIN_VERSION} REQUIRED)
find_package(KF6Config ${KF_MIN_VERSION} REQUIRED)
find_package(KF6CoreAddons ${KF_MIN_VERSION} REQUIRED)
find_package(KF6Crash ${KF_MIN_VERSION} REQUIRED)
find_package(KF6DNSSD ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6GlobalAccel ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6I18n ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6IconThemes ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6ItemModels ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6ItemViews ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6KCMUtils ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6NewStuff ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Notifications ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6NotifyConfig ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Parts ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6TextWidgets ${KF_MIN_VERSION} REQUIRED)
find_package(KF6WidgetsAddons ${KF_MIN_VERSION} REQUIRED)
find_package(KF6WindowSystem ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6XmlGui ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6StatusNotifierItem ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6DocTools ${KF_MIN_VERSION})
set_package_properties(KF6DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)


# Find KdepimLibs Package
find_package(KPim6Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KPim6AkonadiNotes ${AKONADINOTES_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6CalendarUtils ${CALENDARUTILS_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6KontactInterface ${KONTACTINTERFACE_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6PimCommonAkonadi ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6GrantleeTheme ${GRANTLEETHEME_LIB_VERSION} CONFIG REQUIRED)

find_package(KF6TextAutoCorrectionWidgets ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)

find_package(KF6TextUtils ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6TextCustomEditor ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)

find_package(KPim6AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG REQUIRED)
set_package_properties(KPim6AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "https://invent.kde.org/pim/akonadi-search" TYPE REQUIRED PURPOSE "Provides search capabilities in pim apps and Akonadi")


if (NOT APPLE)
    find_package(X11)
endif()

set(KDEPIM_HAVE_X11 ${X11_FOUND})

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include_directories(${knotes_SOURCE_DIR} ${knotes_BINARY_DIR})

add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
ecm_set_disabled_deprecation_versions(QT 6.6
     KF 6.2.0
)

configure_file(knotes-version.h.in ${CMAKE_CURRENT_BINARY_DIR}/knotes-version.h @ONLY)
if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
   find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Test)
endif()
option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()

add_subdirectory(noteshared)
add_subdirectory(src)
add_subdirectory(notesagent)

ecm_qt_install_logging_categories(
        EXPORT KNOTES
        FILE knotes.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

if(KF6DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
