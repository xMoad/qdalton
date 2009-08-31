#ifndef RENDER_CONE_H
#define RENDER_CONE_H

#include <Eigen/Array>

#include "Render/RenderMaterial.h"
#include "Render/RenderStyle.h"
#include "Render/RenderQuality.h"
#include "Render/RenderQuadric.h"

namespace Render
{
  class Cone
  {
  public:
    Cone(const Eigen::Vector3f& vertex1,
         const Eigen::Vector3f& vertex2,
         GLfloat radius,
         const Render::Material& material);
    void draw(Render::Style style, Render::Quality quality) const;
  private:
    Eigen::Vector3f vertex1_;
    Eigen::Vector3f vertex2_;
    GLfloat radius_;
    Render::Material material_;
    Render::Quadric quadric_;
  };

}

#endif // RENDER_CONE_H
