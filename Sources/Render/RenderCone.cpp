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

#include <cmath>
#include <Eigen/Geometry>
#include "Render/RenderCone.h"

const double PI = 4.0 * atan(1.0);

Render::Cone::Cone():
    vertex1_(Eigen::Vector3f(0.0f, 0.0f, 0.0f)),
    vertex2_(Eigen::Vector3f(0.0f, 0.0f, 0.0f)),
    radius_(0.0f),
    material_(Render::Material())
{
}

const Eigen::Vector3f& Render::Cone::vertex1() const
{
  return vertex1_;
}

void Render::Cone::setVertex1(const Eigen::Vector3f& vertex1)
{
  vertex1_ = vertex1;
}

const Eigen::Vector3f& Render::Cone::vertex2() const
{
  return vertex2_;
}

void Render::Cone::setVertex2(const Eigen::Vector3f& vertex2)
{
  vertex2_ = vertex2;
}

GLfloat Render::Cone::radius() const
{
  return radius_;
}

void Render::Cone::setRadius(GLfloat radius)
{
  radius_ = radius;
}

const Render::Material& Render::Cone::material() const
{
  return material_;
}

void Render::Cone::setMaterial(const Render::Material& material)
{
  material_ = material;
}

void Render::Cone::draw(Render::Style style,
                        Render::Quality quality) const
{
  material_.prepare();
  quadric_.prepare(style);
  GLint slices = quality;

  // construct the 4D transformation matrix
  Eigen::Matrix4f matrix;
  matrix.row(3) << 0, 0, 0, 1;
  matrix.block<3,1>(0,2) = vertex2() - vertex1(); // the axis

  // construct an orthogonal basis whose first vector is the axis, and whose other vectors
  // have norm equal to 'radius'.
  Eigen::Vector3f axisNormalized = matrix.block<3,1>(0,2).normalized();
  matrix.block<3,1>(0,0) = axisNormalized.unitOrthogonal() * radius();
  matrix.block<3,1>(0,1) = axisNormalized.cross(matrix.block<3,1>(0,0));
  matrix.block<3,1>(0,3) = vertex1();

  glPushMatrix();
  {
    glMultMatrixf(matrix.data());
    // First draw the lateral surface.
    gluQuadricOrientation(quadric_.gluQuadricObj_, GLU_OUTSIDE);
    gluCylinder(quadric_.gluQuadricObj_, 1.0f, 0.0f, 1.0f, slices, 1);
    // Next draw the cone base.
    gluQuadricOrientation(quadric_.gluQuadricObj_, GLU_INSIDE);
    gluDisk(quadric_.gluQuadricObj_, 0.0, 1.0f, slices, 1);
  }
  glPopMatrix();
}
