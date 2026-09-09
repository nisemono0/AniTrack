#include "base/recognition/title_normalizer.hpp"

#include <utf8proc.h>

#include <QRegularExpression>


// This follows taiga's normalization. Some parts are shamelessly taken from it
// https://github.com/erengy/taiga/blob/520d10f9ef7cd671f8ed653706f914a79cdf1274/src/track/recognition_normalize.cpp#L27
namespace {
struct ReplaceRule {
    QRegularExpression regex;
    QString value;
};

void convertRomanNumbers(QString &title) {
    static const QList<ReplaceRule> roman_numbers{
        { QRegularExpression(QStringLiteral(R"(\bII\b)")),   QStringLiteral("2") },
        { QRegularExpression(QStringLiteral(R"(\bIII\b)")),  QStringLiteral("3") },
        { QRegularExpression(QStringLiteral(R"(\bIV\b)")),   QStringLiteral("4") },
        { QRegularExpression(QStringLiteral(R"(\bV\b)")),    QStringLiteral("5") },
        { QRegularExpression(QStringLiteral(R"(\bVI\b)")),   QStringLiteral("6") },
        { QRegularExpression(QStringLiteral(R"(\bVII\b)")),  QStringLiteral("7") },
        { QRegularExpression(QStringLiteral(R"(\bVIII\b)")), QStringLiteral("8") },
        { QRegularExpression(QStringLiteral(R"(\bIX\b)")),   QStringLiteral("9") },
        { QRegularExpression(QStringLiteral(R"(\bXI\b)")),   QStringLiteral("11") },
        { QRegularExpression(QStringLiteral(R"(\bXII\b)")),  QStringLiteral("12") },
        { QRegularExpression(QStringLiteral(R"(\bXIII\b)")), QStringLiteral("13") },
    };

    for (const auto &rule : roman_numbers) {
        title.replace(
            rule.regex,
            rule.value
        );
    }
}

void transliterateTitle(QString &title) {
    for (int i = 0; i < title.size(); i++) {
        QChar &ch = title[i];
        switch (ch.unicode()) {
            case u'@': {
                ch = u'a';
                break;
            }
            // multiplication sign (e.g. "Tasogare Otome x Amnesia")
            case 0x00D7: {
                ch = u'x';
                break;
            }
            // modifier letter colon (e.g. "Nisekoi:")
            case 0xA789: {
                ch = u':';
                break;
            }
            // latin capital letter o with macron
            case 0x014C: {
                title.replace(i, 1, QStringLiteral("ou"));
                break;
            }
            // latin small letter o with macron
            case 0x014D: {
                title.replace(i, 1, QStringLiteral("ou"));
                break;
            }
            // latin small letter u with macron
            case 0x016B: {
                title.replace(i, 1, QStringLiteral("uu"));
                break;
            }
        }
    }

    static const QList<ReplaceRule> transliterations{
        { QRegularExpression(QStringLiteral(R"(\bwa\b)")), QStringLiteral("ha") },
        { QRegularExpression(QStringLiteral(R"(\be\b)")),  QStringLiteral("he") },
        { QRegularExpression(QStringLiteral(R"(\bo\b)")),  QStringLiteral("wo") },
    };

    for (const auto &rule : transliterations) {
        title.replace(rule.regex, rule.value);
    }
}

void normalizeUnicode(QString &title) {
    // Use utf8proc since doing this with Qt seems like a pain
    constexpr utf8proc_option_t options = static_cast<utf8proc_option_t>(
        UTF8PROC_COMPAT | UTF8PROC_COMPOSE | UTF8PROC_STABLE    |
        UTF8PROC_IGNORE | UTF8PROC_STRIPCC | UTF8PROC_STRIPMARK |
        UTF8PROC_LUMP   |
        UTF8PROC_CASEFOLD
    );

    utf8proc_uint8_t *normalized_title = nullptr;
    const QByteArray utf8_title = title.toUtf8();

    const utf8proc_ssize_t length = utf8proc_map(
        reinterpret_cast<const utf8proc_uint8_t *>(utf8_title.constData()),
        static_cast<utf8proc_ssize_t>(utf8_title.size()),
        &normalized_title,
        options
    );

    if (length < 0 || !normalized_title) {
        free(normalized_title);
        return;
    }

    title = QString::fromUtf8(
        reinterpret_cast<const char *>(normalized_title),
        static_cast<qsizetype>(length)
    );

    free(normalized_title);
}

void convertOrdinalNumbers(QString &title) {
    static const QList<ReplaceRule> ordinal_numbers{
        { QRegularExpression(QStringLiteral(R"(\bfirst\b)")),   QStringLiteral("1st") },
        { QRegularExpression(QStringLiteral(R"(\bsecond\b)")),  QStringLiteral("2nd") },
        { QRegularExpression(QStringLiteral(R"(\bthird\b)")),   QStringLiteral("3rd") },
        { QRegularExpression(QStringLiteral(R"(\bfourth\b)")),  QStringLiteral("4th") },
        { QRegularExpression(QStringLiteral(R"(\bfifth\b)")),   QStringLiteral("5th") },
        { QRegularExpression(QStringLiteral(R"(\bsixth\b)")),   QStringLiteral("6th") },
        { QRegularExpression(QStringLiteral(R"(\bseventh\b)")), QStringLiteral("7th") },
        { QRegularExpression(QStringLiteral(R"(\beighth\b)")),  QStringLiteral("8th") },
        { QRegularExpression(QStringLiteral(R"(\bninth\b)")),   QStringLiteral("9th") },
    };

    for (const auto &rule : ordinal_numbers) {
        title.replace(rule.regex, rule.value);
    }
}

void convertSeasonNumbers(QString &title) {
    static const QList<ReplaceRule> season_numbers{
        { QRegularExpression(QStringLiteral(R"(\b(?:1st season|season 1|series 1|s1)\b)")), QStringLiteral("1") },
        { QRegularExpression(QStringLiteral(R"(\b(?:2nd season|season 2|series 2|s2)\b)")), QStringLiteral("2") },
        { QRegularExpression(QStringLiteral(R"(\b(?:3rd season|season 3|series 3|s3)\b)")), QStringLiteral("3") },
        { QRegularExpression(QStringLiteral(R"(\b(?:4th season|season 4|series 4|s4)\b)")), QStringLiteral("4") },
        { QRegularExpression(QStringLiteral(R"(\b(?:5th season|season 5|series 5|s5)\b)")), QStringLiteral("5") },
        { QRegularExpression(QStringLiteral(R"(\b(?:6th season|season 6|series 6|s6)\b)")), QStringLiteral("6") }
    };

    for (const auto &rule : season_numbers) {
        title.replace(rule.regex, rule.value);
    }
}

void cleanTitle(QString &title) {
    static const QList<ReplaceRule> clean_title{
        { QRegularExpression(QStringLiteral(R"(&)")),                 QStringLiteral("and") },
        { QRegularExpression(QStringLiteral(R"(\bthe animation\b)")), "" },
        { QRegularExpression(QStringLiteral(R"(\bthe\b)")),           "" },
        { QRegularExpression(QStringLiteral(R"(\bepisode\b)")),       "" },
        { QRegularExpression(QStringLiteral(R"(\boad\b)")),           QStringLiteral("ova") },
        { QRegularExpression(QStringLiteral(R"(\boav\b)")),           QStringLiteral("ova") },
        { QRegularExpression(QStringLiteral(R"(\bspecials\b)")),      QStringLiteral("sp") },
        { QRegularExpression(QStringLiteral(R"(\bspecial\b)")),       QStringLiteral("sp") },
        { QRegularExpression(QStringLiteral(R"(\(tv\))")),            "" }
    };

    for (const auto &rule : clean_title) {
        title.replace(rule.regex, rule.value);
    }
}

void replacePunctuations(QString &title) {
    for (auto &ch : title) {
        if (ch.isPunct()) {
            ch = u' ';
        } else if (0x2000 < ch.unicode() && ch.unicode() < 0x2767) {
            ch = u' ';
        }
    }
}

void simplifyTitle(QString &title) {
    title = title.simplified();
}
} // namespace

void TitleNormalizer::normalize(QString &title) {
    convertRomanNumbers(title);
    transliterateTitle(title);
    normalizeUnicode(title);
    convertOrdinalNumbers(title);
    convertSeasonNumbers(title);
    cleanTitle(title);
    replacePunctuations(title);
    simplifyTitle(title);
}

