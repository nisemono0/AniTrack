#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "gui/widgets/date_picker.hpp"

#include <QWidget>
#include <QDate>
#include <QDateEdit>


class AnimeListCompletedDateEditor final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimeListCompletedDateEditor(QWidget *parent = nullptr);
    ~AnimeListCompletedDateEditor() = default;

    void setData(const AnilistAnime &anime);

    QDate completedDate() const;
    const AnilistAnime& anime() const;

signals:
    void completedDateAccepted();

private:
    AnilistAnime anime_;
    QDate completed_date_;

    DatePicker *date_picker_;

};
