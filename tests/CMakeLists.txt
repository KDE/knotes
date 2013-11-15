
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}") 
include_directories(
  ${CMAKE_SOURCE_DIR}/pimcommon
  ${CMAKE_SOURCE_DIR}/noteshared
  ${CMAKE_BINARY_DIR}/noteshared
)


set(knotes_akonadi_SRCS main.cpp 
                        knotesakonaditray.cpp 
                        knotesakonadiapp.cpp 
                        knoteschangerecorder.cpp 
                        knoteakonadinote.cpp
                        knotesakonaditreemodel.cpp 
)
kde4_add_executable(knotes_akonadi_test TEST ${knotes_akonadi_SRCS})
target_link_libraries(knotes_akonadi_test
  ${QT_QTTEST_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
  ${KDEPIMLIBS_KMIME_LIBS}
  akonadi_next
  noteshared
)

