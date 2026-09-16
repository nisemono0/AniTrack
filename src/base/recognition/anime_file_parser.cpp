#include "base/recognition/anime_file_parser.hpp"

#include <charconv>
#include <algorithm>
#include <string>
#include <vector>

#include <anitomy.hpp>


AnimeFileParser::AnimeFileInfo AnimeFileParser::parse(const QString &filename) {
    const std::string utf8_filename = filename.normalized(QString::NormalizationForm_KC).toUtf8().toStdString();
    std::vector<anitomy::Element> elements = anitomy::parse(utf8_filename);

    AnimeFileParser::AnimeFileInfo parsed_anime_file;

    // Get the parse anime title
    const auto title_element = std::ranges::find_if(elements, [] (const anitomy::Element &element) {
        return element.kind == anitomy::ElementKind::Title;
    });
    if (title_element != elements.end()) {
        parsed_anime_file.title = QString::fromStdString(title_element->value);
    }

    // Get the parse episode number
    const auto episode_element = std::ranges::find_if(elements, [] (const anitomy::Element &element) {
        return element.kind == anitomy::ElementKind::Episode;
    });
    if (episode_element != elements.end()) {
        const auto result = std::from_chars(
            episode_element->value.data(),
            episode_element->value.data() + episode_element->value.size(),
            parsed_anime_file.episode,
            10
        );
    }

    // Get the parsed season text and append it to the title
    const auto season_element = std::ranges::find_if(elements, [] (const anitomy::Element &element) {
        return element.kind == anitomy::ElementKind::Season;
    });
    if (season_element != elements.end()) {
        const auto first_non_zero = season_element->value.find_first_not_of('0');

        if (first_non_zero == std::string::npos) {
            season_element->value = "0";
        } else {
            season_element->value.erase(0, first_non_zero);
        }

        parsed_anime_file.title.append(
            QStringLiteral(" S%1").arg(season_element->value)
        );
    }

    return parsed_anime_file;
}

