/**********************************************************************
  Copyright (C) 2008, 2009, 2010 Anton Simakov

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

//#include <openbabel/forcefield.h>

#include "ui_MainWindow.h"
#include "Render/RenderMolecule.h"

namespace File
{
  class Dal;
  class Mol;
}

namespace Render
{
  class Viewer;
}

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = 0);
  void addToLog(const QString& string);
  void importMoleculeFromFile(const QString& fileName,
                              OpenBabel::OBFormat* obFormat);
  const QString& dir() const;
  void setDir(const QString& dir);

public slots:
  void newMolecule();
  void openMolecule();
  void saveMolecule();
  void saveMoleculeAs(const QString& absoluteFilePath = "");
//  void fillConformersTable(double threshold);
  void newTab(const QString& title, const File::Mol& fileMol);
  void goToTab(int index);
  void showImportStructureFromFileDialog();
  void showAboutDialog();
  void showFileMolGeneratorDialog();
  void updateActionsForEmptyMolecule();
  void updateActionsForNonemptyMolecule();
  void startOptimization();
  void onOptimizationFinished();
//  void startConformationalSearch();
//  void onConformationalSearchFinished();
//  void onTableWidgetConformersCurrentCellChanged(int currentRow);
  void onSearchTypeChanged(int index);
  void setObForceFieldName(const QString& obForceFieldName);
  void showFontDialogForLabelsOnAtoms();
  void onTabCloseRequested(int index);

private:
  Render::Viewer& currentViewer();
  Render::Viewer& viewer(int index);

  Ui::MainWindow ui_;
  std::ostringstream os_;
  QString obForceFieldName_;
  QString dir_;

private slots:
  void on_actionExit_triggered();
  void on_actionStructureExportImage_triggered();
};

#endif // MAINWINDOW_H
