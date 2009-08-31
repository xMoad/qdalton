/** Copyright (C) 2009 Anton Simakov
 *
 */

#ifndef RENDER_ARROW_H
#define RENDER_ARROW_H

#include <Eigen/Array>

#include "Render/RenderMaterial.h"
#include "Render/RenderQuality.h"
#include "Render/RenderStyle.h"

namespace Render
{
  /**
   * @class Arrow Arrow.h "Render/RenderArrow.h"
   * @brief Represents and draw arrow.
   * @author Anton Simakov
   * @version 0.1
   */
  class Arrow
  {
  public:
    Arrow(const Eigen::Vector3f& origin, const Eigen::Vector3f& terminus,
          GLfloat radius, const Material& material);
    void draw(Style style, Render::Quality quality) const;
    void setOrigin(const Eigen::Vector3f& origin);
    void setTerminus(const Eigen::Vector3f& terminus);
  private:
    Eigen::Vector3f origin_;
    Eigen::Vector3f terminus_;
    GLfloat radius_;
    Render::Material material_;
  };
}

#endif // RENDER_ARROW_H
