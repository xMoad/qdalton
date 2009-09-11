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

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderMolecule.h"
#include "Render/RenderCylinder.h"

const GLfloat Render::Bond::DEFAULT_THIKNESS = 0.15f;

Render::Bond::Bond(Render::Molecule* molecule,
                      quint16 beginIndex,
                      quint16 endIndex):
    isSelected_(false)
{
  molecule_ = molecule;
  beginIndex_ = beginIndex;
  endIndex_ = endIndex;
}

Render::Bond::Bond(const Render::Bond& bond) :
    molecule_(bond.molecule_),
    beginIndex_(bond.beginIndex_),
    endIndex_(bond.endIndex_)
{
}

const Render::Atom& Render::Bond::beginAtom() const
{
  return molecule_->atomAt(beginIndex_);
}

const Render::Atom& Render::Bond::endAtom() const
{
  return molecule_->atomAt(endIndex_);
}

quint16 Render::Bond::beginIndex() const
{
  return beginIndex_;
}

quint16 Render::Bond::endIndex() const
{
  return endIndex_;
}

void Render::Bond::draw(Quality quality) const
{
  Render::Atom atom1 = beginAtom();
  Render::Atom atom2 = endAtom();
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

  Render::Cylinder cylinder1(atom1.centre(), vMiddle, DEFAULT_THIKNESS, material1);
  Render::Cylinder cylinder2(vMiddle, atom2.centre(), DEFAULT_THIKNESS, material2);
  cylinder1.draw(STYLE_FILL, quality);
  cylinder2.draw(STYLE_FILL, quality);
}

void Render::Bond::drawSelection(Render::Quality quality) const
{
  Eigen::Vector3f point1;
  Eigen::Vector3f point2;
  // Enable blending
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
  Material material(Color::selection(),
                    Material::defaultComponent(Material::DIFFUSE),
                    Material::defaultComponent(Material::EMISSION),
                    Material::defaultComponent(Material::SPECULAR),
                    Material::defaultShininess());
  Eigen::Vector3f v = endAtom().centre() -
                      beginAtom().centre();
  if (beginAtom().isSelected())
  {
    point1 = -v * (v.norm() - beginAtom().drawRadius() + 0.05f) +
             endAtom().centre();
  }
  else
    point1 = beginAtom().centre();
  if (endAtom().isSelected())
    point2 = v * (v.norm() - endAtom().drawRadius() + 0.05f) +
             beginAtom().centre();
  else
    point2 = endAtom().centre();
  Cylinder cylinder(point1, point2, DEFAULT_THIKNESS + 0.1f, material);
  cylinder.draw(STYLE_FILL, quality);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
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
