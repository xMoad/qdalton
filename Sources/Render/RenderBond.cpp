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

Render::Bond::Bond(const Render::Bond& bond) :
    obBond_(bond.obBond_),
    isSelected_(bond.isSelected_)
{
}

Render::Bond::Bond(OpenBabel::OBBond* obBond) :
    obBond_(obBond),
    isSelected_(false)
{
}

OpenBabel::OBBond* Render::Bond::obBond() const
{
  return obBond_;
}

void Render::Bond::draw(Render::Bond::DrawStyle drawStyle,
                        const Eigen::Vector3f& planeNormalVector,
                        Render::Quality quality) const
{
  float shift;
  Render::Cylinder cylinder1;
  Render::Cylinder cylinder2;

  Render::Atom atom1(obBond_->GetParent()->GetAtom(obBond_->GetBeginAtomIdx()));
  Render::Atom atom2(obBond_->GetParent()->GetAtom(obBond_->GetEndAtomIdx()));
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

  if (drawStyle == Render::Bond::DrawStyleBond)
  {
    cylinder1.setRadius(Render::Bond::BOND_THIKNESS);
    cylinder2.setRadius(Render::Bond::BOND_THIKNESS);

    if (obBond_->GetBondOrder() < 3)
    {
      shift = 0.1f;
    }
    else
    {
      shift = 0.2f;
    }

    cylinder1.drawMulti(Render::StyleFill,
                        quality,
                        obBond_->GetBondOrder(),
                        shift,
                        planeNormalVector);
    cylinder2.drawMulti(Render::StyleFill,
                        quality,
                        obBond_->GetBondOrder(),
                        shift,
                        planeNormalVector);
  }
  else
  {
    cylinder1.setRadius(Render::Bond::STICK_THIKNESS);
    cylinder2.setRadius(Render::Bond::STICK_THIKNESS);

    cylinder1.draw(Render::StyleFill, quality);
    cylinder2.draw(Render::StyleFill, quality);
  }
}

void Render::Bond::drawSelection(Render::Quality quality) const
{
  OpenBabel::OBAtom* obAtomPtr;
  Render::Cylinder cylinder;
  Render::Material material(Color::selection(), true);

  obAtomPtr = obBond_->GetBeginAtom();
  cylinder.setVertex1(Eigen::Vector3f(obAtomPtr->GetX(),
                                      obAtomPtr->GetY(),
                                      obAtomPtr->GetZ()));
  obAtomPtr = obBond_->GetEndAtom();
  cylinder.setVertex2(Eigen::Vector3f(obAtomPtr->GetX(),
                                      obAtomPtr->GetY(),
                                      obAtomPtr->GetZ()));

  cylinder.setRadius(0.2f);
  cylinder.setMaterial(material);
  // Enable blending
  glEnable(GL_BLEND);
  //  glDisable(GL_DEPTH_TEST);
  cylinder.draw(Render::StyleFill, quality);
  glDisable(GL_BLEND);
  //  glEnable(GL_DEPTH_TEST);
}

bool Render::Bond::isSelected() const
{
  return isSelected_;
}

void Render::Bond::setSelected(bool selected)
{
  isSelected_ = selected;
}

void Render::Bond::toggleSelected()
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

void Render::Bond::cycleOrder()
{
  quint8 order = obBond_->GetBondOrder();
  if (order == 3)
  {
    obBond_->SetBondOrder(1);
  }
  else
  {
    obBond_->SetBondOrder(order + 1);
  }
}
