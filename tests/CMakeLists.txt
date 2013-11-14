
set(knotes_akonadi_SRCS main.cpp knotesakonaditray.cpp knotesakonadiapp.cpp knoteschangerecorder.cpp)
kde4_add_executable(knotes_akonadi_test TEST ${knotes_akonadi_SRCS})
target_link_libraries(knotes_akonadi_test
  ${QT_QTTEST_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
  akonadi_next
)

