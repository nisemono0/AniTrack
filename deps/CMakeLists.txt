add_library(AniTrack-deps INTERFACE)

add_subdirectory(anitomy)
add_subdirectory(utf8proc)

target_link_libraries(AniTrack-deps
    INTERFACE
    anitomy
    utf8proc
)
