#pragma once

#include "app/app.hpp"

#include <QCommandLineParser>


class CommandLineParser final {
public:
    explicit CommandLineParser(const App &app);
    ~CommandLineParser() = default;

    bool isDialogOptionSet() const;

private:
    QCommandLineParser parser_;

    const QCommandLineOption window_option_ = {
        {"dialog", "d"},
        "Start the application as a dialog (hacky way to start as floating in some tiling window managers).",
    };
};
