QT *= core \
    gui \
    opengl \
    xml

# QMAKE_CXXFLAGS *= -Wold-style-cast -Wfloat-equal -Woverloaded-virtual -Wshadow -Wundef
TEMPLATE = app
CONFIG += qt \
    opengl \
    warn_on \
    thread \
    rtti \
    console \
    embed_manifest_exe
INCLUDEPATH += Sources \
    Include
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
    Sources/Render/RenderMolecule.h \
    Sources/Render/RenderStyle.h \
    Sources/Render/RenderQuadric.h \
    Sources/Render/RenderSphere.h \
    Sources/Render/RenderViewer.h \
    Sources/File/FileText.h \
    Sources/File/FileMol.h \
    Sources/File/FileSubmodule.h \
    Sources/File/FileParametre.h \
    Sources/File/FileModule.h \
    Sources/File/FileDal.h \
    Sources/AboutDialog.h \
    Sources/Chemistry/ChemistryForceField.h \
    Sources/ImportMoleculeFromFileDialog.h \
    Sources/FileMolGeneratorDialog.h
SOURCES = Sources/main.cpp \
    Sources/MainWindow.cpp \
    Sources/Render/RenderArrow.cpp \
    Sources/Render/RenderAtom.cpp \
    Sources/Render/RenderBond.cpp \
    Sources/Render/RenderColor.cpp \
    Sources/Render/RenderCone.cpp \
    Sources/Render/RenderCylinder.cpp \
    Sources/Render/RenderMaterial.cpp \
    Sources/Render/RenderMolecule.cpp \
    Sources/Render/RenderQuadric.cpp \
    Sources/Render/RenderSphere.cpp \
    Sources/Render/RenderViewer.cpp \
    Sources/File/FileText.cpp \
    Sources/File/FileMol.cpp \
    Sources/File/FileSubmodule.cpp \
    Sources/File/FileParametre.cpp \
    Sources/File/FileModule.cpp \
    Sources/File/FileDal.cpp \
    Sources/AboutDialog.cpp \
    Sources/ImportMoleculeFromFileDialog.cpp \
    Sources/FileMolGeneratorDialog.cpp
FORMS += Sources/MainWindow.ui \
    Sources/AboutDialog.ui \
    Sources/ImportMoleculeFromFileDialog.ui \
    Sources/FileMolGeneratorDialog.ui
RESOURCES += Resources/icons.qrc \
    Resources/images.qrc
win32 { 
    MOC_DIR = Build/Temp/win32
    RCC_DIR = Build/Temp/win32
    UI_DIR = Build/Temp/win32
    OBJECTS_DIR = Build/Objects/win32
    DESTDIR = Build/Binaries/Windows
    LIBS += -LLibraries/Windows \
        -lQGLViewer2 \
        -lopenbabel-2
}
macx { 
    MOC_DIR = Build/Temp/macx
    RCC_DIR = Build/Temp/macx
    UI_DIR = Build/Temp/macx
    OBJECTS_DIR = Build/Objects/macx
    DESTDIR = Build/Binaries/MacOS
    LIBS += -framework \
        QGLViewer \
        -lopenbabel
}
linux-g++ { 
    MOC_DIR = Build/Temp/linux
    RCC_DIR = Build/Temp/linux
    UI_DIR = Build/Temp/linux
    OBJECTS_DIR = Build/Objects/linux
    DESTDIR = Build/Binaries/Linux
    LIBS *= -lqglviewer-qt4 \
        -lopenbabel
}
