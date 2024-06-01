#pragma once

#include <vector>
#include <optional>
#include <initializer_list>
#include <assert.h>
#include "card.h"

class CardGroup {
public:
    enum class Category {
        Single,
        Pair,
        Triple,
        Straight,
        Bomb,
        //! TODO: ...
    };

public:
    static bool validate(Category category, std::vector<Card> &cards);
    static int  score(const CardGroup &group);

    bool operator>(const CardGroup &group) const {
        return score(*this) > score(group);
    }

    bool operator==(const CardGroup &group) const {
        return score(*this) == score(group);
    }

    CardGroup(Category category, const std::vector<Card> &cards)
        : category_(category)
        , cards_(cards) {
        assert(validate(category_, cards_));
    }

    Category category() const {
        return category_;
    }

    const std::vector<Card> &cards() const {
        return cards_;
    }

    class CardGroupBuilder {
    public:
        CardGroupBuilder(Category category)
            : category_(category) {}

        CardGroupBuilder &with_card(Card card) {
            cards_.push_back(card);
            return *this;
        }

        CardGroupBuilder &with_card_set(std::initializer_list<Card> &&cards) {
            for (const auto &card : cards) { with_card(card); }
            return *this;
        }

        std::optional<CardGroup> build() {
            if (validate(category_, cards_)) {
                return {CardGroup(category_, cards_)};
            }
            return std::nullopt;
        }

    private:
        Category          category_;
        std::vector<Card> cards_;
    };

private:
    const Category    category_;
    std::vector<Card> cards_;
};
