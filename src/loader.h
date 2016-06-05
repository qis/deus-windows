#pragma once
#include <span>
#include <string>
#include <cstdint>

class loader {
public:
  // Returns the file contents of the resource.
  template <typename T = std::uint8_t>
  std::span<const T> get(const std::string& name)
  {
    auto file = find(name);
    auto data = reinterpret_cast<const T*>(file.data());
    auto size = file.size() / sizeof(T);
    return std::span<const T>(data, size);
  }

  // Returns the file contents of the resource.
  template <>
  std::span<const std::uint8_t> get<std::uint8_t>(const std::string& name)
  {
    return find(name);
  }

  // Returns the file contents of the resource as a specific type.
  // See loader.cc for supported types.
  template <typename T>
  T load(const std::string& name);

private:
  std::span<const std::uint8_t> find(const std::string& name);
};
