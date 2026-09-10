#include "base/recognition/string_similarity.hpp"

#include <QList>


double StringSimilarity::levenshteinSimilarity(const QString &left, const QString &right) {
    if (left == right) {
        return 1.0;
    }

    const int left_len = left.size();
    const int right_len = right.size();

    QList<int> prev_row(right_len + 1, 0);
    QList<int> curr_row(right_len + 1, 0);

    for (int j = 0; j <= right_len; j++) {
        prev_row[j] = j;
    }

    for (int i = 1; i <= left_len; i++) {
        curr_row[0] = i;

        for (int j = 1; j <= right_len; j++) {
            if (left[i - 1] == right[j - 1]) {
                curr_row[j] = prev_row[j - 1];
            }
            else {
                curr_row[j] = 1 + qMin(
                    curr_row[j - 1],
                    qMin(prev_row[j], prev_row[j - 1])
                );
            }
        }

        prev_row.swap(curr_row);
    }

    const double distance = prev_row.at(right_len);

    return 1.0 - (distance / qMax(left.size(), right.size()));
}

