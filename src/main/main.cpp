#include "Game.h"
#include "Logger.h"

int main() {
  core::Logger::log("Starting modular SDL3 application");

  game::GameApp app;
  app.initialize();
  app.run();

  return 0;
}
