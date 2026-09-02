#include "gui/dialogs/logger_dialog.hpp"

#include "utils/dialog.hpp"
#include "utils/log.hpp"

#include <QFile>
#include <QTextStream>


LoggerDialog::LoggerDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::LoggerDialog) {

    // Ui setup
    this->ui_->setupUi(this);

    // Event filters for buttosn
    this->ui_->pushButtonSaveLogs->installEventFilter(this);
    this->ui_->pushButtonClearLogs->installEventFilter(this);
    this->ui_->pushButtonClose->installEventFilter(this);

    // Show default status tip
    this->showDefaultStatusTip();

    // Save/Clear/Close buttons
    connect(this->ui_->pushButtonSaveLogs, &QPushButton::clicked, this, &LoggerDialog::saveLogs);
    connect(this->ui_->pushButtonClearLogs, &QPushButton::clicked, this->ui_->plainTextEditLogs, &QPlainTextEdit::clear);
    connect(this->ui_->pushButtonClose, &QPushButton::clicked, this, &LoggerDialog::hide);
    // Update status tip when block count changes
    connect(this->ui_->plainTextEditLogs, &QPlainTextEdit::blockCountChanged, this, &LoggerDialog::onBlockCountChanged);

}

LoggerDialog::~LoggerDialog() {
    delete this->ui_;
}

void LoggerDialog::showOrFocus() {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
        return;
    }
    this->show();
}

void LoggerDialog::appendLogMessage(const QString &message) {
    this->ui_->plainTextEditLogs->appendHtml(message);
}

bool LoggerDialog::eventFilter(QObject *o, QEvent *e) {
    auto widget = qobject_cast<QWidget*>(o);

    if (!widget) {
        return QDialog::eventFilter(o, e);
    }

    switch (e->type()) {
        // Mouse enter widget area
        case QEvent::Enter:
        {
            this->ui_->labelLogStatus->setText(widget->statusTip());
            return true;
        }
        // Mouse leaves widget area
        case QEvent::Leave:
        {
            this->showDefaultStatusTip();
            return true;
        }
        default:
            break;
    }

    return QDialog::eventFilter(o, e);
}

void LoggerDialog::showDefaultStatusTip() {
    int num_lines = 0;
    if (!this->ui_->plainTextEditLogs->document()->isEmpty()) {
        num_lines = this->ui_->plainTextEditLogs->blockCount();
    }

    this->ui_->labelLogStatus->setText(QStringLiteral("Lines: %1").arg(num_lines));
}

void LoggerDialog::saveLogs() {
    QString save_path = DialogUtils::saveFile(this);

    if (save_path.isEmpty()) {
        return;
    }

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Saving log file: %1").arg(save_path)
    );

    QFile log_file = QFile(save_path);
    if (log_file.open(QFile::ReadWrite)) {
        QTextStream text_stream = QTextStream(&log_file);
        text_stream << this->ui_->plainTextEditLogs->toPlainText();
        text_stream.flush();
        if (text_stream.status() == QTextStream::Ok) {
            Log::info(
                CONTEXT_CLASS,
                QStringLiteral("Log file saved: %1").arg(save_path)
            );
            DialogUtils::information(QStringLiteral("Save log"), QStringLiteral("Log written to file"), this);
        } else if (text_stream.status() == QTextStream::WriteFailed) {
            Log::error(
                CONTEXT_CLASS,
                QStringLiteral("Failed to save log file: %1").arg(save_path)
            );
            DialogUtils::error(QStringLiteral("Save log"), QStringLiteral("Could not save log file"), this);
        }
        log_file.close();
    }
}

void LoggerDialog::onBlockCountChanged(int newBlockCount) {
    this->ui_->labelLogStatus->setText(QStringLiteral("Lines: %1").arg(newBlockCount));
}

