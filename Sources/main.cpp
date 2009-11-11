/**********************************************************************
  Copyright (C) 2008, 2009 Anton Simakov

  This file is part of QDalton.
  For more information, see <http://code.google.com/p/qdalton/>

  QDalton is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  QDalton is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with QDalton. If not, see <http://www.gnu.org/licenses/>.

 **********************************************************************/

#include <QtGlobal>

#ifdef Q_CC_MSVC
#define NOMINMAX
#endif

#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);

#ifdef Q_WS_MAC
  a.setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

  QCoreApplication::setApplicationName("QDalton");
  QCoreApplication::setApplicationVersion("0.0.6.1");
  QCoreApplication::setOrganizationDomain("ctcc.no");
  QCoreApplication::setOrganizationName("CTCC");

  MainWindow w;
  w.setWindowTitle(QCoreApplication::applicationName() + " " +
                   QCoreApplication::applicationVersion());
  w.show();
  return a.exec();
}
