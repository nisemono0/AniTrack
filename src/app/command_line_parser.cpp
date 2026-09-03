#include "app/command_line_parser.hpp"
#include "app/app_config.hpp"

#include <QStyleFactory>


CommandLineParser::CommandLineParser(const App &app) {
    this->parser_.setApplicationDescription(AppConfig::Description);
    this->parser_.addHelpOption();

    this->parser_.addOption(this->window_option_);
    this->parser_.addOption(this->style_option_);

    this->parser_.process(app);
}

bool CommandLineParser::isWindowOptionSet() {
    return this->parser_.isSet(this->window_option_);
}

QString CommandLineParser::selectedStyle() {
    QString selected_style = this->parser_.value(this->style_option_);

    QStringList style_keys = QStyleFactory::keys();
    for (const auto &app_style : std::as_const(style_keys)) {
        if (app_style.compare(selected_style, Qt::CaseInsensitive) == 0) {
            return app_style;
        }
    }

    return QString();
}
