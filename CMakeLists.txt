cmake_minimum_required(VERSION 3.21)
project(Checkers)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(Checkers main.cpp src/interface/board.cpp src/interface/board.hpp src/interface/space.hpp src/interface/piece.hpp src/interface/exceptions.hpp src/interface/position.hpp src/interface/move.cpp src/interface/move.hpp src/player/bot/bot.hpp src/interface/checkers.cpp src/interface/checkers.hpp src/util.hpp src/player/bot/random_bot/random_bot.hpp src/player/human.hpp src/player/player.hpp src/util.cpp src/interface/pieces/king.cpp src/interface/pieces/man.cpp src/gui/scene.hpp src/gui/window.cpp src/gui/window.hpp src/gui/scenes/game.cpp src/gui/scenes/game.hpp src/gui/scenes/config.cpp src/gui/scenes/config.hpp src/gui/scenes/game_over.hpp src/gui/scenes/game_over.cpp src/gui/templates/button.cpp src/gui/templates/button.hpp src/player/bot/human_bot/human_bot.hpp src/player/bot/calculating_bot/calculating_bot.hpp src/gui/scenes/buttons/choice_button.hpp src/gui/scenes/buttons/proceed_button.hpp)

target_link_libraries(Checkers sfml-graphics sfml-audio)