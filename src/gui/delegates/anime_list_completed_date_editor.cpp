#include "gui/delegates/anime_list_completed_date_editor.hpp"

#include <QHBoxLayout>
#include <QMetaEnum>
#include <QSignalBlocker>


AnimeListCompletedDateEditor::AnimeListCompletedDateEditor(QWidget *parent) : QWidget(parent) {
    this->date_picker_ = new DatePicker(this);
    this->date_picker_->setTextAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->addWidget(this->date_picker_);

    connect(this->date_picker_, &DatePicker::dateChanged, this, [this] (QDate date) {
        this->completed_date_ = date;
        emit completedDateAccepted();
    });
}

void AnimeListCompletedDateEditor::setData(const AnilistAnime &anime) {
    const QSignalBlocker date_edit_blocker(this->date_picker_);

    this->anime_ = anime;
    this->completed_date_ = anime.entry.state().completed_at;

    this->date_picker_->setDate(this->completed_date_);
}

QDate AnimeListCompletedDateEditor::completedDate() const {
    return this->completed_date_;
}

const AnilistAnime& AnimeListCompletedDateEditor::anime() const {
    return this->anime_;
}

