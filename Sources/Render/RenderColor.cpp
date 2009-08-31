#include "Render/RenderColor.h"

Render::Color::Color()
{
  r_ = 0.0f;
  g_ = 0.0f;
  b_ = 0.0f;
  a_ = 1.0f;
}

Render::Color::Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
  r_ = r;
  g_ = g;
  b_ = b;
  a_ = a;
}

Render::Color::Color(const Render::Color& color)
{
  r_ = color.r_;
  g_ = color.g_;
  b_ = color.b_;
  a_ = color.a_;
}

GLfloat Render::Color::r() const
{
  return r_;
}

GLfloat Render::Color::g() const
{
  return g_;
}

GLfloat Render::Color::b() const
{
  return b_;
}

GLfloat Render::Color::a() const
{
  return a_;
}

const Render::Color& Render::Color::blue()
{
  static Render::Color* p = new Render::Color(0.0f, 0.0f, 1.0f, 1.0f);
  return *p;
}

const Render::Color& Render::Color::green()
{
  static Render::Color* p = new Render::Color(0.0f, 1.0f, 0.0f, 1.0f);
  return *p;
}

const Render::Color& Render::Color::red()
{
  static Render::Color* p = new Render::Color(1.0f, 0.0f, 0.0f, 1.0f);
  return *p;
}

const Render::Color& Render::Color::yellow()
{
  static Render::Color* p = new Render::Color(1.0f, 1.0f, 0.0f, 1.0f);
  return *p;
}

const Render::Color& Render::Color::selection()
{
  static Render::Color* p = new Render::Color(0.2f, 0.4f, 0.2f, 1.0f);
  return *p;
}
