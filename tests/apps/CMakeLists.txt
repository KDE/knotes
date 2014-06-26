
 
include_directories(
  ${CMAKE_SOURCE_DIR}/pimcommon
  ${CMAKE_SOURCE_DIR}/noteshared
  ${CMAKE_BINARY_DIR}/noteshared
)


set(knotes_akonadi_SRCS main.cpp 
                        knotesakonaditray.cpp 
                        knotesakonadiapp.cpp 
                        knoteakonadinote.cpp
)
add_executable(knotes_akonadi_test ${knotes_akonadi_SRCS})
target_link_libraries(knotes_akonadi_test
  Qt5::Test
  KF5::AkonadiCore
  KF5::Mime
  akonadi_next
  noteshared
)

