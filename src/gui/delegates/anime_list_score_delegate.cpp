#include "gui/delegates/anime_list_score_delegate.hpp"

#include "gui/models/anime_list_model.hpp"


AnimeListScoreDelegate::AnimeListScoreDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeListScoreDelegate::stopEditing() {
    if (!this->editor_) {
        return;
    }

    emit closeEditor(
        this->editor_,
        QAbstractItemDelegate::NoHint
    );
}

QWidget* AnimeListScoreDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) {
        return nullptr;
    }

    AnimeListScoreEditor *editor = new AnimeListScoreEditor(parent);
    this->editor_ = editor;

    connect(editor, &AnimeListScoreEditor::scoreAccepted, this, &AnimeListScoreDelegate::onScoreAccepted);

    return editor;
}

void AnimeListScoreDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto *my_editor = qobject_cast<AnimeListScoreEditor*>(editor);
    if (!my_editor) {
        return;
    }

    my_editor->setData(
        index.data(AnimeListModel::UserRoles::Anime).value<AnilistAnime>()
    );
}

void AnimeListScoreDelegate::onScoreAccepted() {
    auto *editor = qobject_cast<AnimeListScoreEditor*>(this->sender());

    if (!editor) {
        return;
    }

    emit requestSetAnimeScore(
        editor->anime(),
        editor->score()
    );

    emit closeEditor(editor);
}
