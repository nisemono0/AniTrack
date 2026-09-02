#include "gui/widgets/header_view.hpp"


HeaderView::HeaderView(
    Qt::Orientation orientation,
    QWidget *parent
) : QHeaderView(orientation, parent) {}

void HeaderView::setSectionPadding(int column, int size) {
    if (size <= 0) {
        this->paddings_.remove(column);
    } else {
        this->paddings_[column] = size;
    }
}

int HeaderView::sectionPadding(int column) const {
    return this->paddings_.value(column, 0);
}

void HeaderView::saveCurrentState() {
    this->previous_state_ = this->saveState();
}

void HeaderView::restorePreviousState() {
    if (this->previous_state_.isEmpty()) {
        return;
    }

    this->restoreState(this->previous_state_);
}

QSize HeaderView::sectionSizeFromContents(int logical_index) const {
    QSize size = QHeaderView::sectionSizeFromContents(logical_index);

    size.rwidth() += this->sectionPadding(logical_index);

    return size;
}

