#include "app/command_line_parser.hpp"
#include "app/app_config.hpp"

#include <QStyleFactory>


CommandLineParser::CommandLineParser(const App &app) {
    this->parser_.setApplicationDescription(AppConfig::Description);
    this->parser_.addHelpOption();
    this->parser_.addVersionOption();

    this->parser_.addOption(this->window_option_);

    this->parser_.process(app);
}

bool CommandLineParser::isDialogOptionSet() const {
    return this->parser_.isSet(this->window_option_);
}

