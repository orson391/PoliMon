#include <cstdlib>

#include "Game.h"
#include "Logger.h"

int main() {
  core::Logger::log("Starting PoliMon");

  game::GameApp app;
  if (!app.initialize()) {
    return EXIT_FAILURE;
  }

  app.run();

  return EXIT_SUCCESS;
}
