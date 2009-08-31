/** Copyright (C) 2009 Anton Simakov
 *
 */

#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H

#include "Render/RenderColor.h"

namespace Render
{
   /**
   * @class Material Material.h "Render/RenderMaterial.h"
   * @brief Represents material.
   * @author Anton Simakov
   * @version 0.1
   */
  class Material
  {
  public:
    enum Component
    {
      AMBIENT,
      DIFFUSE,
      EMISSION,
      SPECULAR
    };
    Material(const Material& material);
    Material();
    Material(const Color& ambient,
             const Color& diffuse,
             const Color& emission,
             const Color& specular,
             GLfloat shininess);
    Material(const Color& ambient, bool optimal);

    Render::Color& ambient();
    Render::Color& diffuse();
    Render::Color& emission();
    Render::Color& specular();
    GLfloat& shininess();
    void prepare() const;
    // OpenGL default material properties of the objects
    static Color& defaultComponent(Component component);
    static Color& optimalComponent(Component component);
    static GLfloat optimalShininess();
    static GLfloat defaultShininess();

    static const Material& blue();
    static const Material& green();
    static const Material& red();
    static const Material& yellow();
  private:
    Render::Color ambient_;
    Render::Color diffuse_;
    Render::Color emission_;
    Render::Color specular_;
    GLfloat shininess_;
  };
}

#endif // RENDER_MATERIAL_H
