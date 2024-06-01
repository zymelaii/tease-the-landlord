#include "player.h"

class RobotPlayer : public Player {
public:
    std::optional<CardGroup> make_decision() override;
};
