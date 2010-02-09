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
    fileMol_(),
    view_(Render::Viewer::ViewBallsAndBonds),
    atomSelectedBefore_(0),
    obAtomNew_(0),
    displayListAxes_(0)
{
  isAxesVisible_ = true;
  isDebugInfoVisible_ = false;
  axesSize_ = 1.0f;
  atomicNumber_ = 6;
  setMouseBinding(Qt::CTRL + Qt::LeftButton, FRAME, TRANSLATE);
}

Render::Viewer::~Viewer()
{
  glDeleteLists(displayListAxes_, 0);
}

void Render::Viewer::draw()
{
  if (isAxesVisible_)
    glCallList(displayListAxes_);

  switch (view_)
  {
  case (Render::Viewer::ViewBallsAndBonds):
    for (int i = 0; i < atomsList_.size(); ++i)
      atomsList_[i].draw(Render::Atom::DrawStyleAtom);
    for (int i = 0; i < bondsList_.size(); ++i)
      bondsList_[i].draw(Render::Bond::DrawStyleBond);
    break;
  case (Render::Viewer::ViewBallsAndSticks):
    for (int i = 0; i < atomsList_.size(); ++i)
      atomsList_[i].draw(Render::Atom::DrawStyleAtom);
    for (int i = 0; i < bondsList_.size(); ++i)
      bondsList_[i].draw(Render::Bond::DrawStyleStick);
    break;
  case (Render::Viewer::ViewSticks):
    for (int i = 0; i < atomsList_.size(); ++i)
      atomsList_[i].draw(Render::Atom::DrawStyleConnector);
    for (int i = 0; i < bondsList_.size(); ++i)
      bondsList_[i].draw(Render::Bond::DrawStyleStick);
    break;
  case (Render::Viewer::ViewVdWSpheres):
    for (int i = 0; i < atomsList_.size(); ++i)
      atomsList_[i].draw(Render::Atom::DrawStyleVdW);
    break;
  }

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
               QString("Molecule: %1 (%2 atoms)").arg(fileMol_.molecule()->formula(),
                   QString::number(fileMol_.molecule()->atomsCount())),
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

void Render::Viewer::drawWithNames()
{
  int n = 0;

  glInitNames();

  for (int i = 0; i < atomsList_.size(); ++i)
  {
    glPushName(n);
    {
      switch (view_)
      {
      case Render::Viewer::ViewBallsAndBonds:
      case Render::Viewer::ViewBallsAndSticks:
        atomsList_[i].draw(Render::Atom::DrawStyleAtom);
        break;
      case Render::Viewer::ViewSticks:
        atomsList_[i].draw(Render::Atom::DrawStyleConnector);
        break;
      case Render::Viewer::ViewVdWSpheres:
        atomsList_[i].draw(Render::Atom::DrawStyleVdW);
        break;
      }
    }
    glPopName();
    n++;
  }

  if (view_ != Render::Viewer::ViewVdWSpheres)
  {
    for (int i = 0; i < bondsList_.size(); ++i)
    {
      glPushName(n);
      {
        bondsList_[i].draw(Render::Bond::DrawStyleStick);
      }
      glPopName();
      n++;
    }
  }
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

  // Enable the depth buffer updating
  glEnable(GL_DEPTH_TEST);

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

/* Place inside draw()?
  const float fogColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.002f);
  glHint(GL_FOG_HINT, GL_NICEST);
  glFogf(GL_FOG_START, -sceneRadius());
  glFogf(GL_FOG_END, sceneRadius());
*/

  glEnable(GL_NORMALIZE);

  // As mentioned in QGLViewer documentation for select() method,
  // one can encounter problems with backface culling.
  // If so one can try to glDisable(GL_CULL_FACE). It works! Not always. =(
  glDisable(GL_CULL_FACE);

  displayListAxes_ = makeAxes(axesSize_);

  setManipulatedFrame(new qglviewer::ManipulatedFrame());
}

GLuint Render::Viewer::makeAxes(GLfloat size)
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
    x.draw(Render::StyleFill);
    y.draw(Render::StyleFill);
    z.draw(Render::StyleFill);
  }
  glEndList();

  return list;
}

void Render::Viewer::setView(Render::Viewer::View view)
{
  view_ = view;
  updateGL();
}

void Render::Viewer::setAxes(bool visibility, GLfloat size)
{
  isAxesVisible_ = visibility;
  axesSize_ = size;

  glDeleteLists(displayListAxes_, 1);

  displayListAxes_ = makeAxes(axesSize_);

  updateGL();
}

void Render::Viewer::setDebugInfoVisibility(bool visibility)
{
  isDebugInfoVisible_ = visibility;
  updateGL();
}

File::Mol& Render::Viewer::fileMol()
{
  return fileMol_;
}

void Render::Viewer::setFileMol(const File::Mol& fileMol)
{
  fileMol_ = fileMol;
  setSceneRadius(fileMol_.molecule()->radius() + 10.0f);
  showEntireScene();
  updateMolecule();
}

void Render::Viewer::setAtomicNumber(quint8 atomicNumber)
{
  atomicNumber_ = atomicNumber;
}

void Render::Viewer::updateMolecule()
{
  updateRenderAtoms();
  updateRenderBonds();
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

void Render::Viewer::displayConformer(quint16 index)
{
  fileMol_.molecule()->setConformer(index);
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
//    if (e->modifiers() == (Qt::NoModifier | Qt::MetaModifier))
//    {
      QGLViewer::mouseMoveEvent(e);
//    }
    break;
  default:
    break;
  }
}

void Render::Viewer::mousePressEvent(QMouseEvent* e)
{
//  for (int i = 0; i < bondsList_.size(); ++i)
//    bondsList_[i].draw(Render::Bond::DrawStyleStick);

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

void Render::Viewer::updateRenderAtoms()
{
  atomsList_.clear();
  for (quint16 i = 0; i < fileMol_.molecule()->atomsCount(); ++i)
  {
    Atom atom(fileMol_.molecule()->obAtom(i));
    addAtom(atom);
  }
}

void Render::Viewer::updateRenderBonds()
{
  bondsList_.clear();
  for (quint16 i = 0; i < fileMol_.molecule()->bondsCount(); ++i)
  {
    Bond bond(fileMol_.molecule()->obBond(i));
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
    camera()->setRevolveAroundPoint(qglviewer::Vec(
        atomsList_[selectedName()].centre().x(),
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
  }
}

void Render::Viewer::mouseLeftButtonWithCtrlPressEvent(QMouseEvent* e)
{
  if (isSomethingUnderPixel(e->pos()))
  {
    if (selectedName() < atomsList_.size())
    {
      atomSelectedBefore_ = &atomsList_[selectedName()];
      obAtomNew_ = new OpenBabel::OBAtom();
      obAtomNew_->SetAtomicNum(atomicNumber_);
      obAtomNew_->SetVector(atomSelectedBefore_->obAtom()->GetVector());
    }
    else
    {
      bondsList_[selectedName() - atomsList_.size()].cycleOrder();
    }
  }
  else
  {
    atomSelectedBefore_ = 0;
    if (fileMol_.molecule()->atomsCount() == 0)
    {
      fileMol_.molecule()->newAtom(atomicNumber_);
      atomsList_ << Render::Atom(
          fileMol_.molecule()->obAtom(fileMol_.molecule()->atomsCount() - 1));
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
  case (Qt::ControlModifier):
    mouseRightButtonWithCtrlPressEvent(e);
    break;
  default:
    break;
  }
}

void Render::Viewer::mouseRightButtonWithNoModifierPressEvent(QMouseEvent* e)
{
  QGLViewer::mousePressEvent(e);
}

void Render::Viewer::mouseRightButtonWithCtrlPressEvent(QMouseEvent* e)
{
  if (isSomethingUnderPixel(e->pos()))
  {
    if (selectedName() < atomsList_.size())
    {
      OpenBabel::OBAtom* obAtom = atomsList_[selectedName()].obAtom();
      for(OpenBabel::OBAtomBondIter b(obAtom); b; ++b)
      {
        for (quint16 i = 0; i < bondsList_.count(); ++i)
        {
          if (bondsList_[i].obBond() == &*b)
          {
            bondsList_.removeAt(i);
          }
        }
      }
      fileMol_.molecule()->deleteAtom(obAtom);
      atomsList_.removeAt(selectedName());
    }
    else
    {
      fileMol_.molecule()->deleteBond(bondsList_[selectedName() - atomsList_.size()].obBond());
      bondsList_.removeAt(selectedName() - atomsList_.size());
    }
  }
}

void Render::Viewer::mouseLeftButtonReleaseEvent(QMouseEvent* e)
{
  switch (e->modifiers())
  {
  case Qt::NoModifier:
    for (int i = 0; i < bondsList_.size(); ++i)
      bondsList_[i].setPlaneNormalVector(
          Eigen::Vector3f(camera()->position().x,
                          camera()->position().y,
                          camera()->position().z));
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
    if (atomSelectedBefore_ != 0)
    {
      if (manipulatedFrame()->position() == qglviewer::Vec())
      {
        atomSelectedBefore_->setAtomicNumber(obAtomNew_->GetAtomicNum());
        for(OpenBabel::OBAtomBondIter b(atomSelectedBefore_->obAtom()); b; ++b)
        {
          for (quint16 i = 0; i < bondsList_.count(); ++i)
          {
            if (bondsList_[i].obBond() == &*b)
            {
              bondsList_[i].update();
            }
          }
        }
      }
      else
      {
        if (!isSomethingUnderPixel(e->pos()))
        {
          OpenBabel::vector3 p(
              obAtomNew_->x() + manipulatedFrame()->position().x,
              obAtomNew_->y() + manipulatedFrame()->position().y,
              obAtomNew_->z() + manipulatedFrame()->position().z);
          obAtomNew_->SetVector(p);
          fileMol_.molecule()->addObAtom(*obAtomNew_);
          fileMol_.molecule()->addBond(
              atomSelectedBefore_->obAtom(),
              fileMol_.molecule()->obAtom(
                  fileMol_.molecule()->atomsCount() - 1), 1);
          atomsList_ << Render::Atom(
              fileMol_.molecule()->obAtom(fileMol_.molecule()->atomsCount() - 1));
          bondsList_ << Render::Bond(
              fileMol_.molecule()->obBond(fileMol_.molecule()->bondsCount() - 1));
        }
        else
        {
          fileMol_.molecule()->addBond(atomSelectedBefore_->obAtom(),
                                       atomsList_[selectedName()].obAtom(),
                                       1);
          bondsList_ << Render::Bond(
              fileMol_.molecule()->obBond(fileMol_.molecule()->bondsCount() - 1));
        }
      }
    }
  }

  if (atomSelectedBefore_ != 0)
  {
    delete obAtomNew_;
    atomSelectedBefore_ = 0;
  }

  manipulatedFrame()->setPosition(0.0f, 0.0f, 0.0f);
}
