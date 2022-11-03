#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include "../scene.hpp"
#include "../../interface/checkers.hpp"
#include "../../player/human.hpp"


class GameScene : public Scene {
 public:
  GameScene(std::shared_ptr<Player> white_player, std::shared_ptr<Player> black_player,
            const sf::Font & font)
            : white_human_player_(std::dynamic_pointer_cast<Human>(white_player)),
              black_human_player_(std::dynamic_pointer_cast<Human>(black_player)), move_finished_(false),
              sequential_move_in_progress_(false),
              game_(std::move(white_player), std::move(black_player)), font_(font) {}

  GameScene(std::shared_ptr<Player> white_player, std::shared_ptr<Player> black_player, const std::string & config,
            const sf::Font & font)
            : white_human_player_(std::dynamic_pointer_cast<Human>(white_player)),
              black_human_player_(std::dynamic_pointer_cast<Human>(black_player)), move_finished_(false),
              sequential_move_in_progress_(false),
              game_(std::move(white_player), std::move(black_player), config),
              font_(font) {}

  void Init() override;
  void Draw(const sf::RenderWindow & window, sf::RenderTexture & texture) override;
  void HandleEvent(sf::RenderWindow & window, sf::Event &evt) override;
  void HandleLogic(sf::RenderWindow &window) override;

 private:
  Position GetPositionFromMouse(const sf::RenderWindow & window);

  void DrawBoard(sf::Texture & texture);
  void DrawPiece(sf::RenderTexture & texture, const std::shared_ptr<Piece>& piece, sf::Vector2f top_left_corner);
  void DrawPieces(sf::RenderTexture & texture);
  void DrawFloatingPiece(sf::RenderTexture & texture, sf::Vector2f relative_mouse_coords);
  void DrawSquareInColor(sf::RenderTexture & texture, const Position & position, const sf::Color & color);
  void DrawPossibleMoves(sf::RenderTexture & texture);
  void DrawInfo(sf::RenderTexture & texture);

 private:
  sf::Font font_;

  size_t board_size_;
  size_t square_size_;
  size_t info_width_;
  size_t info_height_;

  sf::Vector2f board_position_;
  sf::Vector2f info_position_;

  sf::Texture board_texture_;
  sf::RenderTexture pieces_texture_;
  sf::RenderTexture moves_texture_;
  sf::RenderTexture info_texture_;

  std::vector<Move> move_pool_;
  std::mutex chosen_piece_mutex_;
  std::shared_ptr<Piece> chosen_piece_;
  std::mutex possible_moves_mutex_;
  std::vector<Move> possible_moves_;
  sf::Vector2f floating_offset_;

  bool move_finished_;
  bool sequential_move_in_progress_;

  sf::Color white_square_color;
  sf::Color black_square_color;

  sf::Color white_piece_color;
  sf::Color black_piece_color;

  sf::Color current_square_color;
  sf::Color possible_move_square_color;

  std::shared_ptr<Human> white_human_player_;
  std::shared_ptr<Human> black_human_player_;

  Checkers game_;
};