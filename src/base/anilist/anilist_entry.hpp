#pragma once

#include <QString>
#include <QDate>
#include <QList>
#include <QJsonObject>
#include <QSqlQuery>


class AnilistEntry final {
    Q_GADGET;

public:
    static AnilistEntry fromResponseJson(const QJsonObject &json_obj);
    static AnilistEntry fromDatabaseQuery(const QSqlQuery &query);

    static constexpr int InvalidId = -1;

    // The enums names MUST be exactly as the ones the anilist api returns
    enum class Status {
        INVALID = 0,
        CURRENT,
        PLANNING,
        COMPLETED,
        DROPPED,
        PAUSED,
        REPEATING
    };
    Q_ENUM(Status);

    enum class PendingOperation {
        NONE = 0,
        ADD,
        UPDATE,
        REMOVE
    };
    Q_ENUM(PendingOperation);

    struct State final {
        QString name = QStringLiteral("Initial");
        Status status = Status::INVALID;
        int progress = 0;
        int repeat = 0;
        QString notes;
        int score = 0;
        QDate started_at;
        QDate completed_at;
        qint64 updated_at = 0;
        PendingOperation pending_operation = PendingOperation::NONE;

        bool operator==(const State&) const = default;
    };

    void setLocalId(int id);
    void setId(int id);
    bool setCurrentState(int index);
    void addState(const State &state);

    bool undoState();
    bool redoState();

    int localId() const;
    int id() const;
    bool isPrivate() const;
    const State& state() const;
    const QList<State>& stateHistory() const;
    int currentStateIndex() const;

    bool canUndoState() const;
    bool canRedoState() const;

private:
    static constexpr int MaxUndos = 30;

    int local_id_ = InvalidId;
    int id_ = InvalidId;

    bool is_private_ = false;

    State state_;
    QList<State> state_history_;
    int current_state_idx_ = 0;

    void pushState(const AnilistEntry::State &state);
};

