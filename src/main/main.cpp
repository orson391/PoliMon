#include <cstdlib>

#include "Game.h"
#include "Logger.h"

int main() {
  core::Logger::log("Starting modular SDL3 application");

  game::GameApp app;
  if (!app.initialize()) {
    return EXIT_FAILURE;
  }

  app.run();

  return EXIT_SUCCESS;
}
