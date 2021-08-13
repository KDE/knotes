set(PIM_VERSION "5.18.1")
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(knotes VERSION ${PIM_VERSION})
set(RELEASE_SERVICE_VERSION "21.08.1")


set(KF5_MIN_VERSION "5.83.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${knotes_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
include(ECMInstallIcons)


include(ECMSetupVersion)
include(ECMAddTests)

include(GenerateExportHeader)
include(ECMGenerateHeaders)

include(FeatureSummary)
include(KDEGitCommitHooks)
include(CheckFunctionExists)
include(ECMGeneratePriFile)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)


# Do NOT add quote
set(KDEPIM_DEV_VERSION )

# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()



set(KDEPIM_VERSION "${PIM_VERSION}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")
set(KDEPIM_LIB_VERSION "${PIM_VERSION}")
set(KDEPIM_LIB_SOVERSION "5")


set(AKONADINOTES_LIB_VERSION "5.18.1")
set(AKONADI_VERSION "5.18.1")
set(CALENDARUTILS_LIB_VERSION "5.18.1")
set(KONTACTINTERFACE_LIB_VERSION "5.18.1")
set(KPIMTEXTEDIT_LIB_VERSION "5.18.1")
set(LIBKDEPIM_LIB_VERSION "5.18.1")
set(KMIME_LIB_VERSION "5.18.1")
set(PIMCOMMON_LIB_VERSION "5.18.1")
set(GRANTLEETHEME_LIB_VERSION "5.18.1")
set(AKONADI_SEARCH_VERSION "5.18.1")

set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED DBus Network PrintSupport Test Widgets Xml)
find_package(Qt5X11Extras NO_MODULE)
find_package(Grantlee5 "5.2" CONFIG REQUIRED)
# Find KF5 package
find_package(KF5Completion ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5Config ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5CoreAddons ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5Crash ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5DNSSD ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5GlobalAccel ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5IconThemes ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5ItemModels ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5ItemViews ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5KCMUtils ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5NewStuff ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Notifications ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5NotifyConfig ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Parts ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5TextWidgets ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5WindowSystem ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5XmlGui ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools ${KF5_MIN_VERSION})
set_package_properties(KF5DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)


# Find KdepimLibs Package
find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiNotes ${AKONADINOTES_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5CalendarUtils ${CALENDARUTILS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5KontactInterface ${KONTACTINTERFACE_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimCommonAkonadi ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimTextEdit ${KPIMTEXTEDIT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5GrantleeTheme ${GRANTLEETHEME_LIB_VERSION} CONFIG REQUIRED)


find_package(KF5AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG REQUIRED)
set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "ttps://invent.kde.org/pim/akonadi-search" TYPE REQUIRED PURPOSE "Provides search capabilities in pim apps and Akonadi")

if (NOT APPLE)
    find_package(X11)
endif()

set(KDEPIM_HAVE_X11 ${X11_FOUND})

set(CMAKE_MODULE_PATH ${knotes_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
find_package(Xsltproc)
set_package_properties(Xsltproc PROPERTIES DESCRIPTION "XSLT processor from libxslt" TYPE REQUIRED PURPOSE "Required to generate D-Bus interfaces for all Akonadi resources.")

include_directories(${knotes_SOURCE_DIR} ${knotes_BINARY_DIR})

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055400)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(knotes-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotes-version.h @ONLY)
if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
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

if(KF5DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
