#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../scene.hpp"
#include "../../interface/piece.hpp"
#include "../window.hpp"

class GameOverScene : public Scene {
 public:
  GameOverScene(PieceColor who_won, sf::Font & font, const sf::Texture & game_in_background)
                : who_won_(who_won), font_(font), game_in_background_(game_in_background) {}

  void Init() override;

  void Draw(const sf::RenderWindow & window, sf::RenderTexture & texture) override {
    texture.draw(sf::Sprite(result_));
  }

  void HandleLogic(sf::RenderWindow & window) override {}
  void HandleEvent(sf::RenderWindow & window, sf::Event & evt) override;

 private:
  PieceColor who_won_;
  sf::Font font_;
  sf::Texture game_in_background_;
  sf::Texture result_;
};