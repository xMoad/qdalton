#ifndef RENDER_CYLINDER_H
#define RENDER_CYLINDER_H

#include <Eigen/Array>

#include "Render/RenderMaterial.h"
#include "Render/RenderStyle.h"
#include "Render/RenderQuality.h"
#include "Render/RenderQuadric.h"

namespace Render
{
  class Cylinder
  {
  public:
    Cylinder(const Eigen::Vector3f& vertex1,
             const Eigen::Vector3f& vertex2,
             GLfloat radius,
             const Render::Material& material);
    const Eigen::Vector3f& vertex1() const;
    const Eigen::Vector3f& vertex2() const;
    Eigen::Vector3f centre() const;
    GLfloat radius() const;
    void draw(Render::Style style, Render::Quality quality) const;
  private:
    Eigen::Vector3f vertex1_;
    Eigen::Vector3f vertex2_;
    GLfloat radius_;
    Render::Material material_;
    Render::Quadric quadric_;
  };
}

#endif // RENDER_CYLINDER_H
