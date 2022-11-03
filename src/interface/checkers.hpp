#pragma once

#include <utility>
#include "piece.hpp"
#include "board.hpp"
#include "../util.hpp"
#include "../player/player.hpp"

enum class GameState {
  kRunning,
  kDraw,
  kWhiteWon,
  kBlackWon
};

class Checkers {
 public:
  Checkers(std::shared_ptr<Player> white_player, std::shared_ptr<Player> black_player)
           : Checkers(std::move(white_player), std::move(black_player),
                      "wwwwwwwwwwww........bbbbbbbbbbbbw") {}

  Checkers(std::shared_ptr<Player> white_player, std::shared_ptr<Player> black_player, std::string config)
           : white_player_(std::move(white_player)), black_player_(std::move(black_player)),
             board_(config), state_(GameState::kRunning),
             currently_moving_(ToLowerCase(config.back()) == 'w' ? PieceColor::kWhite : PieceColor::kBlack) {}

  [[nodiscard]] bool IsRunning() const noexcept { return state_ == GameState::kRunning; }
  [[nodiscard]] bool IsOver() const noexcept { return state_ != GameState::kRunning; }
  [[nodiscard]] GameState GetState() const noexcept { return state_; }
  [[nodiscard]] const Board & GetBoard() const noexcept { return board_; }
  [[nodiscard]] PieceColor GetCurrentlyMoving() const noexcept { return currently_moving_; }

  Move Proceed();
  Move ProceedWithIntermediateMove();

 private:
  PieceColor currently_moving_;
  Board board_;
  std::shared_ptr<Player> white_player_;
  std::shared_ptr<Player> black_player_;
  GameState state_;
};