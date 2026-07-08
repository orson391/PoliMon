#include "Logger.h"

#include <iostream>

namespace core {

void Logger::log(const std::string& message) {
  std::cout << "[core] " << message << std::endl;
}

}  // namespace core
