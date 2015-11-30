Get sources archive or get a local copy of the qdalton repository with this command (you need Mercurial packages to do it):
```
hg clone https://qdalton.googlecode.com/hg/ qdalton
```

In order to compile QDalton from sources you need:
  * [Qt 4](http://qt.nokia.com/)
  * [OpenBabel 2](http://openbabel.org)
  * [libQGLViewer 2](http://www.libqglviewer.com)
  * [Eigen 2](http://eigen.tuxfamily.org)

You can obtain libraries using packaging system (apt-get for instance) or compile them from sources. In some cases some workaround (e.g. editing environment variables PATH, LD\_LIBRARY\_PATH) is necessary. Please consult installing/compiling notes for mentioned libraries.

Next edit QDalton.pro file. Go to the end, find section
```
linux-g++ {
  DESTDIR = Build/Binaries/Linux
  LIBS *= -lQGLViewer -lopenbabel
}
```
and add directories with 3rd party libraries includes, e.g.:
```
linux-g++ { 
  INCLUDEPATH += /usr/local/include/openbabel-2.0 \
     /usr/local/include/eigen2
  DESTDIR = Build/Binaries/Linux
  LIBS *= -lQGLViewer -lopenbabel
}
```

Go to the directory with sources and compile Qdalton:
```
$ cd Desktop/qdalton/
$ qmake
$ make release
```

That's all.