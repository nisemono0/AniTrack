#pragma once

#include <QString>
#include <QWidget>


namespace DialogUtils {
// Show a save file dialog in front of parent and return the selected save path
QString saveFile(QWidget *parent);

// Shows an info message box in front of parent with the given title and text
// Blocking means the whole event loop is blocked, defaults to false
void information(const QString &title, const QString &text, QWidget *parent = nullptr, bool blocking = false);

// Shows a warning message box in front of parent with the given title and text
// Blocking means the whole event loop is blocked, defaults to false
void warning(const QString &title, const QString &text, QWidget *parent = nullptr, bool blocking = false);

// Shows an error message box in front of parent with the given title and text
// Blocking means the whole event loop is blocked, defaults to false
void error(const QString &title, const QString &text, QWidget *parent = nullptr, bool blocking = false);

}; // namespace DialogUtils

