cmake_minimum_required(VERSION 3.16)
project(AniTrack LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(AniTrack
    src/app/app.cpp
    src/app/app.hpp
    src/app/app_config.hpp
    src/app/app_controller.cpp
    src/app/app_controller.hpp
    src/app/app_resources.hpp
    src/app/command_line_parser.cpp
    src/app/command_line_parser.hpp
    src/app/system_tray_icon.cpp
    src/app/system_tray_icon.hpp
    src/base/account/account_manager.cpp
    src/base/account/account_manager.hpp
    src/base/anilist/anilist_account.cpp
    src/base/anilist/anilist_account.hpp
    src/base/anilist/anilist_anime.hpp
    src/base/anilist/anilist_api.cpp
    src/base/anilist/anilist_api.hpp
    src/base/anilist/anilist_config.hpp
    src/base/anilist/anilist_entry.cpp
    src/base/anilist/anilist_entry.hpp
    src/base/anilist/anilist_keys.hpp
    src/base/anilist/anilist_media.cpp
    src/base/anilist/anilist_media.hpp
    src/base/anilist/anilist_resources.hpp
    src/base/cache/image_cache.cpp
    src/base/cache/image_cache.hpp
    src/base/cache/image_request.cpp
    src/base/cache/image_request.hpp
    src/base/database/database.cpp
    src/base/database/database.hpp
    src/base/database/database_columns.hpp
    src/base/database/database_config.hpp
    src/base/database/database_controller.cpp
    src/base/database/database_controller.hpp
    src/base/database/database_keys.hpp
    src/base/database/database_resources.hpp
    src/base/logger/logger.cpp
    src/base/logger/logger.hpp
    src/base/network/network.cpp
    src/base/network/network.hpp
    src/base/recognition/mpris_watcher.cpp
    src/base/recognition/mpris_watcher.hpp
    src/base/search/search_manager.cpp
    src/base/search/search_manager.hpp
    src/base/settings/app_settings.cpp
    src/base/settings/app_settings.hpp
    src/base/sync/sync_manager.cpp
    src/base/sync/sync_manager.hpp
    src/gui/delegates/anime_list_adult_delegate.cpp
    src/gui/delegates/anime_list_adult_delegate.hpp
    src/gui/delegates/anime_list_completed_date_delegate.cpp
    src/gui/delegates/anime_list_completed_date_delegate.hpp
    src/gui/delegates/anime_list_completed_date_editor.cpp
    src/gui/delegates/anime_list_completed_date_editor.hpp
    src/gui/delegates/anime_list_pending_delegate.cpp
    src/gui/delegates/anime_list_pending_delegate.hpp
    src/gui/delegates/anime_list_private_delegate.cpp
    src/gui/delegates/anime_list_private_delegate.hpp
    src/gui/delegates/anime_list_progress_delegate.cpp
    src/gui/delegates/anime_list_progress_delegate.hpp
    src/gui/delegates/anime_list_progress_editor.cpp
    src/gui/delegates/anime_list_progress_editor.hpp
    src/gui/delegates/anime_list_score_delegate.cpp
    src/gui/delegates/anime_list_score_delegate.hpp
    src/gui/delegates/anime_list_score_editor.cpp
    src/gui/delegates/anime_list_score_editor.hpp
    src/gui/delegates/anime_list_started_date_delegate.cpp
    src/gui/delegates/anime_list_started_date_delegate.hpp
    src/gui/delegates/anime_list_started_date_editor.cpp
    src/gui/delegates/anime_list_started_date_editor.hpp
    src/gui/delegates/anime_list_status_delegate.cpp
    src/gui/delegates/anime_list_status_delegate.hpp
    src/gui/delegates/anime_list_status_editor.cpp
    src/gui/delegates/anime_list_status_editor.hpp
    src/gui/delegates/anime_search_adult_delegate.cpp
    src/gui/delegates/anime_search_adult_delegate.hpp
    src/gui/delegates/anime_search_in_list_delegate.cpp
    src/gui/delegates/anime_search_in_list_delegate.hpp
    src/gui/dialogs/anime_info_edit_dialog.cpp
    src/gui/dialogs/anime_info_edit_dialog.hpp
    src/gui/dialogs/anime_info_search_dialog.cpp
    src/gui/dialogs/anime_info_search_dialog.hpp
    src/gui/dialogs/logger_dialog.cpp
    src/gui/dialogs/logger_dialog.hpp
    src/gui/dialogs/login_token_dialog.cpp
    src/gui/dialogs/login_token_dialog.hpp
    src/gui/dialogs/set_episode_dialog.cpp
    src/gui/dialogs/set_episode_dialog.hpp
    src/gui/dialogs/set_score_dialog.cpp
    src/gui/dialogs/set_score_dialog.hpp
    src/gui/list_widget_navigation.cpp
    src/gui/list_widget_navigation.hpp
    src/gui/list_widget_separator_delegate.cpp
    src/gui/list_widget_separator_delegate.hpp
    src/gui/main_window.cpp
    src/gui/main_window.hpp
    src/gui/models/anime_list_model.cpp
    src/gui/models/anime_list_model.hpp
    src/gui/models/anime_list_proxy.cpp
    src/gui/models/anime_list_proxy.hpp
    src/gui/models/anime_search_model.cpp
    src/gui/models/anime_search_model.hpp
    src/gui/models/anime_search_proxy.cpp
    src/gui/models/anime_search_proxy.hpp
    src/gui/pages/anime_list_page.cpp
    src/gui/pages/anime_list_page.hpp
    src/gui/pages/anime_list_tab_bar.cpp
    src/gui/pages/anime_list_tab_bar.hpp
    src/gui/pages/anime_search_page.cpp
    src/gui/pages/anime_search_page.hpp
    src/gui/pages/statistics_page.cpp
    src/gui/pages/statistics_page.hpp
    src/gui/status_bar.cpp
    src/gui/status_bar.hpp
    src/gui/tool_bar_quick_actions.cpp
    src/gui/tool_bar_quick_actions.hpp
    src/gui/views/anime_list_header.cpp
    src/gui/views/anime_list_header.hpp
    src/gui/views/anime_list_view.cpp
    src/gui/views/anime_list_view.hpp
    src/gui/views/anime_search_header.cpp
    src/gui/views/anime_search_header.hpp
    src/gui/views/anime_search_view.cpp
    src/gui/views/anime_search_view.hpp
    src/gui/widgets/anime_status_combo_box.cpp
    src/gui/widgets/anime_status_combo_box.hpp
    src/gui/widgets/date_picker.cpp
    src/gui/widgets/date_picker.hpp
    src/gui/widgets/header_view.cpp
    src/gui/widgets/header_view.hpp
    src/gui/widgets/infinite_progress_dialog.cpp
    src/gui/widgets/infinite_progress_dialog.hpp
    src/gui/widgets/progress_dialog.cpp
    src/gui/widgets/progress_dialog.hpp
    src/gui/widgets/spin_box.cpp
    src/gui/widgets/spin_box.hpp
    src/main.cpp
    src/utils/anilist.cpp
    src/utils/anilist.hpp
    src/utils/cache.cpp
    src/utils/cache.hpp
    src/utils/date.cpp
    src/utils/date.hpp
    src/utils/dialog.cpp
    src/utils/dialog.hpp
    src/utils/enums.cpp
    src/utils/enums.hpp
    src/utils/file.cpp
    src/utils/file.hpp
    src/utils/json.cpp
    src/utils/json.hpp
    src/utils/links.cpp
    src/utils/links.hpp
    src/utils/log.cpp
    src/utils/log.hpp
    src/utils/settings.cpp
    src/utils/settings.hpp
    src/utils/sync.cpp
    src/utils/sync.hpp
    src/utils/web.cpp
    src/utils/web.hpp
)

target_sources(AniTrack
    PRIVATE
        src/gui/dialogs/anime_info_edit_dialog.ui
        src/gui/dialogs/anime_info_search_dialog.ui
        src/gui/dialogs/logger_dialog.ui
        src/gui/dialogs/login_token_dialog.ui
        src/gui/dialogs/set_episode_dialog.ui
        src/gui/dialogs/set_score_dialog.ui
        src/gui/main_window.ui
        src/gui/pages/anime_list_widget.ui
        src/gui/pages/anime_search_widget.ui
        src/gui/pages/statistics_widget.ui
)

qt_add_resources(AniTrack_resources src/res/resources.qrc)
target_sources(AniTrack
    PRIVATE
        ${AniTrack_resources}
)

target_link_libraries(AniTrack
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

# !!! cmakeconf.sh !!!
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS DBus)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

target_include_directories(AniTrack PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(AniTrack PRIVATE Qt::Sql)
target_link_libraries(AniTrack PRIVATE Qt::Network)
target_link_libraries(AniTrack PRIVATE Qt::DBus)
target_link_libraries(AniTrack PRIVATE Qt::Concurrent)

if(MINGW)
    message(STATUS "Building for Windows (mingw)")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_property(TARGET AniTrack PROPERTY WIN32_EXECUTABLE true)
    endif()
else()
    message(STATUS "Building for Linux")
endif()
