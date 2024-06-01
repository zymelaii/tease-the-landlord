#include"player.h"
#include"card.h"
#include"handtype.h"

class Human:public Player{
public:
    Human(const std::string& name):Player(name){}
    HandType playCard(const HandType currentHand);
    void showHand() const;
};