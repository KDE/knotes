# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_subdirectory(src)
if (BUILD_TESTING)
   add_subdirectory(autotests)
   add_subdirectory(src/attributes/autotests/)
endif()

