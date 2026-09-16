#include "gui/dialogs/settings_dialog.hpp"

#include "utils/settings.hpp"

#include <QPushButton>


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::SettingsDialog) {

    this->initDialog();
    this->updateDialog();

    for (auto &button : this->ui_->buttonBox->buttons()) {
        button->setFocusPolicy(Qt::NoFocus);
    }

    connect(this, &QDialog::accepted, this, &SettingsDialog::onDialogAccepted);

    auto *restore_button = this->ui_->buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(restore_button, &QPushButton::clicked, this, &SettingsDialog::onResetClicked);
}

SettingsDialog::~SettingsDialog() {
    delete this->ui_;
}

void SettingsDialog::showOrFocus() {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
        return;
    }

    this->updateDialog();
    this->show();
}

void SettingsDialog::initDialog() {
    this->ui_->setupUi(this);
}

void SettingsDialog::updateDialog() {
    this->updateSyncSettings();
    this->updateRecognitionSettings();
    this->updateUiSettings();
}

void SettingsDialog::updateSyncSettings() {
    this->ui_->checkBoxUpdateUserInfo->setChecked(
        Settings::get(Settings::Sync::UpdateUseInfoOnStart, false)
    );
    this->ui_->checkBoxUpdateAnimeInfo->setChecked(
        Settings::get(Settings::Sync::UpdateAnimeInfoOnStart, false)
    );
    this->ui_->checkBoxSyncAnime->setChecked(
        Settings::get(Settings::Sync::SyncAnimeOnStart, false)
    );
}

void SettingsDialog::updateRecognitionSettings() {
    this->ui_->checkBoxEnableRecognition->setChecked(
        Settings::get(Settings::Recognition::EnableAnimeRecognition, true)
    );
    this->ui_->checkBoxEnableRecognitionPopup->setChecked(
        Settings::get(Settings::Recognition::EnableRecognitionPopup, true)
    );
    this->ui_->spinBoxMaxMatches->setValue(
        Settings::get(Settings::Recognition::MaxCacheMatches, 10)
    );
    this->ui_->doubleSpinBoxMatchScore->setValue(
        Settings::get(Settings::Recognition::MinMatchScore, 0.5)
    );
    this->ui_->spinBoxPopupTimerDelay->setValue(
        Settings::get(Settings::Recognition::RecognitionPopupDelay, 120)
    );

    const bool recognition_enabled = this->ui_->checkBoxEnableRecognition->isChecked();
    this->ui_->checkBoxEnableRecognitionPopup->setEnabled(recognition_enabled);
    this->ui_->spinBoxMaxMatches->setEnabled(recognition_enabled);
    this->ui_->doubleSpinBoxMatchScore->setEnabled(recognition_enabled);

    const bool recognition_popup_enabled = this->ui_->checkBoxEnableRecognitionPopup->isChecked() && recognition_enabled;
    this->ui_->spinBoxPopupTimerDelay->setEnabled(recognition_popup_enabled);
}

void SettingsDialog::updateUiSettings() {
    this->ui_->checkBoxStartMinimized->setChecked(
        Settings::get(Settings::Ui::Window::StartMinimized, false)
    );
    this->ui_->checkBoxSaveWindowState->setChecked(
        Settings::get(Settings::Ui::Window::SaveStateOnQuit, false)
    );
    this->ui_->checkBoxSaveAnimeHeaderState->setChecked(
        Settings::get(Settings::Ui::Window::SaveAnimeHeaderState, false)
    );
    this->ui_->checkBoxSaveSearchHeaderState->setChecked(
        Settings::get(Settings::Ui::Window::SaveSearchHeaderState, false)
    );
}

void SettingsDialog::onDialogAccepted() {
    // Save sync settings
    Settings::set(
        Settings::Sync::UpdateUseInfoOnStart,
        this->ui_->checkBoxUpdateUserInfo->isChecked()
    );
    Settings::set(
        Settings::Sync::UpdateAnimeInfoOnStart,
        this->ui_->checkBoxUpdateAnimeInfo->isChecked()
    );
    Settings::set(
        Settings::Sync::SyncAnimeOnStart,
        this->ui_->checkBoxSyncAnime->isChecked()
    );

    // Save recognition settings
    Settings::set(
        Settings::Recognition::EnableAnimeRecognition,
        this->ui_->checkBoxEnableRecognition->isChecked()
    );
    Settings::set(
        Settings::Recognition::EnableRecognitionPopup,
        this->ui_->checkBoxEnableRecognitionPopup->isChecked()
    );
    Settings::set(
        Settings::Recognition::MaxCacheMatches,
        this->ui_->spinBoxMaxMatches->value()
    );
    Settings::set(
        Settings::Recognition::MinMatchScore,
        this->ui_->doubleSpinBoxMatchScore->value()
    );
    Settings::set(
        Settings::Recognition::RecognitionPopupDelay,
        this->ui_->spinBoxPopupTimerDelay->value()
    );

    // Ui settings
    Settings::get(
        Settings::Ui::Window::StartMinimized,
        this->ui_->checkBoxStartMinimized->isChecked()
    );
    Settings::get(
        Settings::Ui::Window::SaveStateOnQuit,
        this->ui_->checkBoxSaveWindowState->isChecked()
    );
    Settings::get(
        Settings::Ui::Window::SaveAnimeHeaderState,
        this->ui_->checkBoxSaveAnimeHeaderState->isChecked()
    );
    Settings::get(
        Settings::Ui::Window::SaveSearchHeaderState,
        this->ui_->checkBoxSaveSearchHeaderState->isChecked()
    );

    // Signal settings changes
    Settings::signalChanges();
}

void SettingsDialog::onResetClicked() {
    // Reset sync settings
    Settings::set(Settings::Sync::UpdateUseInfoOnStart, false);
    Settings::set(Settings::Sync::UpdateAnimeInfoOnStart, false);
    Settings::set(Settings::Sync::SyncAnimeOnStart, false);

    // Reset recognition settings
    Settings::set(Settings::Recognition::EnableAnimeRecognition, true);
    Settings::set(Settings::Recognition::EnableRecognitionPopup, true);
    Settings::set(Settings::Recognition::MaxCacheMatches, 10);
    Settings::set(Settings::Recognition::MinMatchScore, 0.5);
    Settings::set(Settings::Recognition::RecognitionPopupDelay, 120);

    // Reset ui settings
    Settings::set(Settings::Ui::Window::StartMinimized, false);
    Settings::set(Settings::Ui::Window::SaveStateOnQuit, false);
    Settings::set(Settings::Ui::Window::SaveAnimeHeaderState, false);
    Settings::set(Settings::Ui::Window::SaveSearchHeaderState, false);

    this->updateDialog();
}

