#include "gui/delegates/anime_list_progress_editor.hpp"

#include <QHBoxLayout>

AnimeListProgressEditor::AnimeListProgressEditor(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(2);

    this->plus_button_ = new QPushButton(QStringLiteral("+"), this);
    this->plus_button_->setCursor(Qt::PointingHandCursor);
    this->plus_button_->setMaximumWidth(13);
    this->plus_button_->setMaximumHeight(13);

    this->minus_button_ = new QPushButton(QStringLiteral("-"), this);
    this->minus_button_->setCursor(Qt::PointingHandCursor);
    this->minus_button_->setMaximumWidth(13);
    this->minus_button_->setMaximumHeight(13);

    layout->addWidget(this->minus_button_);
    layout->addStretch();
    layout->addWidget(this->plus_button_);

    connect(this->minus_button_, &QPushButton::clicked, this, [this] {
        emit minusButtonClicked(this->anime_);
    });

    connect(this->plus_button_, &QPushButton::clicked, this, [this] {
        emit plusButtonClicked(this->anime_);
    });
}

void AnimeListProgressEditor::setData(const AnilistAnime &anime) {
    this->anime_ = anime;
}

