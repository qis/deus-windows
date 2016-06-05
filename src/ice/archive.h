#pragma once
#include <filesystem>
#include <functional>
#include <memory>
#include <cstdint>

namespace ice {

// Reads files from the given zip archive file or base directory.
class archive {
public:
  using read_handler = std::function<std::size_t(const std::uint8_t* data, std::size_t size)>;

  // Opens an archive file or base directory.
  archive(std::filesystem::path path);
  ~archive();

  // Reads a file from the archive or base directory.
  void read(const std::filesystem::path& path, read_handler handler);

  // Returns the file contents as a specific type.
  // See archive.cc for supported types.
  template <typename T>
  T load(const std::filesystem::path& path);

private:
  class impl;
  std::unique_ptr<impl> impl_;
  std::filesystem::path path_;
};

}  // namespace ice
