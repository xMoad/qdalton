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

#include "MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "AboutDialog.h"
#include "Render/RenderAtom.h"
#include "Render/RenderColor.h"
#include "File/FileDal.h"
#include "File/FileMol.h"

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dalFile_(0),
    molFile_(0)
{
  ui->setupUi(this);
  // Hide all tabs
  ui->tabWidget->hide();
  workDir_ = QDir::currentPath();
  //
  ui->statusbar->addWidget(&statusLabel_, 1);
  statusLabel_.setText(workDir_);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::setDalFile(File::Dal& dalFile)
{
  if (dalFile_ != 0)
    delete dalFile_;
  dalFile_ = new File::Dal(dalFile);
}

void MainWindow::setMolFile(File::Mol& molFile)
{
  if (molFile_ != 0)
    delete molFile_;
  molFile_ = new File::Mol(molFile);
}

void MainWindow::setWorkDir(const QString& workDir)
{
  workDir_ = workDir;
}

QString& MainWindow::getWorkDir()
{
  return workDir_;
}

void MainWindow::on_doubleSpinBoxAxesSize_valueChanged(double value)
{
  ui->viewer->setAxes(true, value);
}

void MainWindow::on_actionJobImportDalton_triggered()
{
  QString fileName = "";
  // Ask user to choose Dalton input file
  fileName = QFileDialog::getOpenFileName(this, "Choose Dalton input file",
                                          getWorkDir(), "Dalton input files (*.dal *.inp)");
  if (!fileName.isEmpty())
  {
    File::Dal dalFile(fileName);
    dalFile.read();
    // Parse Dalton input file
    if (dalFile.parse())
    {
      // If succeeded then ask user to choose molecule input file
      fileName = QFileDialog::getOpenFileName(this, "Open *.MOL File",
                                              getWorkDir(), "*.MOL Files(*.mol)");
      if (!fileName.isEmpty())
      {
        File::Mol molFile(fileName);
        molFile.read();
        // Parse molecule input file
        if (molFile.parse(true))
        {
          // If succeeded then do some magic! =)
          // TODO show save file dialog for project!
          setWorkDir(dalFile.filePath());
          statusLabel_.setText(workDir_);
          setDalFile(dalFile);
          setMolFile(molFile);
          // Fill the text widgets
          ui->plainTextEditDal->setPlainText(dalFile.text());
          ui->plainTextEditMol->setPlainText(molFile.text());
          // Set groupBoxes titles
          ui->groupBoxDalFile->setTitle("Dalton input file: "
                                        + dalFile.fileName());
          ui->groupBoxMolFile->setTitle("Molecule input file: "
                                        + molFile.fileName());
          switch (dalFile.jobType())
          {
          case File::Dal::OPTIMIZE:
            ui->comboBoxJobType->setCurrentIndex(0);
            break;
          case File::Dal::WALK:
            ui->comboBoxJobType->setCurrentIndex(1);
            break;
          case File::Dal::RUN:
            ui->comboBoxJobType->setCurrentIndex(2);
            break;
          }
          switch (molFile.basisType())
          {
          case File::Mol::ATOMBASIS:
            ui->comboBoxBasisType->setCurrentIndex(0);
            break;
          case File::Mol::BASIS:
            ui->comboBoxBasisType->setCurrentIndex(1);
            break;
          case File::Mol::INTGRL:
            ui->comboBoxBasisType->setCurrentIndex(2);
            break;
          }
          ui->tabWidget->setTabText(0, "Job ()");
          ui->tabWidget->setTabText(1, QString("DALTON Input (%1)").arg(dalFile.fileName()));
          ui->tabWidget->setTabText(2, QString("MOLECULE Input (%1)").arg(molFile.fileName()));
          ui->tabWidget->setTabText(3, QString("Viewer (%1)").arg("Input geometry"));
          ui->welcomeWidget->hide();
          ui->tabWidget->show();

          ui->tabWidget->setCurrentIndex(3);
          ui->viewer->setMolecule(molFile.molecule());
          ui->actionStructureExportImage->setEnabled(true);
          //          ui->atomsTableWidget->setRowCount(molFile_->molecule().nucleiCount());
          //          for (int i = 0; i < molFile.molecule().nucleiCount(); ++i)
          //          {
          //            Render::Atom atom = ui->viewer->getAtom(i);
          //            QTableWidgetItem* item = new QTableWidgetItem(
          //                atom.nucleus().label());
          //            item->setBackgroundColor(QColor::fromRgbF(atom.color().r(),
          //                                                      atom.color().g(), atom.color().b(), 0.5));
          //            ui->atomsTableWidget->setItem(i, 1, item);
          //          }
        }
        else
        {
          // show error message
          QMessageBox messageBox;
          messageBox.setIcon(QMessageBox::Critical);
          messageBox.setText("Can't open molecule input file "
                             + molFile.fileName());
          messageBox.setInformativeText("Error at line #" + QString::number(
              molFile.getParseError().index + 1));
          messageBox.setDetailedText(molFile.getParseError().message);
          messageBox.setStandardButtons(QMessageBox::Ok);
          messageBox.exec();
        }
      }
    }
    else
    {
      // Show error mesage
      QMessageBox messageBox;
      messageBox.setIcon(QMessageBox::Critical);
      messageBox.setText("Can't open Dalton input file "
                         + dalFile.fileName());
      messageBox.setInformativeText("Error at line #" + QString::number(
          dalFile.getParseError().index + 1));
      messageBox.setDetailedText(dalFile.getParseError().message);
      messageBox.setStandardButtons(QMessageBox::Ok);
      messageBox.exec();
    }
  }
}

void MainWindow::on_viewQComboBox_currentIndexChanged(int index)
{
  switch (index)
  {
  case 0:
    ui->viewer->setView(ui->viewer->VIEW_BALLS_STICKS);
    break;
  case 1:
    ui->viewer->setView(ui->viewer->VIEW_STICKS);
    break;
  case 2:
    ui->viewer->setView(ui->viewer->VIEW_VDW);
    break;
  }
}

void MainWindow::on_toolBox_currentChanged(int index)
{
  switch (index)
  {
  case 0:
    ui->viewer->setMode(ui->viewer->MODE_VIEW);
    break;
  case 1:
    ui->viewer->setMode(ui->viewer->MODE_ADD);
    break;
  }
}

void MainWindow::on_actionJobNew_triggered()
{
  ui->tabWidget->setTabText(0, "Job");
  ui->tabWidget->setTabText(1, QString("DALTON Input"));
  ui->tabWidget->setTabText(2, QString("MOLECULE Input"));
  ui->tabWidget->setTabText(3, QString("Viewer (%1)").arg("Input geometry"));
  ui->welcomeWidget->hide();
  ui->tabWidget->show();
}

void MainWindow::on_comboBoxAtom_currentIndexChanged(QString s)
{
//  QByteArray byteArray = s.toLatin1();
//  const char* s_char = byteArray.data();
//  ui->viewer->setAtomicNumber(OpenBabel::etab.GetAtomicNum(s_char));
}

void MainWindow::on_actionStructureExportImage_triggered()
{
  ui->viewer->saveSnapshot(false, false);
}

void MainWindow::on_actionExit_triggered()
{
  if (QMessageBox::information(this,
                               QCoreApplication::applicationName(),
                               QString("Exit %1?").arg(QCoreApplication::applicationName()),
                               QMessageBox::Ok | QMessageBox::Cancel,
                               QMessageBox::Cancel) == QMessageBox::Ok)
    close();
}

void MainWindow::on_checkBoxAxes_toggled(bool checked)
{
  if (checked)
  {
    ui->doubleSpinBoxAxesSize->setEnabled(true);
    ui->viewer->setAxes(true, ui->doubleSpinBoxAxesSize->value());
  }
  else
  {
    ui->doubleSpinBoxAxesSize->setEnabled(false);
    ui->viewer->setAxes(false, ui->doubleSpinBoxAxesSize->value());
  }
}

void MainWindow::on_checkBoxDebugInfo_clicked(bool checked)
{
  ui->viewer->setDebugInfoVisibility(checked);
}

void MainWindow::on_actionHelpAboutQt_triggered()
{
  QApplication::aboutQt();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
  if (index == 3)
  {
    ui->actionStructureExportImage->setEnabled(true);
  }
  else
  {
    ui->actionStructureExportImage->setEnabled(false);
  }
}

void MainWindow::on_actionHelpAbout_triggered()
{
  AboutDialog dialog;
  dialog.setHeaderLabelText("<h2>" + QCoreApplication::applicationName() +
                            " " + QCoreApplication::applicationVersion() + "</h2>");
  dialog.exec();
}
