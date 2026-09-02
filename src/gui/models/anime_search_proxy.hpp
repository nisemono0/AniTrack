#pragma once

#include <QSortFilterProxyModel>
#include <QObject>


class AnimeSearchProxy final : public QSortFilterProxyModel {
    Q_OBJECT;
public:
    explicit AnimeSearchProxy(QObject *parent = nullptr);
    ~AnimeSearchProxy() = default;

    void setFilterText(const QString &text);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString filter_text_;
};

