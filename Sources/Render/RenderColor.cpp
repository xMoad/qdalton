/**********************************************************************
  Copyright (C) 2008, 2009, 2010 Anton Simakov

  This file is part of QDalton.
  For more information, see <http://code.google.com/p/qdalton/>

  QDalton is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  QDalton is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with QDalton. If not, see <http://www.gnu.org/licenses/>.

 **********************************************************************/

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
  static Render::Color color(0.0f, 0.0f, 1.0f, 1.0f);
  return color;
}

const Render::Color& Render::Color::green()
{
  static Render::Color color(0.0f, 1.0f, 0.0f, 1.0f);
  return color;
}

const Render::Color& Render::Color::red()
{
  static Render::Color color(1.0f, 0.0f, 0.0f, 1.0f);
  return color;
}

const Render::Color& Render::Color::yellow()
{
  static Render::Color color(1.0f, 1.0f, 0.0f, 1.0f);
  return color;
}

const Render::Color& Render::Color::selection()
{
  static Render::Color color(0.0f, 1.0f, 1.0f, 1.0f);
  return color;
}
