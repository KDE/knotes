set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )


set( knotes_knotesgrantleeprint_source knotesgrantleeprinttest.cpp  )
add_executable( knotesgrantleeprinttest ${knotes_knotesgrantleeprint_source})
add_test(knotesgrantleeprinttest knotesgrantleeprinttest)
ecm_mark_as_test(knotesgrantleeprinttest)
target_link_libraries( knotesgrantleeprinttest Qt5::Test Qt5::Gui KF5::AkonadiCore)

