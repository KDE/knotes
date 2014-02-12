add_subdirectory(apps)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(
)
include ( ${CMAKE_SOURCE_DIR}/cmake/modules/kde4_handle_crypto_rpath_for_executable.cmake )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

# convenience macro to add qtest unit tests
macro(add_knotes_unittest _source)
  set(_test ${_source} )
  get_filename_component(_name ${_source} NAME_WE)
  kde4_add_unit_test(${_name} TESTNAME knotes-${_name} ${_test} )
  target_link_libraries( ${_name}
    ${QT_QTTEST_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${KDE4_KIO_LIBS}
  )
endmacro ()

add_knotes_unittest( notetest.cpp )
