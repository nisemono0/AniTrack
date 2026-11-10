#pragma once

#include "app/app_resources.hpp"

#include <QFrame>
#include <QWidget>
#include <QLabel>
#include <QPixmap>


class AnimeTooltip final : public QFrame {
    Q_OBJECT;
public:
    explicit AnimeTooltip(QWidget *parent = nullptr);
    ~AnimeTooltip() = default;

    // TODO: Add a next_airing_episode argument and display it
    void updateAnimeInfo(const QString &cover_url,
                         const QString &title,
                         const QString &format,
                         const QString &episodes,
                         const QString &status,
                         const QString &season,
                         const QString &genres,
                         const QString &score);

    void showAtPosition(const QPoint &global_pos);
    void updatePosition(const QPoint &global_pos);

private:
    const QPixmap no_image_ = QPixmap(AppResources::Images::NoImage);
    const QPoint cursor_offset_ = QPoint(16, 16);

    QLabel *cover_;

    QLabel *title_;

    QLabel *format_;
    QLabel *episodes_;
    QLabel *next_airing_episode_;
    QLabel *status_;
    QLabel *season_;
    QLabel *genres_;
    QLabel *score_;

    void initWidget();
    void setupWidget();
    void setupLayout();

    QPoint positionFromGlobal(const QPoint &global_pos);

    void adjustTooltipSize();

};

