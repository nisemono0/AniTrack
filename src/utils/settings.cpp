#include "utils/settings.hpp"


void Settings::syncToDisk() {
    App::instance()->settings()->syncToDisk();
}

