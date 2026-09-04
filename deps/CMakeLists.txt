add_subdirectory(anitomy)

add_library(AniTrack-deps INTERFACE)

target_link_libraries(AniTrack-deps
    INTERFACE
        anitomy
)
