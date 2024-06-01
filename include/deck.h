#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include "card.h"

class Deck {
public:
    Deck() {
        // 初始化一副牌
        for (int s = HEARTS; s <= SPADES; ++s) {
            for (int r = KING; r <= ACE; ++r) {
                cards.push_back(Card(static_cast<Suit>(s), static_cast<Rank>(r)));
            }
        }
        // 添加大小王
        cards.push_back(Card(JOKER, SMALL_JOKER));
        cards.push_back(Card(JOKER, BIG_JOKER));
        shuffle();
    }

    // 洗牌函数
    void shuffle() {
        std::shuffle(cards.begin(), cards.end(), std::default_random_engine(std::time(0)));
    }

    // 发牌函数
    Card deal() {
        Card dealtCard = cards.back();
        cards.pop_back();
        return dealtCard;
    }

private:
    std::vector<Card> cards;
};
