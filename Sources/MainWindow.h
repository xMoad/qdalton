#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "ui_MainWindow.h"
#include "LogDialog.h"
#include "Chemistry/ChemistryMolecule.h"

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
  Chemistry::Molecule molecule_;
  QString workDir_;
  File::Dal* dalFile_;
  File::Mol* molFile_;
  QLabel statusLabel_;
  LogDialog* logDialog;

private slots:
  void on_actionViewLog_toggled(bool );
  void on_conformersTableWidget_cellClicked(int row, int column);
  void on_actionStructureConformations_triggered();
  void on_actionStructureImportInChI_triggered();
  void on_actionStructureImportSMILES_triggered();
  void on_pushButtonRun_clicked();
  void on_actionHelpAbout_triggered();
  void on_tabWidget_currentChanged(int index);
  void on_actionHelpAboutQt_triggered();
  void on_checkBoxDebugInfo_clicked(bool checked);
  void on_checkBoxAxes_toggled(bool checked);
  void on_actionExit_triggered();
  void on_actionStructureExportImage_triggered();
  void on_actionStructureImportGaussianOutput_triggered();
  void on_comboBoxAtom_currentIndexChanged(QString );
  void on_actionJobNew_triggered();
  void on_toolBox_currentChanged(int index);
  void on_viewQComboBox_currentIndexChanged(int index);
  void on_actionJobImportDalton_triggered();
  void on_doubleSpinBoxAxesSize_valueChanged(double);
};

#endif // MAINWINDOW_H
