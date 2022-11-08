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


constexpr float EPS = 1e-6;


int FloatSign(float x) {
  if (x > EPS) return 1;
  if (x < -EPS) return -1;
  return 0;
}


bool FloatsEqual(float x, float y) {
  return FloatSign(x - y) == 0;
}