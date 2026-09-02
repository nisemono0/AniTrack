#pragma once

#include "base/anilist/anilist_anime.hpp"

#include <QWidget>
#include <QPushButton>


class AnimeListProgressEditor final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimeListProgressEditor(QWidget *parent = nullptr);
    ~AnimeListProgressEditor() = default;

    void setData(const AnilistAnime &anime);

signals:
    void minusButtonClicked(const AnilistAnime &anime);
    void plusButtonClicked(const AnilistAnime &anime);

private:
    AnilistAnime anime_;

    QPushButton *plus_button_;
    QPushButton *minus_button_;
};

