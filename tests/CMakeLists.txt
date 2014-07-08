add_subdirectory(apps)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include ( ${CMAKE_SOURCE_DIR}/cmake/modules/kde4_handle_crypto_rpath_for_executable.cmake )

# convenience macro to add qtest unit tests
macro(add_knotes_unittest _source)
  set(_test ${_source} )
  get_filename_component(_name ${_source} NAME_WE)
  add_executable( ${_name} ${_test} )
  add_test( ${_name} ${_name} )
  ecm_mark_as_test(knotes-${_name})
  target_link_libraries( ${_name}
    Qt5::Test
    KF5::KIOCore
  )
endmacro ()

add_knotes_unittest( notetest.cpp )
