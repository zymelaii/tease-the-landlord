#pragma once

struct Card {
    enum Suit {
        HEARTS,
        DIAMONDS,
        CLUBS,
        SPADES,
        JOKER,
    };

    enum Rank {
        BIG_JOKER,
        SMALL_JOKER,
        KING,
        QUEEN,
        JACK,
        TEN,
        NINE,
        EIGHT,
        SEVEN,
        SIX,
        FIVE,
        FOUR,
        THREE,
        TWO,
        ACE
    };

    const Suit suit;
    const Rank rank;

    bool operator<(const Card& other) const {
        return rank < other.rank;
    }

    bool operator==(const Card& other) const {
        return rank == other.rank;
    }
};
