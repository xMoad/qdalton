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

#ifndef RENDER_BOND_H
#define RENDER_BOND_H

#include <Eigen/Array>

#include "Render/RenderConstants.h"
#include "Render/RenderGL.h"

namespace OpenBabel
{
  class OBBond;
}

namespace Render
{
  class Bond
  {
  public:
    Bond(OpenBabel::OBBond* obBond);
    Bond(const Render::Bond& bond);
    ~Bond();

    OpenBabel::OBBond* obBond() const;

    void draw(Render::View view,
              const Eigen::Vector3f& planeNormalVector,
              bool isSelected,
              bool fast) const;

    quint8 bondOrder() const;
    void setBondOrder(quint8 bondOrder);
    void cycleOrder();

  private:
    OpenBabel::OBBond* obBond_;
  };
}

#endif // RENDER_BOND_H
