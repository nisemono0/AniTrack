#pragma once

#include <QObject>
#include <QStyledItemDelegate>


class ListWidgetSeparatorDelegate final : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit ListWidgetSeparatorDelegate(QObject *parent = nullptr);
    ~ListWidgetSeparatorDelegate() = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;


};


