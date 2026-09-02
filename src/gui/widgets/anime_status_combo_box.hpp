#pragma once

#include <QComboBox>
#include <QWidget>

#include "base/anilist/anilist_entry.hpp"


class AnimeStatusComboBox final : public QComboBox {
    Q_OBJECT;
public:
    explicit AnimeStatusComboBox(QWidget *parent = nullptr);
    ~AnimeStatusComboBox() = default;

    // Set the text alignment for the displayed items
    void setTextAlignment(Qt::AlignmentFlag alignment);
    // Set the current selected status
    void setCurrentStatus(AnilistEntry::Status status);

signals:
    // Send the selected status in AnilistEntry::Status format
    void animeStatusActivated(AnilistEntry::Status status);

private:
    enum class StatusItems {
        Watching,
        Planning,
        Completed,
        Dropped,
        Paused,
        Rewatching,
    };
    Q_ENUM(StatusItems);

};

