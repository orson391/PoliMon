#pragma once

#include <string>
#include <vector>

namespace core {

struct RectElement {
  int id{0};
  float x{0.0f};
  float y{0.0f};
  float width{0.0f};
  float height{0.0f};
  unsigned char r{255};
  unsigned char g{255};
  unsigned char b{255};
  unsigned char a{255};
  bool filled{true};
  bool normalized{false};
};

struct Config {
  std::string title{"SDL3 Modular App"};
  int width{800};
  int height{600};
  int target_fps{60};
  bool fullscreen{false};
  bool vsync{true};

  std::vector<RectElement> rectangles;

  static bool loadFromFile(const std::string& path, Config& out);
};

}  // namespace core