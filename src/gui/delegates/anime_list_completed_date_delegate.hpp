#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "gui/delegates/anime_list_completed_date_editor.hpp"

#include <QObject>
#include <QStyledItemDelegate>
#include <QPointer>


class AnimeListCompletedDateDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListCompletedDateDelegate(QObject *parent = nullptr);
    ~AnimeListCompletedDateDelegate() = default;

    void stopEditing();

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

signals:
    void requestSetAnimeCompletedDate(const AnilistAnime &anime, const QDate &date);

private:
    mutable QPointer<AnimeListCompletedDateEditor> editor_;

private slots:
    void onCompletedDateAccepted();

};

