namespace graphics::models {
class IClonable {
 public:
  virtual IClonable* clone() const = 0;
  virtual ~IClonable() = default;
};
}  // namespace graphics::models