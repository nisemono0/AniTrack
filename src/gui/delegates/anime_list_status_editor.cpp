#include "gui/delegates/anime_list_status_editor.hpp"

#include <QHBoxLayout>
#include <QMetaEnum>


AnimeListStatusEditor::AnimeListStatusEditor(QWidget *parent) : QWidget(parent) {
    this->status_combo_box_ = new AnimeStatusComboBox(this);
    this->status_combo_box_->setContextMenuPolicy(Qt::NoContextMenu);

    this->status_combo_box_->setTextAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->setContentsMargins(2, 0, 2, 0);
    layout->addWidget(this->status_combo_box_);

    connect(this->status_combo_box_, &AnimeStatusComboBox::animeStatusActivated, this, [this] (AnilistEntry::Status status) {
        if (status == this->status_) {
            return;
        }

        this->status_ = status;
        emit statusAccepted();
    });
}

void AnimeListStatusEditor::setData(const AnilistAnime &anime) {
    this->anime_ = anime;
    this->status_ = anime.entry.state().status;

    this->status_combo_box_->setCurrentStatus(this->status_);
}

AnilistEntry::Status AnimeListStatusEditor::status() const {
    return this->status_;
}

const AnilistAnime& AnimeListStatusEditor::anime() const {
    return this->anime_;
}

