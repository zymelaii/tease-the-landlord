#include <algorithm>
#include "game.h"
#include "deck.h"
#include "arbiter.h"
#include "player.h"

Game::Game(Deck* deck, Arbiter* arbiter)
    : deck_(deck)
    , arbiter_(arbiter)
    , on_play_(false)
    , player_index_(0)
    , current_total_players_(0)
    , last_winner_(-1) {}

int Game::total_players() const {
    return std::count_if(players_.begin(), players_.end(), [](const auto& ptr) {
        return !ptr.get();
    });
}

bool Game::add_player(Player* player) {
    const auto first_available_place =
        std::find_if(players_.begin(), players_.end(), [](const auto& ptr) {
            return ptr.get() != nullptr;
        });
    if (first_available_place == players_.end()) { return false; }
    auto& place = *first_available_place;
    place.reset(player);
    return true;
}

void Game::start() {
    assert(!is_playing());
    deck_->clear();
    deck_->init();
    deck_->shuffle();
    player_index_ = 0;
    assert(players_[player_index_].get() != nullptr);
    current_total_players_ = total_players();
    assert(current_total_players_ > 1);
    on_play_ = true;
}

bool Game::try_handle_current_turn() {
    assert(player_index_ >= 0 && player_index_ < players_.size());
    auto& player = players_[player_index_];
    assert(player.get() != nullptr);

    auto opt = player->make_decision();
    if (!opt) {
        skip();
        return true;
    }

    const bool accepted =
        total_turns() > 0
            ? arbiter_->validate(last_play_record().card_group, *opt)
            : true;

    if (accepted) {
        player->notify_decision_accpted(*opt);
        if (player->is_card_all_out()) {
            finish();
        } else {
            accept(player.get(), *opt);
        }
    } else {
        return false;
    }

    return true;
}

void Game::finish() {
    assert(is_playing());
    assert(player_index_ >= 0 && player_index_ < players_.size());
    auto& player = players_[player_index_];
    assert(player.get() != nullptr);
    assert(player->is_card_all_out());

    last_winner_ = player->uid();

    for (auto& player : players_) { player.reset(); }
    turn_stack_.clear();
    current_total_players_ = 0;

    on_play_ = false;
}

void Game::next_turn() {
    player_index_ = (player_index_ + 1) % current_total_players_;
}
