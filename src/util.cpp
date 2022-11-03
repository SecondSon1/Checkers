#include "util.hpp"

char ToLowerCase(char x) {
  if ('A' <= x && x <= 'Z')
    x |= diff;
  return x;
}

char ToUpperCase(char x) {
  if ('a' <= x && x <= 'z')
    x ^= diff;
  return x;
}