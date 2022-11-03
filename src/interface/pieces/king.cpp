#include "../piece.hpp"
#include "../board.hpp"

constexpr int32_t delta_x[] = { 1, 1, -1, -1 };
constexpr int32_t delta_y[] = { 1, -1, 1, -1 };

void KingMoves(std::vector<Move> & path,
               std::vector<std::vector<Move>> & paths,
               Position position,
               const Board & board,
               std::vector<std::vector<bool>> & taken,
               const Position & starting_position,
               const PieceColor & starting_color) {

  bool taken_at_least_one = false;

  for (size_t d = 0; d < 4; ++d) {
    try {
      Position nearest = position;
      for (int len = 1; len <= 7; ++len) {
        Position current_position(position.GetX() + delta_x[d] * len, position.GetY() + delta_y[d] * len);

        if (board[current_position].IsOccupied() &&
            !taken[current_position.GetX()][current_position.GetY()]) {
          if (nearest == position && board[current_position].GetPiece()->GetColor() != starting_color) {
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
          path.push_back(Move(position, current_position, { nearest }));

          KingMoves(path, paths, current_position, board, taken, starting_position, starting_color);

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
  KingMoves(temp_path, paths, GetPosition(), board, taken,
           GetPosition(), GetColor());
  for (std::vector<Move> & path : paths) {
    for (size_t index = 1; index < path.size(); ++index) {
      path[0] += path[index];
    }
    result.push_back(path[0]);
  }

  return result;
}