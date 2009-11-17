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

#include <QtGlobal>

#ifdef Q_CC_MSVC
#define NOMINMAX
#endif

#include <QMouseEvent>
#include <QTableWidget>
#include <QGLViewer/qglviewer.h>

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderQuality.h"
#include "Chemistry/ChemistryMolecule.h"

namespace Render
{
  class Viewer : public QGLViewer
  {
    Q_OBJECT

  public:
    enum View
    {
      ViewBallsAndBonds,
      ViewBallsAndSticks,
      ViewSticks,
      ViewVdWSpheres
    };

    Viewer(QWidget* parent);
    ~Viewer();

    void setMolecule(Chemistry::Molecule* molecule);

    void setView(View view);
    void setAtomicNumber(quint8 atomicNumber);

    void setAxes(bool visibility, GLfloat size);
    void setDebugInfoVisibility(bool visibility);
    void addAtom(const Render::Atom& atom);
    void addBond(const Render::Bond& bond);

    void conformationalSearch(QTableWidget* targetTableWidget);
    void displayConformer(quint16 index);

    bool isSomethingUnderPixel(const QPoint& pixel);

  public slots:
    void build();
    void addHydrogensAndBuild();
    void removeHydrogens();
    void updateMolecule();

  protected:
    virtual void init();
    virtual void draw();
    virtual void drawWithNames();
    virtual void fastDraw();
    virtual QString helpString() const;

    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

  private:
    enum GLList
    {
      GLLIST_AXES,
      GLLIST_ATOMS,
      GLLIST_BONDS,
      GLLIST_SELECTIONS
    };

    Chemistry::Molecule* molecule_;
    View view_;
    // added atom atomic number
    quint8 atomicNumber_;
    // visibility flags
    bool isDebugInfoVisible_;
    bool isAxesVisible_;
    GLfloat axesSize_;

    OpenBabel::OBAtom* obAtomSelectedBefore_;
    OpenBabel::OBAtom* obAtomNew_;

    QList<Render::Atom> atomsList_;
    QList<Render::Bond> bondsList_;

    void updateGLList(GLList gllist);
    void updateRenderAtoms();
    void updateRenderBonds();

    void mouseLeftButtonPressEvent(QMouseEvent* e);
    void mouseLeftButtonWithNoModifierPressEvent(QMouseEvent* e);
    void mouseLeftButtonWithShiftPressEvent(QMouseEvent* e);
    void mouseLeftButtonWithCtrlPressEvent(QMouseEvent* e);

    void mouseRightButtonPressEvent(QMouseEvent* e);
    void mouseRightButtonWithNoModifierPressEvent(QMouseEvent* e);
    void mouseRightButtonWithCtrlPressEvent(QMouseEvent* e);

    void mouseLeftButtonReleaseEvent(QMouseEvent* e);
    void mouseLeftButtonWithCtrlReleaseEvent(QMouseEvent* e);

    // GLLists, Low & High Quality.
    GLuint axesLow_;
    GLuint axesHigh_;

    GLuint smallBallsLow_;
    GLuint smallBallsHigh_;
    GLuint mediumBallsLow_;
    GLuint mediumBallsHigh_;
    GLuint bigBallsLow_;
    GLuint bigBallsHigh_;

    GLuint bondsLow_;
    GLuint bondsHigh_;

    GLuint sticksLow_;
    GLuint sticksHigh_;

    GLuint selectionsLow_;
    GLuint selectionsHigh_;
    // GLLists generators.
    GLuint makeAxes(GLfloat size, Quality quality);
    GLuint makeSmallBalls(Quality quality);
    GLuint makeMediumBalls(Quality quality);
    GLuint makeBigBalls(Quality quality);
    GLuint makeBonds(Quality quality);
    GLuint makeSticks(Quality quality);
    GLuint makeSelections(Quality quality);
  };
}

#endif // RENDER_VIEWER_H
