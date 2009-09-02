/** Copyright (C) 2009 Anton Simakov
 *
 */

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
   * @class Atom Atom.h "Render/Atom.h"
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
    Atom(const Render::Atom& renderAtom);
    Atom(const Chemistry::Atom& chemistryAtom);
    // Returns
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
    static const short vanderwaalsRadii_[];
    static const int colors[];

    Chemistry::Atom chemistryAtom_;

    bool isSelected_;
    bool isMovable_;
    QString label_;
  };
}

#endif // RENDER_ATOM_H
