#pragma once

#include "gui/widgets/header_view.hpp"


class AnimeSearchHeader final : public HeaderView {
    Q_OBJECT;
public:
    explicit AnimeSearchHeader(QWidget *parent = nullptr);
    ~AnimeSearchHeader();

private slots:
    void onCustomContextMenuRequested(const QPoint &pos);

};

