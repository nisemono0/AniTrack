#pragma once

#include "base/anilist/anilist_account.hpp"

#include <QWidget>
#include <QStatusBar>
#include <QLabel>


class StatusBar final : public QStatusBar {
    Q_OBJECT;
public:
    explicit StatusBar(QWidget *parent = nullptr);
    ~StatusBar() = default;

public slots:
    void onUserChanged(const AnilistAccount::User &user);

private:
    QLabel *permanent_pixmap_label_;
    QLabel *permanent_message_label_;

private slots:
    void showPermanentMessage(const QString &message);
    void showPermanentPixmap(const QPixmap &pixmap);
};

