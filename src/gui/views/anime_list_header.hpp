#pragma once

#include "gui/widgets/header_view.hpp"


class AnimeListHeader final : public HeaderView {
    Q_OBJECT;
public:
    explicit AnimeListHeader(QWidget *parent = nullptr);
    ~AnimeListHeader();

private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
};

