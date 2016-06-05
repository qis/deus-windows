#include "window.h"
#include <windows.h>
#include <shellapi.h>
#include <resource.h>
#include <codecvt>
#include <string>
#include <locale>
#include <vector>
#include <clocale>

int main(int argc, char* argv[])
{
  // Initialize the locale.
  std::setlocale(LC_ALL, "");

  // Check the system version.
  OSVERSIONINFOEX info = {};
  info.dwOSVersionInfoSize = sizeof(info);
  info.dwMajorVersion = 6;
  info.dwMinorVersion = 1;
  DWORDLONG condition = 0;
  VER_SET_CONDITION(condition, VER_MAJORVERSION, VER_GREATER_EQUAL);
  VER_SET_CONDITION(condition, VER_MINORVERSION, VER_GREATER_EQUAL);
  if (!VerifyVersionInfo(&info, VER_MAJORVERSION | VER_MINORVERSION, condition)) {
    MessageBox(nullptr, L"This application requires Windows 7 or newer.", TEXT(PROJECT" Error"), MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    return 1;
  }

  // Check if another instance of this application is already running.
  auto event = CreateEvent(NULL, TRUE, FALSE, TEXT(PRODUCT));
  if (event && GetLastError() == ERROR_ALREADY_EXISTS) {
    if (auto hwnd = FindWindow(TEXT(PRODUCT), nullptr)) {
      SetForegroundWindow(hwnd);
      return 0;
    }
    MessageBox(nullptr, L"This application is already running.", TEXT(PROJECT" Error"), MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
    return 0;
  }

  // Parse the command line arguments.
  auto samples = 8;
  auto fullscreen = false;
  for (int i = 0; i < argc; i++) {
    if (argv[i] == std::string("-s") && i + 1 < argc) {
      samples = std::atoi(argv[++i]);
      continue;
    }
    if (argv[i] == std::string("-f")) {
      fullscreen = true;
      continue;
    }
  }

  // Create the main application window.
  window window;
  window.create(samples, fullscreen);

  // Run the main message loop.
  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return static_cast<int>(msg.wParam);
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR cmd, int)
{
  // Convert the command line arguments.
  auto argc = 0;
  auto list = CommandLineToArgvW(cmd, &argc);
  if (!list) {
    argc = 0;
  }
  std::vector<std::string> args;
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  for (int i = 0; i < argc; i++) {
    args.push_back(conv.to_bytes(list[i]));
  }
  if (list) {
    LocalFree(list);
  }
  std::vector<char*> argv;
  argv.resize(args.size());
  for (std::size_t i = 0, size = args.size(); i < size; i++) {
    argv[i] = &args[i][0];
  }
  return main(static_cast<int>(argv.size()), &argv[0]);
}
