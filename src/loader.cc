#include "loader.h"
#include <ice/exception.h>
#include <windows.h>
#include <codecvt>
#include <locale>

template <>
std::string loader::load<std::string>(const std::string& name)
{
  auto file = get<char>(name);
  return std::string(file.data(), file.size());
}

std::span<const std::uint8_t> loader::find(const std::string& name)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  auto id = conv.from_bytes(name);
  auto resource = FindResource(nullptr, id.c_str(), RT_RCDATA);
  if (!resource) {
    throw ice::runtime_error("Could not find a windows resource.") << "Resource: " << name;
  }
  auto size = SizeofResource(nullptr, resource);
  if (!size) {
    throw ice::runtime_error("Could not get size of a windows resource.") << "Resource: " << name;
  }
  auto data = LoadResource(nullptr, resource);
  if (!data) {
    throw ice::runtime_error("Could not load a windows resource.") << "Resource: " << name << " (" << size << " bytes)";
  }
  auto lock = reinterpret_cast<const std::uint8_t*>(LockResource(data));
  if (!lock) {
    throw ice::runtime_error("Could not lock a windows resource.") << "Resource: " << name << " (" << size << " bytes)";
  }
  return { lock, size };
}
