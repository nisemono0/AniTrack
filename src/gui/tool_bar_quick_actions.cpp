#include "gui/tool_bar_quick_actions.hpp"


ToolBarQuickActions::ToolBarQuickActions(QWidget *parent) : QToolBar(parent) {
    this->spacer_widget_ = new QWidget(this);
    this->spacer_widget_->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
    );

    this->filter_line_edit_ = new QLineEdit(this);
    this->filter_line_edit_->setPlaceholderText(
        QStringLiteral("Filter list or search on Anilist")
    );
    this->filter_line_edit_->setFocusPolicy(Qt::ClickFocus);
    this->filter_line_edit_->setVisible(true);

    connect(this->filter_line_edit_, &QLineEdit::textChanged, this, &ToolBarQuickActions::filterTextChanged);
    connect(this->filter_line_edit_, &QLineEdit::returnPressed, this, [this] {
        QString text = this->filter_line_edit_->text().trimmed();
        if (!text.isEmpty()) {
            emit searchRequested(text);
            this->filter_line_edit_->clear();
        }
    });
}

void ToolBarQuickActions::setupToolBar() {
    this->addWidget(this->spacer_widget_);
    this->addWidget(this->filter_line_edit_);
}

