#pragma once
#include <gl/opengl.h>
#include <gl/shader.h>
#include <initializer_list>
#include <utility>

namespace gl {

class program {
public:
  program() = default;

  program(std::initializer_list<shader> shaders)
  {
    // Reset the error information.
    glGetError();

    // Create a new program.
    program_ = glCreateProgram();
    if (auto ec = make_error()) {
      throw ice::runtime_error("Could not create a new program.")
        << ec.message();
    }

    // Attach the shaders.
    for (const auto& shader : shaders) {
      glAttachShader(program_, shader);
      if (auto ec = make_error()) {
        glDeleteProgram(program_);
        throw ice::runtime_error("Could not attach a shader.")
          << ec.message() << '\n' << shader::format(shader.source());
      }
    }

    // Link the program.
    glLinkProgram(program_);
    if (auto ec = make_error()) {
      glDeleteProgram(program_);
      throw ice::runtime_error("Could not link the program.")
        << ec.message();
    }

    // Detach the shaders.
    for (const auto& shader : shaders) {
      glDetachShader(program_, shader);
      if (auto ec = make_error()) {
        glDeleteProgram(program_);
        throw ice::runtime_error("Could not detach a shader.")
          << ec.message() << '\n' << shader::format(shader.source());
      }
    }

    // Get the program link status.
    GLint success = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (auto ec = make_error()) {
      glDeleteProgram(program_);
      throw ice::runtime_error("Could not get the program link status.")
        << ec.message();
    }

    // Verify that the linkage was successful.
    if (!success) {
      std::string info;
      GLsizei size = 0;
      glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &size);
      if (!make_error()) {
        info.resize(size);
        glGetProgramInfoLog(program_, size, &size, &info[0]);
        if (!make_error()) {
          info.resize(size);
        } else {
          info.clear();
        }
      }
      glDeleteProgram(program_);
      throw ice::runtime_error("Program linkage failed.")
        << info;
    }
  }

  program(program&& other)
  {
    std::swap(program_, other.program_);
  }

  program& operator=(program&& other)
  {
    std::swap(program_, other.program_);
    return *this;
  }

  ~program()
  {
    if (program_) {
      glDeleteProgram(program_);
    }
  }

  operator GLuint() const noexcept
  {
    return program_;
  }

private:
  GLuint program_ = 0;
};

}  // namespace gl
