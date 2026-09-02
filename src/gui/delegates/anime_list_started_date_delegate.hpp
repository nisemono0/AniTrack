#pragma once

#include "base/anilist/anilist_anime.hpp"

#include <QObject>
#include <QStyledItemDelegate>


class AnimeListStartedDateDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListStartedDateDelegate(QObject *parent = nullptr);
    ~AnimeListStartedDateDelegate() = default;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestSetAnimeStartedDate(const AnilistAnime &anime, const QDate &date);

private slots:
    void onStartedDateAccepted();
};

