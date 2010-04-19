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

#ifndef RENDER_CYLINDER_H
#define RENDER_CYLINDER_H

#include <Eigen/Array>

#include "Render/RenderMaterial.h"
#include "Render/RenderQuadric.h"

namespace Render
{
  /**
   * @class Render::Cylinder RenderCylinder.h "Render/RenderCylinder.h"
   * @brief Represents cylinder.
   * @author Anton Simakov
   * @version 0.1
   *
   * Render::Cylinder class is responsible for drawing cylinders in OpenGL scene.
   */
  class Cylinder
  {
  public:
    /** Default constructor.
     *
     */
    Cylinder();

    /** Return the first vertex.
     *
     */
    const Eigen::Vector3f& vertex1() const;
    void setVertex1(const Eigen::Vector3f& vertex1);

    const Eigen::Vector3f& vertex2() const;
    void setVertex2(const Eigen::Vector3f& vertex2);

    GLfloat radius() const;
    void setRadius(GLfloat radius);

    const Render::Material& material() const;
    void setMaterial(const Render::Material& material);

    Eigen::Vector3f centre() const;

    void draw(Render::Style style, GLint slices) const;

    void drawMulti(Render::Style style,
                   quint8 order,
                   float shift,
                   const Eigen::Vector3f& planeNormalVector,
                   GLint slices) const;
  private:
    Eigen::Vector3f vertex1_;
    Eigen::Vector3f vertex2_;
    GLfloat radius_;
    Render::Material material_;
    Render::Quadric quadric_;
  };
}

#endif // RENDER_CYLINDER_H
