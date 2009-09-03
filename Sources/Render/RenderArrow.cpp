/**********************************************************************
  Copyright (C) 2008, 2009 Anton Simakov

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

/** Copyright (C) 2009 Anton Simakov
 *
 */

#include "Render/RenderArrow.h"
#include "Render/RenderCylinder.h"
#include "Render/RenderCone.h"

Render::Arrow::Arrow(const Eigen::Vector3f& origin,
                     const Eigen::Vector3f& terminus,
                     GLfloat radius,
                     const Render::Material& material):
origin_(origin), terminus_(terminus), radius_(radius), material_(material)
{
}

void Render::Arrow::draw(Render::Style style, Render::Quality quality) const
{
  Eigen::Vector3f point = terminus_ - origin_;
  point = point * (point.norm() - 0.3f) / point.norm();
  point = point + origin_;
  Render::Cylinder cylinder(origin_, point, radius_, material_);
  cylinder.draw(style, quality);
  Render::Cone cone(point, terminus_, radius_ * 2, material_);
  cone.draw(style, quality);
}

void Render::Arrow::setOrigin(const Eigen::Vector3f& origin)
{
  origin_ = origin;
}

void Render::Arrow::setTerminus(const Eigen::Vector3f& terminus)
{
  terminus_ = terminus;
}
