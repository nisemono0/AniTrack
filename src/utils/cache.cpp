#include "utils/cache.hpp"

#include "app/app.hpp"


ImageRequest* Cache::requestPixmap(const QString &url) {
    return App::instance()->imageCache()->requestPixmap(url);
}

QString Cache::cachePath() {
    return App::instance()->imageCache()->cachePath();
}

qsizetype Cache::cacheCount() {
    return App::instance()->imageCache()->cacheCount();
}

