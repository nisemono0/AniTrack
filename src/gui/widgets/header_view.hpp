#pragma once

#include <QWidget>
#include <QHeaderView>
#include <QHash>
#include <QSize>


class HeaderView : public QHeaderView {
    Q_OBJECT;
public:
    explicit HeaderView(
        Qt::Orientation orientation,
        QWidget *parent = nullptr
    );
    ~HeaderView() = default;

    // Set padding size for column. Set to <= 0 to remove the padding
    void setSectionPadding(int column, int size);
    // Padding for column
    int sectionPadding(int column) const;
    // Save the current header state
    void saveCurrentState();
    // Resotre the previous saved header state
    void restorePreviousState();

protected:
    QSize sectionSizeFromContents(int logical_index) const override;

private:
    QHash<int, int> paddings_;
    QByteArray previous_state_;
};

