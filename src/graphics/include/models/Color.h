#include "IClonable.h"

namespace graphics::models {
class Color : IClonable {
 public:
  IClonable* clone() const override {
    Color* c = new Color();
    c->r_ = r_;
    c->g_ = g_;
    c->b_ = b_;
    c->a_ = a_;
    return c;
  }

 private:
  float r_;
  float g_;
  float b_;
  float a_;
};
}  // namespace graphics::models