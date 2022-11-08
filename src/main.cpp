#include "gui/scenes/config.hpp"
#include "gui/window.hpp"

int main() {

  sf::Font font;
  assert(font.loadFromFile("../BPmono.ttf"));

  std::unique_ptr<Scene> game_scene_ptr = std::make_unique<ConfigScene>(font);

  sf::Color bg_color(0xE8, 0xDE, 0xD1, 0xFF);
  std::shared_ptr<Window> game = std::make_shared<Window>(1000, 1200, "Checkers",
                                                          bg_color, false,
                                                          std::move(game_scene_ptr));

  game->Run();

  return 0;
}
