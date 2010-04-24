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

#ifndef RENDER_ATOM_H
#define RENDER_ATOM_H

#include <QString>
#include <Eigen/Array>

#include "Render/RenderGL.h"
#include "Render/RenderConstants.h"

namespace Render
{
  class Molecule;
  class Color;
  /**
   * @class Atom Atom.h "Render/RenderAtom.h"
   * @brief Represents and draw atom.
   * @author Anton Simakov
   * @version 0.1
   */
  class Atom
  {
  public:
    Atom(Render::Molecule* molecule,
         quint8 atomicNumber,
         quint8 isotope,
         const Eigen::Vector3f& centre);
    Atom(const Render::Atom& atom);
    Render::Atom& operator=(const Render::Atom& rhs);

    bool operator==(const Render::Atom& atom) const;
    bool operator!=(const Render::Atom& atom) const;

    quint16 index() const;

    float exactMass() const;

    quint8 atomicNumber() const;
    void setAtomicNumber(quint8 atomicNumber);

    quint8 isotope() const;
    void setIsotope(quint8 isotope);

    QString symbol() const;

    GLfloat drawRadius() const;
    GLfloat vanderwaalsRadius() const;

    Render::Color color() const;

    Eigen::Vector3f centre() const;
    void setCentre(const Eigen::Vector3f& centre);

    bool isSelected() const;
    void setSelected(bool selected);
    void toggleSelection();

    void draw(Render::View view,
              bool fast) const;

  private:
    Render::Molecule* molecule_;
    quint8 atomicNumber_;
    quint8 isotope_;
    Eigen::Vector3f centre_;
    bool selected_;
  };
}

#endif // RENDER_ATOM_H
