#include <cmath>

#include "Render/RenderCone.h"

const double PI = 4.0 * atan(1.0);

Render::Cone::Cone(const Eigen::Vector3f& vertex1,
                   const Eigen::Vector3f& vertex2,
                   GLfloat radius,
                   const Render::Material& material):
vertex1_(vertex1), vertex2_(vertex2), radius_(radius), material_(material)
{
}

void Render::Cone::draw(Render::Style style, Render::Quality quality) const
{
  material_.prepare();
  quadric_.prepare(style);
  GLint slices = quality;
  GLfloat vx = vertex2_.x() - vertex1_.x();
  GLfloat vy = vertex2_.y() - vertex1_.y();
  GLfloat vz = vertex2_.z() - vertex1_.z();
  // Handle the degenerate case with an approximation.
  if (fabs(vz) < 0.001f)
  {
    vz = 0.001f;
  }
  GLfloat v = sqrt(vx * vx + vy * vy + vz * vz);
  GLfloat ax = (GLfloat)(180.0f / PI * acos(vz / v));
  if (vz < 0.0)
  {
    ax = -ax;
  }
  GLfloat rx = -vy * vz;
  GLfloat ry = vx * vz;

  glPushMatrix();
  {
    glTranslatef(vertex1_.x(), vertex1_.y(), vertex1_.z());
    glRotatef(ax, rx, ry, 0.0f);
    gluQuadricOrientation(quadric_.GLUquadric_, GLU_OUTSIDE);
    gluCylinder(quadric_.GLUquadric_, radius_, 0.0f, v, slices, 1);
    //draw the cap
    gluQuadricOrientation(quadric_.GLUquadric_, GLU_INSIDE);
    gluDisk(quadric_.GLUquadric_, 0.0, radius_, slices, 1);
    glTranslatef(0, 0, v);
  }
  glPopMatrix();
}
