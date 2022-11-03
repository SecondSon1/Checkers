#pragma once

#include <stdexcept>

class GameIsOverException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Can't make a move, game is already finished";
  }
};

class GameNotOverException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Can't get that information until the game is finished";
  }
};

class InvalidConfigException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Invalid config format. Must be 32 symbols of either b, w or . for"
           "black, white and blank space respectively (capital means king) followed by b or w, "
           "indicating whose move it is (33 symbols in total).";
  }
};

class PositionOutOfBoundsException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Position is out of bounds";
  }
};

class EmptyMoveException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Move cannot be empty";
  }
};

class MovesNotCompatibleException : public std::exception {
 public:
  [[nodiscard]] const char* what() const noexcept override {
    return "Attempting to combine moves which are not compatible "
           "(end of the first must be the same as beginning of the second)";
  }
};