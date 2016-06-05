#pragma once
#include <gl/opengl.h>
#include <utility>

namespace gl {

class vao {
public:
  vao() = default;

  template <typename Initializer>
  explicit vao(Initializer initializer)
  {
    // Reset the error information.
    glGetError();

    // Generate a named vertex array object.
    glGenVertexArrays(1, &vao_);
    if (auto ec = gl::make_error()) {
      throw ice::runtime_error("Could not generate a named vertex array object.")
        << ec.message();
    }

    // Bind a named vertex array object.
    glBindVertexArray(vao_);
    if (auto ec = gl::make_error()) {
      glDeleteVertexArrays(1, &vao_);
      throw ice::runtime_error("Could not bind a named vertex array object.")
        << ec.message();
    }

    // Execute the initializer.
    initializer();
    if (auto ec = gl::make_error()) {
      glDeleteVertexArrays(1, &vao_);
      throw ice::runtime_error("Could not initialize a named vertex array object.")
        << ec.message();
    }

    // Break the existing vertex array object binding.
    glBindVertexArray(0);
    if (auto ec = gl::make_error()) {
      glDeleteVertexArrays(1, &vao_);
      throw ice::runtime_error("Could not break the existing vertex array object binding.")
        << ec.message();
    }
  }

  vao(vao&& other)
  {
    std::swap(vao_, other.vao_);
  }

  vao& operator=(vao&& other)
  {
    std::swap(vao_, other.vao_);
    return *this;
  }

  ~vao()
  {
    if (glIsVertexArray(vao_)) {
      glDeleteVertexArrays(1, &vao_);
    }
  }

  operator GLuint() const noexcept
  {
    return vao_;
  }

private:
  GLuint vao_ = 0;
};

}  // namespace gl
