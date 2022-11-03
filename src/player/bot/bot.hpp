#pragma once

#include "../player.hpp"

class Bot : public Player {
 protected:
  Bot(PieceColor color) : Player(PlayerType::kBot, color) {}
};