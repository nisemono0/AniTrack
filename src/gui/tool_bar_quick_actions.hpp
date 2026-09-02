#pragma once

#include <QWidget>
#include <QToolBar>
#include <QLineEdit>


class ToolBarQuickActions final : public QToolBar {
    Q_OBJECT;
public:
    explicit ToolBarQuickActions(QWidget *parent = nullptr);
    ~ToolBarQuickActions() = default;

    void setupToolBar();

signals:
    void filterTextChanged(const QString &text);
    void searchRequested(const QString &text);

private:
    QWidget *spacer_widget_;

    QLineEdit *filter_line_edit_;
};

