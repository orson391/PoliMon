#pragma once

#include <string>

namespace core {

class Logger {
 public:
  static void log(const std::string& message);
};

}  // namespace core
