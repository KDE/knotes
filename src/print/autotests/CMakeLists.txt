# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

set( knotes_knotesgrantleeprint_source knotesgrantleeprinttest.cpp  )
add_executable( knotesgrantleeprinttest ${knotes_knotesgrantleeprint_source})
add_test(NAME knotesgrantleeprinttest COMMAND knotesgrantleeprinttest)
ecm_mark_as_test(knotesgrantleeprinttest)
target_link_libraries( knotesgrantleeprinttest Qt::Test Qt::Gui KPim6::AkonadiCore knotesprivate KPim6::Mime KPim6::AkonadiNotes KPim6::GrantleeTheme)

if (QT_MAJOR_VERSION STREQUAL "5")
    target_link_libraries(knotesgrantleeprinttest Grantlee5::Templates)
else()
    target_link_libraries(knotesgrantleeprinttest KF6::TextTemplate)
endif()
