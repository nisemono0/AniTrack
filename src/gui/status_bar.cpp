#include "gui/status_bar.hpp"

#include "app/app_resources.hpp"

#include "utils/cache.hpp"
#include "utils/log.hpp"


StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent) {
    this->permanent_pixmap_label_ = new QLabel(this);
    this->permanent_message_label_ = new QLabel(this);

    // Fixed pixmap size
    this->permanent_pixmap_label_->setFixedSize(22, 22);

    // Default message
    this->permanent_message_label_->setText(QStringLiteral("User: N/A"));

    this->addPermanentWidget(this->permanent_pixmap_label_);
    this->addPermanentWidget(this->permanent_message_label_);
}

void StatusBar::onUserChanged(const AnilistAccount::User &user) {
    this->showPermanentMessage(
        QStringLiteral("%1 (%2)").arg(user.username).arg(user.user_id)
    );

    auto request = Cache::requestPixmap(user.avatar_url);
    connect(request, &ImageRequest::finished, this, &StatusBar::showPermanentPixmap);
    connect(request, &ImageRequest::failed, this, [this] {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Cache request failed, using default avatar image")
        );
        this->showPermanentPixmap(
            QPixmap(AppResources::Icons::User)
        );
    });
}

void StatusBar::showPermanentMessage(const QString &message) {
    this->permanent_message_label_->setText(message);
}

void StatusBar::showPermanentPixmap(const QPixmap &pixmap) {
    this->permanent_pixmap_label_->setPixmap(
        pixmap.scaled(
            this->permanent_pixmap_label_->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        )
    );
}

