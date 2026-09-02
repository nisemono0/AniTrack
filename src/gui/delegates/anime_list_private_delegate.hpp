#pragma once

#include "app/app_resources.hpp"

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QIcon>


class AnimeListPrivateDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit AnimeListPrivateDelegate(QObject *parent = nullptr);
    ~AnimeListPrivateDelegate() = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    const QIcon icon_hidden_ = QIcon(AppResources::Icons::EyeOff);
    const QIcon icon_visible_ = QIcon(AppResources::Icons::EyeOn);
};

