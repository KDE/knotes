# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_executable(noteeditorutilstest noteeditorutilstest.cpp noteeditorutilstest.h)
add_test(NAME noteeditorutilstest COMMAND noteeditorutilstest)
ecm_mark_as_test(noteeditorutilstest)
target_link_libraries(noteeditorutilstest  Qt${QT_MAJOR_VERSION}::Test notesharedprivate)

add_executable(noteutilstest noteutilstest.cpp noteutilstest.h)
add_test(NAME noteutilstest COMMAND noteutilstest)
ecm_mark_as_test(noteutilstest)
target_link_libraries(noteutilstest  Qt${QT_MAJOR_VERSION}::Test notesharedprivate)
