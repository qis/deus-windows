#pragma once
#include "client.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <windows.h>
#include <memory>

class window {
public:
  void create(int samples, bool fullscreen);

  void on_create();
  void on_destroy();
  void on_paint();
  void on_size(int cx, int cy);
  void on_dpi(int dpi, LPCRECT rc);

private:
  LRESULT handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  HWND hwnd_ = nullptr;
  HDC hdc_ = nullptr;
  bool fullscreen_ = false;

  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLSurface surface_ = EGL_NO_SURFACE;
  EGLContext context_ = EGL_NO_CONTEXT;

  GLsizei samples_ = 0;
  GLuint rbo_ = 0;
  GLuint fbo_ = 0;
  GLsizei cx_ = 1;
  GLsizei cy_ = 1;

  std::unique_ptr<client> client_;
  client::clock::time_point time_point_;
  client::clock::duration::rep frames_ = 0;
};
