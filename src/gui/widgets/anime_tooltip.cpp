#include "gui/widgets/anime_tooltip.hpp"

#include "utils/cache.hpp"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <QFontMetrics>


AnimeTooltip::AnimeTooltip(QWidget *parent) : QWidget(parent) {
    this->initWidget();
    this->setupWidget();
    this->setupLayout();
}

void AnimeTooltip::updateAnimeInfo(const QString &cover_url,
                                   const QString &title,
                                   const QString &format,
                                   const QString &episodes,
                                   const QString &status,
                                   const QString &season) {

    auto *image_request = Cache::requestPixmap(cover_url);
    connect(image_request, &ImageRequest::finished, this, [this] (const QPixmap &pixmap) {
        this->cover_->setPixmap(
            pixmap.scaled(
                pixmap.width(), this->minimumHeight(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            )
        );
        this->adjustTooltipSize();
    });
    connect(image_request, &ImageRequest::failed, this, [this] {
        this->cover_->setPixmap(
            this->no_image_.scaled(
                this->no_image_.width(), this->minimumHeight(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            )
        );
        this->adjustTooltipSize();
    });

    this->title_->setText(title);
    this->format_->setText(format);
    this->episodes_->setText(episodes);
    this->status_->setText(status);
    this->season_->setText(season);
}

void AnimeTooltip::showAtPosition(const QPoint &global_pos) {
    if (this->isVisible()) {
        return;
    }

    this->adjustTooltipSize();
    this->move(this->positionFromGlobal(global_pos));
    this->show();
}

void AnimeTooltip::updatePosition(const QPoint &global_pos) {
    if (this->isHidden()) {
        return;
    }

    this->move(this->positionFromGlobal(global_pos));
}

void AnimeTooltip::initWidget() {
    this->cover_ = new QLabel(this);
    this->cover_->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    this->title_ = new QLabel(this);

    this->title_->setWordWrap(false);

    QFont font = this->title_->font();
    font.setPointSize(10);
    font.setBold(true);
    this->title_->setFont(font);

    this->format_ = new QLabel(this);
    this->episodes_ = new QLabel(this);
    this->status_ = new QLabel(this);
    this->season_ = new QLabel(this);
}

void AnimeTooltip::setupWidget() {
    this->setWindowFlag(Qt::ToolTip);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowTransparentForInput);

    this->setAttribute(Qt::WA_ShowWithoutActivating);
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setAttribute(Qt::WA_NoMouseReplay);
    this->setAttribute(Qt::WA_NoMousePropagation);

    this->setMinimumHeight(125);
    this->setMaximumWidth(450);

    this->setContentsMargins(0, 0, 0, 0);
}

void AnimeTooltip::setupLayout() {
    auto *h_layout = new QHBoxLayout(this);
    auto *v_layout = new QVBoxLayout();
    auto *g_layout = new QGridLayout();

    // Horizontal layout
    h_layout->setContentsMargins(6, 6, 6, 6);

    h_layout->addWidget(this->cover_, 0, Qt::AlignTop);

    h_layout->addLayout(v_layout);

    // Vertical layout
    v_layout->setContentsMargins(0, 0, 0, 0);
    v_layout->setSpacing(5);

    v_layout->addWidget(this->title_, 0, Qt::AlignTop);

    v_layout->addLayout(g_layout);

    // Grid layout
    g_layout->setContentsMargins(0, 0, 0, 0);
    g_layout->setVerticalSpacing(1);
    g_layout->setHorizontalSpacing(15);

    g_layout->setColumnStretch(0, 0);
    g_layout->setColumnStretch(1, 1);

    g_layout->addWidget(new QLabel(QStringLiteral("Format:"), this), 0, 0);
    g_layout->addWidget(this->format_, 0, 1);

    g_layout->addWidget(new QLabel(QStringLiteral("Episodes:"), this), 1, 0);
    g_layout->addWidget(this->episodes_, 1, 1);

    g_layout->addWidget(new QLabel(QStringLiteral("Status:"), this), 2, 0);
    g_layout->addWidget(this->status_, 2, 1);

    g_layout->addWidget(new QLabel(QStringLiteral("Season:"), this), 3, 0);
    g_layout->addWidget(this->season_, 3, 1);

    v_layout->addStretch();
}

QPoint AnimeTooltip::positionFromGlobal(const QPoint &global_pos) {
    // Edited from: https://code.qt.io/cgit/qt/qtbase.git/tree/src/widgets/kernel/qtooltip.cpp#n319
    QPoint pos = global_pos + this->cursor_offset_;

    QScreen *screen = QApplication::screenAt(global_pos);
    if (!screen) {
        screen = QApplication::primaryScreen();
    }

    const QRect screen_rect = screen->availableGeometry();
    if (pos.x() + this->width() > screen_rect.x() + screen_rect.width()) {
        pos.rx() -= 4 + this->width();
    }

    if (pos.y() + this->height() > screen_rect.y() + screen_rect.height()) {
        pos.ry() -= 24 + this->height();
    }

    if (pos.y() < screen_rect.y()) {
        pos.setY(screen_rect.y());\
    }

    if (pos.x() + this->width() > screen_rect.x() + screen_rect.width()) {
        pos.setX(screen_rect.x() + screen_rect.width() - this->width());
    }

    if (pos.x() < screen_rect.x()) {
        pos.setX(screen_rect.x());
    }

    if (pos.y() + this->height() > screen_rect.y() + screen_rect.height()) {
        pos.setY(screen_rect.y() + screen_rect.height() - this->height());
    }

    return pos;
}

void AnimeTooltip::adjustTooltipSize() {
    const QFontMetrics &font_metrics = this->title_->fontMetrics();

    const int title_width = font_metrics.horizontalAdvance(this->title_->text());
    this->title_->setWordWrap(title_width >= this->maximumWidth());

    this->adjustSize();
}

