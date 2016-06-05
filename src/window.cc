#include "window.h"
#include "dialog.h"
#include <ice/exception.h>
#include <angle_gl.h>
#include <resource.h>
#include <algorithm>
#include <debug>
#include <exception>
#include <string>

namespace {

std::filesystem::path application_path()
{
  DWORD size = 0;
  std::wstring path;
  do {
    path.resize(path.size() + MAX_PATH);
    size = GetModuleFileName(nullptr, &path[0], static_cast<DWORD>(path.size()));
  } while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);
  path.resize(size);
  return std::filesystem::path(path).parent_path();
}

}  // namespace

void window::create(int samples, bool fullscreen)
{
  // Store the settings.
  samples_ = samples;
  fullscreen_ = fullscreen;

  // Get the module instance.
  auto instance = GetModuleHandle(nullptr);

  // Register the main application window class.
  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(wc);
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
    auto self = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (msg == WM_CREATE) {
      self = reinterpret_cast<window*>(reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else if (msg == WM_DESTROY) {
      SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
    }
    return self ? self->handle(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
  };
  wc.hInstance = instance;
  wc.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_MAIN));
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.lpszClassName = TEXT(PRODUCT);

  if (!RegisterClassEx(&wc)) {
    MessageBox(nullptr, L"Could not register the main application window class.", TEXT(PROJECT), MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    PostQuitMessage(1);
    return;
  }

  // Create the main application window.
  auto x = CW_USEDEFAULT;
  auto y = CW_USEDEFAULT;
  auto ws = fullscreen_ ? WS_POPUP : WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
  auto cx = 1028;
  auto cy = 768;
  
  if (!CreateWindowEx(0, TEXT(PRODUCT), TEXT(PROJECT), ws, x, y, cx, cy, nullptr, nullptr, instance, this)) {
    MessageBox(nullptr, L"Could not create the main application window.", TEXT(PROJECT), MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    PostQuitMessage(1);
    return;
  }
}

void window::on_create()
{
  // Get the window monitor.
  auto monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONULL);
  if (!monitor) {
    throw ice::runtime_error("Could not get the monitor handle.");
  }

  // Get the window DPI setting.
  auto dpi = 96;
  auto shcore = LoadLibrary(L"shcore.dll");
  if (!shcore) {
    throw ice::runtime_error("Could not load shcore.dll.");
  }
  typedef HRESULT(WINAPI *GetDpiForMonitorProc)(HMONITOR, int, UINT*, UINT*);
  auto GetDpiForMonitor = reinterpret_cast<GetDpiForMonitorProc>(GetProcAddress(shcore, "GetDpiForMonitor"));
  if (!GetDpiForMonitor) {
    throw ice::runtime_error("Could not get the address of GetDpiForMonitor.");
  }
  UINT xdpi = 0;
  UINT ydpi = 0;
  if (SUCCEEDED(GetDpiForMonitor(monitor, 0, &xdpi, &ydpi))) {
    dpi = static_cast<decltype(dpi)>(ydpi);
  }

  // Center the window.
  MONITORINFO mi = {};
  mi.cbSize = sizeof(mi);
  if (GetMonitorInfo(monitor, &mi)) {
    RECT rc = {};
    if (GetWindowRect(hwnd_, &rc)) {
      auto x = ((mi.rcWork.right - mi.rcWork.left) - (rc.right - rc.left)) / 2;
      auto y = ((mi.rcWork.bottom - mi.rcWork.top) - (rc.bottom - rc.top)) / 2;
      SetWindowPos(hwnd_, nullptr, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
      if (fullscreen_) {
        auto x = mi.rcMonitor.left;
        auto y = mi.rcMonitor.top;
        auto cx = mi.rcMonitor.right - mi.rcMonitor.left;
        auto cy = mi.rcMonitor.bottom - mi.rcMonitor.top;
        SetWindowPos(hwnd_, nullptr, x, y, cx, cy, SWP_NOACTIVATE);
      }
    }
  }
  
  // Get the display handle.
  hdc_ = GetDC(hwnd_);

  // Create OpenGL ES display, surface and context.
  const EGLint display_attributes[] = {
    EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
    EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE,
    EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
    EGL_NONE,
  };
  display_ = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, hdc_, display_attributes);
  if (!display_) {
    debug() << "Could not select a Direct3D 11 display type.";
    display_ = eglGetDisplay(hdc_);
  }
  if (!display_) {
    throw ice::runtime_error("Could not create an OpenGL ES display.");
  }
  if (!eglInitialize(display_, nullptr, nullptr)) {
    throw ice::runtime_error("Could not initialize the OpenGL ES display.");
  }
  EGLConfig config = {};
  EGLint config_count = 0;
  const EGLint attributes[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_STENCIL_SIZE, 0,
    EGL_DEPTH_SIZE, 16,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 0,
    EGL_NONE
  };
  if (!eglChooseConfig(display_, attributes, &config, 1, &config_count) || config_count < 1) {
    throw ice::runtime_error("Could not choose an OpenGL ES 3 config.");
  }
  surface_ = eglCreateWindowSurface(display_, config, hwnd_, nullptr);
  if (!surface_) {
    throw ice::runtime_error("Could not create the OpenGL ES 3 display surface.");
  }
  const EGLint ctxattr[] = {
    EGL_CONTEXT_CLIENT_VERSION, 3,
    EGL_NONE
  };
  context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT, ctxattr);
  if (!context_) {
    throw ice::runtime_error("Could not create the OpenGL ES 3 display context.");
  }
  if (!eglMakeCurrent(display_, surface_, surface_, context_)) {
    throw ice::runtime_error("Could not attach the OpenGL ES 3 context to the display surface.");
  }

  // Get the client size.
  RECT rc = {};
  GetClientRect(hwnd_, &rc);
  cx_ = std::max(1L, rc.right - rc.left);
  cy_ = std::max(1L, rc.bottom - rc.top);

  // Create render and frame buffers for multisampling support.
  if (samples_ > 1) {
    glGenRenderbuffers(1, &rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorageMultisampleANGLE(GL_RENDERBUFFER, samples_, GL_BGRA8_EXT, cx_, cy_);
    if (auto ec = gl::make_error()) {
      throw ice::runtime_error("Could not create the multisample render buffer.")
        << ec.message() << "\nSamples: " << samples_;
    }

    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_);
    if (auto ec = gl::make_error()) {
      throw ice::runtime_error("Could not create the multisample frame buffer.")
        << ec.message() << "\nSamples: " << samples_;
    }
  }

  // Create the client.
#ifndef _DEBUG
  auto path = std::filesystem::canonical(application_path() / "data.pak");
#else
  auto path = std::filesystem::canonical(application_path() / ".." / ".." / "res" / "data");
#endif
  client_ = std::make_unique<client>(path, cx_, cy_, dpi);

  // Show the window.
  ShowWindow(hwnd_, SW_SHOW);

  // Initialize the frame rate timer.
  if (!fullscreen_) {
    time_point_ = client::clock::now();
  }
}

void window::on_destroy()
{
  // Destroy the client.
  client_.reset();

  // Destroy render and frame buffers.
  if (samples_ > 1) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glDeleteRenderbuffers(1, &rbo_);
  }

  // Destroy OpenGL ES 3.0 display, context and surface.
  eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroyContext(display_, context_);
  eglDestroySurface(display_, context_);
  eglTerminate(display_);

  // Release the display handle.
  ReleaseDC(hwnd_, hdc_);

  // Stop the main message loop.
  PostQuitMessage(0);
}

void window::on_paint()
{
  // Draw the client.
  if (client_) {
    if (samples_ > 1) {
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    }

    client_->render();

    if (samples_ > 1) {
      glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBlitFramebufferANGLE(0, 0, cx_, cy_, 0, 0, cx_, cy_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // Post the surface color buffer to the native window.
    eglSwapBuffers(display_, surface_);

    // Update the frame rate.
    if (!fullscreen_) {
      static const auto second = std::chrono::duration_cast<client::clock::duration>(std::chrono::seconds(1));
      auto dt = client::clock::now() - time_point_;
      if (dt >= second) {
        auto frames = static_cast<int>(frames_ * dt.count() / second.count());
        wchar_t title[ARRAYSIZE(PROJECT) + 7 + 12 + 1];
        title[_snwprintf(title, ARRAYSIZE(title), TEXT(PROJECT) L" @ %d FPS", frames)] = L'\0';
        SetWindowText(hwnd_, title);
        time_point_ = client::clock::now();
        frames_ = 0;
      } else {
        frames_++;
      }
    }
  } else {
    PAINTSTRUCT ps = {};
    auto hdc = BeginPaint(hwnd_, &ps);
    RECT rc = { 0, 0, cx_, cy_ };
    FillRect(hdc, &rc, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
    EndPaint(hwnd_, &ps);
  }
}

void window::on_size(int cx, int cy)
{
  // Resize the client.
  if (cx != cx_ || cy != cy_) {
    cx_ = std::max(1, cx);
    cy_ = std::max(1, cy);
    if (client_) {
      if (samples_ > 1) {
        glRenderbufferStorageMultisampleANGLE(GL_RENDERBUFFER, samples_, GL_BGRA8_EXT, cx_, cy_);
      }
      client_->resize(cx_, cy_);
      client_->render();
    }
  }
}

void window::on_dpi(int dpi, LPCRECT rc)
{
  // Scale the client.
  SetWindowPos(hwnd_, nullptr, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, SWP_NOZORDER | SWP_NOACTIVATE);
  if (client_) {
    client_->scale(dpi);
  }
}

LRESULT window::handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // Handle windows messages.
  try {
    switch (msg) {
    case WM_CREATE:
      hwnd_ = hwnd;
      on_create();
      return 0;
    case WM_DESTROY:
      on_destroy();
      hwnd_ = nullptr;
      return 0;
    case WM_PAINT:
      on_paint();
      return 0;
    case WM_SIZE:
      on_size(LOWORD(lparam), HIWORD(lparam));
      return 0;
    case WM_ERASEBKGND:
      return 1;
    case WM_GETMINMAXINFO:
      if (auto mmi = reinterpret_cast<LPMINMAXINFO>(lparam)) {
        mmi->ptMinTrackSize.x = 300;
        mmi->ptMinTrackSize.y = 250;
      }
      return 0;
    case WM_DPICHANGED:
      on_dpi(HIWORD(wparam), reinterpret_cast<LPCRECT>(lparam));
      return 0;
    }
  }
  catch (const ice::exception& e) {
    auto parent = IsWindowVisible(hwnd_) ? hwnd_ : nullptr;
    if (auto info = e.info()) {
      dialog::error(parent, e.what(), info);
    } else {
      dialog::error(parent, e.what());
    }
    DestroyWindow(hwnd);
  }
  catch (const std::exception& e) {
    auto parent = IsWindowVisible(hwnd_) ? hwnd_ : nullptr;
    dialog::error(parent, e.what());
    DestroyWindow(hwnd);
  }
  return DefWindowProc(hwnd, msg, wparam, lparam);
}
