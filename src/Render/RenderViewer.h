/**********************************************************************
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

#include "Chemistry/ChemistryAtom.h"
#include "Chemistry/ChemistryBond.h"
#include "File/FileMol.h"
#include "Render/RenderColor.h"
#include "Render/RenderConstants.h"


namespace Render
{
  class Viewer : public QGLViewer
  {
    Q_OBJECT

  public:
    Viewer(QWidget* parent, const File::Mol& fileMol);
    ~Viewer();

    File::Mol& fileMol();
    const File::Mol& fileMol() const;
    //    void setFileMol(const File::Mol& fileMol);

    bool isAxesVisible() const;
    float axesSize() const;
    bool isDebugInfoVisible() const;

    Render::View view() const;

    Render::LabelsOnAtoms labelsOnAtoms() const;
    const QFont& labelsOnAtomsFont() const;
    void setLabelsOnAtomsFont(const QFont& font);

    Render::LabelsOnBonds labelsOnBonds() const;
    const QFont& labelsOnBondsFont() const;
    void setLabelsOnBondsFont(const QFont& font);

    QString atomSymbol() const;

    //    void displayConformer(quint16 index);

    bool isSomethingUnderPixel(const QPoint& pixel);

    Eigen::Vector3f cameraPosition() const;
    Eigen::Vector3f cameraViewDirection() const;

  public slots:
    void setAxesVisible(bool visible);
    void setAxesSize(double size);
    void setDebugInfoVisible(bool visible);
    void setView(int view);
    void setLabelsOnAtoms(int labelsOnAtoms);
    void setLabelsOnBonds(int labelsOnBonds);
    void setAtomSymbol(const QString& atomSymbol);

  protected:
    virtual void init();
    virtual void draw();
    virtual void fastDraw();
    virtual void drawWithNames();

    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

  private:
    File::Mol fileMol_;
    Render::View view_;

    bool isDebugInfoVisible_;
    bool isAxesVisible_;
    GLfloat axesSize_;

    // added atom atomic number
    quint8 atomicNumber_;
    qint32 atomSelectedBeforeIndex_;

    Render::LabelsOnAtoms labelsOnAtoms_;
    Render::LabelsOnBonds labelsOnBonds_;
    QFont labelsOnAtomsFont_;
    QFont labelsOnBondsFont_;

    GLfloat atomDrawRadius(const Chemistry::Atom& atom) const;
    Render::Color atomColor(const Chemistry::Atom& atom) const;
    void drawAtom(const Chemistry::Atom& atom, bool fast) const;

    void drawBond(const Chemistry::Bond& bond, bool fast) const;

    void drawMolecule(bool fast) const;
    void drawMoleculeWithNames() const;

    void drawAxes(float size) const;
    void drawDebugInfo();

    void mouseLeftButtonPressEvent(QMouseEvent* e);
    void mouseLeftButtonWithNoModifierPressEvent(QMouseEvent* e);
    void mouseLeftButtonWithShiftPressEvent(QMouseEvent* e);
    void mouseLeftButtonWithCtrlPressEvent(QMouseEvent* e);

    void mouseRightButtonPressEvent(QMouseEvent* e);
    void mouseRightButtonWithNoModifierPressEvent(QMouseEvent* e);
    void mouseRightButtonWithCtrlPressEvent(QMouseEvent* e);

    void mouseLeftButtonReleaseEvent(QMouseEvent* e);
    void mouseLeftButtonWithCtrlReleaseEvent(QMouseEvent* e);
  };
}

#endif // RENDER_VIEWER_H
