#include "../pieces.hpp"
#include "../board.hpp"

constexpr int32_t delta_x[] = { 1, 1, -1, -1 };
constexpr int32_t delta_y[] = { 1, -1, 1, -1 };


void KingMoves(std::vector<Move> & path,
               std::vector<std::vector<Move>> & paths,
               Position position,
               const Board & board,
               std::vector<std::vector<bool>> & taken,
               const std::shared_ptr<Piece> & starting_piece) {

  bool taken_at_least_one = false;

  for (size_t d = 0; d < 4; ++d) {
    try {
      Position nearest = position;
      for (int len = 1; len <= 7; ++len) {
        Position current_position(position.GetX() + delta_x[d] * len, position.GetY() + delta_y[d] * len);

        if (board[current_position].IsOccupied() &&
            (!taken[current_position.GetX()][current_position.GetY()] &&
                      current_position != starting_piece->GetPosition())) {
          if (nearest == position && board[current_position].GetPiece()->GetColor() != starting_piece->GetColor()) {
            nearest = current_position;
            continue;
          } else
            break;
        }

        if (nearest == position) {
          if (!path.empty())
            continue;
          path.emplace_back(position, current_position);
          paths.push_back(path);
          path.pop_back();
        } else {
          taken_at_least_one = true;
          taken[nearest.GetX()][nearest.GetY()] = true;
          path.push_back(Move(position, current_position, { board[nearest].GetPiece() }));

          KingMoves(path, paths, current_position, board, taken, starting_piece);

          path.pop_back();
          taken[nearest.GetX()][nearest.GetY()] = false;
        }
      }
    } catch (const PositionOutOfBoundsException & e) {}
  }

  if (!taken_at_least_one && !path.empty()) // Because if you can take, you MUST take
    paths.push_back(path);
}


std::vector<Move> King::GetMoves(const Board & board) const noexcept {
  std::vector<Move> result;

  std::vector<Move> temp_path;
  std::vector<std::vector<bool>> taken(8, std::vector<bool>(8));
  std::vector<std::vector<Move>> paths;
  KingMoves(temp_path, paths, GetPosition(), board, taken, board[GetPosition()].GetPiece());
  for (std::vector<Move> & path : paths) {
    for (size_t index = 1; index < path.size(); ++index) {
      path[0] += path[index];
    }
    result.push_back(path[0]);
  }

  return result;
}


bool PromotionHappens(PieceColor color, Position position) {
  return color == PieceColor::kWhite && position.GetX() == 7 ||
         color == PieceColor::kBlack && position.GetX() == 0;
}

void ManTakes(std::vector<Move> & path,
              std::vector<std::vector<Move>> & paths,
              Position position,
              const Board & board,
              std::vector<std::vector<bool>> & taken,
              const std::shared_ptr<Piece> & starting_piece) {

  auto current_x = static_cast<int32_t>(position.GetX());
  auto current_y = static_cast<int32_t>(position.GetY());

  bool taken_at_least_one = false;

  for (size_t d = 0; d < 4; ++d) {
    try {
      Position middle_position(current_x + delta_x[d], current_y + delta_y[d]);

      if (board[middle_position].IsOccupied()) {
        std::shared_ptr<Piece> piece_ptr = board[middle_position].GetPiece();
        if (piece_ptr->GetColor() == starting_piece->GetColor() ||
            taken[middle_position.GetX()][middle_position.GetY()])
          continue;

        Position last_position(current_x + delta_x[d] * 2, current_y + delta_y[d] * 2);
        if (board[last_position].IsOccupied() && starting_piece->GetPosition() != last_position)
          continue;

        taken_at_least_one = true;
        taken[middle_position.GetX()][middle_position.GetY()] = true;
        path.push_back(Move(position, last_position, { board[middle_position].GetPiece() }));

        if (PromotionHappens(starting_piece->GetColor(), last_position)) {
          path.back().PromotionHappens(starting_piece, last_position);
          KingMoves(path, paths, last_position, board, taken, starting_piece);
        } else {
          ManTakes(path, paths, last_position, board, taken, starting_piece);
        }

        path.pop_back();
        taken[middle_position.GetX()][middle_position.GetY()] = false;
      }
    } catch (const PositionOutOfBoundsException & e) {}
  }

  if (!taken_at_least_one && !path.empty()) // Because if you can take, you MUST take
    paths.push_back(path);
}

std::vector<Move> Man::GetMoves(const Board & board) const noexcept {
  std::vector<Move> result;

  size_t d_from = (GetColor() == PieceColor::kBlack) * 2;
  size_t d_to = d_from + 2;
  for (size_t d = d_from; d < d_to; ++d) {
    try {
      Position move_pos(static_cast<int32_t>(GetPosition().GetX()) + delta_x[d],
                        static_cast<int32_t>(GetPosition().GetY()) + delta_y[d]);
      if (board[move_pos].IsEmpty()) {
        result.emplace_back(GetPosition(), move_pos);
        if (PromotionHappens(GetColor(), move_pos))
          result.back().PromotionHappens(
                board[GetPosition()].GetPiece(), move_pos
              );
      }
    } catch (const PositionOutOfBoundsException & e) {}
  }

  std::vector<Move> temp_path;
  std::vector<std::vector<bool>> taken(8, std::vector<bool>(8));
  std::vector<std::vector<Move>> paths;
  ManTakes(temp_path, paths, GetPosition(), board, taken, board[GetPosition()].GetPiece());
  for (std::vector<Move> & path : paths) {
    for (size_t index = 1; index < path.size(); ++index) {
      path[0] += path[index];
    }
    result.push_back(path[0]);
  }

  return result;
}
