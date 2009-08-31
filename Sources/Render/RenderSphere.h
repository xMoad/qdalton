#ifndef RENDER_SPHERE_H
#define RENDER_SPHERE_H

#include <Eigen/Array>

#include "Render/RenderMaterial.h"
#include "Render/RenderQuadric.h"
#include "Render/RenderQuality.h"

namespace Render
{
  class Sphere
  {
  public:
    Sphere(const Eigen::Vector3f& centre,
           GLfloat radius,
           const Render::Material& material);
    const Eigen::Vector3f& centre() const;
    GLfloat radius() const;
    void setRadius(GLfloat radius);
    void setMaterial(const Render::Material& material);
    void draw(Render::Style style, Render::Quality quality) const;
  private:
    Eigen::Vector3f centre_;
    GLfloat radius_;
    Render::Material material_;
    Render::Quadric quadric_;
  };
}

#endif // RENDER_SPHERE_H
