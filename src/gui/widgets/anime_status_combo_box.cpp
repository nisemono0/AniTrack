#include "gui/widgets/anime_status_combo_box.hpp"

#include <QMetaEnum>

AnimeStatusComboBox::AnimeStatusComboBox(QWidget *parent) : QComboBox(parent) {
    const QMetaEnum meta_enum = QMetaEnum::fromType<StatusItems>();

    for (int i = 0; i < meta_enum.keyCount(); i++) {
        QString name = QString::fromLatin1(meta_enum.key(i));

        this->addItem(
            name,
            // StatusItems enum corresponds to AnilistEntry::Status enum
            // minus the INVALID status so we have to +1 to correspond to it
            static_cast<int>(i + 1)
        );
    }

    connect(this, &QComboBox::activated, this, [this] (int index) {
        emit animeStatusActivated(
            static_cast<AnilistEntry::Status>(
                this->itemData(index).toInt()
            )
        );
    });
}

void AnimeStatusComboBox::setTextAlignment(Qt::AlignmentFlag alignment) {
    for (int i = 0; i < this->count(); i++) {
        this->setItemData(
            i,
            alignment,
            Qt::TextAlignmentRole
        );
    }
}

void AnimeStatusComboBox::setCurrentStatus(AnilistEntry::Status status) {
    const int index = this->findData(
        static_cast<int>(status)
    );

    if (index > 0) {
        this->setCurrentIndex(index);
    }
}

