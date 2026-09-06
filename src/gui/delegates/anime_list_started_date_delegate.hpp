#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "gui/delegates/anime_list_started_date_editor.hpp"

#include <QObject>
#include <QStyledItemDelegate>
#include <QPointer>


class AnimeListStartedDateDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListStartedDateDelegate(QObject *parent = nullptr);
    ~AnimeListStartedDateDelegate() = default;

    void stopEditing();

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestSetAnimeStartedDate(const AnilistAnime &anime, const QDate &date);

private:
    mutable QPointer<AnimeListStartedDateEditor> editor_;

private slots:
    void onStartedDateAccepted();
};

