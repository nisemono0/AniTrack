#include "utils/dialog.hpp"
#include "utils/file.hpp"

#include <QFileDialog>
#include <QMessageBox>


QString DialogUtils::saveFile(QWidget *parent) {
    QFileDialog save_dialog = QFileDialog(parent, QStringLiteral("Save file"));

    save_dialog.setDirectory(QDir::homePath());
    save_dialog.setAcceptMode(QFileDialog::AcceptSave);
    save_dialog.setFileMode(QFileDialog::AnyFile);
    save_dialog.setNameFilter(
            QStringLiteral("Log file (*.log *.txt)")
            );
    save_dialog.setDefaultSuffix(QStringLiteral(".log"));

    if (save_dialog.exec()) {
        QString save_path = save_dialog.selectedFiles().constFirst();
        // Return sanitized save_path
        if (FileUtils::containsExtension(save_path, {"log", "txt"})) {
            return FileUtils::sanitizeFilePath(save_path);
        }
        return FileUtils::sanitizeFilePath(save_path, "log");
    }
    return QString();
}

void DialogUtils::information(const QString &title, const QString &text, QWidget *parent, bool blocking) {
    if (blocking) {
        QMessageBox::information(parent, title, text);
    } else {
        QMessageBox *info = new QMessageBox(
                QMessageBox::Information,
                title,
                text,
                QMessageBox::Ok,
                parent
                );
        info->setAttribute(Qt::WA_DeleteOnClose);
        info->open();
    }
}

void DialogUtils::warning(const QString &title, const QString &text, QWidget *parent, bool blocking) {
    if (blocking) {
        QMessageBox::warning(parent, title, text);
    } else {
        QMessageBox *warn = new QMessageBox(
                QMessageBox::Warning,
                title,
                text,
                QMessageBox::Ok,
                parent
                );
        warn->setAttribute(Qt::WA_DeleteOnClose);
        warn->open();
    }
}

void DialogUtils::error(const QString &title, const QString &text, QWidget *parent, bool blocking) {
    if (blocking) {
        QMessageBox::critical(parent, title, text);
    } else {
        QMessageBox *error = new QMessageBox(
                QMessageBox::Critical,
                title,
                text,
                QMessageBox::Ok,
                parent
                );
        error->setAttribute(Qt::WA_DeleteOnClose);
        error->open();
    }
}

