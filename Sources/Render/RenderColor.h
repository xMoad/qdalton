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

#ifndef RENDER_COLOR_H
#define RENDER_COLOR_H

#include "Render/RenderGL.h"

namespace Render
{
  /**
   * @class Color Color.h "Render/Color.h"
   * @brief Represents 4-component (r, g, b, a) color.
   * @author Anton Simakov
   * @version 0.1
   */
  class Color
  {
  public:
    /** Default constructor.
     *  Create instance with r, g, b and a equals 0.
     */
    Color();


    Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    Color(const Color& color);
    GLfloat r() const;
    GLfloat g() const;
    GLfloat b() const;
    GLfloat a() const;
    // Return one of predefined colors
    static const Color& blue();
    static const Color& green();
    static const Color& red();
    static const Color& white();
    static const Color& yellow();
    static const Color& selection();
  private:
    GLfloat r_;
    GLfloat g_;
    GLfloat b_;
    GLfloat a_;
  };
}

#endif // RENDER_COLOR_H
