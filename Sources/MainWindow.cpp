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

#include "MainWindow.h"

#include <QCompleter>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QtConcurrentRun>

#include <openbabel/forcefield.h>

#include "AboutDialog.h"
#include "ImportMoleculeFromFileDialog.h"
#include "FileMolGeneratorDialog.h"
#include "File/FileDal.h"
#include "File/FileMol.h"
#include "Render/RenderAtom.h"
#include "Render/RenderColor.h"
#include "Render/RenderViewer.h"

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    ui_(),
    viewer_(0),
    os_(),
    obForceField_(0),
    dir_(QDir::homePath())
{
  ui_.setupUi(this);

  newTab("untitled.mol");
  goToTab(0);

  setActionsViewEnabled(true);

#ifdef Q_OS_MAC
  ui_.toolBar->setIconSize(QSize(24, 24));
#endif

  ui_.logPlainTextEdit->hide();

  QStringList headerLabels;
  headerLabels << tr("Number") << tr("Energy");
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

  connect(ui_.doubleSpinBox, SIGNAL(valueChanged(double)),
          this, SLOT(fillConformersTable(double)));

  connect(ui_.actionMoleculeNew, SIGNAL(triggered()),
          this, SLOT(newMolecule()));
  connect(ui_.actionMoleculeOpen, SIGNAL(triggered()),
          this, SLOT(openMolecule()));
  connect(ui_.actionMoleculeImportFromFile, SIGNAL(triggered()),
          this, SLOT(showImportStructureFromFileDialog()));
  connect(ui_.actionMoleculeEditMolFile, SIGNAL(triggered()),
          this, SLOT(showFileMolGeneratorDialog()));

  connect(ui_.actionHelpAbout, SIGNAL(triggered()),
          this, SLOT(showAboutDialog()));
  connect(ui_.actionHelpAboutQt, SIGNAL(triggered()),
          qApp, SLOT(aboutQt()));

  connect(ui_.pushButtonOptimize, SIGNAL(clicked()),
          this, SLOT(startOptimization()));
  connect(ui_.pushButtonSearch, SIGNAL(clicked()),
          this, SLOT(startConformationalSearch()));

  connect(ui_.tabWidget, SIGNAL(currentChanged(int)),
          this, SLOT(goToTab(int)));

  connect(ui_.tableWidgetConformers, SIGNAL(currentCellChanged(int,int,int,int)),
          this, SLOT(onTableWidgetConformersCurrentCellChanged(int)));
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
  Chemistry::Molecule molecule;

  if (!fileName.isEmpty())
  {
    addToLog(QString("Import structure from %1 started.").arg(fileName));
    if (molecule.importFromFile(fileName, obFormat))
    {
      addToLog("Import succeeded.");
      newTab(QFileInfo(fileName).baseName() + ".mol*");
      ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
      File::Mol fileMol;
      fileMol.setAbsoluteFilePath(QFileInfo(fileName).baseName() + ".mol*");
      fileMol.setMolecule(molecule);
      viewer_->setFileMol(fileMol);
      setActionsViewEnabled(true);
      updateActionsForNonemptyMolecule();
    }
    else
    {
      addToLog("Import failed.");
      QMessageBox::critical(this, QCoreApplication::applicationName(),
                            "Can't import file " + fileName);
    }
  }
}

const QString& MainWindow::dir() const
{
  return dir_;
}

void MainWindow::setDir(const QString &dir)
{
  dir_ = dir;
}

// Slots.

void MainWindow::newMolecule()
{
  newTab("untitled.mol");
  ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
}

void MainWindow::openMolecule()
{
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open *.mol file"), dir_, tr("*.mol files(*.mol)"));

  if (!fileName.isEmpty())
  {
    dir_ = QFileInfo(fileName).absolutePath();
    addToLog(QString(
        tr("Reading from *.mol file started. \nFileName: %1")).arg(fileName));
    File::Mol molFile;
    molFile.setAbsoluteFilePath(fileName);

    if (molFile.read())
    {
      addToLog(tr("Reading succeeded."));
      if (molFile.parse(true))
      {
        addToLog(tr("Parsing succeeded."));
        newTab(molFile.fileName());
        ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
        viewer_->setFileMol(molFile);
      }
      else
      {
        addToLog(tr("Parsing failed."));
        QMessageBox messageBox;
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setText(tr("Can't parse *.mol file ")
                           + molFile.fileName());
  //      messageBox.setInformativeText("Error at line #" + QString::number(
  //          molFile.parseError().index + 1));
  //      messageBox.setDetailedText(molFile.parseError().message);
        messageBox.setStandardButtons(QMessageBox::Ok);
        messageBox.exec();
      }
    }
    else
    {
      QMessageBox messageBox;
      messageBox.setIcon(QMessageBox::Critical);
      messageBox.setText(tr("Can't read *.mol file ")
                         + molFile.fileName());
      addToLog(tr("Reading failed."));
    }
  }
}

void MainWindow::fillConformersTable(double threshold)
{
  ui_.tableWidgetConformers->clearContents();
  ui_.tableWidgetConformers->setRowCount(0);

  QList<bool> list;

  for (int i = 0; i < viewer_->fileMol().molecule()->conformersCount(); ++i)
    list.append(true);

  for (quint16 i = 0; i < list.count(); ++i)
    if (list[i])
      for (quint16 j = i + 1; j < list.count(); ++j)
        if (list[j] && qAbs(viewer_->fileMol().molecule()->conformerEnergy(i) -
                            viewer_->fileMol().molecule()->conformerEnergy(j)) <= threshold)
          list[j] = false;

  for (int i = 0; i < viewer_->fileMol().molecule()->conformersCount(); ++i)
    if (list[i])
    {
      ui_.tableWidgetConformers->insertRow(
          ui_.tableWidgetConformers->rowCount());
      QTableWidgetItem* newItem = new QTableWidgetItem(
          QString::number(i + 1).rightJustified(3, '0'));
      ui_.tableWidgetConformers->setItem(
          ui_.tableWidgetConformers->rowCount() - 1, 0, newItem);

      newItem = new QTableWidgetItem(QString::number(
          viewer_->fileMol().molecule()->conformerEnergy(i)));
      ui_.tableWidgetConformers->setItem(
          ui_.tableWidgetConformers->rowCount() - 1, 1, newItem);
    }

  viewer_->displayConformer(0);
}

void MainWindow::newTab(const QString& title)
{
  QWidget* tab = new QWidget();
  QGridLayout* gridLayout = new QGridLayout(tab);
  Render::Viewer* viewer = new Render::Viewer(tab);
  gridLayout->addWidget(viewer, 0, 0, 1, 1);
  ui_.tabWidget->addTab(tab, title);
}

void MainWindow::goToTab(int index)
{
  if (viewer_ != 0)
  {
    viewer_->fileMol().molecule()->disconnect();
    disconnect(ui_.actionStructureAddHydrogens, SIGNAL(triggered()),
            viewer_->fileMol().molecule(), SLOT(addHydrogensAndBuild()));
    disconnect(ui_.actionStructureRemoveHydrogens, SIGNAL(triggered()),
            viewer_->fileMol().molecule(), SLOT(removeHydrogens()));

  }
  viewer_ = ui_.tabWidget->currentWidget()->findChild<Render::Viewer*>();
  // Set connections.
  connect(viewer_->fileMol().molecule(), SIGNAL(becameEmpty()),
          this, SLOT(updateActionsForEmptyMolecule()));
  connect(viewer_->fileMol().molecule(), SIGNAL(becameNonempty()),
          this, SLOT(updateActionsForNonemptyMolecule()));
  connect(viewer_->fileMol().molecule(), SIGNAL(geometryChanged()),
          viewer_, SLOT(updateMolecule()));
  connect(viewer_->fileMol().molecule(), SIGNAL(optimizationFinished()),
          this, SLOT(onOptimizationFinished()));
  connect(viewer_->fileMol().molecule(), SIGNAL(conformationalSearchFinished()),
          this, SLOT(onConformationalSearchFinished()));
  connect(ui_.actionStructureAddHydrogens, SIGNAL(triggered()),
          viewer_->fileMol().molecule(), SLOT(addHydrogensAndBuild()));
  connect(ui_.actionStructureRemoveHydrogens, SIGNAL(triggered()),
          viewer_->fileMol().molecule(), SLOT(removeHydrogens()));
}

void MainWindow::showImportStructureFromFileDialog()
{
  ImportMoleculeFromFileDialog dialog(this);
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

void MainWindow::showFileMolGeneratorDialog()
{
  FileMolGeneratorDialog dialog(this);
  viewer_->fileMol().generate();
  dialog.setText(viewer_->fileMol().text());
  dialog.exec();
}

void MainWindow::setActionsViewEnabled(bool enabled)
{
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
}

void MainWindow::startOptimization()
{
  ui_.centralWidget->setEnabled(false);
  ui_.menuBar->setEnabled(false);
  ui_.toolBar->setEnabled(false);
  viewer_->fileMol().molecule()->setObForceFieldName(
      ui_.comboBoxForceField->currentText());
  QFuture<void> future = QtConcurrent::run(
      viewer_->fileMol().molecule(),
      &Chemistry::Molecule::optimize,
      (Chemistry::Algorithm)ui_.comboBoxAlgorithm->currentIndex(),
      powf(10, -ui_.spinBoxConvergence->value()),
      ui_.spinBoxMaxSteps->value(),
      ui_.spinBoxStepsPerUpdate->value(),
      &os_);
}

void MainWindow::onOptimizationFinished()
{
  ui_.centralWidget->setEnabled(true);
  ui_.menuBar->setEnabled(true);
  ui_.toolBar->setEnabled(true);
  addToLog(QString::fromStdString(os_.str()));
  os_.clear();
  os_.seekp(0);
  viewer_->updateMolecule();
}

void MainWindow::startConformationalSearch()
{
  ui_.centralWidget->setEnabled(false);
  ui_.menuBar->setEnabled(false);
  ui_.toolBar->setEnabled(false);
  viewer_->fileMol().molecule()->setObForceFieldName(
      ui_.comboBoxForceField->currentText());
  QFuture<void> future = QtConcurrent::run(
      viewer_->fileMol().molecule(),
      &Chemistry::Molecule::searchConformers,
      (Chemistry::SearchType)ui_.comboBoxSearchType->currentIndex(),
      ui_.spinBoxConformersCount->value(),
      ui_.spinBoxStepsCount->value(),
      &os_);
}

void MainWindow::onConformationalSearchFinished()
{
  ui_.centralWidget->setEnabled(true);
  ui_.menuBar->setEnabled(true);
  ui_.toolBar->setEnabled(true);
  addToLog(QString::fromStdString(os_.str()));
  os_.clear();
  os_.seekp(0);
  if (viewer_->fileMol().molecule()->conformersCount() > 1)
  {
    fillConformersTable(ui_.doubleSpinBox->value());
    ui_.toolBox->setCurrentIndex(2);
  }
}

void MainWindow::onTableWidgetConformersCurrentCellChanged(int currentRow)
{
  bool ok;

  if (currentRow >= 0)
  {
    quint16 n = ui_.tableWidgetConformers->item(currentRow, 0)->text().toUInt(&ok);
    if (ok)
      viewer_->displayConformer(n - 1);
  }
}

//----------

void MainWindow::on_doubleSpinBoxAxesSize_valueChanged(double value)
{
  viewer_->setAxes(true, value);
}

void MainWindow::on_comboBoxView_currentIndexChanged(int index)
{
 viewer_->setView(Render::Viewer::View(index));
}

void MainWindow::on_actionStructureExportImage_triggered()
{
  viewer_->saveSnapshot(false, false);
}

void MainWindow::on_actionExit_triggered()
{
  if (QMessageBox::information(
      this,
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
    ui_.tabWidget->currentWidget()->findChild<Render::Viewer*>()->setAxes(
        true, ui_.doubleSpinBoxAxesSize->value());
  }
  else
  {
    ui_.doubleSpinBoxAxesSize->setEnabled(false);
    ui_.tabWidget->currentWidget()->findChild<Render::Viewer*>()->setAxes(
        false, ui_.doubleSpinBoxAxesSize->value());
  }
}

void MainWindow::on_checkBoxDebugInfo_clicked(bool checked)
{
  ui_.tabWidget->currentWidget()->findChild<Render::Viewer*>()->setDebugInfoVisibility(checked);
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

void MainWindow::on_comboBoxAtom_editTextChanged(QString string)
{
  QByteArray byteArray = string.toLatin1();
  const char* s_char = byteArray.data();
  viewer_->setAtomicNumber(OpenBabel::etab.GetAtomicNum(s_char));
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
