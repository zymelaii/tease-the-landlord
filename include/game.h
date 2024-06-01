#include <vector>
#include <iostream>
#include "player.h"
#include "deck.h"

class Game {
public:
    Game() {
        // 初始化玩家
        players.push_back(Player("Player 1"));
        players.push_back(Player("Player 2"));
        players.push_back(Player("Player 3"));
    }

    void startGame();

private:
    std::vector<Player> players;
    Deck deck;
    std::vector<Card> bottomCards;
};
