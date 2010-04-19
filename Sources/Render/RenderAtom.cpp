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

Render::Atom::Atom(const Render::Atom& atom) :
    obAtom_(atom.obAtom_)
{
}

Render::Atom::Atom(OpenBabel::OBAtom* obAtom) :
    obAtom_(obAtom)
{
}

Render::Atom::~Atom()
{
}

qreal Render::Atom::exactMass() const
{
  return obAtom_->GetExactMass();
}

OpenBabel::OBAtom* Render::Atom::obAtom() const
{
  return obAtom_;
}

quint8 Render::Atom::atomicNumber() const
{
  return obAtom_->GetAtomicNum();
}

void Render::Atom::setAtomicNumber(quint8 atomicNumber)
{
  obAtom_->SetAtomicNum(atomicNumber);
}

quint8 Render::Atom::isotope() const
{
  return obAtom_->GetIsotope();
}

void Render::Atom::setIsotope(quint8 isotope)
{
  obAtom_->SetIsotope(isotope);
}

QString Render::Atom::symbol() const
{
  return QString(OpenBabel::etab.GetSymbol(atomicNumber()));
}

GLfloat Render::Atom::drawRadius() const
{
#ifdef Q_CC_MSVC
  return pow(exactMass(), 1.0/3.0) / 10.0f + 0.2f;
#else
  return cbrt(exactMass()) / 10.0f + 0.2f;
#endif
}

GLfloat Render::Atom::vanderwaalsRadius() const
{
  return (GLfloat) OpenBabel::etab.GetVdwRad(obAtom_->GetAtomicNum());
}

Render::Color Render::Atom::color() const
{
  std::vector<double> rgb =
      OpenBabel::etab.GetRGB(atomicNumber());
  return Color(rgb[0], rgb[1], rgb[2], 1.0f);
}

void Render::Atom::draw(Render::View view, bool isSelected, bool fast) const
{
  Render::Sphere sphere;

  Eigen::Vector3f centre(obAtom_->GetX(), obAtom_->GetY(), obAtom_->GetZ());

  sphere.setCentre(centre);
  if (isSelected)
    sphere.setMaterial(Render::Material::selection());
  else
    sphere.setMaterial(Render::Material(color(), true));

  switch (view)
  {
  case Render::ViewBallsAndSticks:
  case Render::ViewBallsAndBonds:
    sphere.setRadius(drawRadius());
    break;
  case Render::ViewSticks:
    sphere.setRadius(Render::stickThikness);
    break;
  case Render::ViewVdWSpheres:
    sphere.setRadius(vanderwaalsRadius());
    break;
  }

  if (fast)
    sphere.draw(Render::StyleFill, Render::slicesForFastDrawing);
  else
    sphere.draw(Render::StyleFill, Render::slicesForDrawing);
}

Eigen::Vector3f Render::Atom::centre() const
{
  return Eigen::Vector3f(obAtom_->GetX(), obAtom_->GetY(), obAtom_->GetZ());
}

void Render::Atom::setCentre(const Eigen::Vector3f& centre)
{
  obAtom_->SetVector(OpenBabel::vector3(centre.x(), centre.y(), centre.z()));
}
