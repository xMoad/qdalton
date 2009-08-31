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
