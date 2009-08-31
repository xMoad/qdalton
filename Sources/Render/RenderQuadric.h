// © 2009 Anton Simakov
// Quadric class.

#ifndef RENDER_QUADRIC_H
#define RENDER_QUADRIC_H

#include "Render/RenderGLU.h"
#include "Render/RenderStyle.h"

namespace Render
{
  class Quadric
  {
  public:
    Quadric();
    ~Quadric();
    void prepare(Render::Style style) const;
    GLUquadricObj* GLUquadric_;
  };
}

#endif // RENDER_QUADRIC_H
