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

#ifndef RENDER_ATOM_H
#define RENDER_ATOM_H

#include <QString>
#include <Eigen/Array>

#include "Chemistry/ChemistryAtom.h"
#include "Render/RenderQuality.h"
#include "Render/RenderGL.h"

namespace Render
{
  class Color;
  /**
   * @class Render::Atom RenderAtom.h "Render/RenderAtom.h"
   * @brief Represents and draw atom.
   * @author Anton Simakov
   * @version 0.1
   */
  class Atom
  {
  public:
    enum DrawStyle
    {
      AS_ATOM,
      AS_CONNECTOR,
      AS_VDW
    };

    /** Constructor.
     * @param chemistryAtom is a reference to Chemistry::Atom class instance.
     * This is one and only one way to create Render::Atom object - from Chemistry::Atom instance that already exists.
     */
    Atom(Chemistry::Atom* chemistryAtom);

    /** Copy constructor.
     * @param renderAtom is a reference to Render::Atom class instance to copy from.
     */
    Atom(const Render::Atom& renderAtom);

    /**
     * @return Draw radius for AS_ATOM DrawStyle.
     */
    GLfloat drawRadius() const;

    GLfloat vanderwaalsRadius() const;
    Render::Color color() const;
    const Eigen::Vector3f& centre() const;
    void setCentre(const Eigen::Vector3f& centre);
    const QString& label() const;
    void draw(DrawStyle style, Render::Quality quality) const;
    void drawSelection(DrawStyle style, Render::Quality quality) const;
    bool isSelected() const;
    void setSelected(bool selected);
    bool isMovable() const;
    void setMovable(bool movable);
    Atom& operator= (const Atom& operand);
    static const GLfloat SELECTON_RADIUS;
  private:
    static const uint16_t vanderwaalsRadii_[];
    static const uint32_t colors[];

    Chemistry::Atom* chemistryAtom_;

    bool isSelected_;
    bool isMovable_;
    QString label_;
  };
}

#endif // RENDER_ATOM_H
