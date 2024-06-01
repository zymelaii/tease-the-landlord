enum Suit {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES,
    JOKER // 对应大小王
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

class Card {
public:
    Card(Suit suit, Rank rank) : suit(suit), rank(rank) {}

    Suit getSuit() const { return suit; }
    Rank getRank() const { return rank; }

    // 可以定义显示牌的函数
    void display() const;

private:
    Suit suit;
    Rank rank;
};
