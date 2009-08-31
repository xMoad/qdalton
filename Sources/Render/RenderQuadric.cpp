#include "Render/RenderQuadric.h"

Render::Quadric::Quadric()
{
  GLUquadric_ = gluNewQuadric();
}

Render::Quadric::~Quadric()
{
  gluDeleteQuadric(GLUquadric_);
}

void Render::Quadric::prepare(Style style) const
{
  switch (style)
  {
  case STYLE_FILL:
    gluQuadricDrawStyle(GLUquadric_, GLU_FILL);
    break;
  case STYLE_LINE:
    gluQuadricDrawStyle(GLUquadric_, GLU_LINE);
    break;
  case STYLE_POINT:
    gluQuadricDrawStyle(GLUquadric_, GLU_POINT);
    break;
  case STYLE_SILHOUETTE:
    gluQuadricDrawStyle(GLUquadric_, GLU_SILHOUETTE);
    break;
  }
  gluQuadricNormals(GLUquadric_, GLU_SMOOTH);
}
