#include "base/anilist/anilist_entry.hpp"
#include "base/anilist/anilist_keys.hpp"

#include "base/database/database_columns.hpp"

#include "utils/anilist.hpp"
#include "utils/enums.hpp"
#include "utils/date.hpp"


AnilistEntry AnilistEntry::fromResponseJson(const QJsonObject &json_obj) {
    AnilistEntry entry;

    entry.local_id_ = AnilistEntry::InvalidId;
    entry.id_ = json_obj.value(AnilistKeys::MediaList::Id).toInt();

    entry.is_private_ = json_obj.value(AnilistKeys::MediaList::Private).toBool();

    AnilistEntry::State state;
    state.status = EnumsUtils::enumFromString(
        json_obj.value(AnilistKeys::MediaList::Status).toString(),
        AnilistEntry::Status::INVALID
    );
    state.progress = json_obj.value(AnilistKeys::MediaList::Progress).toInt();
    state.repeat = json_obj.value(AnilistKeys::MediaList::Repeat).toInt();
    state.notes = json_obj.value(AnilistKeys::MediaList::Notes).toString();
    state.score = json_obj.value(AnilistKeys::MediaList::Score).toInt();
    state.started_at = AnilistUtils::dateFromFuzzyDate(
        json_obj.value(AnilistKeys::MediaList::StartedAt).toObject()
    );
    state.completed_at = AnilistUtils::dateFromFuzzyDate(
        json_obj.value(AnilistKeys::MediaList::CompletedAt).toObject()
    );
    state.updated_at = json_obj.value(AnilistKeys::MediaList::UpdatedAt).toInteger();
    state.pending_operation = PendingOperation::NONE;

    entry.state_ = state;
    entry.current_state_idx_ = 0;
    entry.state_history_.append(entry.state_);

    return entry;
}

AnilistEntry AnilistEntry::fromDatabaseQuery(const QSqlQuery &query) {
    AnilistEntry entry;

    entry.local_id_ = query.value(DatabaseColumns::Entry::LocalId).toInt();

    const QVariant id_variant = query.value(DatabaseColumns::Entry::Id);
    entry.id_ = id_variant.isNull() ? InvalidId : id_variant.toInt();

    entry.is_private_ = query.value(DatabaseColumns::Entry::IsPrivate).toBool();

    State state;

    state.status = query.value(DatabaseColumns::Entry::Status).value<AnilistEntry::Status>();
    state.progress = query.value(DatabaseColumns::Entry::Progress).toInt();
    state.repeat = query.value(DatabaseColumns::Entry::Repeat).toInt();
    state.notes = query.value(DatabaseColumns::Entry::Notes).toString();
    state.score = query.value(DatabaseColumns::Entry::Score).toInt();
    state.started_at = DateUtils::dateFromString(
        query.value(DatabaseColumns::Entry::StartedAt).toString()
    );
    state.completed_at = DateUtils::dateFromString(
        query.value(DatabaseColumns::Entry::CompletedAt).toString()
    );
    state.updated_at = query.value(DatabaseColumns::Entry::UpdatedAt).toLongLong();
    state.pending_operation = query.value(DatabaseColumns::Entry::PendingOperation).value<AnilistEntry::PendingOperation>();

    entry.state_ = state;
    entry.current_state_idx_ = 0;
    entry.state_history_.append(entry.state_);

    return entry;
}

void AnilistEntry::setLocalId(int id) {
    this->local_id_ = id;
}

void AnilistEntry::setId(int id) {
    this->id_ = id;
}

bool AnilistEntry::setCurrentState(int index) {
    if (index < 0 || index >= this->state_history_.size()) {
        return false;
    }

    this->current_state_idx_ = index;
    this->state_ = this->state_history_.at(index);

    return true;
}

void AnilistEntry::addState(const State &state) {
    this->state_ = state;
    this->pushState(state);
}

bool AnilistEntry::undoState() {
    if (!this->canUndoState()) {
        return false;
    }

    this->current_state_idx_ -= 1;
    this->state_ = this->state_history_.at(this->current_state_idx_);

    return true;
}

bool AnilistEntry::redoState() {
    if (!this->canRedoState()) {
        return false;
    }

    this->current_state_idx_ += 1;
    this->state_ = this->state_history_.at(this->current_state_idx_);

    return true;
}

int AnilistEntry::localId() const {
    return this->local_id_;
}

int AnilistEntry::id() const {
    return this->id_;
}

bool AnilistEntry::isPrivate() const {
    return this->is_private_;
}

const AnilistEntry::State& AnilistEntry::state() const {
    return this->state_;
}

const QList<AnilistEntry::State>& AnilistEntry::stateHistory() const {
    return this->state_history_;
}

int AnilistEntry::currentStateIndex() const {
    return this->current_state_idx_;
}

bool AnilistEntry::canUndoState() const {
    return this->current_state_idx_ > 0;
}

bool AnilistEntry::canRedoState() const {
    return this->current_state_idx_ + 1 < this->state_history_.size();
}

void AnilistEntry::pushState(const AnilistEntry::State &state) {
    int new_idx = this->current_state_idx_ + 1;

    this->state_history_.resize(new_idx);

    this->state_history_.append(state);
    this->current_state_idx_ = new_idx;

     if (this->state_history_.size() > MaxUndos + 1) {
        this->state_history_.removeFirst();
        this->current_state_idx_ -= 1;
    }
}

