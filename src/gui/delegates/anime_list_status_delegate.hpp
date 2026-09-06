#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "gui/delegates/anime_list_status_editor.hpp"

#include <QObject>
#include <QStyledItemDelegate>
#include <QPointer>


class AnimeListStatusDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListStatusDelegate(QObject *parent = nullptr);
    ~AnimeListStatusDelegate() = default;

    void stopEditing();

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestSetAnimeStatus(const AnilistAnime &anime, AnilistEntry::Status status);

private:
    mutable QPointer<AnimeListStatusEditor> editor_;

private slots:
    void onStatusAccepted();
};

