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

#ifndef RENDER_CONE_H
#define RENDER_CONE_H

#include <Eigen/Array>

#include "Render/RenderMaterial.h"
#include "Render/RenderQuadric.h"

namespace Render
{
  class Cone
  {
  public:
    Cone();

    const Eigen::Vector3f& vertex1() const;
    void setVertex1(const Eigen::Vector3f& vertex1);

    const Eigen::Vector3f& vertex2() const;
    void setVertex2(const Eigen::Vector3f& vertex2);

    GLfloat radius() const;
    void setRadius(GLfloat radius);

    const Render::Material& material() const;
    void setMaterial(const Render::Material& material);

    void draw(Render::Style style) const;
  private:
    Eigen::Vector3f vertex1_;
    Eigen::Vector3f vertex2_;
    GLfloat radius_;
    Render::Material material_;
    Render::Quadric quadric_;
  };

}

#endif // RENDER_CONE_H
