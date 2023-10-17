#pragma once

#include <cstdint>
#include <string>
#include <cassert>

constexpr int diff = static_cast<uint32_t>('a') ^ static_cast<uint32_t>('A');

char ToLowerCase(char x);

char ToUpperCase(char x);

int FloatSign(float x);

bool FloatsEqual(float x, float y);