#include <cmath>
#include "button.hpp"

void Button::Draw(sf::RenderTexture & texture) noexcept {

  button_texture_.clear(sf::Color(255, 255, 255, 0));

  sf::Color button_color, outline_color;
  sf::Text text;
  DrawButton(button_color, outline_color, text);

  sf::RectangleShape rectangle(sf::Vector2f(sf::Vector2<size_t>(width_ - 4, height_ - 4)));
  rectangle.setFillColor(button_color);
  rectangle.setOutlineColor(outline_color);
  rectangle.setOutlineThickness(2.f);
  rectangle.setPosition(sf::Vector2f(2.f, 2.f));

  text.setPosition(sf::Vector2f(
      (static_cast<float>(GetWidth()) - text.getLocalBounds().width) / 2.f,
      (static_cast<float>(GetHeight()) - text.getLocalBounds().height - text.getLocalBounds().top) / 2.f
  ));

  button_texture_.draw(rectangle);
  button_texture_.draw(text);

  button_texture_.display();
  sf::Sprite sprite(button_texture_.getTexture());
  sprite.setPosition(sf::Vector2f(static_cast<float>(x_), static_cast<float>(y_)));
  texture.draw(sprite);

}

bool Button::IsInside(sf::Vector2f mouse_pos_relative) {
  auto mouse_x = static_cast<int32_t>(round(mouse_pos_relative.x));
  auto mouse_y = static_cast<int32_t>(round(mouse_pos_relative.y));
  return (x_ <= mouse_x && mouse_x <= x_ + width_) && (y_ <= mouse_y && mouse_y <= y_ + height_);
}

void Button::HandleEvent(const sf::Event & evt, sf::Vector2f mouse_pos_relative) {
  if (evt.type != sf::Event::MouseButtonPressed && evt.type != sf::Event::MouseButtonReleased)
    return;
  if (evt.mouseButton.button != sf::Mouse::Left)
    return;

  if (evt.type == sf::Event::MouseButtonPressed) {
    if (IsInside(mouse_pos_relative))
      waiting_for_release_ = true;
  } else if (waiting_for_release_) {
    if (IsInside(mouse_pos_relative))
      Callback();
    waiting_for_release_ = false;
  }
}