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

#include <QtGlobal>

#ifdef Q_CC_MSVC
#define NOMINMAX
#endif

#ifndef RENDER_VIEWER_H
#define RENDER_VIEWER_H

#include <QMouseEvent>
#include <QTableWidget>
#include <QGLViewer/qglviewer.h>

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "File/FileMol.h"

namespace Render
{
  class Viewer : public QGLViewer
  {
    Q_OBJECT

  public:
    Viewer(QWidget* parent);
    ~Viewer();

    File::Mol& fileMol();
    void setFileMol(const File::Mol& fileMol);

    void setView(Render::Molecule::View view);
    void setAtomicNumber(quint8 atomicNumber);

    void setAxes(bool visibility, GLfloat size);
    void setDebugInfoVisibility(bool visibility);

    void displayConformer(quint16 index);

    bool isSomethingUnderPixel(const QPoint& pixel);

//  public slots:

  protected:
    virtual void init();
    virtual void draw();
    virtual void drawWithNames();

    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

  private:
    File::Mol fileMol_;
    Render::Molecule::View view_;
    // added atom atomic number
    quint8 atomicNumber_;
    // visibility flags
    bool isDebugInfoVisible_;
    bool isAxesVisible_;
    GLfloat axesSize_;

    qint32 atomSelectedBeforeIndex_;

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
    GLuint displayListAxes_;

    // GLLists generators.
    GLuint makeAxes(GLfloat size);
  };
}

#endif // RENDER_VIEWER_H
