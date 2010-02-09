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
#include "Render/RenderCylinder.h"

const double PI = 4.0 * atan(1.0);

void drawGlu(GLUquadricObj* gluQuadric, GLint slices)
{
  // First draw the lateral surface.
  gluQuadricOrientation(gluQuadric, GLU_OUTSIDE);
  gluCylinder(gluQuadric, 1.0f, 1.0f, 1.0f, slices, 1);
  // Next draw the first base.
  gluQuadricOrientation(gluQuadric, GLU_INSIDE);
  gluDisk(gluQuadric, 0.0, 1.0f, slices, 1);
  glTranslatef(0, 0, 1);
  // Finally draw the second base.
  gluQuadricOrientation(gluQuadric, GLU_OUTSIDE);
  gluDisk(gluQuadric, 0.0, 1.0f, slices, 1);
}

Render::Cylinder::Cylinder():
    vertex1_(Eigen::Vector3f(0.0f, 0.0f, 0.0f)),
    vertex2_(Eigen::Vector3f(0.0f, 0.0f, 0.0f)),
    radius_(0.0f),
    material_(Render::Material())
{
}

void Render::Cylinder::draw(Render::Style style) const
{
  material_.prepare();
  quadric_.prepare(style);
  GLint slices = 24;

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

  //now we can do the actual drawing !
  glPushMatrix();
  {
    glMultMatrixf(matrix.data());
    drawGlu(quadric_.gluQuadricObj_, slices);
  }
  glPopMatrix();
}

void Render::Cylinder::drawMulti(Render::Style style,
                                 quint8 order,
                                 float shift,
                                 const Eigen::Vector3f& planeNormalVector) const
{
  material_.prepare();
  quadric_.prepare(style);
  GLint slices = 24;

  // construct the 4D transformation matrix
  Eigen::Matrix4f matrix;
  matrix.row(3) << 0,0,0,1;
  matrix.block<3,1>(0,3) = vertex1();
  matrix.block<3,1>(0,2) = vertex2() - vertex1(); // the "axis vector" of the line
  // Now we want to construct an orthonormal basis whose third
  // vector is axis.normalized(). The first vector in this
  // basis, which we call ortho1, should be approximately lying in the
  // z=0 plane if possible. This is to ensure double bonds don't look
  // like single bonds from the default point of view.
  Eigen::Vector3f axisNormalized = matrix.block<3,1>(0,2).normalized();
  Eigen::Block<Eigen::Matrix4f, 3, 1> ortho1(matrix, 0, 0);
  ortho1 = axisNormalized.cross(planeNormalVector);
  double ortho1Norm = ortho1.norm();
  if( ortho1Norm > 0.001 ) ortho1 = ortho1.normalized() * radius();
  else ortho1 = axisNormalized.unitOrthogonal() * radius();
  matrix.block<3,1>(0,1) = axisNormalized.cross(ortho1);

  // now the matrix is entirely filled, so we can do the actual drawing !
  glPushMatrix();
  glMultMatrixf( matrix.data() );
  if( order == 1 )
  {
    drawGlu(quadric_.gluQuadricObj_, slices);
  }
  else
  {
    double angleOffset = 0.0;
    if( order >= 3 )
    {
      if( order == 3 ) angleOffset = 90.0;
      else angleOffset = 22.5;
    }

    double displacementFactor = shift / radius();
    for( int i = 0; i < order; i++)
    {
      glPushMatrix();
      {
        glRotated( angleOffset + 360.0 * i / order,
                   0.0, 0.0, 1.0 );
        glTranslated( displacementFactor, 0.0, 0.0 );
        drawGlu(quadric_.gluQuadricObj_, slices);
      }
      glPopMatrix();
    }
  }
  glPopMatrix();
}

const Eigen::Vector3f& Render::Cylinder::vertex1() const
{
  return vertex1_;
}

void Render::Cylinder::setVertex1(const Eigen::Vector3f& vertex1)
{
  vertex1_ = vertex1;
}

const Eigen::Vector3f& Render::Cylinder::vertex2() const
{
  return vertex2_;
}

void Render::Cylinder::setVertex2(const Eigen::Vector3f& vertex2)
{
  vertex2_ = vertex2;
}

GLfloat Render::Cylinder::radius() const
{
  return radius_;
}

void Render::Cylinder::setRadius(GLfloat radius)
{
  radius_ = radius;
}

const Render::Material& Render::Cylinder::material() const
{
  return material_;
}

void Render::Cylinder::setMaterial(const Render::Material& material)
{
  material_ = material;
}

Eigen::Vector3f Render::Cylinder::centre() const
{
  return (vertex1_ + vertex2_) / 2.0f;
}
