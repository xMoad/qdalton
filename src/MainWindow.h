/**********************************************************************
 **********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <openbabel/forcefield.h>

#include "ui_MainWindow.h"
#include "Chemistry/ChemistryMolecularMechanics.h"
#include "Chemistry/ChemistryMolecule.h"

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
  void showFontDialogForLabelsOnBonds();
  void onTabCloseRequested(int index);
  void addHydrogensAndBuild();

private:
  Render::Viewer& currentViewer();
  Render::Viewer& viewer(int index);

  Ui::MainWindow ui_;
  std::ostringstream os_;
  QString obForceFieldName_;
  QString dir_;
  Chemistry::MolecularMechanics mm_;

private slots:
  void on_actionExit_triggered();
  void on_actionStructureExportImage_triggered();
};

#endif // MAINWINDOW_H
