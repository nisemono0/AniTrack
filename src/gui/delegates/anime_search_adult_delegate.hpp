#pragma once

#include "app/app_resources.hpp"

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QIcon>


class AnimeSearchAdultDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeSearchAdultDelegate(QObject *parent = nullptr);
    ~AnimeSearchAdultDelegate() = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    const QIcon icon_yes_ = QIcon(AppResources::Icons::Check);
    const QIcon icon_no_ = QIcon(AppResources::Icons::X);
};
