#include "checkers.hpp"

Move Checkers::GetNextMove() noexcept {
  if (next_move_generated_)
    return next_move_;

  next_move_generated_ = true;
  return next_move_ =
      ((currently_moving_ == PieceColor::kWhite) ? white_player_ : black_player_)->GetNextMove(board_);
}


Move Checkers::Proceed() {

  Move move = ProceedWithIntermediateMove();

  last_move_intermediate_ = false;

  currently_moving_ = currently_moving_ == PieceColor::kWhite ?
                      PieceColor::kBlack : PieceColor::kWhite;

  // Lol I thought that when you don't have moves it`s stalemate like in chess
  // Turns out it counts as a win for another player. Smarter every day ig
  if (board_.HasSomeoneWon() || board_.GetAllLegalMoves(currently_moving_).empty())
    state_ = (currently_moving_ == PieceColor::kBlack) ? GameState::kWhiteWon : GameState::kBlackWon;

  return move;
}

Move Checkers::ProceedWithIntermediateMove() {
  if (IsOver())
    throw GameIsOverException();

  Move move = GetNextMove();
  next_move_generated_ = false;

  board_.MakeMove(move);

  if (last_move_intermediate_)
    move_history_.back() += move;
  else
    move_history_.push_back(move);

  last_move_intermediate_ = true;

  return move;
}

Move Checkers::GoBack() {
  if (move_history_.empty())
    throw EmptyMoveHistoryException();

  Move move = move_history_.back();
  move_history_.pop_back();
  board_.UndoMove(move);

  if (!last_move_intermediate_)
    currently_moving_ = currently_moving_ == PieceColor::kWhite ?
                        PieceColor::kBlack : PieceColor::kWhite;

  last_move_intermediate_ = false;

  return move;
}