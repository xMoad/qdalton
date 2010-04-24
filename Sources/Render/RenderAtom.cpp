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

//#include <openbabel/mol.h>

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderMolecule.h"
#include "Render/RenderSphere.h"

Render::Atom::Atom(Render::Molecule* molecule,
                   quint8 atomicNumber,
                   quint8 isotope,
                   const Eigen::Vector3f& centre) :
    molecule_(molecule),
    atomicNumber_(atomicNumber),
    isotope_(isotope),
    centre_(centre),
    selected_(false)
{
}

Render::Atom::Atom(const Render::Atom& atom) :
    molecule_(atom.molecule_),
    atomicNumber_(atom.atomicNumber_),
    isotope_(atom.isotope_),
    centre_(atom.centre_),
    selected_(atom.selected_)
{
}

Render::Atom& Render::Atom::operator=(const Render::Atom& rhs)
                                     {
  if (this == &rhs)
    return *this;

  molecule_ = rhs.molecule_;
  atomicNumber_ = rhs.atomicNumber_;
  isotope_ = rhs.isotope_;
  centre_ = rhs.centre_;
  selected_ = rhs.selected_;

  return *this;
}

bool Render::Atom::operator==(const Render::Atom& atom) const
{
  return this == &atom;
}

bool Render::Atom::operator!=(const Render::Atom& atom) const
{
  return !(*this == atom);
}

quint16 Render::Atom::index() const
{
  return molecule_->indexOfAtom(*this);
}

float Render::Atom::exactMass() const
{
  return OpenBabel::isotab.GetExactMass(atomicNumber_, isotope_);
}

quint8 Render::Atom::atomicNumber() const
{
  return atomicNumber_;
}

void Render::Atom::setAtomicNumber(quint8 atomicNumber)
{
  atomicNumber_ = atomicNumber;
}

quint8 Render::Atom::isotope() const
{
  return isotope_;
}

void Render::Atom::setIsotope(quint8 isotope)
{
  isotope_ = isotope;
}

QString Render::Atom::symbol() const
{
  return QString(OpenBabel::etab.GetSymbol(atomicNumber()));
}

GLfloat Render::Atom::drawRadius() const
{
#ifdef Q_CC_MSVC
  return pow(exactMass(), 1.0f/3.0f) / 10.0f + 0.2f;
#else
  return cbrt(exactMass()) / 10.0f + 0.2f;
#endif
}

GLfloat Render::Atom::vanderwaalsRadius() const
{
  return (GLfloat) OpenBabel::etab.GetVdwRad(atomicNumber());
}

Render::Color Render::Atom::color() const
{
  std::vector<double> rgb =
      OpenBabel::etab.GetRGB(atomicNumber());
  return Color(rgb[0], rgb[1], rgb[2], 1.0f);
}

void Render::Atom::draw(Render::View view, bool fast) const
{
  Render::Sphere sphere;

  sphere.setCentre(centre());
  if (isSelected())
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

bool Render::Atom::isSelected() const
{
  return selected_;
}

void Render::Atom::setSelected(bool selected)
{
  selected_ = selected;
}

Eigen::Vector3f Render::Atom::centre() const
{
  return centre_;
}

void Render::Atom::setCentre(const Eigen::Vector3f& centre)
{
  centre_ = centre;
}

void Render::Atom::toggleSelection()
{
  selected_ = !selected_;
}
