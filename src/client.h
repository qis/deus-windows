#pragma once
#include <gl/opengl.h>
#include <gl/buffer.h>
#include <gl/program.h>
#include <gl/vao.h>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <string>

class client {
public:
  using clock = std::chrono::high_resolution_clock;

  client(const std::filesystem::path& path, GLsizei cx, GLsizei cy, GLint dpi);

  void render();
  void resize(GLsizei cx, GLsizei cy);

  void scale(GLint dpi);

private:
  std::atomic<clock::time_point> time_point_;
  gl::program program_;
  gl::buffer vbo_;
  gl::vao vao_;
};
