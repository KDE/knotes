macro(add_noteshared_unittest _source)
  set(_test ${_source})
  get_filename_component(_name ${_source} NAME_WE)
  add_executable( ${_name} ${_test} )
  add_test( ${_name} ${_name} )
  ecm_mark_as_test(noteshared-${_name})
  target_link_libraries( ${_name}
    noteshared
    Qt5::Test
    Qt5::Core
    Qt5::Gui
  )
endmacro ()

add_noteshared_unittest( notedisplayattributetest.cpp )
add_noteshared_unittest( notealarmattributetest.cpp )
add_noteshared_unittest( showfoldernotesattributetest.cpp )
add_noteshared_unittest( notelockattributetest.cpp )
