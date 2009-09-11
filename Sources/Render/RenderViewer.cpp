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

#include <cmath>
#include <QMessageBox>
#include <QMouseEvent>

#include "Render/RenderViewer.h"
#include "Render/RenderSphere.h"
#include "Render/RenderCylinder.h"
#include "Render/RenderMaterial.h"
#include "Render/RenderArrow.h"

Render::Viewer::Viewer(QWidget* parent) :
    QGLViewer(parent)
{
  view_ = VIEW_BALLS_STICKS;
  mode_ = MODE_VIEW;
  movableIndex = -1;
  setMouseBinding(Qt::CTRL + Qt::LeftButton, FRAME, TRANSLATE);
  isAxesVisible_ = true;
  isDebugInfoVisible_ = false;
  axesSize_ = 1.0f;
  atomicNumber_ = 6;
}

Render::Viewer::~Viewer()
{
  //  makeCurrent();
  glDeleteLists(mediumBallsLow_, 1);
  glDeleteLists(mediumBallsHigh_, 1);
  glDeleteLists(sticksLow_, 1);
  glDeleteLists(sticksHigh_, 1);
  glDeleteLists(smallBallsLow_, 1);
  glDeleteLists(smallBallsHigh_, 1);
}

void Render::Viewer::init()
{
  float lightAmbient[]      = { 1.0f, 1.0f, 1.0f, 1.0f };
  float lightDiffuse[]      = { 1.0f, 1.0f, 1.0f, 1.0f };
  float lightSpecular[]     = { 1.0f, 1.0f, 1.0f, 1.0f };
  float lightModelAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  float fogColor[]          = { 0.0f, 0.0f, 0.0f, 1.0f };

  // This method is called from QGLViewer::initializeGL(),
  // where GL_COLOR_MATERIAL is enabled. We don't need it!
  glDisable(GL_COLOR_MATERIAL);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // Setup shading (filling) mode to smooth
  glShadeModel(GL_SMOOTH);
  // Enable the depth buffer updating
  glEnable(GL_DEPTH_TEST);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0f);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  setSceneRadius(10.0f);
  showEntireScene();

  //    glEnable(GL_FOG);
  //    glFogi(GL_FOG_MODE, GL_LINEAR);
  //    glFogfv(GL_FOG_COLOR, fogColor);
  //    glFogf(GL_FOG_DENSITY, 0.0001f);
  //    glHint(GL_FOG_HINT, GL_NICEST);
  //    glFogf(GL_FOG_START, -sceneRadius());
  //    glFogf(GL_FOG_END, sceneRadius());

  glEnable(GL_NORMALIZE);

  updateGLList(Viewer::GLLIST_AXES);
  setManipulatedFrame(new qglviewer::ManipulatedFrame());
}

void Render::Viewer::draw()
{
  // Place light at camera position
  const qglviewer::Vec cameraPos = camera()->position();
  const GLfloat lightPosition[4] = {cameraPos[0], cameraPos[1], cameraPos[2], 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

  if (isAxesVisible_)
  {
    glCallList(axesHigh_);
  }
  glCallList(sticksHigh_);
  switch (view_)
  {
  case VIEW_BALLS_STICKS:
    glCallList(mediumBallsHigh_);
    break;
  case VIEW_STICKS:
    glCallList(smallBallsHigh_);
    break;
  case VIEW_VDW:
    glCallList(bigBallsHigh_);
    break;
  }
  glCallList(selectionsHigh_);

  if (isDebugInfoVisible_)
  {
    // Save current color
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    glColor3f(0.0f, 1.0f, 0.0f);

    QFont font;

    font.setBold(true);
    renderText(10, 15, "Debug Info", font);
    font.setBold(false);
    switch (mode_)
    {
    case MODE_VIEW:
      renderText(20, 30, "Mode: View", font);
      renderText(20, 45, QString("Molecule: %1").arg("Foo"), font);
      renderText(20, 60, QString("Atoms: %1").arg(molecule_.atomsCount()), font);
      renderText(20, 75, QString("Bonds: %1").arg(molecule_.bondsCount()), font);
      switch (molecule_.unitOfLength())
      {
      case Render::Molecule::ANGSTROM:
        renderText(20, 90, QString("Unit of length: angstrom"), font);
        break;
      case Render::Molecule::BOHR:
        renderText(20, 90, QString("Unit of length: a.u."), font);
        break;
      }

      break;
    case MODE_ADD:
      renderText(20, 30, "Mode: Add", font);
      renderText(20, 45,
                 QString("(%1, %2)").arg(lastMousePosition_.x()).arg(lastMousePosition_.y()),
                 font);
      //      qglviewer::Vec screen(lastMousePosition_.x(), lastMousePosition_.y(), 0.5f);
      bool found;
      qglviewer::Vec world = camera()->pointUnderPixel(lastMousePosition_, found);
      if (found)
        renderText(20, 60, QString("Something found! @(%1, %2, %3)").arg(world.x).arg(world.y).arg(world.z), font);
      else
        renderText(20, 60, QString("(%1, %2, %3)").arg(world.x).arg(world.y).arg(world.z), font);
      break;
    }
    glColor4fv(color);
  }

  /* buggy code! =)
    glGetFloatv(GL_CURRENT_COLOR, color);
    glColor3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < atomsQList_.size(); i++)
    {
      renderText(atomsQList_[i].centre().x(),
                 atomsQList_[i].centre().y(),
                 atomsQList_[i].centre().z(),
                 atomsQList_[i].label(),
                 QFont("Arial"));
    }
    glColor4fv(color);
*/
  if (movableIndex != -1)
  {
    glPushMatrix();
    {
      // Multiply matrix to get in the frame coordinate system.
      glMultMatrixd(manipulatedFrame()->matrix());
      //      if (showAtoms_)
//      molecule_.atoms_[movableIndex].draw(Atom::AS_ATOM, QUALITY_HIGH);
    }
    glPopMatrix();
  }
}

void Render::Viewer::fastDraw()
{
  if (isAxesVisible_)
  {
    glCallList(axesLow_);
  }
  glCallList(sticksLow_);
  switch (view_)
  {
  case VIEW_BALLS_STICKS:
    glCallList(mediumBallsLow_);
    break;
  case VIEW_STICKS:
    glCallList(smallBallsLow_);
    break;
  case VIEW_VDW:
    glCallList(bigBallsLow_);
    break;
  }
  glCallList(selectionsLow_);
  // Save the current model view matrix (not needed here in fact)
  glPushMatrix();
  // Multiply matrix to get in the frame coordinate system.
  glMultMatrixd(manipulatedFrame()->matrix());
  //  if (showAtoms_)
  //    glCallList(selectedAtomsGLListHQ);
  // Scale down the drawings
  //  glScalef(0.3f, 0.3f, 0.3f);
  // Draw an axis using the QGLViewer static function
  //    drawAxis();
  // Restore the original (world) coordinate system
  glPopMatrix();
}

void Render::Viewer::drawWithNames()
{
  glInitNames();
  for (int i = 0; i < molecule_.atomsCount(); ++i)
  {
    glPushName(i);
    {
      switch (view_)
      {
      case VIEW_BALLS_STICKS:
        molecule_.atomAt(i).draw(Atom::AS_ATOM, QUALITY_LOW);
        break;
      case VIEW_STICKS:
        molecule_.atomAt(i).draw(Atom::AS_CONNECTOR, QUALITY_LOW);
        break;
      case VIEW_VDW:
        molecule_.atomAt(i).draw(Atom::AS_VDW, QUALITY_LOW);
        break;
      }
    }
    glPopName();
  }
}

void Render::Viewer::postSelection(const QPoint& point)
{
  bool found;

  selectedPoint = camera()->pointUnderPixel(point, found);
  if (selectedName() != -1)
  {
    molecule_.atom(selectedName()).setSelected(!molecule_.atomAt(selectedName()).isSelected());
    updateGLList(Viewer::GLLIST_SELECTIONS);
  }
}

GLuint Render::Viewer::makeAxes(GLfloat size, Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    Arrow x(Eigen::Vector3f(0.0f, 0.0f, 0.0f),
            Eigen::Vector3f(size, 0.0f, 0.0f),
            0.04f,
            Material::red());
    x.draw(STYLE_FILL, quality);

    Arrow y(Eigen::Vector3f(0.0f, 0.0f, 0.0f),
            Eigen::Vector3f(0.0f, size, 0.0f),
            0.05f,
            Material::green());
    y.draw(Render::STYLE_FILL, quality);

    Arrow z(Eigen::Vector3f(0.0f, 0.0f, 0.0f),
            Eigen::Vector3f(0.0f, 0.0f, size),
            0.05f,
            Material::blue());
    z.draw(STYLE_FILL, quality);
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeSmallBalls(Render::Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    /*
    QListIterator<Render::Atom*> i(molecule_.atoms_);
    while (i.hasNext())
    {
      i.next()->draw(Atom::AS_CONNECTOR, quality);
    } */
    for (quint16 i = 0; i < molecule_.atomsCount(); ++i)
    {
      molecule_.atomAt(i).draw(Render::Atom::AS_CONNECTOR, quality);
    }
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeMediumBalls(Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    /*
    QListIterator<Render::Atom*> i(molecule_.atoms_);
    while (i.hasNext())
    {
      i.next()->draw(Render::Atom::AS_ATOM, quality);
    } */
    for (quint16 i = 0; i < molecule_.atomsCount(); ++i)
    {
      molecule_.atomAt(i).draw(Render::Atom::AS_ATOM, quality);
    }
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeBigBalls(Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    /*
    QListIterator<Render::Atom*> i(molecule_.atoms_);
    while (i.hasNext())
    {
      i.next()->draw(Render::Atom::AS_VDW, quality);
    } */
    for (quint16 i = 0; i < molecule_.atomsCount(); ++i)
    {
      molecule_.atomAt(i).draw(Render::Atom::AS_VDW, quality);
    }
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeSticks(Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    /*
    QListIterator<Render::Bond*> i(molecule_.bonds_);
    while (i.hasNext())
    {
      i.next()->draw(quality);
    } */
    for (quint16 i = 0; i < molecule_.bondsCount(); ++i)
    {
      molecule_.bondAt(i).draw(quality);
    }
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeSelections(Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    for (int i = 0; i < molecule_.atomsCount(); i++)
    {
      if (molecule_.atomAt(i).isSelected())
      {
        switch (view_)
        {
        case VIEW_BALLS_STICKS:
          molecule_.atomAt(i).drawSelection(Atom::AS_ATOM, quality);
          break;
        case VIEW_STICKS:
          molecule_.atomAt(i).drawSelection(Atom::AS_CONNECTOR, quality);
          break;
        case VIEW_VDW:
          molecule_.atomAt(i).drawSelection(Atom::AS_VDW, quality);
          break;
        }
      }
    }
  }
  glEndList();
  return list;
}

void Render::Viewer::setView(Viewer::View view)
{
  view_ = view;
  updateGLList(GLLIST_SELECTIONS);
  updateGL();
}

void Render::Viewer::setMode(Mode mode)
{
  mode_ = mode;
  updateGL();
}

void Render::Viewer::setAxes(bool visibility, GLfloat size)
{
  isAxesVisible_ = visibility;
  axesSize_ = size;
  updateGLList(GLLIST_AXES);
  updateGL();
}

void Render::Viewer::setDebugInfoVisibility(bool visibility)
{
  isDebugInfoVisible_ = visibility;
  updateGL();
}

void Render::Viewer::setMolecule(const Render::Molecule& molecule)
{
  molecule_ = molecule;
  updateMolecule();
}

void Render::Viewer::setAtomicNumber(quint8 atomicNumber)
{
  atomicNumber_ = atomicNumber;
}

void Render::Viewer::updateMolecule()
{
  updateGLList(Viewer::GLLIST_BALLS);
  updateGLList(Viewer::GLLIST_STICKS);
  updateGL();
}

void Render::Viewer::mouseMoveEvent(QMouseEvent* e)
{
  switch (e->buttons())
  {
  case Qt::LeftButton:
    switch (e->modifiers())
    {
    case Qt::NoModifier:
      QGLViewer::mouseMoveEvent(e);
      break;
    default:
      break;
    }
    break;
  case Qt::RightButton:
    switch (e->modifiers())
    {
    case Qt::NoModifier:
      QGLViewer::mouseMoveEvent(e);
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void Render::Viewer::mousePressEvent(QMouseEvent* e)
{
  switch (e->button())
  {
  case Qt::LeftButton:
    beginSelection(e->pos());
    drawWithNames();
    endSelection(e->pos());
    // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
    bool found;
    selectedPoint = camera()->pointUnderPixel(e->pos(), found);
    switch (mode_)
    {
    case MODE_VIEW:
      if (e->modifiers() == Qt::NoModifier)
      {
        if (found)
          camera()->setRevolveAroundPoint(qglviewer::Vec(molecule_.atomAt(selectedName()).centre().x(),
                                                         molecule_.atomAt(selectedName()).centre().y(),
                                                         molecule_.atomAt(selectedName()).centre().z()));
        else
          camera()->setRevolveAroundPoint(qglviewer::Vec());
      }
      else if (e->modifiers() == Qt::ControlModifier)
      {
        for (int i = 0; i < molecule_.atomsCount(); ++i)
        {
          molecule_.atom(i).setMovable(false);
        }
        if (found)
        {
          movableIndex = selectedName();
          molecule_.atom(selectedName()).setMovable(true);
          updateGLList(Viewer::GLLIST_BALLS);
        }
      }
      QGLViewer::mousePressEvent(e);
      break;
    case MODE_ADD:
      if (found)
      {
//        obatom = obmol_.GetAtom(selectedName()+1);
//        obatom->SetAtomicNum(atomicNumber_);
      }
      else
      {
        Render::Atom chemistryAtom(atomicNumber_);
        molecule_.addAtom(chemistryAtom);
      }
      updateMolecule();
      if (!found)
      {
        qglviewer::Vec v = camera()->projectedCoordinatesOf(qglviewer::Vec());
        QPoint p((int) v.x, (int) v.y);
        p = mapToGlobal(p);
        cursor().setPos(p);
      }
      break;
    }
    break;
  case Qt::RightButton:
    switch (mode_)
    {
    case MODE_VIEW:
      QGLViewer::mousePressEvent(e);
      break;
    case MODE_ADD:
      break;
    }
    break;
  default:
    break;
  }
}

void Render::Viewer::mouseReleaseEvent(QMouseEvent* e)
{
  if (movableIndex != -1)
  {
    Eigen::Vector3f p(manipulatedFrame()->position().x +
                      molecule_.atomAt(movableIndex).centre().x(),
                      manipulatedFrame()->position().y +
                      molecule_.atomAt(movableIndex).centre().y(),
                      manipulatedFrame()->position().z +
                      molecule_.atomAt(movableIndex).centre().z());
    //      atomsQList_[movableIndex].setCentre(p);
    //      atomsQList_[movableIndex].setMovable(false);
//    obmol_.GetAtom(movableIndex + 1)->SetVector(p.x(), p.y(), p.z());
    movableIndex = -1;
    updateGLList(GLLIST_BALLS);
    updateGLList(GLLIST_STICKS);
  }
  manipulatedFrame()->setPosition(0.0f, 0.0f, 0.0f);
  QGLViewer::mouseReleaseEvent(e);
}

void Render::Viewer::updateGLList(Viewer::GLList gllist)
{
  switch (gllist)
  {
  case Viewer::GLLIST_AXES:
    glDeleteLists(axesLow_, 1);
    glDeleteLists(axesHigh_, 1);
    axesLow_ = makeAxes(axesSize_, QUALITY_LOW);
    axesHigh_ = makeAxes(axesSize_, QUALITY_HIGH);
    break;
  case Viewer::GLLIST_BALLS:
    glDeleteLists(smallBallsLow_, 1);
    glDeleteLists(smallBallsHigh_, 1);
    smallBallsLow_ = makeSmallBalls(QUALITY_LOW);
    smallBallsHigh_ = makeSmallBalls(QUALITY_HIGH);
    glDeleteLists(mediumBallsLow_, 1);
    glDeleteLists(mediumBallsHigh_, 1);
    mediumBallsLow_ = makeMediumBalls(QUALITY_LOW);
    mediumBallsHigh_ = makeMediumBalls(QUALITY_HIGH);
    glDeleteLists(bigBallsLow_, 1);
    glDeleteLists(bigBallsHigh_, 1);
    bigBallsLow_ = makeBigBalls(QUALITY_LOW);
    bigBallsHigh_ = makeBigBalls(QUALITY_HIGH);
    break;
  case Viewer::GLLIST_STICKS:
    glDeleteLists(sticksLow_, 1);
    glDeleteLists(sticksHigh_, 1);
    sticksLow_ = makeSticks(QUALITY_LOW);
    sticksHigh_ = makeSticks(QUALITY_HIGH);
    break;
  case Viewer::GLLIST_SELECTIONS:
    glDeleteLists(selectionsLow_, 1);
    glDeleteLists(selectionsHigh_, 1);
    selectionsLow_ = makeSelections(QUALITY_LOW);
    selectionsHigh_ = makeSelections(QUALITY_HIGH);
    break;
  }
}
