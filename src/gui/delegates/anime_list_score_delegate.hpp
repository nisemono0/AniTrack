#pragma once

#include "base/anilist/anilist_anime.hpp"

#include <QObject>
#include <QStyledItemDelegate>


class AnimeListScoreDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListScoreDelegate(QObject *parent = nullptr);
    ~AnimeListScoreDelegate() = default;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestSetAnimeScore(const AnilistAnime &anime, int score);

private slots:
    void onScoreAccepted();
};
