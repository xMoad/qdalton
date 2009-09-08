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

#include "Render/RenderSphere.h"

Render::Sphere::Sphere(const Eigen::Vector3f& centre,
                       GLfloat radius,
                       const Render::Material& material):
centre_(centre), radius_(radius), material_(material)
{
}

void Render::Sphere::setMaterial(const Render::Material& material)
{
  material_ = material;
}

void Render::Sphere::draw(Render::Style style, Render::Quality quality) const
{
  material_.prepare();
  quadric_.prepare(style);
  GLint slices = quality;
  GLint stacks = quality;
  glPushMatrix();
  {
    glTranslatef(centre_.x(), centre_.y(), centre_.z());
    gluSphere(quadric_.GLUquadric_, radius_, slices, stacks);
  }
  glPopMatrix();
  glFlush();
}

const Eigen::Vector3f& Render::Sphere::centre() const
{
  return centre_;
}

GLfloat Render::Sphere::radius() const
{
  return radius_;
}

void Render::Sphere::setRadius(GLfloat radius)
{
  radius_ = radius;
}
