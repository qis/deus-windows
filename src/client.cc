#include "client.h"
#include <ice/exception.h>
#include <loader.h>
#include <array>
#include <cstdint>

client::client(GLsizei cx, GLsizei cy, GLint dpi) :
  time_point_(clock::now())
{
  glViewport(0, 0, cx, cy);
  glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

  loader loader;
  
  program_ = {
    gl::shader(loader.load<std::string>("shader.vert"), GL_VERTEX_SHADER),
    gl::shader(loader.load<std::string>("shader.frag"), GL_FRAGMENT_SHADER)
  };

  GLfloat vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top
  };

  vbo_ = gl::buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  vao_ = gl::vao([&]() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);

    // Color attribute.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
  });

  glReleaseShaderCompiler();
}

void client::render()
{
  auto tp = clock::now();
  auto dt = tp - time_point_.exchange(tp);

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program_);
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}

void client::resize(GLsizei cx, GLsizei cy)
{
  glViewport(0, 0, cx, cy);
}

void client::scale(GLint dpi)
{}
