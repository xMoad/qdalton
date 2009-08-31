#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);

#ifdef __APPLE__
  a.setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

  QCoreApplication::setApplicationName("QDalton");
  QCoreApplication::setApplicationVersion("0.0.5");
  QCoreApplication::setOrganizationDomain("ctcc.no");
  QCoreApplication::setOrganizationName("CTCC");

  MainWindow w;
  w.setWindowTitle(QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
  w.show();
  return a.exec();
}
