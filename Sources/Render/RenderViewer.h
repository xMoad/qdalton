#ifndef RENDER_VIEWER_H
#define RENDER_VIEWER_H

#include <QMouseEvent>
#include <QGLViewer/qglviewer.h>

#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderQuality.h"

namespace Render
{
  class Viewer : public QGLViewer
  {
    Q_OBJECT

  public:
    enum View
    {
      VIEW_BALLS_STICKS,
      VIEW_STICKS,
      VIEW_VDW
    };
    enum Mode
    {
      MODE_VIEW,
      MODE_ADD
    };
    Viewer(QWidget* parent);
    ~Viewer();
    void setMolecule(const Chemistry::Molecule& obmol);
    void updateMolecule();
    void setView(View view);
    void setMode(Mode mode);
    void setAxes(bool visibility, GLfloat size);
    void setDebugInfoVisibility(bool visibility);
    void addAtom(const Render::Atom& atom);
    void addBond(const Render::Bond& bond);
    void setAtomicNumber(int atomicNumber);

  protected:
    virtual void draw();
    virtual void drawWithNames();
    virtual void fastDraw();
    virtual void postSelection(const QPoint& point);
    virtual void initializeGL();
    virtual QString helpString() const;

    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

  private:
    enum GLList
    {
      GLLIST_AXES,
      GLLIST_BALLS,
      GLLIST_STICKS,
      GLLIST_SELECTIONS
    };
    void updateGLList(GLList gllist);
    void updateRenderAtoms();
    void updateRenderBonds();

    Chemistry::Molecule molecule_;
    View view_;
    Mode mode_;

    // GLLists, Low & High Quality.
    GLuint axesLow_;
    GLuint axesHigh_;
    GLuint smallBallsLow_;
    GLuint smallBallsHigh_;
    GLuint mediumBallsLow_;
    GLuint mediumBallsHigh_;
    GLuint bigBallsLow_;
    GLuint bigBallsHigh_;
    GLuint sticksLow_;
    GLuint sticksHigh_;
    GLuint selectionsLow_;
    GLuint selectionsHigh_;

    // GLLists generators.
    GLuint makeAxes(GLfloat size, Quality quality);
    GLuint makeSmallBalls(Quality quality);
    GLuint makeMediumBalls(Quality quality);
    GLuint makeBigBalls(Quality quality);
    GLuint makeSticks(Quality quality);
    GLuint makeSelections(Quality quality);

    // visibility flags
    bool isDebugInfoVisible_;
    bool isAxesVisible_;
    GLfloat axesSize_;

    QPoint lastMousePosition_;
    QList<Render::Atom> atomsQList_;
    int movableIndex;
    QList<Render::Bond> bondsQList_;
    qglviewer::Vec selectedPoint;
    // added atom atomic number
    int atomicNumber_;
  };
}

#endif // RENDER_VIEWER_H
