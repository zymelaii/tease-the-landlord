#include <vector>
#include "card.h"
#include "handtype.h"
class Player {
public:
    Player(const std::string& name) : name(name) {}

    void receiveCard(const Card& card) {
        hand.push_back(card);
    }

    virtual HandType playCard(const HandType currentHand) = 0;

private:
    std::string name;
    std::vector<Card> hand;
};
