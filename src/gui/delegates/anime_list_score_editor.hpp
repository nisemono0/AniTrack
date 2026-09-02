#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "gui/widgets/spin_box.hpp"

#include <QWidget>
#include <QSpinBox>


class AnimeListScoreEditor final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimeListScoreEditor(QWidget *parent = nullptr);
    ~AnimeListScoreEditor() = default;

    void setData(const AnilistAnime &anime);

    int score() const;
    const AnilistAnime& anime() const;

signals:
    void scoreAccepted();

private:
    AnilistAnime anime_;

    SpinBox *score_spin_box_;
    bool accepted_ = false;

private slots:
    void accept();
};
