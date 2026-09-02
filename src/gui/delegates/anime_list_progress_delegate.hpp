#pragma once

#include "base/anilist/anilist_anime.hpp"

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>


class AnimeListProgressDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListProgressDelegate(QObject *parent = nullptr);
    ~AnimeListProgressDelegate() = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestDecreaseAnimeProgress(const AnilistAnime &anime);
    void requestIncreaseAnimeProgress(const AnilistAnime &anime);
};

