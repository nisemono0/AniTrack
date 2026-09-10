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

bool CommandLineParser::isWindowOptionSet() const {
    return this->parser_.isSet(this->window_option_);
}

QString CommandLineParser::selectedStyle() const {
    const QString selected_style = this->parser_.value(this->style_option_);

    const QStringList style_keys = QStyleFactory::keys();
    for (const auto &app_style : style_keys) {
        if (app_style.compare(selected_style, Qt::CaseInsensitive) == 0) {
            return app_style;
        }
    }

    return QString();
}
