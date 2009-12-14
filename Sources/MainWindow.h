#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <openbabel/forcefield.h>

#include "ui_MainWindow.h"
#include "Chemistry/ChemistryMolecule.h"

namespace File
{
  class Dal;
  class Mol;
}

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = 0);
  void setDalFile(File::Dal& dalFile);
  void setMolFile(File::Mol& molFile);
  void setWorkDir(const QString& work_dir);
  QString& getWorkDir();
  void addToLog(const QString& string);
  void importMoleculeFromFile(const QString& fileName,
                              OpenBabel::OBFormat* obFormat);

public slots:
  void fillConformersTable(double threshold);
  void showImportStructureFromFileDialog();
  void showAboutDialog();
  void setActionsViewEnabled(bool enabled);
  void updateActionsForEmptyMolecule();
  void updateActionsForNonemptyMolecule();

private:
  Ui::MainWindow ui_;

  Chemistry::Molecule molecule_;

  OpenBabel::OBForceField* obForceField_;

  File::Dal* dalFile_;
  File::Mol* molFile_;

  QLabel statusLabel_;
  QString workDir_;

private slots:
  void on_pushButtonSearch_clicked();
  void on_comboBoxForceField_currentIndexChanged(QString );
  void on_comboBoxAtom_editTextChanged(QString );
  void on_actionStructureExportXyz_triggered();
  void on_actionViewConformersTable_toggled(bool );
  void on_actionViewToolbox_toggled(bool );
  void on_actionViewLog_toggled(bool checked);
  void on_tableWidgetConformers_cellClicked(int row, int column);
  void on_actionStructureImportInChI_triggered();
  void on_actionStructureImportSMILES_triggered();
  void on_pushButtonOptimize_clicked();
  void on_tabWidget_currentChanged(int index);
  void on_checkBoxDebugInfo_clicked(bool checked);
  void on_checkBoxAxes_toggled(bool checked);
  void on_actionExit_triggered();
  void on_actionStructureExportImage_triggered();
  void on_actionJobNew_triggered();
  void on_comboBoxView_currentIndexChanged(int index);
  void on_actionJobImportDalton_triggered();
  void on_doubleSpinBoxAxesSize_valueChanged(double);
};

#endif // MAINWINDOW_H
