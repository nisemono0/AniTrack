add_library(AniTrack-deps INTERFACE)

target_link_libraries(AniTrack-deps
    INTERFACE
        anitomy
)

add_subdirectory(anitomy)
