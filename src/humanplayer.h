#include "player.h"

class HumanPlayer : public Player {
public:
    std::optional<CardGroup> make_decision() override;
};
