#pragma once

#include "app/app_resources.hpp"

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QIcon>


class AnimeListPendingDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListPendingDelegate(QObject *parent = nullptr);
    ~AnimeListPendingDelegate() = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    const QIcon icon_point_gray_ = QIcon(AppResources::Icons::PointGray);
    const QIcon icon_point_blue_ = QIcon(AppResources::Icons::PointBlue);
    const QIcon icon_point_orange_ = QIcon(AppResources::Icons::PointOrange);
    const QIcon icon_point_red_ = QIcon(AppResources::Icons::PointRed);
};

