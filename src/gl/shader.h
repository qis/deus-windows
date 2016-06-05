#pragma once
#include <gl/opengl.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

namespace gl {

class shader {
public:
  shader() = default;

  explicit shader(const std::string& src, GLenum type)
  {
    // Reset the error information.
    glGetError();

    // Create a new shader.
    shader_ = glCreateShader(type);
    if (auto ec = make_error()) {
      throw ice::runtime_error("Could not create a new shader.")
        << ec.message();
    }

    // Set the shader source.
    auto data = src.data();
    auto size = static_cast<GLint>(src.size());
    glShaderSource(shader_, 1, &data, &size);
    if (auto ec = make_error()) {
      glDeleteShader(shader_);
      throw ice::runtime_error("Could not set the shader source.")
        << ec.message() << '\n' << format(src);
    }

    // Compile the shader.
    glCompileShader(shader_);
    if (auto ec = make_error()) {
      glDeleteShader(shader_);
      throw ice::runtime_error("Could not compile the shader.")
        << ec.message() << '\n' << format(src);
    }

    // Get the shader compile status.
    GLint success = GL_FALSE;
    glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);
    if (auto ec = make_error()) {
      glDeleteShader(shader_);
      throw ice::runtime_error("Could not get the shader compile status.")
        << ec.message() << '\n' << format(src);
    }

    // Verify that the compilation was successful.
    if (!success) {
      std::string info;
      GLsizei size = 0;
      glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &size);
      if (!make_error()) {
        info.resize(size);
        glGetShaderInfoLog(shader_, size, &size, &info[0]);
        if (!make_error()) {
          info.resize(size);
        } else {
          info.clear();
        }
      }
      glDeleteShader(shader_);
      throw ice::runtime_error("Shader compilation failed.")
        << info << format(src);
    }
  }

  shader(shader&& other)
  {
    std::swap(shader_, other.shader_);
  }

  shader& operator=(shader&& other)
  {
    std::swap(shader_, other.shader_);
    return *this;
  }

  ~shader()
  {
    if (shader_) {
      glDeleteShader(shader_);
    }
  }

  operator GLuint() const noexcept
  {
    return shader_;
  }

  std::string source() const
  {
    // Verify that this is a shader.
    if (!shader_ || !glIsShader(shader_)) {
      return{};
    }

    // Get the shader source length.
    GLsizei size = 0;
    glGetShaderiv(shader_, GL_SHADER_SOURCE_LENGTH, &size);
    if (size) {
      return{};
    }

    // Get the shader source.
    std::string src;
    src.resize(size);
    glGetShaderSource(shader_, size, &size, &src[0]);
    if (!size) {
      return{};
    }
    src.resize(size);

    return src;
  }

  static std::string format(const std::string& src)
  {
    std::size_t line = 0;
    std::ostringstream oss;
    std::istringstream iss(src);
    auto width = std::to_string(std::count(src.begin(), src.end(), '\n')).size();
    for (std::string str; std::getline(iss, str);) {
      oss << '\n' << std::setw(width) << ++line << ": " << str;
    }
    return oss.str();
  }

private:
  GLuint shader_ = 0;
};

}  // namespace gl
