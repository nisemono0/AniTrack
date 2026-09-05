#pragma once

#include "gui/widgets/line_edit.hpp"

#include <QWidget>
#include <QToolBar>


class ToolBarQuickActions final : public QToolBar {
    Q_OBJECT;
public:
    explicit ToolBarQuickActions(QWidget *parent = nullptr);
    ~ToolBarQuickActions() = default;

    void setupToolBar();

    bool hasSearchFocus();
    void focusSearchInput();
    void insertSearchText(const QString &text);
    void selectSearchText();

signals:
    void filterTextChanged(const QString &text);
    void searchRequested(const QString &text);

private:
    QWidget *spacer_widget_;

    LineEdit *filter_line_edit_;
};

