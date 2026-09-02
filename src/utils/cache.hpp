#pragma once

#include "base/cache/image_request.hpp"

#include <QString>


// Wrappers around App::instance()->imageCache()
namespace Cache {
// Request cached pixmap at url
ImageRequest *requestPixmap(const QString &url);

// Return the path of th ecache directory
QString cachePath();

// Return the number of files in the cache directory
qsizetype cacheCount();

} // namespace Cache

