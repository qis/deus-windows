#include "dialog.h"
#include <resource.h>
#include <codecvt>
#include <locale>
#include <sstream>

namespace dialog {
namespace {

std::wstring format(const std::wstring& str)
{
  std::wostringstream oss;
  std::wistringstream iss(str);
  bool start = true;
  for (std::wstring line; std::getline(iss, line);) {
    if (start) {
      start = false;
    } else {
      oss << L"\r\n";
    }
    oss << line;
  }
  return oss.str();
}

}  // namespace

void error(HWND parent, const std::string& message)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  MessageBox(parent, format(conv.from_bytes(message)).c_str(), TEXT(PROJECT" Error"), MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
}

void error(HWND parent, const std::string& message, const std::string& details)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;

  struct info {
    std::wstring message;
    std::wstring details;
    HFONT font;
  };
  
  info data = {
    format(conv.from_bytes(message)),
    format(conv.from_bytes(details)),
    nullptr
  };

  auto dialog = MAKEINTRESOURCE(IDD_ERROR);
  DialogBoxParam(nullptr, dialog, parent, [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> INT_PTR {
    auto data = reinterpret_cast<info*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (msg) {
    case WM_INITDIALOG:
      // Set the window userdata.
      data = reinterpret_cast<info*>(lparam);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));

      // Set the window title.
      SetWindowText(hwnd, TEXT(PROJECT" Error"));

      // Set the window icon.
      if (auto icon = LoadIcon(nullptr, IDI_ERROR)) {
        SendDlgItemMessage(hwnd, IDC_ERROR_ICON, STM_SETICON, reinterpret_cast<LPARAM>(icon), 0);
      }

      // Set the message text.
      if (auto message = GetDlgItem(hwnd, IDC_ERROR_MESSAGE)) {
        SetWindowText(message, data->message.c_str());
      }

      // Set the details text and font.
      if (auto details = GetDlgItem(hwnd, IDC_ERROR_DETAILS)) {
        SetWindowText(details, data->details.c_str());
        auto hdc = GetDC(hwnd);
        auto height = -MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        ReleaseDC(hwnd, hdc);
        data->font = CreateFontW(height, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
          CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, L"Consolas");
        SendMessage(details, WM_SETFONT, reinterpret_cast<WPARAM>(data->font), 0);
      }

      // Center the window.
      if (auto monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL)) {
        RECT rc = {};
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);
        if (GetWindowRect(hwnd, &rc) && GetMonitorInfo(monitor, &mi)) {
          auto x = ((mi.rcWork.right - mi.rcWork.left) - (rc.right - rc.left)) / 2;
          auto y = ((mi.rcWork.bottom - mi.rcWork.top) - (rc.bottom - rc.top)) / 2;
          SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
        }
      }
      return TRUE;
    case WM_CLOSE:
      EndDialog(hwnd, IDOK);
      return TRUE;
    case WM_DESTROY:
      DeleteObject(data->font);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
      return TRUE;
    case WM_SHOWWINDOW:
      SendDlgItemMessage(hwnd, IDC_ERROR_DETAILS, EM_SETSEL, 0, 0);
      SetFocus(GetDlgItem(hwnd, IDOK));
      return TRUE;
    case WM_COMMAND:
      switch (LOWORD(wparam)) {
      case IDOK:
        EndDialog(hwnd, LOWORD(wparam));
      }
      return TRUE;
    }
    return FALSE;
  }, reinterpret_cast<LPARAM>(&data));
}

}  // namespace dialog
