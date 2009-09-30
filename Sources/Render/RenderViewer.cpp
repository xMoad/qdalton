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
#include <QDebug>

#include "Render/RenderViewer.h"
#include "Render/RenderSphere.h"
#include "Render/RenderCylinder.h"
#include "Render/RenderMaterial.h"
#include "Render/RenderArrow.h"

Render::Viewer::Viewer(QWidget* parent) :
    QGLViewer(parent),
    molecule_(this),
    view_(Render::Viewer::VIEW_BALLS_BONDS),
    mode_(Render::Viewer::MODE_VIEW)
{
  isAxesVisible_ = true;
  isDebugInfoVisible_ = false;
  axesSize_ = 1.0f;
  atomicNumber_ = 6;
  currentOBAtom_ = 0;
  newOBAtom_ = 0;
}

Render::Viewer::~Viewer()
{
  //  makeCurrent();
  glDeleteLists(smallBallsLow_, 1);
  glDeleteLists(smallBallsHigh_, 1);
  glDeleteLists(mediumBallsLow_, 1);
  glDeleteLists(mediumBallsHigh_, 1);
  glDeleteLists(bigBallsLow_, 1);
  glDeleteLists(bigBallsHigh_, 1);
  glDeleteLists(bondsLow_, 1);
  glDeleteLists(bondsHigh_, 1);
  glDeleteLists(sticksLow_, 1);
  glDeleteLists(sticksHigh_, 1);
}

void Render::Viewer::draw()
{
  if (isAxesVisible_)
  {
    glCallList(axesHigh_);
  }

  switch (view_)
  {
  case (Render::Viewer::VIEW_BALLS_BONDS):
    glCallList(mediumBallsHigh_);
    glCallList(bondsHigh_);
    break;
  case (Render::Viewer::VIEW_BALLS_STICKS):
    glCallList(mediumBallsHigh_);
    glCallList(sticksHigh_);
    break;
  case (Render::Viewer::VIEW_STICKS):
    glCallList(smallBallsHigh_);
    glCallList(sticksHigh_);
    break;
  case (Render::Viewer::VIEW_VDW):
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

    renderText(20, 30, "Mode: View", font);
    renderText(20, 45,
               QString("Molecule: %1 (%2 atoms)").arg(
                   molecule_.formula(),
                   QString::number(molecule_.atomsCount())),
               font);

    glColor4fv(color);
  }

  /* buggy code! =)
    glGetFloatv(GL_CURRENT_COLOR, color);
    glColor3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < atomsList_.size(); i++)
    {
      renderText(atomsList_[i].centre().x(),
                 atomsList_[i].centre().y(),
                 atomsList_[i].centre().z(),
                 atomsList_[i].label(),
                 QFont("Arial"));
    }
    glColor4fv(color);
*/
}

void Render::Viewer::fastDraw()
{
  if (isAxesVisible_)
  {
    glCallList(axesLow_);
  }

  switch (view_)
  {
  case Render::Viewer::VIEW_BALLS_BONDS:
    glCallList(mediumBallsLow_);
    glCallList(bondsLow_);
    break;
  case Render::Viewer::VIEW_BALLS_STICKS:
    glCallList(mediumBallsLow_);
    glCallList(sticksLow_);
    break;
  case Render::Viewer::VIEW_STICKS:
    glCallList(smallBallsLow_);
    glCallList(sticksLow_);
    break;
  case Render::Viewer::VIEW_VDW:
    glCallList(bigBallsLow_);
    break;
  }
  glCallList(selectionsLow_);
}

void Render::Viewer::drawWithNames()
{
  int i = 0;
  QListIterator<Render::Atom> a(atomsList_);
  QListIterator<Render::Bond> b(bondsList_);

  glInitNames();

  while (a.hasNext())
  {
    glPushName(i);
    {
      Render::Atom atom(a.next());
      switch (view_)
      {
      case Render::Viewer::VIEW_BALLS_BONDS:
      case Render::Viewer::VIEW_BALLS_STICKS:
        atom.draw(Render::Atom::DRAW_STYLE_ATOM,
                           Render::QUALITY_LOW);
        break;
      case Render::Viewer::VIEW_STICKS:
        atom.draw(Render::Atom::DRAW_STYLE_CONNECTOR,
                           Render::QUALITY_LOW);
        break;
      case Render::Viewer::VIEW_VDW:
        atom.draw(Render::Atom::DRAW_STYLE_VDW,
                           Render::QUALITY_LOW);
        break;
      }
    }
    glPopName();
    i++;
  }

  while (b.hasNext())
  {
    glPushName(i);
    {
      Render::Bond bond(b.next());
      switch (view_)
      {
      case Render::Viewer::VIEW_BALLS_BONDS:
      case Render::Viewer::VIEW_BALLS_STICKS:
      case Render::Viewer::VIEW_STICKS:
        bond.draw(Render::Bond::DRAW_STYLE_STICK,
                           Render::QUALITY_LOW);
        break;
      case Render::Viewer::VIEW_VDW:
        break;
      }
    }
    glPopName();
    i++;
  }
}

void Render::Viewer::postSelection(const QPoint& point)
{
  bool found;

  selectedPoint = camera()->pointUnderPixel(point, found);

  if (selectedName() != -1)
  {
    if (selectedName() < atomsList_.size())
    {
      atomsList_[selectedName()].toggleSelected();    
    }
    else
    {
      bondsList_[selectedName() - atomsList_.size()].toggleSelected();
    }
    updateGLList(Viewer::GLLIST_SELECTIONS);
  }
}

QString Render::Viewer::helpString() const
{
  QString text("<h2>QDalton \"Draft 1\"</h2>");
  text += "Hi there! =)";
  return text;
}

void Render::Viewer::init()
{
  const float    LIGHT_AMBIENT[4]                  = {0.2f, 0.2f, 0.2f, 1.0f};

  const float    LIGHT0_DIFFUSE[4]                 = {1.0f, 1.0f, 1.0f, 1.0f};
  const float    LIGHT0_SPECULAR[4]                = {1.0f, 1.0f, 1.0f, 1.0f};
  const float    LIGHT0_POSITION[4]                = {0.8f, 0.7f, 1.0f, 0.0f};

  const float    LIGHT1_DIFFUSE[4]                 = {0.3f, 0.3f, 0.3f, 1.0f};
  const float    LIGHT1_SPECULAR[4]                = {0.5f, 0.5f, 0.5f, 1.0f};
  const float    LIGHT1_POSITION[4]                = {-0.8f, 0.7f, -0.5f, 0.0f};

  // This method is called from QGLViewer::initializeGL(),
  // where GL_COLOR_MATERIAL is enabled. We don't need it!
  glDisable(GL_COLOR_MATERIAL);

  // As mentioned in QGLViewer documentation for select() method,
  // one can encounter problems with backface culling.
  // If so one can try to glDisable(GL_CULL_FACE). It works! Not always. =(
  glDisable(GL_CULL_FACE);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Setup shading (filling) mode to smooth
  glShadeModel(GL_SMOOTH);
  // Enable the depth buffer updating
  glEnable(GL_DEPTH_TEST);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

//  glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL_EXT,
//                 GL_SEPARATE_SPECULAR_COLOR_EXT );

  // Due to the bug found with Mesa 6.5.3 in the Radeon DRI driver
  // in radeon_state.c in radeonUpdateSpecular(),
  // it is important to set GL_SEPARATE_SPECULAR_COLOR_EXT
  // _before_ enabling lighting
  glEnable(GL_LIGHTING);

  glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT0_DIFFUSE);
  glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT0_SPECULAR);
  glLightfv(GL_LIGHT0, GL_POSITION, LIGHT0_POSITION);
  glEnable(GL_LIGHT0);

  // Create a second light source to illuminate shadows a little better
  glLightfv(GL_LIGHT1, GL_AMBIENT, LIGHT_AMBIENT);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LIGHT1_DIFFUSE);
  glLightfv(GL_LIGHT1, GL_SPECULAR, LIGHT1_SPECULAR);
  glLightfv(GL_LIGHT1, GL_POSITION, LIGHT1_POSITION);
  glEnable(GL_LIGHT1);

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

GLuint Render::Viewer::makeAxes(GLfloat size, Quality quality)
{
  Render::Arrow x;
  Render::Arrow y;
  Render::Arrow z;

  x.setOrigin(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
  x.setTerminus(Eigen::Vector3f(size, 0.0f, 0.0f));
  x.setRadius(0.04f);
  x.setMaterial(Render::Material::red());

  y.setOrigin(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
  y.setTerminus(Eigen::Vector3f(0.0f, size, 0.0f));
  y.setRadius(0.04f);
  y.setMaterial(Render::Material::green());

  z.setOrigin(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
  z.setTerminus(Eigen::Vector3f(0.0f, 0.0f, size));
  z.setRadius(0.04f);
  z.setMaterial(Render::Material::blue());

  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    x.draw(STYLE_FILL, quality);
    y.draw(Render::STYLE_FILL, quality);
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
    for (int i = 0; i < atomsList_.size(); ++i)
      atomsList_[i].draw(Render::Atom::DRAW_STYLE_CONNECTOR, quality);
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeMediumBalls(Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    for (int i = 0; i < atomsList_.size(); ++i)
      atomsList_[i].draw(Render::Atom::DRAW_STYLE_ATOM, quality);
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeBigBalls(Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    for (int i = 0; i < atomsList_.size(); ++i)
      atomsList_[i].draw(Render::Atom::DRAW_STYLE_VDW, quality);
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeBonds(Render::Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    for (int i = 0; i < bondsList_.size(); ++i)
      bondsList_[i].draw(Render::Bond::DRAW_STYLE_BOND, quality);
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeSticks(Render::Quality quality)
{
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    for (int i = 0; i < bondsList_.size(); ++i)
      bondsList_[i].draw(Render::Bond::DRAW_STYLE_STICK, quality);
  }
  glEndList();
  return list;
}

GLuint Render::Viewer::makeSelections(Quality quality)
{
  int i = 0;
  QListIterator<Render::Atom> a(atomsList_);
  QListIterator<Render::Bond> b(bondsList_);

  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    while (a.hasNext())
    {
      Render::Atom atom(a.next());
      if (atom.isSelected())
      {  
        switch (view_)
        {
        case Render::Viewer::VIEW_BALLS_BONDS:
        case Render::Viewer::VIEW_BALLS_STICKS:
          atom.drawSelection(Render::Atom::DRAW_STYLE_ATOM, quality);
          break;
        case Render::Viewer::VIEW_STICKS:
          atom.drawSelection(Render::Atom::DRAW_STYLE_CONNECTOR, quality);
          break;
        case Render::Viewer::VIEW_VDW:
          atom.drawSelection(Render::Atom::DRAW_STYLE_VDW, quality);
          break;
        }
      }
      i++;
    }

    while (b.hasNext())
    {
      Render::Bond bond(b.next());
      if (bond.isSelected())
      {
        switch (view_)
        {
        case Render::Viewer::VIEW_BALLS_BONDS:
        case Render::Viewer::VIEW_BALLS_STICKS:
        case Render::Viewer::VIEW_STICKS:
          bond.drawSelection(quality);
          break;
        case Render::Viewer::VIEW_VDW:
          break;
        }
        i++;
      }
    }
  }
  glEndList();
  return list;
}

void Render::Viewer::setView(Render::Viewer::View view)
{
  view_ = view;
  updateGLList(GLLIST_SELECTIONS);
  updateGL();
}

void Render::Viewer::setMode(Render::Viewer::Mode mode)
{
  mode_ = mode;
  switch (mode_)
  {
  case (MODE_VIEW):
    setMouseBinding(Qt::LeftButton, CAMERA, ROTATE);
//    setMouseBinding(Qt::CTRL + Qt::RightButton, FRAME, TRANSLATE);
    break;
  case (MODE_ADD):
    setMouseBinding(Qt::LeftButton, FRAME, TRANSLATE);
    break;
  }
//  updateGL();
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

bool Render::Viewer::isMoleculeEmpty() const
{
  return molecule_.atomsCount() == 0;
}

void Render::Viewer::setAtomicNumber(quint8 atomicNumber)
{
  atomicNumber_ = atomicNumber;
}

void Render::Viewer::updateMolecule()
{
  updateRenderAtoms();
  updateGLList(Render::Viewer::GLLIST_ATOMS);
  updateRenderBonds();
  updateGLList(Render::Viewer::GLLIST_BONDS);
  updateGL();
}

void Render::Viewer::addAtom(const Atom& atom)
{
  atomsList_ << atom;
}

void Render::Viewer::addBond(const Bond& bond)
{
  bondsList_ << bond;
}

void Render::Viewer::build()
{
  molecule_.build();
  updateMolecule();
}

void Render::Viewer::addHydrogensAndBuild()
{
  molecule_.addHydrogensAndBuild();
  updateMolecule();
}

void Render::Viewer::removeHydrogens()
{
  molecule_.removeHydrogens();
  updateMolecule();
}

void Render::Viewer::mouseMoveEvent(QMouseEvent* e)
{
//  updateGL();
  switch (e->buttons())
  {
  case Qt::LeftButton:
    switch (mode_)
    {
    case (MODE_VIEW):
      QGLViewer::mouseMoveEvent(e);
      break;
    case (MODE_ADD):
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
    case Qt::MetaModifier:
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
  beginSelection(e->pos());
  drawWithNames();
  endSelection(e->pos());

  // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
  bool found;
  selectedPoint = camera()->pointUnderPixel(e->pos(), found);

  switch (e->button())
  {
  case Qt::LeftButton:
    switch (mode_)
    {
    case MODE_VIEW:
      if (e->modifiers() == Qt::NoModifier)
      {
        if (found)
          camera()->setRevolveAroundPoint(qglviewer::Vec(atomsList_[selectedName()].centre().x(),
                                                         atomsList_[selectedName()].centre().y(),
                                                         atomsList_[selectedName()].centre().z()));
        else
          camera()->setRevolveAroundPoint(qglviewer::Vec());
      }
      QGLViewer::mousePressEvent(e);
      break;
    case MODE_ADD:
      if (found)
      {
        currentOBAtom_ = molecule_.obAtom(selectedName());
//        currentOBAtom_->SetAtomicNum(atomicNumber_);
        newOBAtom_ = new OpenBabel::OBAtom();
        newOBAtom_->SetAtomicNum(atomicNumber_);
        newOBAtom_->SetVector(currentOBAtom_->GetVector());
      }
      else
      {
        if (molecule_.atomsCount() == 0)
        {
          molecule_.addAtom(atomicNumber_);
          updateMolecule();
        }
      }
      QGLViewer::mousePressEvent(e);
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
      switch (e->modifiers())
      {
      case (Qt::NoModifier):
        molecule_.deleteAtom(atomsList_[selectedName()].obAtom());
        updateMolecule();
        break;
      case (Qt::MetaModifier):
        QGLViewer::mousePressEvent(e);
      }
      break;
    }
    break;
  default:
    break;
  }
}

void Render::Viewer::mouseReleaseEvent(QMouseEvent* e)
{
  switch (e->button())
  {
  case (Qt::LeftButton):
    switch (mode_)
    {
    case (MODE_VIEW):
      break;
    case (MODE_ADD):

      beginSelection(e->pos());
      drawWithNames();
      endSelection(e->pos());

      // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
      bool found;
      selectedPoint = camera()->pointUnderPixel(e->pos(), found);

      if (currentOBAtom_ != 0)
      {
        qglviewer::Vec v(newOBAtom_->x(),
                         newOBAtom_->y(),
                         newOBAtom_->z());
        if (manipulatedFrame()->position() == qglviewer::Vec())
        {
          currentOBAtom_->SetAtomicNum(newOBAtom_->GetAtomicNum());
        }
        else
        {
          if (!found)
          {
            OpenBabel::vector3 p(newOBAtom_->x() + manipulatedFrame()->position().x,
                                 newOBAtom_->y() + manipulatedFrame()->position().y,
                                 newOBAtom_->z() + manipulatedFrame()->position().z);
            newOBAtom_->SetVector(p);
            molecule_.addObAtom(*newOBAtom_);
            molecule_.addBond(currentOBAtom_,
                              molecule_.obAtom(molecule_.atomsCount() - 1),
                              1);
          }
          else
          {
            molecule_.addBond(currentOBAtom_,
                              atomsList_[selectedName()].obAtom(),
                              1);
          }
        }
        delete newOBAtom_;
        currentOBAtom_ = 0;
      }

      updateMolecule();
      manipulatedFrame()->setPosition(0.0f, 0.0f, 0.0f);
        break;
    }
    break;
  }
  QGLViewer::mouseReleaseEvent(e);
}

void Render::Viewer::updateGLList(Viewer::GLList gllist)
{
  switch (gllist)
  {
  case Render::Viewer::GLLIST_AXES:
    glDeleteLists(axesLow_, 1);
    glDeleteLists(axesHigh_, 1);
    axesLow_ = makeAxes(axesSize_, Render::QUALITY_LOW);
    axesHigh_ = makeAxes(axesSize_, Render::QUALITY_HIGH);
    break;
  case Render::Viewer::GLLIST_ATOMS:
    glDeleteLists(smallBallsLow_, 1);
    glDeleteLists(smallBallsHigh_, 1);
    smallBallsLow_ = makeSmallBalls(Render::QUALITY_LOW);
    smallBallsHigh_ = makeSmallBalls(Render::QUALITY_HIGH);
    glDeleteLists(mediumBallsLow_, 1);
    glDeleteLists(mediumBallsHigh_, 1);
    mediumBallsLow_ = makeMediumBalls(Render::QUALITY_LOW);
    mediumBallsHigh_ = makeMediumBalls(Render::QUALITY_HIGH);
    glDeleteLists(bigBallsLow_, 1);
    glDeleteLists(bigBallsHigh_, 1);
    bigBallsLow_ = makeBigBalls(Render::QUALITY_LOW);
    bigBallsHigh_ = makeBigBalls(Render::QUALITY_HIGH);
    break;
  case Render::Viewer::GLLIST_BONDS:
    glDeleteLists(bondsLow_, 1);
    glDeleteLists(bondsHigh_, 1);
    bondsLow_ = makeBonds(Render::QUALITY_LOW);
    bondsHigh_ = makeBonds(Render::QUALITY_HIGH);
    glDeleteLists(sticksLow_, 1);
    glDeleteLists(sticksHigh_, 1);
    sticksLow_ = makeSticks(Render::QUALITY_LOW);
    sticksHigh_ = makeSticks(Render::QUALITY_HIGH);
    break;
  case Render::Viewer::GLLIST_SELECTIONS:
    glDeleteLists(selectionsLow_, 1);
    glDeleteLists(selectionsHigh_, 1);
    selectionsLow_ = makeSelections(Render::QUALITY_LOW);
    selectionsHigh_ = makeSelections(Render::QUALITY_HIGH);
    break;
  }
}

void Render::Viewer::updateRenderAtoms()
{
  atomsList_.clear();
  for (quint16 i = 0; i < molecule_.atomsCount(); ++i)
  {
    Atom atom(molecule_.obAtom(i));
    addAtom(atom);
  }
}

void Render::Viewer::updateRenderBonds()
{
  bondsList_.clear();
  for (quint16 i = 0; i < molecule_.bondsCount(); ++i)
  {
    Bond bond(molecule_.obBond(i));
    addBond(bond);
  }
}
