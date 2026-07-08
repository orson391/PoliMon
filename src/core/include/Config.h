#pragma once

#include <string>

namespace core {

struct Config {
  std::string title{"SDL3 Modular App"};
  int width{800};
  int height{600};
  int target_fps{60};
  bool fullscreen{false};
  bool vsync{true};

  static bool loadFromFile(const std::string& path, Config& out);
};

}  // namespace core
