#pragma once

#include "app/app.hpp"

#include <QCommandLineParser>


class CommandLineParser final {
public:
    explicit CommandLineParser(const App &app);
    ~CommandLineParser() = default;

    bool isWindowOptionSet();
    QString selectedStyle();

private:
    QCommandLineParser parser_;

    QCommandLineOption window_option_ = {
        {"window", "w"},
        "Start the application as a dialog window."
    };

    QCommandLineOption style_option_ = {
        {"style", "s"},
        "Force set the application style (Fusion or Windows). Uses the system default if not specified.",
        "style"
    };
};
