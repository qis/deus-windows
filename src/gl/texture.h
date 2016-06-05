#pragma once
#include <gl/opengl.h>
#include <gl/image.h>
#include <utility>

namespace gl {

class texture {
public:
  texture() = default;

  explicit texture(GLenum target, GLint level, const gl::image& image)
  {
    // Reset the error information.
    glGetError();

    // Generate a texture object.
    glGenTextures(1, &texture_);
    if (auto ec = gl::make_error()) {
      throw ice::runtime_error("Could not generate a texture object.")
        << ec.message();
    }

    // Bind a texture object.
    glBindTexture(target, texture_);
    if (auto ec = gl::make_error()) {
      glDeleteTextures(1, &texture_);
      throw ice::runtime_error("Could not bind a texture object.")
        << ec.message();
    }

    // Specify a texture image.
    glTexImage2D(target, level, image.format(), image.cx(), image.cy(), 0, image.format(), image.type(), image.data());
    if (auto ec = gl::make_error()) {
      glDeleteTextures(1, &texture_);
      throw ice::runtime_error("Could not specify a texture image.")
        << ec.message() << "\nImage: " << image;
    }

    // Break the existing texture object binding.
    glBindTexture(target, 0);
    if (auto ec = gl::make_error()) {
      glDeleteTextures(1, &texture_);
      throw ice::runtime_error("Could not break the existing texture object binding.")
        << ec.message();
    }
  }

  texture(texture&& other)
  {
    std::swap(texture_, other.texture_);
  }

  texture& operator=(texture&& other)
  {
    std::swap(texture_, other.texture_);
    return *this;
  }

  ~texture()
  {
    if (glIsTexture(texture_)) {
      glDeleteTextures(1, &texture_);
    }
  }

  operator GLuint() const noexcept
  {
    return texture_;
  }

  GLsizei cx() const noexcept
  {
    return cx_;
  }

  GLsizei cy() const noexcept
  {
    return cy_;
  }

private:
  GLsizei cx_ = 0;
  GLsizei cy_ = 0;
  GLuint texture_ = 0;
};

}  // namespace gl
