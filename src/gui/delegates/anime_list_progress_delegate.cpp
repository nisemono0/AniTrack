#include "gui/delegates/anime_list_progress_delegate.hpp"

#include "gui/models/anime_list_model.hpp"


AnimeListProgressDelegate::AnimeListProgressDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeListProgressDelegate::stopEditing() {
    if (!this->editor_) {
        return;
    }

    emit closeEditor(
        this->editor_,
        QAbstractItemDelegate::NoHint
    );

}

void AnimeListProgressDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    if (!option.widget) {
        return;
    }

    int progress = index.data(AnimeListModel::UserRoles::EntryProgress).toInt();
    int episodes = index.data(AnimeListModel::UserRoles::MediaEpisodes).toInt();

    QStyleOptionProgressBar option_progressbar;
    option_progressbar.rect = option.rect;
    option_progressbar.minimum = 0;
    option_progressbar.maximum = qMax(episodes, 1);
    option_progressbar.progress = progress;
    if (episodes <= 0) {
        option_progressbar.text = QStringLiteral("%1/?").arg(progress);
    } else {
        option_progressbar.text = QStringLiteral("%1/%2").arg(progress)
                                                         .arg(episodes);
    }

    option_progressbar.textVisible = true;
    option.widget->style()->drawControl(
        QStyle::CE_ProgressBar,
        &option_progressbar,
        painter
    );
}

QWidget* AnimeListProgressDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) {
        return nullptr;
    }

    AnimeListProgressEditor *editor = new AnimeListProgressEditor(parent);
    this->editor_ = editor;

    connect(editor, &AnimeListProgressEditor::minusButtonClicked, this, &AnimeListProgressDelegate::requestDecreaseAnimeProgress);
    connect(editor, &AnimeListProgressEditor::plusButtonClicked, this, &AnimeListProgressDelegate::requestIncreaseAnimeProgress);

    return editor;
}

void AnimeListProgressDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto *my_editor = qobject_cast<AnimeListProgressEditor*>(editor);
    if (!my_editor) {
        return;
    }

    my_editor->setData(
        index.data(AnimeListModel::UserRoles::Anime).value<AnilistAnime>()
    );
}

