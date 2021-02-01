add_executable(noteeditorutilstest noteeditorutilstest.cpp )
add_test(NAME noteeditorutilstest COMMAND noteeditorutilstest)
ecm_mark_as_test(noteeditorutilstest)
target_link_libraries(noteeditorutilstest  Qt5::Test notesharedprivate)

add_executable(noteutilstest noteutilstest.cpp )
add_test(NAME noteutilstest COMMAND noteutilstest)
ecm_mark_as_test(noteutilstest)
target_link_libraries(noteutilstest  Qt5::Test notesharedprivate)
