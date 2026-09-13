#include "gui/dialogs/about_dialog.hpp"

#include <QPushButton>
#include <QClipboard>


AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::AboutDialog) {

    this->ui_->setupUi(this);

    for (auto &button : this->ui_->buttonBox->buttons()) {
        button->setFocusPolicy(Qt::NoFocus);
    }

    this->setupDialog();

    connect(this->ui_->pushButtonCopyToClipboard, &QPushButton::clicked, this, [this] {
        auto *clipboard = QApplication::clipboard();
        const QString info = QStringLiteral(
            "%1\n"
            "Qt: %2\n"
            "tabler-icons: %3\n"
            "anitomy: %4\n"
            "anime-relations: %5\n"
            "utf8proc: %6"
        ).arg(
            this->ui_->labelAppNameVer->text(),
            this->ui_->labelQtVer->text(),
            this->ui_->labelTablerIconsVer->text(),
            this->ui_->labelAnitomyVer->text(),
            this->ui_->labelAnimeRelationsVer->text(),
            this->ui_->labelUtf8procVer->text()
        );

        clipboard->setText(info);
    });
}

AboutDialog::~AboutDialog() {
    delete this->ui_;
}

void AboutDialog::showOrFocus() {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
        return;
    }
    this->show();
}

void AboutDialog::setupDialog() {
    this->ui_->labelAppNameVer->setText(
        QStringLiteral("%1 v%2").arg(
            QApplication::applicationName(),
            QApplication::applicationVersion()
        )
    );

    this->ui_->labelQtVer->setText(qVersion());
    this->ui_->labelTablerIconsVer->setText(TABLER_ICONS_VERSION);
    this->ui_->labelAnitomyVer->setText(ANITOMY_GIT_COMMIT);
    this->ui_->labelAnimeRelationsVer->setText(ANIME_RELATIONS_GIT_COMMIT);
    this->ui_->labelUtf8procVer->setText(UTF8PROC_GIT_COMMIT);

}

