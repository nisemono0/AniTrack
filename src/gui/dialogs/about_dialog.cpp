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
        QString info = QStringLiteral(
            "%1\n"
            "Qt: %2\n"
            "tabler-icons: %3\n"
#ifdef ENABLE_WIP_FEATURES
            "anitomy: %4\n"
            "anime-relations: %5"
#endif // ENABLE_WIP_FEATURES
        ).arg(
            this->ui_->labelAppNameVer->text(),
            this->ui_->labelQtVer->text(),
            this->ui_->labelTablerIconsVer->text()
#ifdef ENABLE_WIP_FEATURES
            ,
            this->ui_->labelAnitomyVer->text(),
            this->ui_->labelAnimeRelationsVer->text()
#endif // ENABLE_WIP_FEATURES
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

#ifdef ENABLE_WIP_FEATURES
    this->ui_->labelAnitomyVer->setText(ANITOMY_GIT_COMMIT);
    this->ui_->labelAnimeRelationsVer->setText(ANIME_RELATIONS_GIT_COMMIT);
#else
    this->ui_->labelAnitomy->hide();
    this->ui_->labelAnitomyVer->hide();
    this->ui_->labelAnimeRelations->hide();
    this->ui_->labelAnimeRelationsVer->hide();
#endif // ENABLE_WIP_FEATURES

}

