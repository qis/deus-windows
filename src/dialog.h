#pragma once
#include <windows.h>
#include <string>

namespace dialog {

void error(HWND parent, const std::string& message);
void error(HWND parent, const std::string& message, const std::string& details);

}  // namespace dialog
