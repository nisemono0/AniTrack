#include "gui/delegates/anime_list_started_date_editor.hpp"

#include <QHBoxLayout>
#include <QMetaEnum>
#include <QSignalBlocker>


AnimeListStartedDateEditor::AnimeListStartedDateEditor(QWidget *parent) : QWidget(parent) {
    this->date_picker_ = new DatePicker(this);
    this->date_picker_->setTextAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->addWidget(this->date_picker_);

    connect(this->date_picker_, &DatePicker::dateChanged, this, [this] (QDate date) {
        this->started_date_ = date;
        emit startedDateAccepted();
    });
}

void AnimeListStartedDateEditor::setData(const AnilistAnime &anime) {
    const QSignalBlocker date_edit_blocker(this->date_picker_);

    this->anime_ = anime;
    this->started_date_ = anime.entry.state().started_at;

    this->date_picker_->setDate(this->started_date_);
}

QDate AnimeListStartedDateEditor::startedDate() const {
    return this->started_date_;
}

const AnilistAnime& AnimeListStartedDateEditor::anime() const {
    return this->anime_;
}

