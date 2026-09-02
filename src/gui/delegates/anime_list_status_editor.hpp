#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "gui/widgets/anime_status_combo_box.hpp"

#include <QWidget>


class AnimeListStatusEditor final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimeListStatusEditor(QWidget *parent = nullptr);
    ~AnimeListStatusEditor() = default;

    void setData(const AnilistAnime &anime);

    AnilistEntry::Status status() const;
    const AnilistAnime& anime() const;

signals:
    void statusAccepted();

private:
    AnilistAnime anime_;

    AnilistEntry::Status status_;
    AnimeStatusComboBox *status_combo_box_;

};
