#pragma once

#include "base/anilist/anilist_anime.hpp"

#include <QObject>
#include <QStyledItemDelegate>


class AnimeListCompletedDateDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListCompletedDateDelegate(QObject *parent = nullptr);
    ~AnimeListCompletedDateDelegate() = default;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestSetAnimeCompletedDate(const AnilistAnime &anime, const QDate &date);

private slots:
    void onCompletedDateAccepted();
};

