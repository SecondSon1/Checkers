#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include "../scene.hpp"
#include "../../interface/checkers.hpp"
#include "../../player/human.hpp"
#include "buttons/choice_button.hpp"


enum class AnimationState {
  kIdle, kGoing, kFinished
};


class GameScene : public Scene {
 public:
  GameScene(std::shared_ptr<Player> white_player, std::shared_ptr<Player> black_player,
            const sf::Font & font)
            : white_human_player_(std::dynamic_pointer_cast<Human>(white_player)),
              black_human_player_(std::dynamic_pointer_cast<Human>(black_player)), move_entered_(false),
              sequential_move_in_progress_(false), animation_state_(AnimationState::kIdle),
              animated_move_(Position(0), Position(1)),
              animated_currently_moving_(animated_move_), animated_move_speed_(0.01f),
              game_(std::move(white_player), std::move(black_player)), font_(font),
              white_square_color_(238, 238, 215, 255),
              black_square_color_(129, 147, 99, 255),
              white_piece_color_(248, 248, 248, 255),
              black_piece_color_(85, 83, 82, 255),
              current_square_color_(247, 82, 82, 100),
              possible_move_square_color_(201, 22, 22, 100) {}

  GameScene(std::shared_ptr<Player> white_player, std::shared_ptr<Player> black_player,
            const sf::Font & font, std::string config)
      : white_human_player_(std::dynamic_pointer_cast<Human>(white_player)),
        black_human_player_(std::dynamic_pointer_cast<Human>(black_player)), move_entered_(false),
        sequential_move_in_progress_(false), animation_state_(AnimationState::kIdle),
        animated_move_(Position(0), Position(1)),
        animated_currently_moving_(animated_move_), animated_move_speed_(0.01f),
        game_(std::move(white_player), std::move(black_player), std::move(config)),
        font_(font), white_square_color_(238, 238, 215, 255),
        black_square_color_(129, 147, 99, 255),
        white_piece_color_(248, 248, 248, 255),
        black_piece_color_(85, 83, 82, 255),
        current_square_color_(247, 82, 82, 100),
        possible_move_square_color_(201, 22, 22, 100) {}

 private:
  void Init() override;
  void Draw(const sf::RenderWindow & window, sf::RenderTexture & texture) override;
  void HandleEvent(sf::RenderWindow & window, sf::Event &evt) override;
  void HandleLogic(sf::RenderWindow &window) override;

 private:
  Position GetPositionFromMouse(const sf::RenderWindow & window) const;
  sf::Vector2f GetCoordsFromPosition(const Position & pos) const;

  void DrawBoard(sf::Texture & texture);
  void DrawPiece(sf::RenderTexture & texture, const std::shared_ptr<Piece>& piece, sf::Vector2f top_left_corner);
  void DrawPieces(sf::RenderTexture & texture);
  void DrawFloatingPiece(sf::RenderTexture & texture, sf::Vector2f relative_mouse_coords);
  void DrawSquareInColor(sf::RenderTexture & texture, const Position & position, const sf::Color & color) const;
  void DrawPossibleMoves(sf::RenderTexture & texture);
  void DrawInfo(sf::RenderTexture & texture);

  float GetMoveTime(const Move & move) const;
  void Animate(sf::RenderTexture & texture);

  [[nodiscard]] bool IsOneCurrentlyMovingHuman() const noexcept {
    return game_.GetCurrentlyMoving() == PieceColor::kWhite ?
           white_human_player_ != nullptr : black_human_player_ != nullptr;
  }

 private:
  void SetUpAnimation();

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

  bool move_entered_;
  bool sequential_move_in_progress_;

  sf::Color white_square_color_;
  sf::Color black_square_color_;

  sf::Color white_piece_color_;
  sf::Color black_piece_color_;

  sf::Color current_square_color_;
  sf::Color possible_move_square_color_;

  std::shared_ptr<Human> white_human_player_;
  std::shared_ptr<Human> black_human_player_;

  std::mutex animation_mutex_;
  AnimationState animation_state_;
  Move animated_move_;
  Move animated_currently_moving_;
  float animated_move_speed_;
  float animated_move_time_;
  std::shared_ptr<Piece> animated_piece_;
  std::chrono::time_point<std::chrono::steady_clock> animation_start_time_;

  std::vector<std::shared_ptr<ChoiceButton>> bot_move_choice_buttons_;
  std::shared_ptr<std::shared_ptr<ChoiceButton>> bot_move_chosen_;

  Checkers game_;

  std::atomic<bool> waiting_for_drawing_;
};