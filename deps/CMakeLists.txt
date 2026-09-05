add_library(AniTrack-deps INTERFACE)

if(ENABLE_WIP_FEATURES)
    add_subdirectory(anitomy)
    target_link_libraries(AniTrack-deps INTERFACE anitomy)
endif()
