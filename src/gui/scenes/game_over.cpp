#include "game_over.hpp"
#include "config.hpp"

void GameOverScene::Init() {
  sf::Color semi_transparent(255, 255, 255, 40);

  size_t width = GetWindowPtr()->GetWidth();
  size_t height = GetWindowPtr()->GetHeight();

  sf::RenderTexture texture;
  assert(texture.create(width, height));

  sf::Vector2f size(sf::Vector2u(width, height));
  sf::RectangleShape semi_transparent_cover(size);
  semi_transparent_cover.setFillColor(semi_transparent);

  texture.draw(sf::Sprite(game_in_background_));
  texture.draw(semi_transparent_cover);

  std::string str;
  switch (who_won_) {
    case PieceColor::kWhite:
      str = "White player won!";
      break;
    case PieceColor::kBlack:
      str = "Black player won!";
      break;
  }
  sf::Text text(str, font_, 50);
  text.setFillColor(sf::Color::Black);

  sf::Vector2f text_size(text.getLocalBounds().width, text.getLocalBounds().height);
  sf::Vector2f margin(20.f, 20.f);
  sf::RectangleShape dialog_box(text_size + margin * 2.f);
  dialog_box.setFillColor(sf::Color::White);
  dialog_box.setOutlineColor(sf::Color::Black);
  dialog_box.setOutlineThickness(3.f);

  sf::Vector2f position((static_cast<float>(width) - dialog_box.getSize().x) / 2.f,
                        (static_cast<float>(height) - dialog_box.getSize().y) / 2.f);
  dialog_box.setPosition(position);
  text.setPosition(position + margin - sf::Vector2f(0.f, text.getLocalBounds().top));

  texture.draw(dialog_box);
  texture.draw(text);

  texture.display();

  result_ = texture.getTexture();
}

void GameOverScene::HandleEvent(sf::RenderWindow & window, sf::Event & evt) {
  if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Enter) {
    SwitchScene(std::make_unique<ConfigScene>(font_));
  }
}
