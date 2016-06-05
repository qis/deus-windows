#include <ice/archive.h>
#include <ice/exception.h>
#include <zip.h>
#include <algorithm>
#include <fstream>

namespace ice {

class archive::impl : public mz_zip_archive {
public:
  impl(const std::filesystem::path& path) : mz_zip_archive({})
  {
    is.open(path, std::ios::binary);
    if (!is) {
      throw ice::runtime_error("Could not open archive.") << path.u8string();
    }

    m_pIO_opaque = &is;
    m_pRead = [](void* handle, mz_uint64 offset, void* data, size_t size) -> size_t {
      auto& is = *reinterpret_cast<std::ifstream*>(handle);
      is.seekg(offset, std::ios::beg);
      is.read(reinterpret_cast<char*>(data), size);
      return static_cast<size_t>(is.gcount());
    };

    if (!mz_zip_reader_init(this, std::filesystem::file_size(path), 0)) {
      throw ice::runtime_error("Could not read archive.") << path.u8string();
    }
  }

  ~impl()
  {
    mz_zip_reader_end(this);
  }

private:
  std::ifstream is;
};

archive::archive(std::filesystem::path path) :
  path_(std::move(path))
{
  if (std::filesystem::is_regular_file(path)) {
    impl_ = std::make_unique<impl>(path_);
  } else if (!std::filesystem::is_directory(path_)) {
    throw ice::runtime_error("Missing archive or directory.") << path_.u8string();
  }
}

archive::~archive()
{}

void archive::read(const std::filesystem::path& path, read_handler handler)
{
  if (impl_) {
    auto success = mz_zip_reader_extract_file_to_callback(impl_.get(), path.generic_string().c_str(),
      [](void* handle, mz_uint64 offset, const void* data, size_t size) -> size_t
    {
      auto& handler = *reinterpret_cast<read_handler*>(handle);
      if (handler) {
        return handler(reinterpret_cast<const std::uint8_t*>(data), size);
      }
      return size;
    }, &handler, 0);
    if (!success) {
      throw ice::runtime_error("Could not extract file from archive.")
        << "Archive: " << path_.u8string() << '\n'
        << "File:    " << path.generic_u8string();
    }
  } else {
    auto filename = path_ / path;
    std::ifstream is(filename, std::ios::binary | std::ios::ate);
    if (!is) {
      throw ice::runtime_error("Could not open file.") << filename.u8string();
    }
    auto size = static_cast<std::size_t>(is.tellg());
    std::vector<std::uint8_t> data;
    data.resize(std::min(size, 1024ULL * 1024 * 4));
    is.seekg(0, std::ios::beg);
    do {
      is.read(reinterpret_cast<char*>(&data[0]), data.size());
      auto bytes = static_cast<std::size_t>(is.gcount());
      handler(data.data(), bytes);
    } while (is);
    if (is.bad()) {
      throw ice::runtime_error("Could not read file.") << filename.u8string();
    }
  }
}

template <>
std::string archive::load<std::string>(const std::filesystem::path& path)
{
  std::string file;
  read(path, [&file](const std::uint8_t* data, std::size_t size) {
    file.append(reinterpret_cast<const char*>(data), size);
    return size;
  });
  return file;
}

}  // namespace ice
