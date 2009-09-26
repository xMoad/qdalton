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

#include <openbabel/mol.h>
#include <openbabel/forcefield.h>
#include <openbabel/builder.h>

#include "Render/RenderViewer.h"
#include "Render/RenderSphere.h"
#include "Render/RenderCylinder.h"
#include "Render/RenderMaterial.h"
#include "Render/RenderArrow.h"

Render::Viewer::Viewer(QWidget* parent) :
    QGLViewer(parent),
    view_(VIEW_BALLS_STICKS),
    mode_(MODE_VIEW)
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
  case VIEW_BALLS_STICKS:
    glCallList(mediumBallsHigh_);
    glCallList(bondsHigh_);
    break;
  case VIEW_STICKS:
    glCallList(smallBallsHigh_);
    glCallList(sticksHigh_);
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

    renderText(20, 30, "Mode: View", font);
    renderText(20, 45,
               QString("Molecule: %1 (%2 atoms)").arg(
                   QString::fromStdString(obmol_.GetFormula()),
                   QString::number(obmol_.NumAtoms())),
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
  case VIEW_BALLS_STICKS:
    glCallList(mediumBallsLow_);
    glCallList(bondsLow_);
    break;
  case VIEW_STICKS:
    glCallList(smallBallsLow_);
    glCallList(sticksLow_);
    break;
  case VIEW_VDW:
    glCallList(bigBallsLow_);
    break;
  }
  glCallList(selectionsLow_);
}

void Render::Viewer::drawWithNames()
{
  glInitNames();
  for (int i = 0; i < atomsList_.size(); ++i)
  {
    glPushName(i);
    {
      switch (view_)
      {
      case VIEW_BALLS_STICKS:
        atomsList_[i].draw(Render::Atom::DRAW_STYLE_ATOM, QUALITY_LOW);
        break;
      case VIEW_STICKS:
        atomsList_[i].draw(Render::Atom::DRAW_STYLE_CONNECTOR, QUALITY_LOW);
        break;
      case VIEW_VDW:
        atomsList_[i].draw(Render::Atom::DRAW_STYLE_VDW, QUALITY_LOW);
        break;
      }
    }
    glPopName();
  }

  for (int i = 0; i < bondsList_.size(); ++i)
  {
    glPushName(i);
    {
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
    atomsList_[selectedName()].toggleSelected();
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
//  const float    LIGHT_AMBIENT[4]                  = {0.2f, 0.2f, 0.2f, 1.0f};
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
  // If so one can try to glDisable(GL_CULL_FACE). It works!
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
  glEnable( GL_LIGHTING );

  glLightfv( GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, LIGHT0_DIFFUSE );
  glLightfv( GL_LIGHT0, GL_SPECULAR, LIGHT0_SPECULAR );
  glLightfv( GL_LIGHT0, GL_POSITION, LIGHT0_POSITION );
  glEnable( GL_LIGHT0 );

  // Create a second light source to illuminate shadows a little better
  glLightfv( GL_LIGHT1, GL_AMBIENT, LIGHT_AMBIENT );
  glLightfv( GL_LIGHT1, GL_DIFFUSE, LIGHT1_DIFFUSE );
  glLightfv( GL_LIGHT1, GL_SPECULAR, LIGHT1_SPECULAR );
  glLightfv( GL_LIGHT1, GL_POSITION, LIGHT1_POSITION );
  glEnable( GL_LIGHT1 );

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
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  {
    for (int i = 0; i < atomsList_.size(); ++i)
    {
      if (atomsList_[i].isSelected())
      {
        switch (view_)
        {
        case VIEW_BALLS_STICKS:
          atomsList_[i].drawSelection(Render::Atom::DRAW_STYLE_ATOM, quality);
          break;
        case VIEW_STICKS:
          atomsList_[i].drawSelection(Render::Atom::DRAW_STYLE_CONNECTOR, quality);
          break;
        case VIEW_VDW:
          atomsList_[i].drawSelection(Render::Atom::DRAW_STYLE_VDW, quality);
          break;
        }
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

void Render::Viewer::setMolecule(const OpenBabel::OBMol& obmol)
{
  obmol_ = obmol;
  updateMolecule();
}

bool Render::Viewer::isMoleculeEmpty() const
{
  return obmol_.NumAtoms() == 0;
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
  OpenBabel::OBBuilder obbuilder;
  obbuilder.Build(obmol_);
  optimize(Render::Viewer::FF_UFF,
           Render::Viewer::ALGORITHM_STEEPEST_DESCENT,
           1.0e-7,
           50,
           0);
  obmol_.Center();
  updateMolecule();
}

void Render::Viewer::addHydrogensAndBuild()
{
  OpenBabel::OBBuilder obbuilder;
  obbuilder.Build(obmol_);
  obmol_.AddHydrogens();
  obbuilder.Build(obmol_);
  obmol_.Center();
  updateMolecule();
}

void Render::Viewer::removeHydrogens()
{
  obmol_.DeleteHydrogens();
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
        currentOBAtom_ = obmol_.GetAtom(selectedName()+1);
//        currentOBAtom_->SetAtomicNum(atomicNumber_);
        newOBAtom_ = new OpenBabel::OBAtom();
        newOBAtom_->SetAtomicNum(atomicNumber_);
        newOBAtom_->SetVector(currentOBAtom_->GetVector());
      }
      else
      {
        if (obmol_.NumAtoms() == 0)
        {
          obmol_.BeginModify();
          newOBAtom_ = obmol_.NewAtom();
          newOBAtom_->SetAtomicNum(atomicNumber_);
          obmol_.EndModify();
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
        obmol_.BeginModify();
        obmol_.DeleteAtom(atomsList_[selectedName()].obatom());
        obmol_.EndModify();
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
      OpenBabel::OBBond* obbond;

      beginSelection(e->pos());
      drawWithNames();
      endSelection(e->pos());

      // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
      bool found;
      selectedPoint = camera()->pointUnderPixel(e->pos(), found);

      obmol_.BeginModify();

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
#ifdef Q_CC_MSVC
#undef AddAtom
#endif
            obmol_.AddAtom(*newOBAtom_);
            obbond = obmol_.NewBond();
            obbond->SetBegin(currentOBAtom_);
            obbond->SetEnd(obmol_.GetAtom(obmol_.NumAtoms()));
            obbond->SetBondOrder(1);
            currentOBAtom_->AddBond(obbond);
            obmol_.GetAtom(obmol_.NumAtoms())->AddBond(obbond);
          }
          else
          {
            obbond = obmol_.NewBond();
            obbond->SetBegin(currentOBAtom_);
            obbond->SetEnd(atomsList_[selectedName()].obatom());
            obbond->SetBondOrder(1);
            currentOBAtom_->AddBond(obbond);
            atomsList_[selectedName()].obatom()->AddBond(obbond);
          }
        }
        delete newOBAtom_;
        currentOBAtom_ = 0;
      }

      obmol_.EndModify();
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
  for(OpenBabel::OBMolAtomIter obatom(obmol_); obatom; ++obatom)
  {
    Atom atom(&*obatom);
    addAtom(atom);
  }
}

void Render::Viewer::updateRenderBonds()
{
  bondsList_.clear();
  for(OpenBabel::OBMolBondIter obbond(obmol_); obbond; ++obbond)
  {
    Bond bond(&*obbond);
    addBond(bond);
  }
}

void Render::Viewer::optimize(ForceField forceField,
                              Algorithm algorithm,
                              double convergenceCriteria,
                              quint16 maxSteps,
                              quint8 stepsPerUpdate)
{
  OpenBabel::OBForceField* ff;

  switch (forceField)
  {
  case (Render::Viewer::FF_GHEMICAL):
    ff = OpenBabel::OBForceField::FindForceField("Ghemical");
    break;
  case (Render::Viewer::FF_MMFF94):
    ff = OpenBabel::OBForceField::FindForceField("MMFF94");
    break;
  case (Render::Viewer::FF_MMFF94s):
    ff = OpenBabel::OBForceField::FindForceField("MMFF94s");
    break;
  case (Render::Viewer::FF_UFF):
    ff = OpenBabel::OBForceField::FindForceField("UFF");
    break;
  default:
    break;
  }

  if (!ff)
  {
    std::cout << "No FF!";
  }

  if (!ff->Setup(obmol_))
  {
    std::cout << "Setup error!";
  }

  ff->SetLogFile(&std::cout);
  ff->SetLogLevel(OBFF_LOGLVL_LOW);

  switch (algorithm)
  {
  case (ALGORITHM_STEEPEST_DESCENT):
    if (stepsPerUpdate != 0)
    {
      ff->SteepestDescentInitialize(maxSteps, convergenceCriteria);
      while (ff->SteepestDescentTakeNSteps(stepsPerUpdate))
      {
        ff->GetCoordinates(obmol_);
        updateMolecule();
      }
    }
    else
    {
      ff->SteepestDescent(maxSteps, convergenceCriteria);
      ff->GetCoordinates(obmol_);
    }
    break;
  case (ALGORITHM_CONJUGATE_GRADIENTS):
    if (stepsPerUpdate != 0)
    {
      ff->ConjugateGradientsInitialize(maxSteps, convergenceCriteria);
      while (ff->ConjugateGradientsTakeNSteps(stepsPerUpdate))
      {
        ff->GetCoordinates(obmol_);
        updateMolecule();
      }
    }
    else
    {
      ff->ConjugateGradients(maxSteps, convergenceCriteria);
      ff->GetCoordinates(obmol_);
    }
    break;
  }
  updateMolecule();
}
