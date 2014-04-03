QT *= core gui opengl xml

TEMPLATE = app

CONFIG += qt opengl warn_on thread rtti embed_manifest_exe

INCLUDEPATH += src \
    ../3rd/eigen/2/include

HEADERS += src/MainWindow.h \
    src/Render/RenderGL.h \
    src/Render/RenderGLU.h \
    src/Render/RenderArrow.h \
    src/Chemistry/ChemistryAtom.h \
    src/Chemistry/ChemistryBond.h \
    src/Render/RenderColor.h \
    src/Render/RenderCone.h \
    src/Render/RenderCylinder.h \
    src/Render/RenderMaterial.h \
    src/Chemistry/ChemistryMolecule.h \
    src/Render/RenderQuadric.h \
    src/Render/RenderSphere.h \
    src/Render/RenderViewer.h \
    src/File/FileText.h \
    src/File/FileMol.h \
    src/File/FileSubmodule.h \
    src/File/FileParametre.h \
    src/File/FileModule.h \
    src/File/FileDal.h \
    src/AboutDialog.h \
    src/ImportMoleculeFromFileDialog.h \
    src/FileMolGeneratorDialog.h \
    src/Render/RenderConstants.h \
    src/Constants.h \
    src/File/FileParseError.h \
    src/Chemistry/ChemistryMolecularMechanics.h

SOURCES = src/main.cpp \
    src/MainWindow.cpp \
    src/Render/RenderArrow.cpp \
    src/Chemistry/ChemistryAtom.cpp \
    src/Chemistry/ChemistryBond.cpp \
    src/Render/RenderColor.cpp \
    src/Render/RenderCone.cpp \
    src/Render/RenderCylinder.cpp \
    src/Render/RenderMaterial.cpp \
    src/Chemistry/ChemistryMolecule.cpp \
    src/Render/RenderQuadric.cpp \
    src/Render/RenderSphere.cpp \
    src/Render/RenderViewer.cpp \
    src/File/FileText.cpp \
    src/File/FileMol.cpp \
    src/File/FileSubmodule.cpp \
    src/File/FileParametre.cpp \
    src/File/FileModule.cpp \
    src/File/FileDal.cpp \
    src/AboutDialog.cpp \
    src/ImportMoleculeFromFileDialog.cpp \
    src/FileMolGeneratorDialog.cpp \
    src/File/FileParseError.cpp \
    src/Chemistry/ChemistryMolecularMechanics.cpp

FORMS += src/MainWindow.ui \
    src/AboutDialog.ui \
    src/ImportMoleculeFromFileDialog.ui \
    src/FileMolGeneratorDialog.ui

RESOURCES += rsrc/icons.qrc \
    rsrc/images.qrc

VERSION = $$system(hg parents --template '{latesttag}.{latesttagdistance}')
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG(release, debug|release) {
    MOC_DIR = tmp/release
    RCC_DIR = tmp/release
    UI_DIR = tmp/release
    OBJECTS_DIR = obj/release
    DESTDIR = bin/release
} else {
    MOC_DIR = tmp/debug
    RCC_DIR = tmp/debug
    UI_DIR = tmp/debug
    OBJECTS_DIR = obj/debug
    DESTDIR = bin/debug
}

win32-g++* {
    QMAKE_CXXFLAGS += -std=c++0x
    QMAKE_LFLAGS += -static -static-libstdc++ -static-libgcc
    INCLUDEPATH += c:/src/boost/boost_1_55_0 \
        ../3rd/mgw4.8-qt4/libQGLViewer/2.5.1/include \
        ../3rd/mgw4.8-qt4/log4cpp/1.1.1/include \
        ../3rd/mgw4.8-qt4/openbabel/2.3.90/include
    CONFIG(release, debug|release) {
        LIBS += -L../3rd/mingw/qglviewer/lib/release \
            -L../3rd/mingw/log4cpp/lib/release_static \
            -L../3rd/mingw/boost/lib/release_static \
            -L../3rd/mingw/openbabel/lib/release_static \
            -lQGLViewer2 \
            -llog4cpp \
            -lboost_filesystem-mgw48-mt-1_55 \
            -lboost_regex-mgw48-mt-1_55 \
            -lboost_system-mgw48-mt-1_55 \
            -lopenbabel \
            -lzlibstatic
    } else {
        LIBS += -L../3rd/mgw4.8-qt4/boost/1.55/lib \
            -L../3rd/mgw4.8-qt4/libQGLViewer/2.5.1/lib \
            -L../3rd/mgw4.8-qt4/log4cpp/1.1.1/lib \
            -L../3rd/mgw4.8-qt4/openbabel/2.3.90/lib/debug \
            -lboost_filesystem-mgw48-mt-d-1_55 \
            -lboost_regex-mgw48-mt-d-1_55 \
            -lboost_system-mgw48-mt-d-1_55 \
            -lQGLViewerd2 \
            -llog4cppD \
            -lopenbabel \
            -lzlibstatic
    }
    DEFINES += LOG4CPP_HAVE_INT64_T USING_DYNAMIC_LIBS NOGDI
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
