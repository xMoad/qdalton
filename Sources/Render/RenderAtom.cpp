/**********************************************************************
  Copyright (C) 2008, 2009 Anton Simakov

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

#include <QDebug>
#include <cmath>

#include <openbabel/mol.h>

#include "Render/RenderAtom.h"
#include "Render/RenderArrow.h"
#include "Render/RenderBond.h"
#include "Render/RenderSphere.h"

const GLfloat Render::Atom::SELECTON_RADIUS = 0.15f;

Render::Atom::Atom(const Render::Atom& atom) :
    obatom_(atom.obatom_),
    isSelected_(atom.isSelected_)
{
}

Render::Atom::Atom(OpenBabel::OBAtom* obatom) :
    obatom_(obatom),
    isSelected_(false)
{
}

GLfloat Render::Atom::drawRadius() const
{
#ifdef Q_CC_MSVC
  return pow(obatom_->GetExactMass(), 1.0/3.0) / 10.0f + 0.2f;
#else
  return cbrt(obatom_->GetExactMass()) / 10.0f + 0.2f;
#endif
}

GLfloat Render::Atom::vanderwaalsRadius() const
{
  return (GLfloat) OpenBabel::etab.GetVdwRad(obatom_->GetAtomicNum());
}

Render::Color Render::Atom::color() const
{
  std::vector<double> rgb = OpenBabel::etab.GetRGB(obatom_->GetAtomicNum());
  return Color(rgb[0], rgb[1], rgb[2], 1.0f);
}

void Render::Atom::draw(Render::Atom::DrawStyle style,
                        Render::Quality quality) const
{
  Render::Sphere sphere;
  Render::Material material(color(), true);
  Eigen::Vector3f centre(obatom_->GetX(), obatom_->GetY(), obatom_->GetZ());

  sphere.setCentre(centre);

  switch (style)
  {
  case Render::Atom::DRAW_STYLE_ATOM:
    sphere.setRadius(drawRadius());
    break;
  case Render::Atom::DRAW_STYLE_CONNECTOR:
    sphere.setRadius(Render::Bond::STICK_THIKNESS);
    break;
  case Render::Atom::DRAW_STYLE_VDW:
    sphere.setRadius(vanderwaalsRadius());
    break;
  }

  sphere.setMaterial(material);

  sphere.draw(STYLE_FILL, quality);
}

void Render::Atom::drawSelection(Atom::DrawStyle style, Quality quality) const
{
  Render::Sphere sphere;
  Render::Material material(Color::selection(), true);
  Eigen::Vector3f centre(obatom_->GetX(), obatom_->GetY(), obatom_->GetZ());

  sphere.setCentre(centre);

  switch (style)
  {
  case Render::Atom::DRAW_STYLE_ATOM:
    sphere.setRadius(drawRadius() + SELECTON_RADIUS);
    break;
  case Render::Atom::DRAW_STYLE_CONNECTOR:
    sphere.setRadius(Render::Bond::STICK_THIKNESS + SELECTON_RADIUS);
    break;
  case Render::Atom::DRAW_STYLE_VDW:
    sphere.setRadius(vanderwaalsRadius() + SELECTON_RADIUS * 2);
    break;
  }

  sphere.setMaterial(material);

  // Enable blending
  glEnable(GL_BLEND);
  //  glDisable(GL_DEPTH_TEST);
  sphere.draw(STYLE_FILL, quality);
  glDisable(GL_BLEND);
  //  glEnable(GL_DEPTH_TEST);
}

Eigen::Vector3f Render::Atom::centre() const
{
  return Eigen::Vector3f(obatom_->GetX(), obatom_->GetY(), obatom_->GetZ());;
}

void Render::Atom::setCentre(const Eigen::Vector3f& point)
{
  obatom_->SetVector(OpenBabel::vector3(point.x(), point.y(), point.z()));
}

bool Render::Atom::isSelected() const
{
  return isSelected_;
}

void Render::Atom::setSelected(bool selected)
{
  isSelected_ = selected;
}

void Render::Atom::toggleSelected()
{
  if (isSelected())
  {
    setSelected(false);
  }
  else
  {
    setSelected(true);
  }
}

OpenBabel::OBAtom* Render::Atom::obatom() const
{
  return obatom_;
}
