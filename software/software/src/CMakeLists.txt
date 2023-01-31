set(SOURCE_FILES main.c)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(player)
add_executable(mp3player ${SOURCE_FILES})


target_link_libraries(mp3player player)
target_link_libraries(mp3player asound)
target_link_libraries(mp3player m)


target_link_libraries(mp3player avformat)
target_link_libraries(mp3player avcodec)
target_link_libraries(mp3player avutil)



message("${SOURCE_FILES}")