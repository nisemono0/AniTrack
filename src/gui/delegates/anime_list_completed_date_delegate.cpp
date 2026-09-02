#include "gui/delegates/anime_list_completed_date_delegate.hpp"
#include "gui/delegates/anime_list_completed_date_editor.hpp"

#include "gui/models/anime_list_model.hpp"


AnimeListCompletedDateDelegate::AnimeListCompletedDateDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QWidget* AnimeListCompletedDateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) {
        return nullptr;
    }

    AnimeListCompletedDateEditor *editor = new AnimeListCompletedDateEditor(parent);
    connect(editor, &AnimeListCompletedDateEditor::completedDateAccepted, this, &AnimeListCompletedDateDelegate::onCompletedDateAccepted);

    return editor;
}

void AnimeListCompletedDateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto *my_editor = qobject_cast<AnimeListCompletedDateEditor*>(editor);

    if (!my_editor) {
        return;
    }

    my_editor->setData(
        index.data(AnimeListModel::UserRoles::Anime).value<AnilistAnime>()
    );
}

void AnimeListCompletedDateDelegate::onCompletedDateAccepted() {
    auto *editor = qobject_cast<AnimeListCompletedDateEditor*>(this->sender());

    if (!editor) {
        return;
    }

    emit requestSetAnimeCompletedDate(
        editor->anime(),
        editor->completedDate()
    );

    emit closeEditor(editor);
}

