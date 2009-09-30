QT *= core \
    gui \
    opengl \
    xml

# QMAKE_CXXFLAGS *= -Wold-style-cast -Wfloat-equal -Woverloaded-virtual -Wshadow -Wundef
TEMPLATE = app
CONFIG += debug_and_release \
    qt \
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
    Sources/Render/RenderMolecule.h
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
    Sources/Render/RenderMolecule.cpp
FORMS += Sources/MainWindow.ui \
    Sources/AboutDialog.ui
RESOURCES += Resources/icons.qrc \
    Resources/images.qrc
CONFIG(debug, debug|release) { 
    MOC_DIR = Build/Temp/debug
    RCC_DIR = Build/Temp/debug
    UI_DIR = Build/Temp/debug
    OBJECTS_DIR = Build/Objects/debug
}
else { 
    CONFIG -= console
    MOC_DIR = Build/Temp/release
    RCC_DIR = Build/Temp/release
    UI_DIR = Build/Temp/release
    OBJECTS_DIR = Build/Objects/release
}
win32 { 
    CONFIG(debug, debug|release) { 
        DESTDIR = Build/Binaries/Windows/debug
        LIBS += -LLibraries/Windows \
            -ldQGLViewer2 \
            -ldopenbabel-2
    }
    else { 
        DESTDIR = Build/Binaries/Windows/release
        LIBS += -LLibraries/Windows \
            -lQGLViewer2 \
            -lopenbabel-2
    }
}
macx { 
    DESTDIR = Build/Binaries/MacOS
    LIBS += -framework \
        QGLViewer \
        -lopenbabel
    CONFIG(debug, debug|release):DYLD_IMAGE_SUFFIX = _debug
}
linux-g++ { 
    DESTDIR = Build/Binaries/Linux
    LIBS *= -lQGLViewer \
        -lopenbabel
}
