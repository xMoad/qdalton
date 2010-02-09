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

#include <QDebug>
#include <cmath>

#include <openbabel/mol.h>

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderSphere.h"

const GLfloat Render::Atom::SELECTON_RADIUS = 0.15f;

Render::Atom::Atom(const Render::Atom& atom) :
    obAtom_(atom.obAtom_),
    isSelected_(atom.isSelected_),
    displayListAtom_(0),
    displayListConnector_(0),
    displayListVdW_(0)
{
}

Render::Atom::Atom(OpenBabel::OBAtom* obAtom) :
    obAtom_(obAtom),
    isSelected_(false),
    displayListAtom_(0),
    displayListConnector_(0),
    displayListVdW_(0)
{
}

Render::Atom::~Atom()
{
  deleteDisplayLists();
}

quint8 Render::Atom::atomicNumber() const
{
  return obAtom_->GetAtomicNum();
}

void Render::Atom::setAtomicNumber(quint8 atomicNumber)
{
  obAtom_->SetAtomicNum(atomicNumber);
  createDisplayLists();
}

GLfloat Render::Atom::drawRadius() const
{
#ifdef Q_CC_MSVC
  return pow(obAtom_->GetExactMass(), 1.0/3.0) / 10.0f + 0.2f;
#else
  return cbrt(obAtom_->GetExactMass()) / 10.0f + 0.2f;
#endif
}

GLfloat Render::Atom::vanderwaalsRadius() const
{
  return (GLfloat) OpenBabel::etab.GetVdwRad(obAtom_->GetAtomicNum());
}

Render::Color Render::Atom::color() const
{
  std::vector<double> rgb = OpenBabel::etab.GetRGB(obAtom_->GetAtomicNum());
  return Color(rgb[0], rgb[1], rgb[2], 1.0f);
}

void Render::Atom::draw(Render::Atom::DrawStyle style)
{
  if (displayListAtom_ == 0)
    createDisplayLists();
  switch (style)
  {
  case Render::Atom::DrawStyleAtom:
    glCallList(displayListAtom_);
    break;
  case Render::Atom::DrawStyleConnector:
    glCallList(displayListConnector_);
    break;
  case Render::Atom::DrawStyleVdW:
    glCallList(displayListVdW_);
    break;
  }
}

Eigen::Vector3f Render::Atom::centre() const
{
  return Eigen::Vector3f(obAtom_->GetX(), obAtom_->GetY(), obAtom_->GetZ());;
}

void Render::Atom::setCentre(const Eigen::Vector3f& point)
{
  obAtom_->SetVector(OpenBabel::vector3(point.x(), point.y(), point.z()));
}

bool Render::Atom::isSelected() const
{
  return isSelected_;
}

void Render::Atom::setSelected(bool selected)
{
  isSelected_ = selected;
  createDisplayLists();
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

OpenBabel::OBAtom* Render::Atom::obAtom() const
{
  return obAtom_;
}

void Render::Atom::createDisplayLists()
{
  deleteDisplayLists();

  Render::Sphere sphere;
  Render::Material material(color(), true);
  if (isSelected_)
    material.setAmbient(Render::Color::selection());
  Eigen::Vector3f centre(obAtom_->GetX(), obAtom_->GetY(), obAtom_->GetZ());

  sphere.setCentre(centre);
  sphere.setMaterial(material);
  sphere.setRadius(drawRadius());

  displayListAtom_ = glGenLists(1);
  glNewList(displayListAtom_, GL_COMPILE);
  {
    sphere.draw(Render::StyleFill);
  }
  glEndList();

  sphere.setRadius(Render::Bond::STICK_THIKNESS);
  displayListConnector_ = glGenLists(1);
  glNewList(displayListConnector_, GL_COMPILE);
  {
    sphere.draw(Render::StyleFill);
  }
  glEndList();

  sphere.setRadius(vanderwaalsRadius());
  displayListVdW_ = glGenLists(1);
  glNewList(displayListVdW_, GL_COMPILE);
  {
    sphere.draw(Render::StyleFill);
  }
  glEndList();
}

void Render::Atom::deleteDisplayLists()
{
  glDeleteLists(displayListAtom_, 1);
  glDeleteLists(displayListConnector_, 1);
  glDeleteLists(displayListVdW_, 1);
}
