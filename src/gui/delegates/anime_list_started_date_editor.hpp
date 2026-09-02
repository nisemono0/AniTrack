#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "gui/widgets/date_picker.hpp"

#include <QWidget>


class AnimeListStartedDateEditor final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimeListStartedDateEditor(QWidget *parent = nullptr);
    ~AnimeListStartedDateEditor() = default;

    void setData(const AnilistAnime &anime);

    QDate startedDate() const;
    const AnilistAnime& anime() const;

signals:
    void startedDateAccepted();

private:
    AnilistAnime anime_;
    QDate started_date_;

    DatePicker *date_picker_;

};
