/** Copyright (C) 2009 Anton Simakov
 *
 */

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
    /**
     * Default constructor.
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
