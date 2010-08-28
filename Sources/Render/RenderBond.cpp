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

//#include <openbabel/mol.h>

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderCylinder.h"
#include "Render/RenderMolecule.h"

Render::Bond::Bond(Render::Molecule* molecule,
                   Render::Atom* beginAtom,
                   Render::Atom* endAtom,
                   quint8 bondOrder) :
molecule_(molecule),
beginAtom_(beginAtom),
endAtom_(endAtom),
bondOrder_(bondOrder),
selected_(false)
{
}

Render::Bond::Bond(const Render::Bond& bond) :
        molecule_(bond.molecule_),
        beginAtom_(bond.beginAtom_),
        endAtom_(bond.endAtom_),
        bondOrder_(bond.bondOrder_),
        selected_(bond.selected_)
{
}

Render::Bond& Render::Bond::operator=(const Render::Bond& rhs)
                                     {
    if (this == &rhs)
        return *this;

    molecule_ = rhs.molecule_;
    beginAtom_ = rhs.beginAtom_;
    endAtom_ = rhs.endAtom_;
    bondOrder_ = rhs.bondOrder_;
    selected_ = rhs.selected_;

    return *this;
}

bool Render::Bond::operator==(const Render::Bond& bond) const
{
    return this == &bond;
}

bool Render::Bond::operator!=(const Render::Bond& bond) const
{
    return !(*this == bond);
}

quint16 Render::Bond::index() const
{
    return molecule_->indexOfBond(*this);
}

const Render::Atom& Render::Bond::beginAtom() const
{
    return *beginAtom_;
}

const Render::Atom& Render::Bond::endAtom() const
{
    return *endAtom_;
}

void Render::Bond::draw(Render::View view,
                        const Eigen::Vector3f& planeNormalVector,
                        bool fast) const
{
    float shift;

    if (bondOrder() < 3)
        shift = 0.125f;
    else
        shift = 0.2f;

    Render::Cylinder cylinder1;
    Render::Cylinder cylinder2;

    // Compute the centre of bond
    Eigen::Vector3f vec1 = endAtom().centre() - beginAtom().centre();
    vec1 = vec1 * (vec1.norm() - endAtom().drawRadius()) / vec1.norm();
    vec1 = vec1 + beginAtom().centre();
    Eigen::Vector3f vec2 = beginAtom().centre() - endAtom().centre();
    vec2 = vec2 * (vec2.norm() - beginAtom().drawRadius()) / vec2.norm();
    vec2 = vec2 + endAtom().centre();
    Eigen::Vector3f vMiddle = (vec1 + vec2) / 2;

    cylinder1.setVertex1(beginAtom().centre());
    cylinder1.setVertex2(vMiddle);

    cylinder2.setVertex1(vMiddle);
    cylinder2.setVertex2(endAtom().centre());

    if (isSelected())
    {
        cylinder1.setMaterial(Render::Material::selection());
        cylinder2.setMaterial(Render::Material::selection());
    }
    else
    {
        cylinder1.setMaterial(Render::Material(beginAtom().color(), true));
        cylinder2.setMaterial(Render::Material(endAtom().color(), true));
    }

    switch (view)
    {
    case Render::ViewBallsAndBonds:
        cylinder1.setRadius(Render::bondThikness);
        cylinder2.setRadius(Render::bondThikness);
        if (fast)
        {
            cylinder1.drawMulti(Render::StyleFill,
                                bondOrder(),
                                shift,
                                planeNormalVector,
                                Render::slicesForFastDrawing);
            cylinder2.drawMulti(Render::StyleFill,
                                bondOrder(),
                                shift,
                                planeNormalVector,
                                Render::slicesForFastDrawing);
        }
        else
        {
            cylinder1.drawMulti(Render::StyleFill,
                                bondOrder(),
                                shift,
                                planeNormalVector,
                                Render::slicesForDrawing);
            cylinder2.drawMulti(Render::StyleFill,
                                bondOrder(),
                                shift,
                                planeNormalVector,
                                Render::slicesForDrawing);
        }
        break;
  case Render::ViewBallsAndSticks:
  case Render::ViewSticks:
        cylinder1.setRadius(Render::stickThikness);
        cylinder2.setRadius(Render::stickThikness);
        if (fast)
        {
            cylinder1.draw(Render::StyleFill, Render::slicesForFastDrawing);
            cylinder2.draw(Render::StyleFill, Render::slicesForFastDrawing);
        }
        else
        {
            cylinder1.draw(Render::StyleFill, Render::slicesForDrawing);
            cylinder2.draw(Render::StyleFill, Render::slicesForDrawing);
        }
        break;
  case Render::ViewVdWSpheres:
        break;
    }
}

quint8 Render::Bond::bondOrder() const
{
    return bondOrder_;
}

void Render::Bond::setBondOrder(quint8 bondOrder)
{
    bondOrder_ = bondOrder;
}

void Render::Bond::cycleBondOrder()
{
    quint8 order = bondOrder();

    if (order == 3)
        setBondOrder(1);
    else
        setBondOrder(order + 1);
}

float Render::Bond::length() const
{
  return (endAtom_->centre() - beginAtom_->centre()).norm();
}

Eigen::Vector3f Render::Bond::centre() const
{
  return (endAtom_->centre() + beginAtom_->centre()) / 2;
}

bool Render::Bond::isSelected() const
{
    return selected_;
}

void Render::Bond::toggleSelection()
{
    selected_ = !selected_;
}
