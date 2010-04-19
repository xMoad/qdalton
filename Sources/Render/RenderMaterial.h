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
      ComponentAmbient,
      ComponentDiffuse,
      ComponentEmission,
      ComponentSpecular
    };
    Material();
    Material(const Color& ambient,
             const Color& diffuse,
             const Color& emission,
             const Color& specular,
             GLfloat shininess);
    Material(const Color& ambient, bool optimal);
    Material(const Material& material);

    const Render::Color& ambient() const;
    void setAmbient(const Render::Color& ambient);

    const Render::Color& diffuse() const;
    const Render::Color& emission() const;
    const Render::Color& specular() const;
    GLfloat shininess() const;

    void prepare() const;
    // OpenGL default material properties of the objects
    static Render::Color& defaultComponent(Component component);
    static Render::Color& optimalComponent(Component component);
    static GLfloat optimalShininess();
    static GLfloat defaultShininess();

    static const Render::Material& blue();
    static const Render::Material& green();
    static const Render::Material& red();
    static const Render::Material& yellow();
    static const Render::Material& white();
    static const Render::Material& selection();

  private:
    Render::Color ambient_;
    Render::Color diffuse_;
    Render::Color emission_;
    Render::Color specular_;
    GLfloat shininess_;
  };
}

#endif // RENDER_MATERIAL_H
