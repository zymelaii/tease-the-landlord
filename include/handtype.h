#include "card.h"
enum HandCategory {
    SINGLE,
    PAIR,
    TRIPLE,
    STRAIGHT,
    BOMB,
    UNKNOWN
};

enum ComparisonResult {
    SMALLER,
    EQUAL,
    GREATER,
    UNKNOWN
};


#include <vector>
#include <algorithm>

class HandType {
public:
    HandType(const std::vector<Card>& cards) {
        this->cards = cards;
        determineCategory();
    }

    HandCategory getCategory() const {
        return category;
    }

    ComparisonResult compare(const HandType& other) const {
        //TODO: 实现该函数，比较两手牌的大小
    }

private:
    std::vector<Card> cards;
    HandCategory category;

    void determineCategory() {
        //TODO: 实现该函数，确定手牌的类型
    }

    bool allSameRank() const {
        Rank rank = cards[0].getRank();
        for (const Card& card : cards) {
            if (card.getRank() != rank) {
                return false;
            }
        }
        return true;
    }

    ComparisonResult compareBombs(const HandType& other) const {
        if (cards[0].getRank() > other.cards[0].getRank()) {
            return GREATER;
        } else if (cards[0].getRank() < other.cards[0].getRank()) {
            return SMALLER;
        } else {
            return EQUAL;
        }
    }

    ComparisonResult compareStandard(const HandType& other) const {
        // 假设是单张，其他类型需额外处理
        if (cards[0].getRank() > other.cards[0].getRank()) {
            return GREATER;
        } else if (cards[0].getRank() < other.cards[0].getRank()) {
            return SMALLER;
        } else {
            return EQUAL;
        }
    }
};
