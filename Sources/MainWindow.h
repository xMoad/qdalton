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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "ui_MainWindow.h"

namespace File
{
  class Dal;
  class Mol;
}

namespace Ui
{
  class MainWindow;
}

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = 0);
  ~MainWindow();
  void setDalFile(File::Dal& dalFile);
  void setMolFile(File::Mol& molFile);
  void setWorkDir(const QString& work_dir);
  QString& getWorkDir();

private:
  Ui::MainWindow* ui;
  QString workDir_;
  File::Dal* dalFile_;
  File::Mol* molFile_;
  QLabel statusLabel_;

private slots:
  void on_atomComboBox_currentIndexChanged(QString );
  void on_actionHelpAbout_triggered();
  void on_tabWidget_currentChanged(int index);
  void on_actionHelpAboutQt_triggered();
  void on_checkBoxDebugInfo_clicked(bool checked);
  void on_checkBoxAxes_toggled(bool checked);
  void on_actionExit_triggered();
  void on_actionStructureExportImage_triggered();
  void on_actionJobNew_triggered();
  void on_toolBox_currentChanged(int index);
  void on_viewQComboBox_currentIndexChanged(int index);
  void on_actionJobImportDalton_triggered();
  void on_doubleSpinBoxAxesSize_valueChanged(double);
};

#endif // MAINWINDOW_H
