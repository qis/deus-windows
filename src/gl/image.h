#pragma once
#include <gl/opengl.h>
#include <iomanip>
#include <ostream>
#include <vector>
#include <cstdint>

namespace gl {

// Format             | Type                      | Data     | Size
// -------------------+---------------------------+----------+---------------------------
// GL_ALPHA           | GL_UNSIGNED_BYTE          | uint8_t  | cx * cy
// -------------------+---------------------------+----------+---------------------------
// GL_RGB             | GL_UNSIGNED_BYTE          | uint8_t  | cx * cy * 3
//                    | GL_UNSIGNED_SHORT_5_6_5   | uint16_t | cx * cy * sizeof(uint16_t)
// -------------------+---------------------------+----------+----------------------------
// GL_RGBA            | GL_UNSIGNED_BYTE          | uint8_t  | cx * cy * 4
//                    | GL_UNSIGNED_SHORT_4_4_4_4 | uint16_t | cx * cy * sizeof(uint16_t)
//                    | GL_UNSIGNED_SHORT_5_5_5_1 | uint16_t | cx * cy * sizeof(uint16_t)
// -------------------+---------------------------+----------+----------------------------
// GL_LUMINANCE_ALPHA | GL_UNSIGNED_BYTE          | uint8_t  | cx * cy * 2
// -------------------+---------------------------+----------+----------------------------
// GL_LUMINANCE       | GL_UNSIGNED_BYTE          | uint8_t  | cx * cy

class image {
public:
  image() :
    cx_(0), cy_(0), format_(0), type_(0)
  {}

  explicit image(GLsizei cx, GLsizei cy, GLenum format, GLenum type) :
    cx_(cx), cy_(cy), format_(format), type_(type)
  {
    switch (format_) {
    case GL_ALPHA:
    case GL_LUMINANCE:
      switch (type_) {
      case GL_UNSIGNED_BYTE:
        data_.resize(cx_ * cy_, 0);
        break;
      default:
        throw ice::runtime_error("Invalid image type.") << *this;
        break;
      }
      break;
    case GL_LUMINANCE_ALPHA:
      switch (type_) {
      case GL_UNSIGNED_BYTE:
        data_.resize(cx_ * cy_ * 2, 0);
        break;
      default:
        throw ice::runtime_error("Invalid image type.") << *this;
        break;
      }
      break;
    case GL_RGB:
      switch (type_) {
      case GL_UNSIGNED_BYTE:
        data_.resize(cx_ * cy_ * 3, 0);
        break;
      case GL_UNSIGNED_SHORT_5_6_5:
        data_.resize(cx_ * cy_ * sizeof(std::uint16_t), 0);
        break;
      default:
        throw ice::runtime_error("Invalid image type.") << *this;
        break;
      }
      break;
    case GL_RGBA:
      switch (type_) {
      case GL_UNSIGNED_BYTE:
        data_.resize(cx_ * cy_ * 4, 0);
        break;
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_5_5_5_1:
        data_.resize(cx_ * cy_ * sizeof(std::uint16_t), 0);
        break;
      default:
        throw ice::runtime_error("Invalid image type.") << *this;
        break;
      }
      break;
    default:
      throw ice::runtime_error("Invalid image format.") << *this;
      break;
    }
  }

  GLsizei cx() const noexcept
  {
    return cx_;
  }

  GLsizei cy() const noexcept
  {
    return cy_;
  }

  GLenum format() const noexcept
  {
    return format_;
  }

  GLenum type() const noexcept
  {
    return type_;
  }

  void* data() noexcept
  {
    return data_.data();
  }

  const void* data() const noexcept
  {
    return data_.data();
  }

  std::size_t size() const noexcept
  {
    return data_.size();
  }

private:
  GLsizei cx_;
  GLsizei cy_;
  GLenum format_;
  GLenum type_;
  std::vector<std::uint8_t> data_;
};

inline std::ostream& operator<<(std::ostream& os, const gl::image& image)
{
  auto flags = os.flags();
  os << std::dec << image.cx() << ' ' << image.cy() << ' ';
  switch (image.format()) {
  case GL_ALPHA: os << "ALPHA"; break;
  case GL_RGB: os << "RGB"; break;
  case GL_RGBA: os << "RGBA"; break;
  case GL_LUMINANCE_ALPHA: os << "LUMINANCE_ALPHA"; break;
  case GL_LUMINANCE: os << "LUMINANCE"; break;
  default: os << "0x" << std::setfill('0') << std::hex << std::setw(4) << image.format() << std::dec;
  }
  os << ' ';
  switch (image.type()) {
  case GL_UNSIGNED_BYTE: os << "UNSIGNED_BYTE"; break;
  case GL_UNSIGNED_SHORT_5_6_5: os << "UNSIGNED_SHORT_5_6_5"; break;
  case GL_UNSIGNED_SHORT_4_4_4_4: os << "UNSIGNED_SHORT_4_4_4_4"; break;
  case GL_UNSIGNED_SHORT_5_5_5_1: os << "UNSIGNED_SHORT_5_5_5_1"; break;
  default: os << "0x" << std::setfill('0') << std::hex << std::setw(4) << image.type() << std::dec;
  }
  os << " (" << image.size() << " bytes)";
  os.flags(flags);
  return os;
}

}  // namespace gl
