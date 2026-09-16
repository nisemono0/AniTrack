#include "utils/settings.hpp"


void Settings::syncToDisk() {
    App::instance()->settings()->syncToDisk();
}

void Settings::signalChanges() {
    App::instance()->settings()->signalChanges();
}

