#pragma once

#include "app/app.hpp"

#include <QCommandLineParser>


class CommandLineParser final {
public:
    explicit CommandLineParser(const App &app);
    ~CommandLineParser() = default;

    bool isWindowOptionSet() const;

private:
    QCommandLineParser parser_;

    const QCommandLineOption window_option_ = {
        {"window", "w"},
        "Start the application as a dialog window."
    };
};
