include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${knotes_BINARY_DIR}/noteshared/src/
)

macro(add_noteshared_unittest _source)
  set(_test ${_source})
  get_filename_component(_name ${_source} NAME_WE)
  add_executable(${_name} ${_test})
  add_test(NAME ${_name} COMMAND ${_name})
  ecm_mark_as_test(noteshared-${_name})
  target_link_libraries(${_name}
    notesharedprivate
    Qt::Test
    Qt::Core
    Qt::Gui
    KF5::AkonadiCore
)
endmacro()

add_noteshared_unittest(notedisplayattributetest.cpp)
add_noteshared_unittest(notealarmattributetest.cpp)
add_noteshared_unittest(showfoldernotesattributetest.cpp)
add_noteshared_unittest(notelockattributetest.cpp)
