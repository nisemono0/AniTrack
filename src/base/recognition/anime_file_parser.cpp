#include "base/recognition/anime_file_parser.hpp"

#include <charconv>
#include <algorithm>
#include <string>
#include <vector>

#include <anitomy.hpp>


AnimeFileParser::ParsedAnimeFile AnimeFileParser::parse(const QString &filename) {
    const std::string utf8_filename = filename.normalized(QString::NormalizationForm_KC).toUtf8().toStdString();
    const std::vector<anitomy::Element> elements = anitomy::parse(utf8_filename);

    AnimeFileParser::ParsedAnimeFile parsed_anime_file;

    const auto title_element = std::ranges::find_if(elements, [] (const anitomy::Element &element) {
        return element.kind == anitomy::ElementKind::Title;
    });
    if (title_element != elements.end()) {
        parsed_anime_file.title = QString::fromStdString(title_element->value);
    }

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

    return parsed_anime_file;
}

