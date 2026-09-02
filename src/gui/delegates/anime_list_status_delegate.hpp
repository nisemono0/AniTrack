#pragma once

#include "base/anilist/anilist_anime.hpp"

#include <QObject>
#include <QStyledItemDelegate>


class AnimeListStatusDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListStatusDelegate(QObject *parent = nullptr);
    ~AnimeListStatusDelegate() = default;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestSetAnimeStatus(const AnilistAnime &anime, AnilistEntry::Status status);

private slots:
    void onStatusAccepted();
};

