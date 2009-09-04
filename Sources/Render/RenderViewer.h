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

#ifndef RENDER_VIEWER_H
#define RENDER_VIEWER_H

#include <QMouseEvent>
#include <QGLViewer/qglviewer.h>

#include "Chemistry/ChemistryMolecule.h"
#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderQuality.h"

namespace Render
{
  /**
   * @class Render::Viewer RenderViewer.h "Render/RenderViewer.h"
   * @brief
   * @author Anton Simakov
   * @version 0.1
   */
  class Viewer : public QGLViewer
  {
    Q_OBJECT

  public:
    enum View
    {
      VIEW_BALLS_STICKS,
      VIEW_STICKS,
      VIEW_VDW
    };

    enum Mode
    {
      MODE_VIEW,
      MODE_ADD
    };

    Viewer(QWidget* parent);
    ~Viewer();

    /** Set Chemistry::Molecule instance to view.
     * @param chemistryMolecule is
     */
    void setMolecule(const Chemistry::Molecule& chemistryMolecule);

    void updateMolecule();
    void setView(View view);
    void setMode(Mode mode);
    void setAxes(bool visibility, GLfloat size);
    void setDebugInfoVisibility(bool visibility);
    void addAtom(const Render::Atom& atom);
    void addBond(const Render::Bond& bond);
    void setAtomicNumber(int atomicNumber);

  protected:
    virtual void draw();
    virtual void drawWithNames();
    virtual void fastDraw();
    virtual void postSelection(const QPoint& point);
    virtual void initializeGL();
    virtual QString helpString() const;

    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

  private:
    enum GLList
    {
      GLLIST_AXES,
      GLLIST_BALLS,
      GLLIST_STICKS,
      GLLIST_SELECTIONS
    };

    Chemistry::Molecule chemistryMolecule_;
    View view_;
    Mode mode_;

    // GLLists, Low & High Quality.
    GLuint axesLow_;
    GLuint axesHigh_;
    GLuint smallBallsLow_;
    GLuint smallBallsHigh_;
    GLuint mediumBallsLow_;
    GLuint mediumBallsHigh_;
    GLuint bigBallsLow_;
    GLuint bigBallsHigh_;
    GLuint sticksLow_;
    GLuint sticksHigh_;
    GLuint selectionsLow_;
    GLuint selectionsHigh_;

    void updateGLList(GLList gllist);
    void updateRenderAtoms();
    void updateRenderBonds();

    // GLLists generators.
    GLuint makeAxes(GLfloat size, Quality quality);
    GLuint makeSmallBalls(Quality quality);
    GLuint makeMediumBalls(Quality quality);
    GLuint makeBigBalls(Quality quality);
    GLuint makeSticks(Quality quality);
    GLuint makeSelections(Quality quality);

    // visibility flags
    bool isDebugInfoVisible_;
    bool isAxesVisible_;
    GLfloat axesSize_;

    QPoint lastMousePosition_;
    QList<Render::Atom> renderAtomsList_;
    QList<Render::Bond> renderBondsList_;

    int movableIndex;

    qglviewer::Vec selectedPoint;
    // added atom atomic number
    int atomicNumber_;
  };
}

#endif // RENDER_VIEWER_H
