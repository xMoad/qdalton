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

#include "Render/RenderMaterial.h"

Render::Material::Material()
{
  ambient_   = defaultComponent(Render::Material::ComponentAmbient);
  diffuse_   = defaultComponent(Render::Material::ComponentDiffuse);
  emission_  = defaultComponent(Render::Material::ComponentEmission);
  specular_  = defaultComponent(Render::Material::ComponentSpecular);
  shininess_ = defaultShininess();
}

Render::Material::Material(const Render::Color& ambient,
                           const Render::Color& diffuse,
                           const Render::Color& emission,
                           const Render::Color& specular,
                           GLfloat shininess):
ambient_(ambient), diffuse_(diffuse), emission_(emission),
specular_(specular), shininess_(shininess)
{
}

Render::Material::Material(const Render::Color& ambient, bool optimal)
{
  ambient_ = ambient;
  if (optimal)
  {
    diffuse_   = optimalComponent(Render::Material::ComponentDiffuse);
    emission_  = optimalComponent(Render::Material::ComponentEmission);
    specular_  = optimalComponent(Render::Material::ComponentSpecular);
    shininess_ = optimalShininess();
  }
  else
  {
    diffuse_   = defaultComponent(Render::Material::ComponentDiffuse);
    emission_  = defaultComponent(Render::Material::ComponentEmission);
    specular_  = defaultComponent(Render::Material::ComponentSpecular);
    shininess_ = defaultShininess();
  }
}

Render::Material::Material(const Render::Material& material):
    ambient_(material.ambient_), diffuse_(material.diffuse_),
    emission_(material.emission_), specular_(material.specular_),
    shininess_(material.shininess_)
{
}

const Render::Color& Render::Material::ambient() const
{
  return ambient_;
}

void Render::Material::setAmbient(const Render::Color &ambient)
{
  ambient_ = ambient;
}

const Render::Color& Render::Material::diffuse() const
{
  return diffuse_;
}

const Render::Color& Render::Material::emission() const
{
  return emission_;
}

const Render::Color& Render::Material::specular() const
{
  return specular_;
}

GLfloat Render::Material::shininess() const
{
  return shininess_;
}

void Render::Material::prepare() const
{
  GLfloat ambient[4];
  ambient[0] = ambient_.r();
  ambient[1] = ambient_.g();
  ambient[2] = ambient_.b();
  ambient[3] = ambient_.a();
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

  GLfloat diffuse[4];
  diffuse[0] = diffuse_.r();
  diffuse[1] = diffuse_.g();
  diffuse[2] = diffuse_.b();
  diffuse[3] = diffuse_.a();
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

  GLfloat emission[4];
  emission[0] = emission_.r();
  emission[1] = emission_.g();
  emission[2] = emission_.b();
  emission[3] = emission_.a();
  glMaterialfv(GL_FRONT, GL_EMISSION, emission);

  GLfloat specular[4];
  specular[0] = specular_.r();
  specular[1] = specular_.g();
  specular[2] = specular_.b();
  specular[3] = specular_.a();
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

  glMaterialf(GL_FRONT, GL_SHININESS, shininess_);
}

Render::Color& Render::Material::defaultComponent(Component component)
{
  static Color* c;

  switch (component)
  {
  case Render::Material::ComponentAmbient:
    c = new Color(0.2f, 0.2f, 0.2f, 1.0f);
    break;
  case Render::Material::ComponentDiffuse:
    c = new Color(0.8f, 0.8f, 0.8f, 1.0f);
    break;
  case Render::Material::ComponentEmission:
    c = new Color(0.0f, 0.0f, 0.0f, 1.0f);
    break;
  case Render::Material::ComponentSpecular:
    c = new Color(0.0f, 0.0f, 0.0f, 1.0f);
    break;
  }

  return *c;
}

Render::Color& Render::Material::optimalComponent(Component component)
{
  static Color* c;

  switch (component)
  {
  case Render::Material::ComponentAmbient:
    c = new Color(0.2f, 0.2f, 0.2f, 1.0f);
    break;
  case Render::Material::ComponentDiffuse:
//    c = new Color(0.4f, 0.4f, 0.4f, 1.0f);
    c = new Color(0.2f, 0.2f, 0.2f, 1.0f);
    break;
  case Render::Material::ComponentEmission:
    c = new Color(0.0f, 0.0f, 0.0f, 1.0f);
    break;
  case Render::Material::ComponentSpecular:
    c = new Color(0.6f, 0.6f, 0.6f, 1.0f);
    break;
  }

  return *c;
}

GLfloat Render::Material::defaultShininess()
{
  return 0.0f;
}

GLfloat Render::Material::optimalShininess()
{
  return 32.0f;
}

const Render::Material& Render::Material::blue()
{
  static Material* m = new Material(Render::Color::blue(), false);
  return *m;
}

const Render::Material& Render::Material::green()
{
  static Material* m = new Material(Render::Color::green(), false);
  return *m;
}

const Render::Material& Render::Material::red()
{
  static Material* m = new Material(Render::Color::red(), false);
  return *m;
}

const Render::Material& Render::Material::yellow()
{
  static Material* m = new Material(Render::Color::yellow(), false);
  return *m;
}

const Render::Material& Render::Material::white()
{
  static Material* m = new Material(Render::Color::white(), false);
  return *m;
}

const Render::Material& Render::Material::selection()
{
  static Material* m = new Material(Render::Color::selection(), false);
  return *m;
}
