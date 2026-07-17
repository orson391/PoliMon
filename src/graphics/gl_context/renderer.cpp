#include "gl_context/renderer.h"

#include <array>
#include <memory>
#include <string>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "Logger.h"

namespace graphics {

Texture::~Texture() {
  if (id_ != 0) {
    glDeleteTextures(1, &id_);
  }
}

namespace gl_context {
namespace {

constexpr const char* kVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;
uniform vec2 uViewport;
out vec2 vTexCoord;
void main() {
  vec2 clip = vec2((aPosition.x / uViewport.x) * 2.0 - 1.0,
                   1.0 - (aPosition.y / uViewport.y) * 2.0);
  gl_Position = vec4(clip, 0.0, 1.0);
  vTexCoord = aTexCoord;
}
)";

constexpr const char* kFragmentShader = R"(
#version 330 core
in vec2 vTexCoord;
uniform vec4 uColor;
uniform bool uTextured;
uniform sampler2D uTexture;
out vec4 fragColor;
void main() {
  fragColor = uTextured ? texture(uTexture, vTexCoord) * uColor : uColor;
}
)";

unsigned int compileShader(unsigned int type, const char* source) {
  const unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  int success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[1024] = {};
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    core::Logger::log(std::string("OpenGL shader compilation failed: ") + log);
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

}  // namespace

Renderer::~Renderer() {
  if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
  if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
  if (program_ != 0) glDeleteProgram(program_);
}

bool Renderer::initialize(int width, int height) {
  if (!createPipeline()) return false;
  setViewport(width, height);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  return true;
}

bool Renderer::createPipeline() {
  const unsigned int vertex = compileShader(GL_VERTEX_SHADER, kVertexShader);
  const unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, kFragmentShader);
  if (vertex == 0 || fragment == 0) {
    if (vertex != 0) glDeleteShader(vertex);
    if (fragment != 0) glDeleteShader(fragment);
    return false;
  }

  program_ = glCreateProgram();
  glAttachShader(program_, vertex);
  glAttachShader(program_, fragment);
  glLinkProgram(program_);
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  int linked = 0;
  glGetProgramiv(program_, GL_LINK_STATUS, &linked);
  if (!linked) {
    char log[1024] = {};
    glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
    core::Logger::log(std::string("OpenGL program linking failed: ") + log);
    glDeleteProgram(program_);
    program_ = 0;
    return false;
  }

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, nullptr, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
                        reinterpret_cast<void*>(sizeof(float) * 2));
  glEnableVertexAttribArray(1);

  viewportUniform_ = glGetUniformLocation(program_, "uViewport");
  colorUniform_ = glGetUniformLocation(program_, "uColor");
  texturedUniform_ = glGetUniformLocation(program_, "uTextured");
  glUseProgram(program_);
  glUniform1i(glGetUniformLocation(program_, "uTexture"), 0);
  return true;
}

void Renderer::setViewport(int width, int height) {
  viewportWidth_ = width > 0 ? width : 1;
  viewportHeight_ = height > 0 ? height : 1;
  glViewport(0, 0, viewportWidth_, viewportHeight_);
}

void Renderer::beginFrame(Color clearColor) {
  glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::drawRect(const Rect& rect, Color color) {
  drawQuad(rect, nullptr, nullptr, color);
}

void Renderer::drawTexture(const Texture& texture, const Rect& source, const Rect& destination) {
  drawQuad(destination, &source, &texture, {1.0f, 1.0f, 1.0f, 1.0f});
}

void Renderer::drawQuad(const Rect& destination, const Rect* source, const Texture* texture, Color color) {
  if (program_ == 0 || destination.width <= 0.0f || destination.height <= 0.0f) return;

  float left = 0.0f;
  float right = 1.0f;
  float top = 1.0f;
  float bottom = 0.0f;
  if (source && texture && texture->width_ > 0 && texture->height_ > 0) {
    left = source->x / texture->width_;
    right = (source->x + source->width) / texture->width_;
    top = 1.0f - source->y / texture->height_;
    bottom = 1.0f - (source->y + source->height) / texture->height_;
  }

  const std::array<float, 24> vertices = {
      destination.x,                     destination.y,                      left,  top,
      destination.x + destination.width, destination.y,                      right, top,
      destination.x + destination.width, destination.y + destination.height, right, bottom,
      destination.x,                     destination.y + destination.height, left,  bottom,
      destination.x,                     destination.y,                      left,  top,
      destination.x + destination.width, destination.y + destination.height, right, bottom,
  };

  glUseProgram(program_);
  glUniform2f(viewportUniform_, static_cast<float>(viewportWidth_), static_cast<float>(viewportHeight_));
  glUniform4f(colorUniform_, color.r, color.g, color.b, color.a);
  glUniform1i(texturedUniform_, texture ? 1 : 0);
  if (texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id_);
  }
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::endFrame() {
  glBindVertexArray(0);
}

std::shared_ptr<Texture> Renderer::loadTexture(const std::string& filename) {
  SDL_Surface* loaded = IMG_Load(filename.c_str());
  if (!loaded) {
    core::Logger::log("OpenGL texture load failed for '" + filename + "': " + SDL_GetError());
    return nullptr;
  }

  SDL_Surface* rgba = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(loaded);
  if (!rgba) {
    core::Logger::log("OpenGL texture conversion failed for '" + filename + "': " + SDL_GetError());
    return nullptr;
  }

  auto texture = std::make_shared<Texture>();
  texture->width_ = rgba->w;
  texture->height_ = rgba->h;
  glGenTextures(1, &texture->id_);
  glBindTexture(GL_TEXTURE_2D, texture->id_);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->width_, texture->height_, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, rgba->pixels);
  SDL_DestroySurface(rgba);
  return texture;
}

}  // namespace gl_context
}  // namespace graphics
