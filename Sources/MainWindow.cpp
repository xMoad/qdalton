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

#include <QCompleter>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSplitter>

#include "AboutDialog.h"
#include "ImportStructureFromFileDialog.h"
#include "Render/RenderAtom.h"
#include "Render/RenderColor.h"
#include "File/FileDal.h"
#include "File/FileMol.h"

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    molecule_(),
    obForceField_(0),
    dalFile_(0),
    molFile_(0),
    workDir_(QDir::currentPath())
{
  ui_.setupUi(this);

  setActionsViewEnabled(false);

#ifdef Q_OS_MAC
  ui_.toolBar->setIconSize(QSize(24, 24));
#endif

  // Hide all tabs
  ui_.tabWidget->hide();

  ui_.logPlainTextEdit->hide();
  ui_.widgetConformers->hide();

  ui_.statusbar->addWidget(&statusLabel_, 1);
  statusLabel_.setText(workDir_);

  QStringList headerLabels;
  headerLabels << " Number" << "Energy";
  ui_.tableWidgetConformers->setHorizontalHeaderLabels(headerLabels);
  ui_.tableWidgetConformers->horizontalHeader()->setVisible(true);

  QStringList stringList;

  for (quint8 i = 1; i <= OpenBabel::etab.GetNumberOfElements(); ++i)
  {
    stringList.append(QString::fromAscii(OpenBabel::etab.GetSymbol(i)));
  }

  QCompleter* completer = new QCompleter(stringList, ui_.comboBoxAtom);
  ui_.comboBoxAtom->setCompleter(completer);

  ui_.comboBoxForceField->setCurrentIndex(2);

  // Set connections.
  connect(&molecule_, SIGNAL(becameEmpty()),
          this, SLOT(updateActionsForEmptyMolecule()));
  connect(&molecule_, SIGNAL(becameNonempty()),
          this, SLOT(updateActionsForNonemptyMolecule()));
  connect(&molecule_, SIGNAL(geometryChanged()),
          ui_.viewer, SLOT(updateMolecule()));

  connect(ui_.doubleSpinBox, SIGNAL(valueChanged(double)),
          this, SLOT(fillConformersTable(double)));

  connect(ui_.actionStructureImportFromFile, SIGNAL(triggered()),
          this, SLOT(showImportStructureFromFileDialog()));
  connect(ui_.actionStructureAddHydrogens, SIGNAL(triggered()),
          &molecule_, SLOT(addHydrogensAndBuild()));
  connect(ui_.actionStructureRemoveHydrogens, SIGNAL(triggered()),
          &molecule_, SLOT(removeHydrogens()));

  connect(ui_.actionHelpAbout, SIGNAL(triggered()),
          this, SLOT(showAboutDialog()));
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

void MainWindow::addToLog(const QString& string)
{
  QDateTime dateTime_ = QDateTime::currentDateTime();
  ui_.logPlainTextEdit->appendHtml(QString("<b>%1</b>").arg(dateTime_.time().toString()));
  ui_.logPlainTextEdit->appendPlainText(string);
  ui_.logPlainTextEdit->appendPlainText("");
}

void MainWindow::importMoleculeFromFile(const QString& fileName,
                                        OpenBabel::OBFormat* obFormat)
{
  if (!fileName.isEmpty())
  {
    addToLog(QString("Import structure from %1 started.").arg(fileName));
    if (molecule_.importFromFile(fileName, obFormat))
    {
      addToLog("Import succeeded.");
      ui_.welcomeWidget->hide();
      ui_.tabWidget->show();
      ui_.tabWidget->setCurrentIndex(3);
      ui_.viewer->setMolecule(&molecule_);
      setActionsViewEnabled(true);
    }
    else
    {
      addToLog("Import failed.");
      QMessageBox::critical(this, QCoreApplication::applicationName(),
                            "Can't import file " + fileName);
    }
  }
}

// Slots.

void MainWindow::fillConformersTable(double threshold)
{
  QList<bool> list;

  ui_.tableWidgetConformers->setRowCount(0);

  for (int i = 0; i < molecule_.conformersCount(); ++i)
  {
    list.append(true);
  }

  for (quint16 i = 1; i < list.count(); ++i)
  {
    if (list[i])
    {
      for (quint16 j = i + 1; j < list.count(); ++j)
      {
        if (list[j] && qAbs(molecule_.conformerEnergy(i) -
                            molecule_.conformerEnergy(j)) <= threshold)
        {
          list[j] = false;
        }
      }
    }
  }

  for (int i = 0; i < molecule_.conformersCount(); ++i)
  {
    if (list[i])
    {
      ui_.tableWidgetConformers->insertRow(
          ui_.tableWidgetConformers->rowCount());
      QTableWidgetItem* newItem = new QTableWidgetItem(
          QString::number(i + 1).rightJustified(3, '0'));
      ui_.tableWidgetConformers->setItem(
          ui_.tableWidgetConformers->rowCount() - 1, 0, newItem);

      newItem = new QTableWidgetItem(QString::number(
          molecule_.conformerEnergy(i)));
      ui_.tableWidgetConformers->setItem(
          ui_.tableWidgetConformers->rowCount() - 1, 1, newItem);
    }
  }
  ui_.viewer->displayConformer(0);
}

void MainWindow::showImportStructureFromFileDialog()
{
  ImportStructureFromFileDialog dialog(this);
  dialog.exec();
}

void MainWindow::showAboutDialog()
{
  AboutDialog dialog;
  dialog.setHeaderLabelText(
      "<h1>" + QCoreApplication::applicationName() +
      " " + QCoreApplication::applicationVersion() + "</h1>");
  dialog.exec();
}

void MainWindow::setActionsViewEnabled(bool enabled)
{
  ui_.actionViewConformersTable->setEnabled(enabled);
  ui_.actionViewLog->setEnabled(enabled);
  ui_.actionViewToolbox->setEnabled(enabled);
}

void MainWindow::updateActionsForEmptyMolecule()
{
  ui_.actionStructureAddHydrogens->setEnabled(false);
  ui_.actionStructureRemoveHydrogens->setEnabled(false);
  ui_.actionStructureConformations->setEnabled(false);
  ui_.actionStructureExportImage->setEnabled(false);
  ui_.actionStructureExportImage->setEnabled(false);
}

void MainWindow::updateActionsForNonemptyMolecule()
{
  ui_.actionStructureAddHydrogens->setEnabled(true);
  ui_.actionStructureRemoveHydrogens->setEnabled(true);
  ui_.actionStructureConformations->setEnabled(true);
  ui_.actionStructureExportImage->setEnabled(true);
  ui_.actionStructureExportXyz->setEnabled(true);
}

void MainWindow::on_doubleSpinBoxAxesSize_valueChanged(double value)
{
  ui_.viewer->setAxes(true, value);
}

void MainWindow::on_actionJobImportDalton_triggered()
{
  QString fileName;
  QMessageBox::StandardButton reply;

  if (molecule_.atomsCount() != 0)
  {
    reply = QMessageBox::question(this,
                                  QCoreApplication::applicationName(),
                                  tr("Molecule is not empty. Proceed?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);
    if (reply == QMessageBox::No)
    {
      return;
    }
  }

  // Ask user to choose Dalton input file.
  fileName = QFileDialog::getOpenFileName(this, "Open *.dal file",
                                          getWorkDir(), "*.dal files (*.dal)");
  if (!fileName.isEmpty())
  {
    addToLog(QString("Reading from *.dal file started. \nFileName: %1").arg(fileName));
    File::Dal dalFile(fileName);
    dalFile.read();
    // Parse Dalton input file
    if (dalFile.parse())
    {
      // If succeeded then ask user to choose molecule input file.
      addToLog("Reading succeeded.");
      fileName = QFileDialog::getOpenFileName(this, "Open *.mol file",
                                              getWorkDir(), "*.mol files(*.mol)");
      if (!fileName.isEmpty())
      {
        addToLog(QString("Reading from *.mol file started. \nFileName: %1").arg(fileName));
        File::Mol molFile(fileName);
        molFile.read();
        // Parse molecule input file
        if (molFile.parse(true))
        {
          // If succeeded then do some magic! =)
          // TODO show save file dialog for project!
          addToLog("Reading succeeded.");
          setWorkDir(dalFile.filePath());
          statusLabel_.setText(workDir_);
          setDalFile(dalFile);
          setMolFile(molFile);
          // Fill the text widgets
          ui_.plainTextEditDal->setPlainText(dalFile.text());
          ui_.plainTextEditMol->setPlainText(molFile.text());
          // Set groupBoxes titles
          ui_.groupBoxDalFile->setTitle("Dalton input file: "
                                        + dalFile.fileName());
          ui_.groupBoxMolFile->setTitle("Molecule input file: "
                                        + molFile.fileName());
          switch (dalFile.jobType())
          {
          case File::Dal::JobTypeOptimize:
            ui_.comboBoxJobType->setCurrentIndex(0);
            break;
          case File::Dal::JobTypeWalk:
            ui_.comboBoxJobType->setCurrentIndex(1);
            break;
          case File::Dal::JobTypeRun:
            ui_.comboBoxJobType->setCurrentIndex(2);
            break;
          }
          switch (molFile.basisType())
          {
          case File::Mol::BasisTypeAtombasis:
            ui_.comboBoxBasisType->setCurrentIndex(0);
            break;
          case File::Mol::BasisTypeBasis:
            ui_.comboBoxBasisType->setCurrentIndex(1);
            break;
          case File::Mol::BasisTypeIntgrl:
            ui_.comboBoxBasisType->setCurrentIndex(2);
            break;
          }
          ui_.tabWidget->setTabText(0, "Job ()");
          ui_.tabWidget->setTabText(1, QString("DALTON Input (%1)").arg(dalFile.fileName()));
          ui_.tabWidget->setTabText(2, QString("MOLECULE Input (%1)").arg(molFile.fileName()));
          ui_.tabWidget->setTabText(3, QString("Viewer (%1)").arg("Input geometry"));
          ui_.welcomeWidget->hide();
          ui_.tabWidget->show();

          ui_.tabWidget->setCurrentIndex(3);
          molecule_ = molFile.molecule();
          ui_.viewer->setMolecule(&molecule_);
        }
        else
        {
          // show error message
          addToLog("Reading failed.");
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
      addToLog("Reading failed.");
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

void MainWindow::on_comboBoxView_currentIndexChanged(int index)
{
  switch (index)
  {
  case 0:
    ui_.viewer->setView(Render::Viewer::ViewBallsAndBonds);
    break;
  case 1:
    ui_.viewer->setView(Render::Viewer::ViewBallsAndSticks);
    break;
  case 2:
    ui_.viewer->setView(Render::Viewer::ViewSticks);
    break;
  case 3:
    ui_.viewer->setView(Render::Viewer::ViewVdWSpheres);
    break;
  }
}

void MainWindow::on_actionJobNew_triggered()
{
//  if (molecule_.atomsCount() != 0)
//  {
//    if (QMessageBox::question(this,
//                              QCoreApplication::applicationName(),
//                              tr("Molecule is not empty. Proceed?"),
//                              QMessageBox::Yes | QMessageBox::No,
//                              QMessageBox::No))
//    {
//    }
//  }
  ui_.tabWidget->setTabText(0, "Job");
  ui_.tabWidget->setTabText(1, QString("DALTON Input"));
  ui_.tabWidget->setTabText(2, QString("MOLECULE Input"));
  ui_.tabWidget->setTabText(3, QString("Viewer (%1)").arg("Input geometry"));
  ui_.welcomeWidget->hide();
  ui_.tabWidget->show();
  ui_.viewer->setMolecule(&molecule_);
  setActionsViewEnabled(true);
}

void MainWindow::on_actionStructureExportImage_triggered()
{
  ui_.viewer->saveSnapshot(false, false);
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
    ui_.doubleSpinBoxAxesSize->setEnabled(true);
    ui_.viewer->setAxes(true, ui_.doubleSpinBoxAxesSize->value());
  }
  else
  {
    ui_.doubleSpinBoxAxesSize->setEnabled(false);
    ui_.viewer->setAxes(false, ui_.doubleSpinBoxAxesSize->value());
  }
}

void MainWindow::on_checkBoxDebugInfo_clicked(bool checked)
{
  ui_.viewer->setDebugInfoVisibility(checked);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
  if (index == 3)
  {
    ui_.actionStructureExportImage->setEnabled(true);
  }
  else
  {
    ui_.actionStructureExportImage->setEnabled(false);
  }
}

void MainWindow::on_pushButtonOptimize_clicked()
{
  std::ostringstream os;

  setCursor(Qt::WaitCursor);
  ui_.pushButtonOptimize->setText(tr("Running..."));
  qApp->processEvents();
  ui_.pushButtonOptimize->setEnabled(false);
  molecule_.optimize(obForceField_,
                     (Chemistry::Algorithm)ui_.comboBoxAlgorithm->currentIndex(),
                     powf(10, -ui_.spinBoxConvergence->value()),
                     ui_.spinBoxMaxSteps->value(),
                     ui_.spinBoxStepsPerUpdate->value(),
                     &os);
  addToLog(QString::fromStdString(os.str()));
  ui_.pushButtonOptimize->setEnabled(true);
  ui_.pushButtonOptimize->setText(tr("Run"));
  setCursor(Qt::ArrowCursor);
}

void MainWindow::on_actionStructureImportSMILES_triggered()
{
//  bool ok;
//  QString text;
//
//  text = QInputDialog::getText(this,
//                               tr("QDalton"),
//                               tr("Input SMILES:"),
//                               QLineEdit::Normal,
//                               "",
//                               &ok);
//  if (ok)
//  {
//    if (molecule_.importFromString(Chemistry::FormatSmiles,
//                                  text))
//    {
//      ui_.tabWidget->setCurrentIndex(3);
//      ui_.viewer->setMolecule(&molecule_);
//    }
//  }
}

void MainWindow::on_actionStructureImportInChI_triggered()
{
//  bool ok;
//  QString text;
//
//  text = QInputDialog::getText(this,
//                               tr("QDalton"),
//                               tr("Input InChI:"),
//                               QLineEdit::Normal,
//                               "",
//                               &ok);
//  if (ok)
//  {
//    if (molecule_.importFromString(Chemistry::FormatInchi,
//                                  text))
//    {
//      ui_.tabWidget->setCurrentIndex(3);
//      ui_.viewer->setMolecule(&molecule_);
//    }
//  }
}

void MainWindow::on_tableWidgetConformers_cellClicked(int row, int column)
{
  bool ok;

  quint16 n = ui_.tableWidgetConformers->item(row, 0)->text().toUInt(&ok);
  if (ok)
  {
    ui_.viewer->displayConformer(n - 1);
  }
}

void MainWindow::on_actionViewLog_toggled(bool checked)
{
  if (checked)
  {
    ui_.logPlainTextEdit->show();
  }
  else
  {
    ui_.logPlainTextEdit->hide();
  }
}

void MainWindow::on_actionViewToolbox_toggled(bool checked)
{
  if (checked)
  {
    ui_.toolBox->show();
  }
  else
  {
    ui_.toolBox->hide();
  }
}

void MainWindow::on_actionViewConformersTable_toggled(bool checked)
{
  if (checked)
  {
    ui_.widgetConformers->show();
  }
  else
  {
    ui_.widgetConformers->hide();
  }
}

void MainWindow::on_actionStructureExportXyz_triggered()
{
//  ui_.plainTextEditMol->setPlainText(molecule_.toString(Chemistry::FormatXyz));
}

void MainWindow::on_comboBoxAtom_editTextChanged(QString string)
{
  QByteArray byteArray = string.toLatin1();
  const char* s_char = byteArray.data();
  ui_.viewer->setAtomicNumber(OpenBabel::etab.GetAtomicNum(s_char));
}

void MainWindow::on_comboBoxForceField_currentIndexChanged(QString string)
{
  obForceField_ = OpenBabel::OBForceField::FindForceField(string.toStdString());
  if (obForceField_ == 0)
  {
    addToLog(QString("Force field %1 not found.").arg(string));
    ui_.groupBoxOptimization->setEnabled(false);
    ui_.groupBoxConformationalSearch->setEnabled(false);
  }
  else
  {
    addToLog(QString("Force field %1 found.").arg(string));
    ui_.groupBoxOptimization->setEnabled(true);
    ui_.groupBoxConformationalSearch->setEnabled(true);
  }
}

void MainWindow::on_pushButtonSearch_clicked()
{
  std::ostringstream os;

  setCursor(Qt::WaitCursor);
//  ui_.pushButtonOptimize->setText(tr("Running..."));
//  qApp->processEvents();
//  ui_.pushButtonOptimize->setEnabled(false);
  molecule_.searchConformers(obForceField_,
                             (Chemistry::SearchType)ui_.comboBoxSearchType->currentIndex(),
                             ui_.spinBoxConformersCount->value(),
                             ui_.spinBoxStepsCount->value(),
                             &os);
  addToLog(QString::fromStdString(os.str()));
//  ui_.pushButtonOptimize->setEnabled(true);
//  ui_.pushButtonOptimize->setText(tr("Run"));
  setCursor(Qt::ArrowCursor);
  fillConformersTable(ui_.doubleSpinBox->value());
  ui_.actionViewConformersTable->setChecked(true);
}
