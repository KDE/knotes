project(knotes)

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )

add_subdirectory(noteshared)
add_subdirectory(src)
add_subdirectory(notesagent)

