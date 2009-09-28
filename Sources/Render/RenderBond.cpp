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

#include <openbabel/mol.h>

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderCylinder.h"

const GLfloat Render::Bond::BOND_THIKNESS = 0.05f;
const GLfloat Render::Bond::STICK_THIKNESS = 0.15f;

Render::Bond::Bond(OpenBabel::OBBond* obbond):
    obbond_(obbond),
    isSelected_(false)
{
}

void Render::Bond::draw(Render::Bond::DrawStyle drawStyle,
                        Render::Quality quality) const
{
  Render::Cylinder cylinder1;
  Render::Cylinder cylinder2;

  Render::Atom atom1(obbond_->GetParent()->GetAtom(obbond_->GetBeginAtomIdx()));
  Render::Atom atom2(obbond_->GetParent()->GetAtom(obbond_->GetEndAtomIdx()));
  Render::Material material1(atom1.color(), true);
  Render::Material material2(atom2.color(), true);

  // Compute the centre of bond
  Eigen::Vector3f vec1 = atom2.centre() - atom1.centre();
  vec1 = vec1 * (vec1.norm() - atom2.drawRadius()) / vec1.norm();
  vec1 = vec1 + atom1.centre();
  Eigen::Vector3f vec2 = atom1.centre() - atom2.centre();
  vec2 = vec2 * (vec2.norm() - atom1.drawRadius()) / vec2.norm();
  vec2 = vec2 + atom2.centre();
  Eigen::Vector3f vMiddle = (vec1 + vec2) / 2;

  cylinder1.setVertex1(atom1.centre());
  cylinder1.setVertex2(vMiddle);
  cylinder1.setMaterial(material1);

  cylinder2.setVertex1(vMiddle);
  cylinder2.setVertex2(atom2.centre());
  cylinder2.setMaterial(material2);

  if (drawStyle == Render::Bond::DRAW_STYLE_BOND)
  {
    cylinder1.setRadius(Render::Bond::BOND_THIKNESS);
    cylinder2.setRadius(Render::Bond::BOND_THIKNESS);

    cylinder1.drawMulti(Render::STYLE_FILL,
                        quality,
                        obbond_->GetBondOrder(),
                        0.1f,
                        Eigen::Vector3f(0.0f, 0.0f, 1.0f));
    cylinder2.drawMulti(Render::STYLE_FILL,
                        quality,
                        obbond_->GetBondOrder(),
                        0.1f,
                        Eigen::Vector3f(0.0f, 0.0f, 1.0f));
  }
  else
  {
    cylinder1.setRadius(Render::Bond::STICK_THIKNESS);
    cylinder2.setRadius(Render::Bond::STICK_THIKNESS);

    cylinder1.draw(Render::STYLE_FILL, quality);
    cylinder2.draw(Render::STYLE_FILL, quality);
  }
}

void Render::Bond::drawSelection(Render::Quality quality) const
{
  /*  Stereometry::Point* point1;
  Stereometry::Point* point2;
  // Enable blending
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
  Material material(Color::selection(),
                    Material::defaultComponent(Material::DIFFUSE),
                    Material::defaultComponent(Material::EMISSION),
                    Material::defaultComponent(Material::SPECULAR),
                    Material::defaultShininess());
  Stereometry::Vector v(atom1_->centre(), atom2_->centre());
  if (atom1_->isSelected())
    point1 = new Stereometry::Point(v.pointAt(atom1_->radius() + 0.05f));
  else
    point1 = new Stereometry::Point(v.origin());
  if (atom2_->isSelected())
    point2 = new Stereometry::Point(v.pointAt(v.modulus() - (atom2_->radius() + 0.05f)));
  else
    point2 = new Stereometry::Point(v.terminus());
  Cylinder cylinderRender(*point1, *point2, DEFAULT_THIKNESS + 0.1f, material);
  cylinderRender.draw(STYLE_FILL, quality);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);*/
}

bool Render::Bond::isSelected() const
{
  return isSelected_;
}

void Render::Bond::setSelected(bool selected)
{
  if (selected == true)
    isSelected_ = true;
  else
    isSelected_ = false;
}
