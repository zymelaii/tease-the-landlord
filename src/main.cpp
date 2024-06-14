#include <QImage>
#include <QDebug>
#include <QApplication>
#include <memory>
#include "ui/scene.h"
#include "card.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    auto w = std::make_unique<ui::Scene>();

    w->cards.push_back(Card{Card::Suit::HEARTS, Card::Rank::FIVE});
    w->cards.push_back(Card{Card::Suit::DIAMONDS, Card::Rank::FIVE});
    w->cards.push_back(Card{Card::Suit::HEARTS, Card::Rank::SEVEN});
    w->cards.push_back(Card{Card::Suit::JOKER, Card::Rank::SMALL_JOKER});
    w->cards.push_back(Card{Card::Suit::CLUBS, Card::Rank::ACE});

    w->player_left_total_cards  = 8;
    w->player_right_total_cards = 13;

    w->show();

    return app.exec();
}
