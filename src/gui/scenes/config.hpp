#pragma once

#include "../scene.hpp"
#include "buttons/choice_button.hpp"
#include "buttons/proceed_button.hpp"

#include <iostream>

class ConfigScene : public Scene {
 public:
  ConfigScene(sf::Font & font) : font_(font) {}

  void Init() override;
  void Draw(const sf::RenderWindow &window, sf::RenderTexture &texture) override;
  void HandleEvent(sf::RenderWindow &window, sf::Event &evt) override;
  void HandleLogic(sf::RenderWindow &window) override;

 private:
  void DrawInfo(sf::RenderTexture & texture);
  void DrawChoices(sf::RenderTexture & texture);

 private:
  sf::Font font_;
  std::vector<std::shared_ptr<ChoiceButton>> white_choice_buttons_, black_choice_buttons_;
  std::shared_ptr<ProceedButton> proceed_button_;

  size_t width_;
  size_t info_height_;
  size_t choice_height_;
  sf::Vector2f info_position_;
  sf::Vector2f choice_position_;

  size_t button_width_;
  size_t button_height_;

  sf::RenderTexture info_texture_;
  sf::RenderTexture choice_texture_;

  std::shared_ptr<std::shared_ptr<ChoiceButton>> white_chosen_;
  std::shared_ptr<std::shared_ptr<ChoiceButton>> black_chosen_;
};