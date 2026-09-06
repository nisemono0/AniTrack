#include "gui/delegates/anime_list_started_date_delegate.hpp"

#include "gui/models/anime_list_model.hpp"


AnimeListStartedDateDelegate::AnimeListStartedDateDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeListStartedDateDelegate::stopEditing() {
    if (!this->editor_) {
        return;
    }

    emit closeEditor(
        this->editor_,
        QAbstractItemDelegate::NoHint
    );
}

QWidget* AnimeListStartedDateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) {
        return nullptr;
    }

    AnimeListStartedDateEditor *editor = new AnimeListStartedDateEditor(parent);
    this->editor_ = editor;

    connect(editor, &AnimeListStartedDateEditor::startedDateAccepted, this, &AnimeListStartedDateDelegate::onStartedDateAccepted);

    return editor;
}

void AnimeListStartedDateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto *my_editor = qobject_cast<AnimeListStartedDateEditor*>(editor);

    if (!my_editor) {
        return;
    }

    my_editor->setData(
        index.data(AnimeListModel::UserRoles::Anime).value<AnilistAnime>()
    );
}

void AnimeListStartedDateDelegate::onStartedDateAccepted() {
    auto *editor = qobject_cast<AnimeListStartedDateEditor*>(this->sender());

    if (!editor) {
        return;
    }

    emit requestSetAnimeStartedDate(
        editor->anime(),
        editor->startedDate()
    );

    emit closeEditor(editor);
}

