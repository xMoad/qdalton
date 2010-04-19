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
    view_(Render::ViewBallsAndBonds),
    isDebugInfoVisible_(false),
    isAxesVisible_(false),
    axesSize_(1.0f),
    atomicNumber_(6),
    atomSelectedBeforeIndex_(-1),
    labelsOnAtoms_(Render::LabelsOnAtomsNone),
    labelsOnAtomsFont_()
{
  setMouseBinding(Qt::CTRL + Qt::LeftButton, FRAME, TRANSLATE);
}

Render::Viewer::~Viewer()
{
}

void Render::Viewer::draw()
{
  if (isAxesVisible_)
    drawAxes(axesSize_);

  fileMol().molecule().draw(view_, false);

  if (isDebugInfoVisible_)
    drawDebugInfo();

  if (labelsOnAtoms() == Render::LabelsOnAtomsSymbol)
  {
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    glColor3f(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < fileMol().molecule().atomsCount(); ++i)
    {
      Render::Atom atom(fileMol().molecule().atom(i));
      Eigen::Vector3f v = atom.centre() - cameraPosition();
      v = v * (v.norm() - atom.drawRadius() - 0.1f) / v.norm();
      v = v + cameraPosition();

      renderText(v.x(), v.y(), v.z(), atom.symbol(), labelsOnAtomsFont());
    }

    glColor4fv(color);
  }
}

void Render::Viewer::fastDraw()
{
  if (isAxesVisible_)
    drawAxes(axesSize_);

  fileMol().molecule().draw(view_, true);
}

void Render::Viewer::drawWithNames()
{
  fileMol().molecule().drawWithNames(view_);
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

  glEnable(GL_NORMALIZE);

  // As mentioned in QGLViewer documentation for select() method,
  // one can encounter problems with backface culling.
  // If so one can try to glDisable(GL_CULL_FACE).
  glDisable(GL_CULL_FACE);

  setManipulatedFrame(new qglviewer::ManipulatedFrame());
}

void Render::Viewer::drawAxes(float size)
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

  x.draw(Render::StyleFill);
  y.draw(Render::StyleFill);
  z.draw(Render::StyleFill);
}

void Render::Viewer::drawDebugInfo()
{
  GLfloat color[4];

  glGetFloatv(GL_CURRENT_COLOR, color);

  glColor3f(0.0f, 1.0f, 0.0f);

  QFont font;

  font.setBold(true);
  renderText(10, 15, "Debug Info", font);
  font.setBold(false);

  renderText(20, 30, QString("Distance: %1").arg(sceneRadius()), font);
  renderText(20, 45,
             QString("Molecule: %1 (%2 atoms)").arg(fileMol().molecule().formula(),
                 QString::number(fileMol().molecule().atomsCount())),
             font);
  renderText(20, 60, QString("%1 %2 %3").arg(cameraPosition().x()).arg(cameraPosition().y()).arg(cameraPosition().z()), font);

  glColor4fv(color);
}

Render::View Render::Viewer::view() const
{
  return view_;
}

Render::LabelsOnAtoms Render::Viewer::labelsOnAtoms() const
{
  return labelsOnAtoms_;
}

const QFont& Render::Viewer::labelsOnAtomsFont() const
{
  return labelsOnAtomsFont_;
}

void Render::Viewer::setView(int view)
{
  view_ = (Render::View)view;
  updateGL();
}

void Render::Viewer::setLabelsOnAtoms(int labelsOnAtoms)
{
  labelsOnAtoms_ = (Render::LabelsOnAtoms)labelsOnAtoms;
  updateGL();
}

void Render::Viewer::setLabelsOnAtomsFont(const QFont& font)
{
  labelsOnAtomsFont_ = font;
  updateGL();
}

bool Render::Viewer::isAxesVisible() const
{
  return isAxesVisible_;
}

void Render::Viewer::setAxesVisible(bool visible)
{
  isAxesVisible_ = visible;
  updateGL();
}

float Render::Viewer::axesSize() const
{
  return axesSize_;
}

void Render::Viewer::setAxesSize(double size)
{
  axesSize_ = size;
  updateGL();
}

bool Render::Viewer::isDebugInfoVisible() const
{
  return isDebugInfoVisible_;
}

void Render::Viewer::setDebugInfoVisible(bool visible)
{
  isDebugInfoVisible_ = visible;
  updateGL();
}

void Render::Viewer::setAtomSymbol(const QString& atomSymbol)
{
  QByteArray byteArray = atomSymbol.toLatin1();
  const char* s_char = byteArray.data();
  atomicNumber_ = OpenBabel::etab.GetAtomicNum(s_char);
}

QString Render::Viewer::atomSymbol() const
{
  return QString(OpenBabel::etab.GetSymbol(atomicNumber_));
}

File::Mol& Render::Viewer::fileMol()
{
  return fileMol_;
}

void Render::Viewer::setFileMol(const File::Mol& fileMol)
{
  fileMol_ = fileMol;
  setSceneRadius(fileMol_.molecule().radius() + 5.0f);
  showEntireScene();
}

void Render::Viewer::displayConformer(quint16 index)
{
  fileMol().molecule().setConformer(index);
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

Eigen::Vector3f Render::Viewer::cameraPosition()
{
  return Eigen::Vector3f(camera()->position().x,
                         camera()->position().y,
                         camera()->position().z);
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
  if (isSomethingUnderPixel(e->pos()) && selectedName() < fileMol().molecule().atomsCount())
  {
    camera()->setRevolveAroundPoint(qglviewer::Vec(
        fileMol().molecule().atom(selectedName()).centre().x(),
        fileMol().molecule().atom(selectedName()).centre().y(),
        fileMol().molecule().atom(selectedName()).centre().z()));
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
    if (selectedName() < fileMol().molecule().atomsCount())
      fileMol().molecule().toggleSelectionOfAtom(selectedName());
    else
      fileMol().molecule().toggleSelectionOfBond(
          selectedName() - fileMol().molecule().atomsCount());
  }
}

void Render::Viewer::mouseLeftButtonWithCtrlPressEvent(QMouseEvent* e)
{
  if (isSomethingUnderPixel(e->pos()))
  {
    if (selectedName() < fileMol().molecule().atomsCount())
      atomSelectedBeforeIndex_ = selectedName();
    else
      fileMol().molecule().bond(
          selectedName() - fileMol().molecule().atomsCount()).cycleOrder();
  }
  else
  {
    atomSelectedBeforeIndex_ = -1;
    if (fileMol().molecule().atomsCount() == 0)
    {
      Render::Atom atom = fileMol().molecule().newAtom();
      atom.setAtomicNumber(atomicNumber_);
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
    if (selectedName() < fileMol().molecule().atomsCount())
    {
      fileMol().molecule().removeAtom(selectedName());
    }
    else
    {
      fileMol().molecule().removeBond(
          selectedName() - fileMol().molecule().atomsCount());
    }
  }
}

void Render::Viewer::mouseLeftButtonReleaseEvent(QMouseEvent* e)
{
  switch (e->modifiers())
  {
  case Qt::NoModifier:
    for (int i = 0; i < fileMol().molecule().bondsCount(); ++i)
      fileMol().molecule().setPlaneNormalVector(
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
    if (atomSelectedBeforeIndex_ != -1)
    {
      if (manipulatedFrame()->position() == qglviewer::Vec())
      {
        fileMol().molecule().atom(atomSelectedBeforeIndex_).setAtomicNumber(
            atomicNumber_);
      }
      else
      {
        if (!isSomethingUnderPixel(e->pos()))
        {
          Eigen::Vector3f p(
              fileMol().molecule().atom(atomSelectedBeforeIndex_).centre().x() +
              manipulatedFrame()->position().x,
              fileMol().molecule().atom(atomSelectedBeforeIndex_).centre().y() +
              manipulatedFrame()->position().y,
              fileMol().molecule().atom(atomSelectedBeforeIndex_).centre().z() +
              manipulatedFrame()->position().z);
          Render::Atom atom = fileMol().molecule().newAtom();
          atom.setCentre(p);
          atom.setAtomicNumber(atomicNumber_);
          fileMol().molecule().addBond(
              atomSelectedBeforeIndex_,
              fileMol().molecule().atomsCount() - 1,
              1);
        }
        else
        {
          fileMol().molecule().addBond(
              atomSelectedBeforeIndex_,
              selectedName(),
              1);
        }
      }
    }
  }

  if (atomSelectedBeforeIndex_ != -1)
    atomSelectedBeforeIndex_ = -1;

  manipulatedFrame()->setPosition(0.0f, 0.0f, 0.0f);
}
