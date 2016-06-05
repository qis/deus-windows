#pragma once
#include <gl/opengl.h>
#include <utility>

namespace gl {

class buffer {
public:
  buffer() = default;

  explicit buffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
  {
    // Reset the error information.
    glGetError();

    // Generate a named buffer object.
    glGenBuffers(1, &buffer_);
    if (auto ec = gl::make_error()) {
      throw ice::runtime_error("Could not generate a named buffer object.")
        << ec.message();
    }

    // Bind a named buffer object.
    glBindBuffer(target, buffer_);
    if (auto ec = gl::make_error()) {
      glDeleteBuffers(1, &buffer_);
      throw ice::runtime_error("Could not bind a named buffer object.")
        << ec.message();
    }

    // Create and initialize a buffer object's data store.
    glBufferData(target, size, data, usage);
    if (auto ec = gl::make_error()) {
      glDeleteBuffers(1, &buffer_);
      throw ice::runtime_error("Could not create and initialize a buffer object's data store.")
        << ec.message();
    }

    // Break the existing named buffer object binding.
    glBindBuffer(target, 0);
    if (auto ec = gl::make_error()) {
      glDeleteBuffers(1, &buffer_);
      throw ice::runtime_error("Could not break the existing named buffer object binding.")
        << ec.message();
    }
  }

  buffer(buffer&& other)
  {
    std::swap(buffer_, other.buffer_);
  }

  buffer& operator=(buffer&& other)
  {
    std::swap(buffer_, other.buffer_);
    return *this;
  }

  ~buffer()
  {
    if (glIsBuffer(buffer_)) {
      glDeleteBuffers(1, &buffer_);
    }
  }

  operator GLuint() const noexcept
  {
    return buffer_;
  }

private:
  GLuint buffer_ = 0;
};

}  // namespace gl
