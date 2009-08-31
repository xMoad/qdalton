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
