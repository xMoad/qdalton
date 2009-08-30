TEMPLATE = app
INCLUDEPATH += src \
    include
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = obj
HEADERS += src/MainWindow.h \
    src/Render/RenderGL.h \
    src/Render/RenderGLU.h \
    src/Render/RenderArrow.h \
    src/Render/RenderAtom.h \
    src/Render/RenderBond.h \
    src/Render/RenderColor.h \
    src/Render/RenderCone.h \
    src/Render/RenderCylinder.h \
    src/Render/RenderMaterial.h \
    src/Render/RenderStyle.h \
    src/Render/RenderQuadric.h \
    src/Render/RenderQuality.h \
    src/Render/RenderSphere.h \
    src/Render/RenderViewer.h \
    src/File/FileText.h \
    src/File/FileMol.h \
    src/File/FileJob.h \
    src/File/FileSubmodule.h \
    src/File/FileParametre.h \
    src/File/FileModule.h \
    src/File/FileDal.h \
    src/AboutDialog.h \
    src/Chemistry/ChemistryAtom.h \
    src/Chemistry/ChemistryMolecule.h \
    src/Chemistry/ChemistryBond.h
SOURCES = src/main.cpp \
    src/MainWindow.cpp \
    src/Render/RenderArrow.cpp \
    src/Render/RenderAtom.cpp \
    src/Render/RenderBond.cpp \
    src/Render/RenderColor.cpp \
    src/Render/RenderCone.cpp \
    src/Render/RenderCylinder.cpp \
    src/Render/RenderMaterial.cpp \
    src/Render/RenderQuadric.cpp \
    src/Render/RenderSphere.cpp \
    src/Render/RenderViewer.cpp \
    src/File/FileText.cpp \
    src/File/FileMol.cpp \
    src/File/FileJob.cpp \
    src/File/FileSubmodule.cpp \
    src/File/FileParametre.cpp \
    src/File/FileModule.cpp \
    src/File/FileDal.cpp \
    src/AboutDialog.cpp \
    src/Chemistry/ChemistryAtom.cpp \
    src/Chemistry/ChemistryMolecule.cpp \
    src/Chemistry/ChemistryBond.cpp
FORMS += src/MainWindow.ui \
    src/AboutDialog.ui
RESOURCES += rsrc/icons.qrc \
    rsrc/images.qrc
QT *= xml \
    opengl

# CONFIG -= debug \
# debug_and_release
# CONFIG += debug \
# qt \
# opengl \
# warn_on \
# thread \
# console \
# rtti \
# embed_manifest_exe
win32 {
  DESTDIR = ..\Binaries\Windows
  debug{
    LIBS *= -L..\Libraries\Windows \
      -ldQGLViewer2
  }
  release{
    LIBS *= -L..\Libraries\Windows \
      -lQGLViewer2
  }
}

macx {
  DESTDIR = ../Binaries/MacOS
  LIBS *= -framework \
    QGLViewer
}

linux-g++ {
  DESTDIR = ../Binaries/Linux
  LIBS *= -lQGLViewer
}
