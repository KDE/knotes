set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

set( knotes_knotesgrantleeprint_source knotesgrantleeprinttest.cpp  )
kde4_add_unit_test( knotesgrantleeprinttest ${knotes_knotesgrantleeprint_source})
target_link_libraries( knotesgrantleeprinttest ${QT_QTTEST_LIBRARY} ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} ${KDEPIMLIBS_AKONADI_LIBS} knotesprivate)

