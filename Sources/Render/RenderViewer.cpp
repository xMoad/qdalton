/**********************************************************************
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

Render::Viewer::Viewer(QWidget* parent, const File::Mol& fileMol) :
    QGLViewer(parent),
    fileMol_(fileMol),
    view_(Render::ViewBallsAndBonds),
    isDebugInfoVisible_(false),
    isAxesVisible_(false),
    axesSize_(1.0f),
    atomicNumber_(6),
    atomSelectedBeforeIndex_(-1),
    labelsOnAtoms_(Render::LabelsOnAtomsNone),
    labelsOnBonds_(Render::LabelsOnBondsNone),
    labelsOnAtomsFont_(),
    labelsOnBondsFont_()
{
  setMouseBinding(Qt::CTRL + Qt::LeftButton, FRAME, TRANSLATE);
  camera()->frame()->setSpinningSensitivity(100.0f);
}

Render::Viewer::~Viewer()
{
}

void Render::Viewer::draw()
{
  if (isAxesVisible_)
    drawAxes(axesSize_);

  drawMolecule(false);

  if (isDebugInfoVisible_)
    drawDebugInfo();

  if (labelsOnAtoms() != Render::LabelsOnAtomsNone)
  {
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    glColor3f(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < fileMol().molecule().atomsCount(); ++i)
    {
      Chemistry::Atom& atom = fileMol().molecule().atom(i);
      Eigen::Vector3f v = atom.centre() - cameraPosition();
      v = v * (v.norm() - atomDrawRadius(atom) - 0.1f) / v.norm();
      v = v + cameraPosition();

      QString text;

      switch (labelsOnAtoms())
      {
      case Render::LabelsOnAtomsSymbol:
        text = atom.symbol();
        break;
      case Render::LabelsOnAtomsIndex:
        text = QString::number(atom.index() + 1);
        break;
      }

      renderText(v.x(), v.y(), v.z(), text, labelsOnAtomsFont());
    }

    glColor4fv(color);
  }

  if (labelsOnBonds() != Render::LabelsOnBondsNone)
  {
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    glColor3f(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < fileMol().molecule().bondsCount(); ++i)
    {
      Chemistry::Bond& bond = fileMol().molecule().bond(i);
      Eigen::Vector3f v = bond.centre() - cameraPosition();
      v = v * (v.norm() - 1.0f) / v.norm();
      v = v + cameraPosition();

      QString text;

      switch (labelsOnBonds())
      {
      case Render::LabelsOnBondsLength:
        text = QString::number(bond.length(), 'g', 3);
        break;
      }

      renderText(v.x(), v.y(), v.z(), text, labelsOnBondsFont());
    }

    glColor4fv(color);
  }
}

void Render::Viewer::fastDraw()
{
  if (isAxesVisible_)
    drawAxes(axesSize_);

  drawMolecule(true);
}

void Render::Viewer::drawWithNames()
{
  drawMoleculeWithNames();
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

  glEnable(GL_NORMALIZE);

  // As mentioned in QGLViewer documentation for select() method,
  // one can encounter problems with backface culling.
  // If so one can try to use glDisable(GL_CULL_FACE).
  glDisable(GL_CULL_FACE);

  setManipulatedFrame(new qglviewer::ManipulatedFrame());

  setSceneRadius(fileMol_.molecule().radius() + 1.0f);
  showEntireScene();
}

void Render::Viewer::drawAxes(float size) const
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
  renderText(20, 60, QString("View direction: %1 %2 %3").arg(
      camera()->viewDirection().x).arg(
          camera()->viewDirection().y).arg(
              camera()->viewDirection().z), font);
  renderText(20, 75, QString("Camera position: %1 %2 %3").arg(
      cameraPosition().x()).arg(
          cameraPosition().y()).arg(
              cameraPosition().z()), font);
  renderText(20, 90, QString("%1 %2 %3").arg(
      fileMol().molecule().centreOfMass().x()).arg(
          fileMol().molecule().centreOfMass().y()).arg(
              fileMol().molecule().centreOfMass().z()), font);

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

void Render::Viewer::setLabelsOnAtoms(int labelsOnAtoms)
{
  labelsOnAtoms_ = (Render::LabelsOnAtoms)labelsOnAtoms;
  updateGL();
}

const QFont& Render::Viewer::labelsOnAtomsFont() const
{
  return labelsOnAtomsFont_;
}

void Render::Viewer::setLabelsOnAtomsFont(const QFont& font)
{
  labelsOnAtomsFont_ = font;
  updateGL();
}

Render::LabelsOnBonds Render::Viewer::labelsOnBonds() const
{
  return labelsOnBonds_;
}

void Render::Viewer::setLabelsOnBonds(int labelsOnBonds)
{
  labelsOnBonds_ = (Render::LabelsOnBonds)labelsOnBonds;
  updateGL();
}

const QFont& Render::Viewer::labelsOnBondsFont() const
{
  return labelsOnBondsFont_;
}

void Render::Viewer::setLabelsOnBondsFont(const QFont& font)
{
  labelsOnBondsFont_ = font;
  updateGL();
}

void Render::Viewer::setView(int view)
{
  view_ = (Render::View)view;
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

const File::Mol& Render::Viewer::fileMol() const
{
  return fileMol_;
}

//void Render::Viewer::setFileMol(const File::Mol& fileMol)
//{
//  fileMol_ = fileMol;
//  setSceneRadius(fileMol_.molecule().radius() + 5.0f);
//  showEntireScene();
//}

//void Render::Viewer::displayConformer(quint16 index)
//{
//  fileMol().molecule().setConformer(index);
//}

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

GLfloat Render::Viewer::atomDrawRadius(const Chemistry::Atom& atom) const
{
#ifdef Q_CC_MSVC
  return pow(atom.exactMass(), 1.0f/3.0f) / 10.0f + 0.2f;
#else
  return cbrt(atom.exactMass()) / 10.0f + 0.2f;
#endif
}

Render::Color Render::Viewer::atomColor(const Chemistry::Atom& atom) const
{
  std::vector<double> rgb =
      OpenBabel::etab.GetRGB(atom.atomicNumber());
  return Render::Color(rgb[0], rgb[1], rgb[2], 1.0f);
}

void Render::Viewer::drawAtom(const Chemistry::Atom& atom,
                              bool fast) const
{
  Render::Sphere sphere;

  sphere.setCentre(atom.centre());
  if (atom.isSelected())
    sphere.setMaterial(Render::Material::selection());
  else
    sphere.setMaterial(Render::Material(atomColor(atom), true));

  switch (view_)
  {
  case Render::ViewBallsAndSticks:
  case Render::ViewBallsAndBonds:
    sphere.setRadius(atomDrawRadius(atom));
    break;
  case Render::ViewSticks:
    sphere.setRadius(Render::stickThikness);
    break;
  case Render::ViewVdWSpheres:
    sphere.setRadius(atom.vanDerWaalsRadius());
    break;
  }

  if (fast)
    sphere.draw(Render::StyleFill, Render::slicesForFastDrawing);
  else
    sphere.draw(Render::StyleFill, Render::slicesForDrawing);
}

void Render::Viewer::drawBond(const Chemistry::Bond& bond,
                              bool fast) const
{
  float shift;

  if (bond.bondOrder() < 3)
    shift = 0.125f;
  else
    shift = 0.2f;

  Render::Cylinder cylinder1;
  Render::Cylinder cylinder2;

  // Compute the centre of bond
  Eigen::Vector3f vec1 = bond.endAtom().centre() - bond.beginAtom().centre();
  vec1 = vec1 * (vec1.norm() - atomDrawRadius(bond.endAtom())) / vec1.norm();
  vec1 = vec1 + bond.beginAtom().centre();
  Eigen::Vector3f vec2 = bond.beginAtom().centre() - bond.endAtom().centre();
  vec2 = vec2 * (vec2.norm() - atomDrawRadius(bond.beginAtom())) / vec2.norm();
  vec2 = vec2 + bond.endAtom().centre();
  Eigen::Vector3f vMiddle = (vec1 + vec2) / 2;

  cylinder1.setVertex1(bond.beginAtom().centre());
  cylinder1.setVertex2(vMiddle);

  cylinder2.setVertex1(vMiddle);
  cylinder2.setVertex2(bond.endAtom().centre());

  if (bond.isSelected())
  {
    cylinder1.setMaterial(Render::Material::selection());
    cylinder2.setMaterial(Render::Material::selection());
  }
  else
  {
    cylinder1.setMaterial(Render::Material(atomColor(bond.beginAtom()), true));
    cylinder2.setMaterial(Render::Material(atomColor(bond.endAtom()), true));
  }

  switch (view_)
  {
  case Render::ViewBallsAndBonds:
    cylinder1.setRadius(Render::bondThikness);
    cylinder2.setRadius(Render::bondThikness);
    if (fast)
    {
      cylinder1.drawMulti(Render::StyleFill,
                          bond.bondOrder(),
                          shift,
                          fileMol().molecule().planeNormalVector(),
                          Render::slicesForFastDrawing);
      cylinder2.drawMulti(Render::StyleFill,
                          bond.bondOrder(),
                          shift,
                          fileMol().molecule().planeNormalVector(),
                          Render::slicesForFastDrawing);
    }
    else
    {
      cylinder1.drawMulti(Render::StyleFill,
                          bond.bondOrder(),
                          shift,
                          fileMol().molecule().planeNormalVector(),
                          Render::slicesForDrawing);
      cylinder2.drawMulti(Render::StyleFill,
                          bond.bondOrder(),
                          shift,
                          fileMol().molecule().planeNormalVector(),
                          Render::slicesForDrawing);
    }
    break;
  case Render::ViewBallsAndSticks:
  case Render::ViewSticks:
    cylinder1.setRadius(Render::stickThikness);
    cylinder2.setRadius(Render::stickThikness);
    if (fast)
    {
      cylinder1.draw(Render::StyleFill, Render::slicesForFastDrawing);
      cylinder2.draw(Render::StyleFill, Render::slicesForFastDrawing);
    }
    else
    {
      cylinder1.draw(Render::StyleFill, Render::slicesForDrawing);
      cylinder2.draw(Render::StyleFill, Render::slicesForDrawing);
    }
    break;
  case Render::ViewVdWSpheres:
    break;
  }
}

void Render::Viewer::drawMolecule(bool fast) const
{
  for (int i = 0; i < fileMol().molecule().atomsCount(); ++i)
    drawAtom(fileMol().molecule().atom(i), fast);

  if (view_ != Render::ViewVdWSpheres)
  {
    for (int i = 0; i < fileMol().molecule().bondsCount(); ++i)
      drawBond(fileMol().molecule().bond(i), fast);
  }
}

void Render::Viewer::drawMoleculeWithNames() const
{
  int n = 0;

  glInitNames();

  for (int i = 0; i < fileMol().molecule().atomsCount(); ++i)
  {
    glPushName(n);
    {
      drawAtom(fileMol().molecule().atom(i), false);
    }
    glPopName();
    n++;
  }

  if (view_ != Render::ViewVdWSpheres)
    for (int i = 0; i < fileMol().molecule().bondsCount(); ++i)
    {
    glPushName(n);
    {
      drawBond(fileMol().molecule().bond(i),
                   false);
    }
    glPopName();
    n++;
  }
}

Eigen::Vector3f Render::Viewer::cameraPosition() const
{
  return Eigen::Vector3f(camera()->position().x,
                         camera()->position().y,
                         camera()->position().z);
}

Eigen::Vector3f Render::Viewer::cameraViewDirection() const
{
  return Eigen::Vector3f(camera()->viewDirection().x,
                         camera()->viewDirection().y,
                         camera()->viewDirection().z);
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
  if (isSomethingUnderPixel(e->pos()) &&
      selectedName() < fileMol().molecule().atomsCount())
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
      fileMol().molecule().atom(selectedName()).toggleSelection();
    else
      fileMol().molecule().bond(
          selectedName() - fileMol().molecule().atomsCount()).toggleSelection();
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
          selectedName() - fileMol().molecule().atomsCount()).cycleBondOrder();
  }
  else
  {
    atomSelectedBeforeIndex_ = -1;
    if (fileMol().molecule().atomsCount() == 0)
      fileMol().molecule().newAtom(atomicNumber_, 0, Eigen::Vector3f(0.0f, 0.0f, 0.0f));
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
          fileMol().molecule().newAtom(atomicNumber_, 0, p);
          fileMol().molecule().newBond(
              atomSelectedBeforeIndex_,
              fileMol().molecule().atomsCount() - 1,
              1);
        }
        else
        {
          fileMol().molecule().newBond(
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
