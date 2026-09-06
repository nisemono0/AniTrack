#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "gui/delegates/anime_list_score_editor.hpp"

#include <QObject>
#include <QStyledItemDelegate>
#include <QPointer>


class AnimeListScoreDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListScoreDelegate(QObject *parent = nullptr);
    ~AnimeListScoreDelegate() = default;

    void stopEditing();

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestSetAnimeScore(const AnilistAnime &anime, int score);

private:
    mutable QPointer<AnimeListScoreEditor> editor_;

private slots:
    void onScoreAccepted();
};
