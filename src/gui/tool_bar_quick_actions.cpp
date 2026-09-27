#include "gui/tool_bar_quick_actions.hpp"

#include "app/app_resources.hpp"

#include <QMenu>


namespace {
struct RandomAction {
    QString name;
    AnilistEntry::Status status;
};
} // namespace

ToolBarQuickActions::ToolBarQuickActions(QWidget *parent) : QToolBar(parent) {
    this->spacer_widget_ = new QWidget(this);
    this->spacer_widget_->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
    );

    this->filter_line_edit_ = new LineEdit(this);
    this->filter_line_edit_->setPlaceholderText(
        QStringLiteral("Filter list or search on Anilist")
    );
    this->filter_line_edit_->setFocusPolicy(Qt::ClickFocus);
    this->filter_line_edit_->setVisible(true);

    this->random_toolbutton_ = new QToolButton(this);
    this->random_toolbutton_->setFocusPolicy(Qt::NoFocus);
    this->random_toolbutton_->setText(QStringLiteral("Random"));
    this->random_toolbutton_->setToolTip(QStringLiteral("Random"));
    this->random_toolbutton_->setStatusTip(QStringLiteral("Show random anime"));
    this->random_toolbutton_->setIcon(QIcon(AppResources::Icons::ArrowsRandom));
    this->random_toolbutton_->setPopupMode(QToolButton::InstantPopup);

    QMenu *tool_menu = new QMenu(this->random_toolbutton_);
    static const QList<RandomAction> random_actions{
        { QStringLiteral("Watching"),   AnilistEntry::Status::CURRENT },
        { QStringLiteral("Rewatching"), AnilistEntry::Status::REPEATING },
        { QStringLiteral("Completed"),  AnilistEntry::Status::COMPLETED },
        { QStringLiteral("Paused"),     AnilistEntry::Status::PAUSED },
        { QStringLiteral("Dropped"),    AnilistEntry::Status::DROPPED },
        { QStringLiteral("Planning"),   AnilistEntry::Status::PLANNING },
    };
    for (const auto &action : random_actions) {
        auto *menu_action = tool_menu->addAction(action.name);
        connect(menu_action, &QAction::triggered, this, [this, status = action.status] {
            emit requestLoadRandomAnime(status);
        });
    }

    this->random_toolbutton_->setMenu(tool_menu);

    connect(this->filter_line_edit_, &QLineEdit::textChanged, this, &ToolBarQuickActions::filterTextChanged);
    connect(this->filter_line_edit_, &QLineEdit::returnPressed, this, [this] {
        const QString text = this->filter_line_edit_->text().trimmed();
        if (!text.isEmpty()) {
            emit searchRequested(text);
            this->filter_line_edit_->clear();
        }
    });
}

void ToolBarQuickActions::setupToolBar() {
    this->addSeparator();
    this->addWidget(this->random_toolbutton_);
    this->addWidget(this->spacer_widget_);
    this->addWidget(this->filter_line_edit_);
}

bool ToolBarQuickActions::hasSearchFocus() {
    return this->filter_line_edit_->hasFocus();
}

void ToolBarQuickActions::focusSearchInput() {
    this->filter_line_edit_->setFocus();
}

void ToolBarQuickActions::clearSearchInputFocus() {
    this->filter_line_edit_->clearFocus();
}

void ToolBarQuickActions::insertSearchText(const QString &text) {
    this->filter_line_edit_->insert(text);
}

void ToolBarQuickActions::selectSearchText() {
    this->filter_line_edit_->selectAll();
}

