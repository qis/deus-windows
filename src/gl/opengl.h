#pragma once
#include <ice/exception.h>
#include <GLES3/gl32.h>
#include <stdexcept>
#include <string>

namespace gl {

enum class errc : GLenum {
  no_error = GL_NO_ERROR,
  invalid_enum = GL_INVALID_ENUM,
  invalid_value = GL_INVALID_VALUE,
  invalid_operation = GL_INVALID_OPERATION,
  invalid_framebuffer_operation = GL_INVALID_FRAMEBUFFER_OPERATION,
  out_of_memory = GL_OUT_OF_MEMORY
};

class error_category : public std::error_category {
public:
  const char* name() const noexcept override
  {
    return "OpenGL Error";
  }

  std::string message(int code) const override
  {
    switch (static_cast<errc>(code)) {
    case errc::no_error: return "No error has been recorded.";
    case errc::invalid_enum: return "An unacceptable value is specified for an enumerated argument.";
    case errc::invalid_value: return "A numeric argument is out of range.";
    case errc::invalid_operation: return "The specified operation is not allowed in the current state.";
    case errc::invalid_framebuffer_operation: return "The framebuffer object is not complete.";
    case errc::out_of_memory: return "There is not enough memory left to execute the command.";
    }
    return "Unknown error code: " + std::to_string(code);
  }

  static error_category& get()
  {
    static error_category category;
    return category;
  }
};

inline std::error_code make_error(errc code)
{
  return std::error_code(static_cast<int>(code), gl::error_category::get());
}

inline std::error_code make_error(GLenum code)
{
  return std::error_code(static_cast<int>(code), gl::error_category::get());
}

inline std::error_code make_error()
{
  return make_error(glGetError());
}

}  // namespace gl

namespace std {

template<>
struct is_error_condition_enum<gl::errc> : true_type {};

inline error_code make_error_code(gl::errc code)
{
  return error_code(static_cast<int>(code), gl::error_category::get());
}

inline error_condition make_error_condition(gl::errc code)
{
  return error_condition(static_cast<int>(code), gl::error_category::get());
}

}  // namespace std
