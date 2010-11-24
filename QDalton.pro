QT *= core \
    gui \
    opengl \
    xml
TEMPLATE = app
CONFIG += qt \
    opengl \
    warn_on \
    thread \
    rtti \
    embed_manifest_exe
INCLUDEPATH += Sources
HEADERS += Sources/MainWindow.h \
    Sources/Render/RenderGL.h \
    Sources/Render/RenderGLU.h \
    Sources/Render/RenderArrow.h \
    Sources/Chemistry/ChemistryAtom.h \
    Sources/Chemistry/ChemistryBond.h \
    Sources/Render/RenderColor.h \
    Sources/Render/RenderCone.h \
    Sources/Render/RenderCylinder.h \
    Sources/Render/RenderMaterial.h \
    Sources/Chemistry/ChemistryMolecule.h \
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
    Sources/ImportMoleculeFromFileDialog.h \
    Sources/FileMolGeneratorDialog.h \
    Sources/Render/RenderConstants.h \
    Sources/Constants.h \
    Sources/File/FileParseError.h \
    Sources/Chemistry/ChemistryMolecularMechanics.h
SOURCES = Sources/main.cpp \
    Sources/MainWindow.cpp \
    Sources/Render/RenderArrow.cpp \
    Sources/Chemistry/ChemistryAtom.cpp \
    Sources/Chemistry/ChemistryBond.cpp \
    Sources/Render/RenderColor.cpp \
    Sources/Render/RenderCone.cpp \
    Sources/Render/RenderCylinder.cpp \
    Sources/Render/RenderMaterial.cpp \
    Sources/Chemistry/ChemistryMolecule.cpp \
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
    Sources/FileMolGeneratorDialog.cpp \
    Sources/File/FileParseError.cpp \
    Sources/Chemistry/ChemistryMolecularMechanics.cpp
FORMS += Sources/MainWindow.ui \
    Sources/AboutDialog.ui \
    Sources/ImportMoleculeFromFileDialog.ui \
    Sources/FileMolGeneratorDialog.ui
RESOURCES += Resources/icons.qrc \
    Resources/images.qrc
win32 { 
    INCLUDEPATH += Includes/Windows \
        Includes/Windows/QGLViewer
    MOC_DIR = Temp/win32
    RCC_DIR = Temp/win32
    UI_DIR = Temp/win32
    OBJECTS_DIR = Objects/win32
    DESTDIR = Binaries/Windows
    LIBS += -LLibraries/Windows \
        -lQGLViewer2 \
        -lopenbabel-2
    DEFINES += USING_DYNAMIC_LIBS
}
macx {
    INCLUDEPATH += /usr/local/include/openbabel-2.0 \
        /usr/local/include/eigen2 \
        /opt/local/include/eigen2/ \
        /usr/include/QGLViewer #\
        #/Library/Frameworks/QGLViewer.framework/Headers
    MOC_DIR = Temp/macx
    RCC_DIR = Temp/macx
    UI_DIR = Temp/macx
    OBJECTS_DIR = Objects/macx
    DESTDIR = Binaries/MacOS
    LIBS += -framework \
        QGLViewer \
        -lopenbabel
    DEFINES += USING_DYNAMIC_LIBS
}
linux-g++ {
    INCLUDEPATH += /usr/include/openbabel-2.0 \
        /usr/include/eigen2 \
        /usr/include/QGLViewer
    MOC_DIR = Temp/linux
    RCC_DIR = Temp/linux
    UI_DIR = Temp/linux
    OBJECTS_DIR = Objects/linux
    DESTDIR = Binaries/Linux
    LIBS *= -lqglviewer-qt4 \
        -lopenbabel
}
