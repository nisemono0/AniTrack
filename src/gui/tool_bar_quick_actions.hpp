#pragma once

#include "gui/widgets/line_edit.hpp"

#include "base/anilist/anilist_entry.hpp"

#include <QWidget>
#include <QToolBar>
#include <QToolButton>


class ToolBarQuickActions final : public QToolBar {
    Q_OBJECT;
public:
    explicit ToolBarQuickActions(QWidget *parent = nullptr);
    ~ToolBarQuickActions() = default;

    void setupToolBar();

    bool hasSearchFocus();
    void focusSearchInput();
    void clearSearchInputFocus();
    void insertSearchText(const QString &text);
    void selectSearchText();

signals:
    void filterTextChanged(const QString &text);
    void searchRequested(const QString &text);

    void requestLoadRandomAnime(AnilistEntry::Status status);

private:
    QWidget *spacer_widget_;

    QToolButton *random_toolbutton_;

    LineEdit *filter_line_edit_;
};

