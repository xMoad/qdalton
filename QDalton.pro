QT *= core gui opengl xml
TEMPLATE = app
CONFIG += debug_and_release
INCLUDEPATH += Sources Include

HEADERS += Sources/MainWindow.h \
    Sources/Render/RenderGL.h \
    Sources/Render/RenderGLU.h \
    Sources/Render/RenderArrow.h \
    Sources/Render/RenderAtom.h \
    Sources/Render/RenderBond.h \
    Sources/Render/RenderColor.h \
    Sources/Render/RenderCone.h \
    Sources/Render/RenderCylinder.h \
    Sources/Render/RenderMaterial.h \
    Sources/Render/RenderStyle.h \
    Sources/Render/RenderQuadric.h \
    Sources/Render/RenderQuality.h \
    Sources/Render/RenderSphere.h \
    Sources/Render/RenderViewer.h \
    Sources/File/FileText.h \
    Sources/File/FileMol.h \
    Sources/File/FileJob.h \
    Sources/File/FileSubmodule.h \
    Sources/File/FileParametre.h \
    Sources/File/FileModule.h \
    Sources/File/FileDal.h \
    Sources/AboutDialog.h \
    Sources/Chemistry/ChemistryAtom.h \
    Sources/Chemistry/ChemistryMolecule.h \
    Sources/Chemistry/ChemistryBond.h
SOURCES = Sources/main.cpp \
    Sources/MainWindow.cpp \
    Sources/Render/RenderArrow.cpp \
    Sources/Render/RenderAtom.cpp \
    Sources/Render/RenderBond.cpp \
    Sources/Render/RenderColor.cpp \
    Sources/Render/RenderCone.cpp \
    Sources/Render/RenderCylinder.cpp \
    Sources/Render/RenderMaterial.cpp \
    Sources/Render/RenderQuadric.cpp \
    Sources/Render/RenderSphere.cpp \
    Sources/Render/RenderViewer.cpp \
    Sources/File/FileText.cpp \
    Sources/File/FileMol.cpp \
    Sources/File/FileJob.cpp \
    Sources/File/FileSubmodule.cpp \
    Sources/File/FileParametre.cpp \
    Sources/File/FileModule.cpp \
    Sources/File/FileDal.cpp \
    Sources/AboutDialog.cpp \
    Sources/Chemistry/ChemistryAtom.cpp \
    Sources/Chemistry/ChemistryMolecule.cpp \
    Sources/Chemistry/ChemistryBond.cpp
FORMS += Sources/MainWindow.ui \
    Sources/AboutDialog.ui
RESOURCES += Resources/icons.qrc \
    Resources/images.qrc

CONFIG(debug, debug|release) {
  MOC_DIR = Build/Temp/debug
  RCC_DIR = Build/Temp/debug
  UI_DIR = Build/Temp/debug
  OBJECTS_DIR = Build/Objects/debug
#  TARGET = QDalton-debug
} else {
  MOC_DIR = Build/Temp/release
  RCC_DIR = Build/Temp/release
  UI_DIR = Build/Temp/release
  OBJECTS_DIR = Build/Objects/release
#  TARGET = QDalton
}

win32 {
  DESTDIR = Build/Binaries/Windows
  CONFIG(debug, debug|release) {
    LIBS += -LLibraries/Windows -ldQGLViewer2
  } else {
    LIBS += -LLibraries/Windows -lQGLViewer2
  }
}

macx {
  DESTDIR = Build/Binaries/MacOS
  CONFIG(debug, debug|release) {
    LIBS += -framework QGLViewer_debug
  } else {
    LIBS += -framework QGLViewer
  }
}

linux-g++ {
  DESTDIR = Binaries/Linux
  LIBS *= -lQGLViewer
}
