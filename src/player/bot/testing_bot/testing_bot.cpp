#include "testing_bot.hpp"


bool TestingBot::MoveComparison(const Move & lhs, const Move & rhs, PieceColor color) {
  if (lhs.DoesPromotionHappen() && !rhs.DoesPromotionHappen())
    return true;
  size_t left_taken = lhs.GetTakenPieces().size();
  size_t right_taken = rhs.GetTakenPieces().size();
  if (left_taken != right_taken)
    return left_taken < right_taken;

  int left_x = lhs.GetEndPosition().GetX();
  int relative_left_x = color == PieceColor::kWhite ? left_x : 7 - left_x;

  int right_x = rhs.GetEndPosition().GetX();
  int relative_right_x = color == PieceColor::kWhite ? right_x : 7 - right_x;

  return relative_left_x > relative_right_x;
}

int32_t TestingBot::Evaluate(Board & board, int32_t current_color_index, int32_t alpha, int32_t beta, int32_t depth) {
  std::vector<Move> moves = board.GetAllLegalMoves(colors_[current_color_index]);

  if (moves.empty())
    return -1e9;

  if (depth == 0) {

    std::vector<Move> other_moves = board.GetAllLegalMoves(colors_[current_color_index ^ 1]);

    static std::vector<std::vector<bool>> can_move(8, std::vector<bool>(8, false));
    for (int i = 0; i < 8; ++i)
      std::fill(can_move[i].begin(), can_move[i].end(), false);

    for (const Move & move : moves)
      can_move[move.GetStartPosition().GetX()][move.GetStartPosition().GetY()] = true;

    for (const Move & move : other_moves)
      can_move[move.GetStartPosition().GetX()][move.GetStartPosition().GetY()] = true;


    int total[2] = { 0,0 };
    for (size_t index = 0; index < 32; ++index) {
      Position pos(index);
      if (((const Board &) board)[pos].IsOccupied()) {
        int cost = can_move[pos.GetX()][pos.GetY()] ? normal_value_ : blocked_value_;
        if (((const Board &) board)[pos].GetPiece()->GetType() == PieceType::kKing)
          cost = king_value_;
        total[((const Board &) board)[pos].GetPiece()->GetColor() == colors_[1]] += cost;
      }
    }

    return total[current_color_index] - total[current_color_index ^ 1];
  }

//  std::shuffle(moves.begin(), moves.end(), rng_);
  std::sort(moves.begin(), moves.end(),
            [this, current_color_index](const Move & lhs, const Move & rhs) {
              return MoveComparison(lhs, rhs, colors_[current_color_index]);
            });

  for (const Move & move : moves) {
    board.MakeMove(move);

    int eval = -Evaluate(board, current_color_index ^ 1,
                         -beta, -alpha, depth - 1);

    board.UndoMove(move);

    if (eval > alpha)
      alpha = eval;

    if (beta <= alpha)
      return alpha;

  }

  return alpha;
}


Move TestingBot::GetNextMove(const Board & const_board) noexcept {
  Board board(const_board);

  colors_[0] = GetColor();
  colors_[1] = GetColor() == PieceColor::kWhite ? PieceColor::kBlack : PieceColor::kWhite;

  int alpha = -1e9;
  int beta = 1e9;
  auto our_moves = board.GetAllLegalMoves(GetColor());
  Move resulting_move = our_moves[0];

  for (const Move & move : our_moves) {
    board.MakeMove(move);

    int eval = -Evaluate(board, 1, -beta, -alpha, depth_ - 1);

    if (eval > alpha) {
      alpha = eval;
      resulting_move = move;
    }

    board.UndoMove(move);
  }

  return resulting_move;

}
