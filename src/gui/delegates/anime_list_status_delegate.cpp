#include "gui/delegates/anime_list_status_delegate.hpp"
#include "gui/delegates/anime_list_status_editor.hpp"

#include "gui/models/anime_list_model.hpp"


AnimeListStatusDelegate::AnimeListStatusDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeListStatusDelegate::stopEditing() {
    if (!this->editor_) {
        return;
    }

    emit closeEditor(
        this->editor_,
        QAbstractItemDelegate::NoHint
    );
}

QWidget* AnimeListStatusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) {
        return nullptr;
    }

    AnimeListStatusEditor *editor = new AnimeListStatusEditor(parent);
    this->editor_ = editor;

    connect(editor, &AnimeListStatusEditor::statusAccepted, this, &AnimeListStatusDelegate::onStatusAccepted);

    return editor;
}

void AnimeListStatusDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto *my_editor = qobject_cast<AnimeListStatusEditor*>(editor);

    if (!my_editor) {
        return;
    }

    my_editor->setData(
        index.data(AnimeListModel::UserRoles::Anime).value<AnilistAnime>()
    );
}

void AnimeListStatusDelegate::onStatusAccepted() {
    auto *editor = qobject_cast<AnimeListStatusEditor*>(this->sender());

    if (!editor) {
        return;
    }

    emit requestSetAnimeStatus(
        editor->anime(),
        editor->status()
    );

    emit closeEditor(editor);
}

