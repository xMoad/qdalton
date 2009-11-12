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
    molecule_(0),
    view_(Render::Viewer::ViewBallsAndBonds),
//    mode_(Render::Viewer::ModeView),
    obAtomSelectedBefore_(0),
    obAtomNew_(0)
{
  isAxesVisible_ = true;
  isDebugInfoVisible_ = false;
  axesSize_ = 1.0f;
  atomicNumber_ = 6;
  setMouseBinding(Qt::CTRL + Qt::LeftButton, FRAME, TRANSLATE);
}

Render::Viewer::~Viewer()
{
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
  case (Render::Viewer::ViewBallsAndBonds):
    glCallList(mediumBallsHigh_);
    glCallList(bondsHigh_);
    break;
  case (Render::Viewer::ViewBallsAndSticks):
    glCallList(mediumBallsHigh_);
    glCallList(sticksHigh_);
    break;
  case (Render::Viewer::ViewSticks):
    glCallList(smallBallsHigh_);
    glCallList(sticksHigh_);
    break;
  case (Render::Viewer::ViewVdWSpheres):
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
                   molecule_->formula(),
                   QString::number(molecule_->atomsCount())),
               font);

    glColor4fv(color);
  }
/*
  GLfloat color[4];
  glGetFloatv(GL_CURRENT_COLOR, color);
  glColor3f(0.0f, 1.0f, 0.0f);
  for (int i = 0; i < atomsList_.size(); ++i)
  {
    renderText(atomsList_[i].centre().x(),
               atomsList_[i].centre().y(),
               atomsList_[i].centre().z(),
               "H");
  }
  glColor4fv(color); */
}

void Render::Viewer::fastDraw()
{
  if (isAxesVisible_)
  {
    glCallList(axesLow_);
  }

  switch (view_)
  {
  case Render::Viewer::ViewBallsAndBonds:
    glCallList(mediumBallsLow_);
    glCallList(bondsLow_);
    break;
  case Render::Viewer::ViewBallsAndSticks:
    glCallList(mediumBallsLow_);
    glCallList(sticksLow_);
    break;
  case Render::Viewer::ViewSticks:
    glCallList(smallBallsLow_);
    glCallList(sticksLow_);
    break;
  case Render::Viewer::ViewVdWSpheres:
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
      case Render::Viewer::ViewBallsAndBonds:
      case Render::Viewer::ViewBallsAndSticks:
        atom.draw(Render::Atom::DrawStyleAtom,
                  Render::QualityLow);
        break;
      case Render::Viewer::ViewSticks:
        atom.draw(Render::Atom::DrawStyleConnector,
                  Render::QualityLow);
        break;
      case Render::Viewer::ViewVdWSpheres:
        atom.draw(Render::Atom::DrawStyleVdW,
                  Render::QualityLow);
        break;
      }
    }
    glPopName();
    i++;
  }

  if (view_ != Render::Viewer::ViewVdWSpheres)
  {
    while (b.hasNext())
    {
      glPushName(i);
      {
        Render::Bond bond(b.next());
        bond.draw(Render::Bond::DrawStyleStick,
                  Eigen::Vector3f(0.0f, 0.0f, 1.0f),
                  Render::QualityLow);
      }
      glPopName();
      i++;
    }
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
  const float LIGHT_AMBIENT[4] = {0.2f, 0.2f, 0.2f, 1.0f};

  const float LIGHT0_DIFFUSE[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  const float LIGHT0_SPECULAR[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  const float LIGHT0_POSITION[4] = {0.8f, 0.7f, 1.0f, 0.0f};

  const float LIGHT1_DIFFUSE[4] = {0.3f, 0.3f, 0.3f, 1.0f};
  const float LIGHT1_SPECULAR[4] = {0.5f, 0.5f, 0.5f, 1.0f};
  const float LIGHT1_POSITION[4] = {-0.8f, 0.7f, -0.5f, 0.0f};

  // This method is called from QGLViewer::initializeGL(),
  // where GL_COLOR_MATERIAL is enabled. We don't need it!
  glDisable(GL_COLOR_MATERIAL);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Setup shading (filling) mode to smooth
//  glShadeModel(GL_SMOOTH);
  // Enable the depth buffer updating
//  glEnable(GL_DEPTH_TEST);

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

  // As mentioned in QGLViewer documentation for select() method,
  // one can encounter problems with backface culling.
  // If so one can try to glDisable(GL_CULL_FACE). It works! Not always. =(
  glDisable(GL_CULL_FACE);

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
    x.draw(Render::StyleFill, quality);
    y.draw(Render::StyleFill, quality);
    z.draw(Render::StyleFill, quality);
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
      atomsList_[i].draw(Render::Atom::DrawStyleConnector, quality);
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
      atomsList_[i].draw(Render::Atom::DrawStyleAtom, quality);
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
      atomsList_[i].draw(Render::Atom::DrawStyleVdW, quality);
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
      bondsList_[i].draw(Render::Bond::DrawStyleBond,
                         Eigen::Vector3f(camera()->position().x,
                                         camera()->position().y,
                                         camera()->position().z),
                         quality);
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
      bondsList_[i].draw(Render::Bond::DrawStyleStick,
                         Eigen::Vector3f(0.0f, 0.0f, 1.0f),
                         quality);
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
        case Render::Viewer::ViewBallsAndBonds:
        case Render::Viewer::ViewBallsAndSticks:
          atom.drawSelection(Render::Atom::DrawStyleAtom, quality);
          break;
        case Render::Viewer::ViewSticks:
          atom.drawSelection(Render::Atom::DrawStyleConnector, quality);
          break;
        case Render::Viewer::ViewVdWSpheres:
          atom.drawSelection(Render::Atom::DrawStyleVdW, quality);
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
        case Render::Viewer::ViewBallsAndBonds:
        case Render::Viewer::ViewBallsAndSticks:
        case Render::Viewer::ViewSticks:
          bond.drawSelection(quality);
          break;
        case Render::Viewer::ViewVdWSpheres:
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

//void Render::Viewer::setMode(Render::Viewer::Mode mode)
//{
//  mode_ = mode;
//  switch (mode_)
//  {
//  case (Render::Viewer::ModeView):
////    setMouseBinding(Qt::LeftButton, CAMERA, ROTATE);
//    break;
//  case (Render::Viewer::ModeEdit):
////    setMouseBinding(Qt::LeftButton, FRAME, TRANSLATE);
//    break;
//  }
//}

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

void Render::Viewer::setMolecule(Chemistry::Molecule* molecule)
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
  molecule_->build();
  updateMolecule();
}

void Render::Viewer::addHydrogensAndBuild()
{
  molecule_->addHydrogensAndBuild();
  updateMolecule();
}

void Render::Viewer::removeHydrogens()
{
  molecule_->removeHydrogens();
  updateMolecule();
}

void Render::Viewer::conformationalSearch(QTableWidget* targetTableWidget)
{
  molecule_->conformationalSearch();
  targetTableWidget->setRowCount(molecule_->conformersCount());
  for (int i = 0; i < molecule_->conformersCount(); ++i)
  {
    QTableWidgetItem* newItem = new QTableWidgetItem(QString::number(i + 1).rightJustified(3, '0'));
    targetTableWidget->setItem(i, 0, newItem);
    newItem = new QTableWidgetItem(QString::number(molecule_->conformerEnergy(i)));
    targetTableWidget->setItem(i, 1, newItem);
  }
  displayConformer(0);
}

void Render::Viewer::displayConformer(quint16 index)
{
  molecule_->setConformer(index);
  updateMolecule();
}

bool Render::Viewer::isSomethingUnderPixel(const QPoint& pixel)
{
  bool found;

  beginSelection(pixel);
  drawWithNames();
  endSelection(pixel);

  // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
  camera()->pointUnderPixel(pixel, found);

  return found;
}

void Render::Viewer::mouseMoveEvent(QMouseEvent* e)
{
  switch (e->buttons())
  {
  case Qt::LeftButton:
    QGLViewer::mouseMoveEvent(e);
    break;
  case Qt::RightButton:
    if (e->modifiers() == Qt::NoModifier | Qt::MetaModifier)
    {
      QGLViewer::mouseMoveEvent(e);
    }
    break;
  default:
    break;
  }
}

void Render::Viewer::mousePressEvent(QMouseEvent* e)
{
  glCallList(sticksLow_);

  switch (e->button())
  {
  case Qt::LeftButton:
    mouseLeftButtonPressEvent(e);
    break;
  case Qt::RightButton:
    mouseRightButtonPressEvent(e);
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
    mouseLeftButtonReleaseEvent(e);
    break;
  default:
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
    axesLow_ = makeAxes(axesSize_, Render::QualityLow);
    axesHigh_ = makeAxes(axesSize_, Render::QualityHigh);
    break;
  case Render::Viewer::GLLIST_ATOMS:
    glDeleteLists(smallBallsLow_, 1);
    glDeleteLists(smallBallsHigh_, 1);
    smallBallsLow_ = makeSmallBalls(Render::QualityLow);
    smallBallsHigh_ = makeSmallBalls(Render::QualityHigh);
    glDeleteLists(mediumBallsLow_, 1);
    glDeleteLists(mediumBallsHigh_, 1);
    mediumBallsLow_ = makeMediumBalls(Render::QualityLow);
    mediumBallsHigh_ = makeMediumBalls(Render::QualityHigh);
    glDeleteLists(bigBallsLow_, 1);
    glDeleteLists(bigBallsHigh_, 1);
    bigBallsLow_ = makeBigBalls(Render::QualityLow);
    bigBallsHigh_ = makeBigBalls(Render::QualityHigh);
    break;
  case Render::Viewer::GLLIST_BONDS:
    glDeleteLists(bondsLow_, 1);
    glDeleteLists(bondsHigh_, 1);
    bondsLow_ = makeBonds(Render::QualityLow);
    bondsHigh_ = makeBonds(Render::QualityHigh);
    glDeleteLists(sticksLow_, 1);
    glDeleteLists(sticksHigh_, 1);
    sticksLow_ = makeSticks(Render::QualityLow);
    sticksHigh_ = makeSticks(Render::QualityHigh);
    break;
  case Render::Viewer::GLLIST_SELECTIONS:
    glDeleteLists(selectionsLow_, 1);
    glDeleteLists(selectionsHigh_, 1);
    selectionsLow_ = makeSelections(Render::QualityLow);
    selectionsHigh_ = makeSelections(Render::QualityHigh);
    break;
  }
}

void Render::Viewer::updateRenderAtoms()
{
  atomsList_.clear();
  for (quint16 i = 0; i < molecule_->atomsCount(); ++i)
  {
    Atom atom(molecule_->obAtom(i));
    addAtom(atom);
  }
}

void Render::Viewer::updateRenderBonds()
{
  bondsList_.clear();
  for (quint16 i = 0; i < molecule_->bondsCount(); ++i)
  {
    Bond bond(molecule_->obBond(i));
    addBond(bond);
  }
}

void Render::Viewer::mouseLeftButtonPressEvent(QMouseEvent* e)
{
  switch (e->modifiers())
  {
  case Qt::NoModifier:
    mouseLeftButtonWithNoModifierPressEvent(e);
    QGLViewer::mousePressEvent(e);
    break;
  case Qt::ShiftModifier:
    mouseLeftButtonWithShiftPressEvent(e);
    break;
  case Qt::ControlModifier:
    mouseLeftButtonWithCtrlPressEvent(e);
    QGLViewer::mousePressEvent(e);
    break;
  default:
    break;
  }
}

void Render::Viewer::mouseLeftButtonWithNoModifierPressEvent(QMouseEvent* e)
{
  if (isSomethingUnderPixel(e->pos()) && selectedName() < atomsList_.size())
  {
    camera()->setRevolveAroundPoint(qglviewer::Vec(atomsList_[selectedName()].centre().x(),
                                                   atomsList_[selectedName()].centre().y(),
                                                   atomsList_[selectedName()].centre().z()));
  }
  else
  {
    camera()->setRevolveAroundPoint(qglviewer::Vec());
  }
}

void Render::Viewer::mouseLeftButtonWithShiftPressEvent(QMouseEvent* e)
{
  if (isSomethingUnderPixel(e->pos()))
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

void Render::Viewer::mouseLeftButtonWithCtrlPressEvent(QMouseEvent* e)
{
  if (isSomethingUnderPixel(e->pos()))
  {
    if (selectedName() < atomsList_.size())
    {
      obAtomSelectedBefore_ = molecule_->obAtom(selectedName());
      obAtomNew_ = new OpenBabel::OBAtom();
      obAtomNew_->SetAtomicNum(atomicNumber_);
      obAtomNew_->SetVector(obAtomSelectedBefore_->GetVector());
    }
    else
    {
      bondsList_[selectedName() - atomsList_.size()].cycleOrder();
    }
  }
  else
  {
    obAtomSelectedBefore_ = 0;
    if (molecule_->atomsCount() == 0)
    {
      molecule_->addAtom(atomicNumber_);
      updateMolecule();
    }
  }
}

void Render::Viewer::mouseRightButtonPressEvent(QMouseEvent* e)
{
  switch (e->modifiers())
  {
  case (Qt::NoModifier):
    mouseRightButtonWithNoModifierPressEvent(e);
    break;
  default:
    break;
  }
}

void Render::Viewer::mouseRightButtonWithNoModifierPressEvent(QMouseEvent* e)
{
  if (isSomethingUnderPixel(e->pos()))
  {
    if (selectedName() < atomsList_.size())
    {
      molecule_->deleteAtom(atomsList_[selectedName()].obAtom());
    }
    else
    {
      molecule_->deleteBond(bondsList_[selectedName() - atomsList_.size()].obBond());
    }
    updateMolecule();
  }
  else
  {
    QGLViewer::mousePressEvent(e);
  }
}

void Render::Viewer::mouseLeftButtonReleaseEvent(QMouseEvent* e)
{
  switch (e->modifiers())
  {
  case Qt::NoModifier:
    updateGLList(GLLIST_BONDS);
    break;
  case Qt::ControlModifier:
    mouseLeftButtonWithCtrlReleaseEvent(e);
    break;
  }
}

void Render::Viewer::mouseLeftButtonWithCtrlReleaseEvent(QMouseEvent* e)
{
  if (rect().contains(e->pos()))
  {
    if (obAtomSelectedBefore_ != 0)
    {
      qglviewer::Vec v(obAtomNew_->x(),
                       obAtomNew_->y(),
                       obAtomNew_->z());
      if (manipulatedFrame()->position() == qglviewer::Vec())
      {
        obAtomSelectedBefore_->SetAtomicNum(obAtomNew_->GetAtomicNum());
      }
      else
      {
        if (!isSomethingUnderPixel(e->pos()))
        {
          OpenBabel::vector3 p(obAtomNew_->x() + manipulatedFrame()->position().x,
                               obAtomNew_->y() + manipulatedFrame()->position().y,
                               obAtomNew_->z() + manipulatedFrame()->position().z);
          obAtomNew_->SetVector(p);
          molecule_->addObAtom(*obAtomNew_);
          molecule_->addBond(obAtomSelectedBefore_,
                             molecule_->obAtom(molecule_->atomsCount() - 1),
                             1);
        }
        else
        {
          molecule_->addBond(obAtomSelectedBefore_,
                             atomsList_[selectedName()].obAtom(),
                             1);
        }
      }
    }
    updateMolecule();
  }

  if (obAtomSelectedBefore_ != 0)
  {
    delete obAtomNew_;
    obAtomSelectedBefore_ = 0;
  }

  manipulatedFrame()->setPosition(0.0f, 0.0f, 0.0f);
}
