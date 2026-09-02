#include "gui/delegates/anime_list_score_editor.hpp"

#include <QHBoxLayout>


AnimeListScoreEditor::AnimeListScoreEditor(QWidget *parent) : QWidget(parent) {
    this->score_spin_box_ = new SpinBox(this);
    this->score_spin_box_->setRange(0, 100);
    this->score_spin_box_->setFocusPolicy(Qt::WheelFocus);
    this->score_spin_box_->setButtonSymbols(QSpinBox::NoButtons);
    this->score_spin_box_->setAlignment(Qt::AlignCenter);
    this->score_spin_box_->setContextMenuPolicy(Qt::NoContextMenu);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->addWidget(this->score_spin_box_);

    connect(this->score_spin_box_, &SpinBox::returnPressed, this, &AnimeListScoreEditor::accept);
    connect(this->score_spin_box_, &SpinBox::focusLost, this, &AnimeListScoreEditor::accept);
}

void AnimeListScoreEditor::setData(const AnilistAnime &anime) {
    this->anime_ = anime;
    this->score_spin_box_->setValue(anime.entry.state().score);
}

int AnimeListScoreEditor::score() const {
    return this->score_spin_box_->value();
}

const AnilistAnime& AnimeListScoreEditor::anime() const {
    return this->anime_;
}

void AnimeListScoreEditor::accept() {
    if (this->accepted_) {
        return;
    }

    this->accepted_ = true;
    emit scoreAccepted();
}

