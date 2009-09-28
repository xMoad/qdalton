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

#include "Render/RenderArrow.h"
#include "Render/RenderCylinder.h"
#include "Render/RenderCone.h"

Render::Arrow::Arrow():
    origin_(Eigen::Vector3f(0.0f, 0.0f, 0.0f)),
    terminus_(Eigen::Vector3f(0.0f, 0.0f, 0.0f)),
    radius_(0.0f),
    material_(Render::Material())
{
}

const Eigen::Vector3f& Render::Arrow::origin() const
{
  return origin_;
}

void Render::Arrow::setOrigin(const Eigen::Vector3f& origin)
{
  origin_ = origin;
}

const Eigen::Vector3f& Render::Arrow::terminus() const
{
  return terminus_;
}

void Render::Arrow::setTerminus(const Eigen::Vector3f& terminus)
{
  terminus_ = terminus;
}

GLfloat Render::Arrow::radius() const
{
  return radius_;
}

void Render::Arrow::setRadius(GLfloat radius)
{
  radius_ = radius;
}

const Render::Material& Render::Arrow::material() const
{
  return material_;
}

void Render::Arrow::setMaterial(const Render::Material& material)
{
  material_ = material;
}

void Render::Arrow::draw(Render::Style style, Render::Quality quality) const
{
  Eigen::Vector3f point;
  Render::Cylinder cylinder;
  Render::Cone cone;

  point = terminus_ - origin_;
  point = point * (point.norm() - 0.3f) / point.norm();
  point = point + origin_;

  cylinder.setVertex1(origin_);
  cylinder.setVertex2(point);
  cylinder.setRadius(radius_);
  cylinder.setMaterial(material_);
  cylinder.draw(style, quality);

  cone.setVertex1(point);
  cone.setVertex2(terminus_);
  cone.setRadius(radius_ * 2);
  cone.setMaterial(material_);
  cone.draw(style, quality);
}
