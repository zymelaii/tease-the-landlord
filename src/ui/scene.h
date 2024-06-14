#pragma once

#include <QWidget>
#include <set>
#include "../card.h"

namespace ui {

class Scene : public QWidget {
    Q_OBJECT

public:
    static QString get_card_name(Card card);

public:
    explicit Scene(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public:
    std::vector<Card> cards;
    int               player_left_total_cards;
    int               player_right_total_cards;

private:
    std::set<int> selected_cards_;
    QSize         card_size_;
};

} // namespace ui
