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
#include <QFontDialog>
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
    os_(),
    obForceFieldName_(),
    dir_(QDir::homePath())
{
  ui_.setupUi(this);

  File::Mol fileMol;
  newTab("untitled.mol", fileMol);
  goToTab(0);

  ui_.plainTextEditLog->hide();

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
  connect(ui_.actionMoleculeSave, SIGNAL(triggered()),
          this, SLOT(saveMolecule()));
  connect(ui_.actionMoleculeSaveAs, SIGNAL(triggered()),
          this, SLOT(saveMoleculeAs()));

  connect(ui_.actionViewToolBox, SIGNAL(toggled(bool)),
          ui_.toolBox, SLOT(setVisible(bool)));
  connect(ui_.actionViewLog, SIGNAL(toggled(bool)),
          ui_.plainTextEditLog, SLOT(setVisible(bool)));

  connect(ui_.actionHelpAbout, SIGNAL(triggered()),
          this, SLOT(showAboutDialog()));
  connect(ui_.actionHelpAboutQt, SIGNAL(triggered()),
          qApp, SLOT(aboutQt()));

  connect(ui_.pushButtonLabelsOnAtomsFont, SIGNAL(clicked()),
          this, SLOT(showFontDialogForLabelsOnAtoms()));
  connect(ui_.pushButtonLabelsOnBondsFont, SIGNAL(clicked()),
          this, SLOT(showFontDialogForLabelsOnBonds()));

  connect(ui_.comboBoxForceField, SIGNAL(currentIndexChanged(QString)),
          this, SLOT(setObForceFieldName(QString)));
  connect(ui_.comboBoxSearchType, SIGNAL(currentIndexChanged(int)),
          this, SLOT(onSearchTypeChanged(int)));
  connect(ui_.pushButtonOptimize, SIGNAL(clicked()),
          this, SLOT(startOptimization()));
  connect(ui_.pushButtonSearch, SIGNAL(clicked()),
          this, SLOT(startConformationalSearch()));

  connect(ui_.tabWidget, SIGNAL(currentChanged(int)),
          this, SLOT(goToTab(int)));
  connect(ui_.tabWidget, SIGNAL(tabCloseRequested(int)),
          this, SLOT(onTabCloseRequested(int)));

  connect(ui_.tableWidgetConformers, SIGNAL(currentCellChanged(int,int,int,int)),
          this, SLOT(onTableWidgetConformersCurrentCellChanged(int)));

  ui_.comboBoxForceField->setCurrentIndex(2);
}

void MainWindow::addToLog(const QString& string)
{
  QDateTime dateTime_ = QDateTime::currentDateTime();
  ui_.plainTextEditLog->appendHtml(QString("<b>%1</b>").arg(dateTime_.time().toString()));
  ui_.plainTextEditLog->appendPlainText(string);
  ui_.plainTextEditLog->appendPlainText("");
}

void MainWindow::importMoleculeFromFile(const QString& fileName,
                                        OpenBabel::OBFormat* obFormat)
{
  Render::Molecule molecule;

  if (!fileName.isEmpty())
  {
    addToLog(QString("Import structure from %1 started.").arg(fileName));
    if (molecule.importFromFile(fileName, obFormat))
    {
      File::Mol fileMol(molecule);

      addToLog("Import succeeded.");
      newTab("untitled.mol", fileMol);

      ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
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
  File::Mol fileMol;
  newTab("untitled.mol", fileMol);
  ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
}

void MainWindow::openMolecule()
{
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open *.mol file"), dir(), tr("*.mol files(*.mol)"));

  if (!fileName.isEmpty())
  {
    for (int i = 0; i < ui_.tabWidget->count(); ++i)
      if (ui_.tabWidget->widget(i)->findChild<Render::Viewer*>()->fileMol().absoluteFilePath() == fileName)
      {
      if (QMessageBox::information(
          this,
          QCoreApplication::applicationName(),
          QString("File %1 already opened. Show it?").arg(fileName),
          QMessageBox::Ok | QMessageBox::Cancel,
          QMessageBox::Ok) == QMessageBox::Ok)
        ui_.tabWidget->setCurrentIndex(i);
      return;
    }


    setDir(QFileInfo(fileName).absolutePath());
    addToLog(QString(
        tr("Reading from *.mol file started. \nFileName: %1")).arg(fileName));
    File::Mol molFile;
    molFile.setAbsoluteFilePath(fileName);

    if (molFile.read())
    {
      addToLog(tr("Reading succeeded."));
      molFile.parse(true);
      if (molFile.parsedSuccessfully())
      {
        addToLog(tr("Parsing succeeded."));
        newTab(molFile.fileName(), molFile);
        ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
        updateActionsForNonemptyMolecule();
      }
      else
      {
        addToLog(tr("Parsing failed."));
        QMessageBox messageBox;
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setText(tr("Can't parse *.mol file ")
                           + molFile.fileName());
        messageBox.setInformativeText("Errors: " + QString::number(molFile.parseErrorsCount()));

        QString detailedText;
        for (int i = 0; i < molFile.parseErrorsCount(); ++i)
        {
          detailedText += "Line #" + QString::number(molFile.parseError(i).index() + 1) + ": " + molFile.parseError(i).message();
        }
        messageBox.setDetailedText(detailedText);
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

void MainWindow::saveMolecule()
{
  if (currentViewer().fileMol().absoluteFilePath().isEmpty())
    saveMoleculeAs();
  else
    saveMoleculeAs(currentViewer().fileMol().absoluteFilePath());
}

void MainWindow::saveMoleculeAs(const QString& absoluteFilePath)
{
  QString fileName;

  if (absoluteFilePath.isEmpty())
    fileName = QFileDialog::getSaveFileName(
        this, tr("Save *.mol file"), dir(), tr("*.mol files(*.mol)"));
  else
    fileName = absoluteFilePath;

  if (!fileName.isEmpty())
  {
    setDir(QFileInfo(fileName).absolutePath());
    addToLog(QString(
        tr("Saving *.mol file started. \nFileName: %1")).arg(fileName));
    if (currentViewer().fileMol().generate())
    {
      if (currentViewer().fileMol().save(fileName))
      {
        ui_.tabWidget->setTabText(ui_.tabWidget->currentIndex(),
                                  currentViewer().fileMol().fileName());
        addToLog(tr("Saving succeeded."));
      }
    }
  }
}

//void MainWindow::fillConformersTable(double threshold)
//{
//  ui_.tableWidgetConformers->clearContents();
//  ui_.tableWidgetConformers->setRowCount(0);
//
//  QList<bool> list;
//
//  for (int i = 0; i < currentViewer().fileMol().molecule().conformersCount(); ++i)
//    list.append(true);
//
//  for (quint16 i = 0; i < list.count(); ++i)
//    if (list[i])
//      for (quint16 j = i + 1; j < list.count(); ++j)
//        if (list[j] && qAbs(currentViewer().fileMol().molecule().conformerEnergy(i) -
//                            currentViewer().fileMol().molecule().conformerEnergy(j)) <= threshold)
//          list[j] = false;
//
//  for (int i = 0; i < currentViewer().fileMol().molecule().conformersCount(); ++i)
//    if (list[i])
//    {
//    ui_.tableWidgetConformers->insertRow(
//        ui_.tableWidgetConformers->rowCount());
//    QTableWidgetItem* newItem = new QTableWidgetItem(
//        QString::number(i + 1).rightJustified(3, '0'));
//    ui_.tableWidgetConformers->setItem(
//        ui_.tableWidgetConformers->rowCount() - 1, 0, newItem);
//
//    newItem = new QTableWidgetItem(QString::number(
//        currentViewer().fileMol().molecule().conformerEnergy(i)));
//    ui_.tableWidgetConformers->setItem(
//        ui_.tableWidgetConformers->rowCount() - 1, 1, newItem);
//  }
//
//  currentViewer().displayConformer(0);
//}

void MainWindow::newTab(const QString& title, const File::Mol& fileMol)
{
  QWidget* tab = new QWidget();
  QGridLayout* gridLayout = new QGridLayout(tab);
  Render::Viewer* viewer = new Render::Viewer(tab, fileMol);
  gridLayout->addWidget(viewer, 0, 0, 1, 1);
  ui_.tabWidget->addTab(tab, title);
}

void MainWindow::goToTab(int index)
{
  if (index >= 0)
  {
    currentViewer().fileMol().molecule().disconnect();
    ui_.actionStructureAddHydrogens->disconnect();
    ui_.actionStructureRemoveHydrogens->disconnect();
    ui_.checkBoxAxes->disconnect();
    ui_.doubleSpinBoxAxesSize->disconnect();
    ui_.checkBoxDebugInfo->disconnect();
    ui_.comboBoxView->disconnect();
    ui_.comboBoxAtom->disconnect();

    ui_.checkBoxAxes->setChecked(currentViewer().isAxesVisible());
    ui_.doubleSpinBoxAxesSize->setValue(currentViewer().axesSize());
    ui_.checkBoxDebugInfo->setChecked(currentViewer().isDebugInfoVisible());
    ui_.comboBoxView->setCurrentIndex(currentViewer().view());
    ui_.comboBoxAtom->lineEdit()->setText(currentViewer().atomSymbol());

    // Set connections.
    connect(&currentViewer().fileMol().molecule(), SIGNAL(becameEmpty()),
            this, SLOT(updateActionsForEmptyMolecule()));
    connect(&currentViewer().fileMol().molecule(), SIGNAL(becameNonempty()),
            this, SLOT(updateActionsForNonemptyMolecule()));
    connect(&currentViewer().fileMol().molecule(), SIGNAL(geometryChanged()),
            &currentViewer(), SLOT(updateGL()));
    connect(&currentViewer().fileMol().molecule(), SIGNAL(optimizationFinished()),
            this, SLOT(onOptimizationFinished()));
    connect(&currentViewer().fileMol().molecule(), SIGNAL(conformationalSearchFinished()),
            this, SLOT(onConformationalSearchFinished()));

    connect(ui_.actionStructureAddHydrogens, SIGNAL(triggered()),
            &currentViewer().fileMol().molecule(), SLOT(addHydrogensAndBuild()));
    connect(ui_.actionStructureRemoveHydrogens, SIGNAL(triggered()),
            &currentViewer().fileMol().molecule(), SLOT(removeHydrogens()));

    connect(ui_.checkBoxAxes, SIGNAL(toggled(bool)),
            &currentViewer(), SLOT(setAxesVisible(bool)));
    connect(ui_.doubleSpinBoxAxesSize, SIGNAL(valueChanged(double)),
            &currentViewer(), SLOT(setAxesSize(double)));
    connect(ui_.checkBoxDebugInfo, SIGNAL(toggled(bool)),
            &currentViewer(), SLOT(setDebugInfoVisible(bool)));
    connect(ui_.comboBoxView, SIGNAL(currentIndexChanged(int)),
            &currentViewer(), SLOT(setView(int)));
    connect(ui_.comboBoxLabelsOnAtoms, SIGNAL(currentIndexChanged(int)),
            &currentViewer(), SLOT(setLabelsOnAtoms(int)));
    connect(ui_.comboBoxLabelsOnBonds, SIGNAL(currentIndexChanged(int)),
            &currentViewer(), SLOT(setLabelsOnBonds(int)));
    connect(ui_.comboBoxAtom, SIGNAL(editTextChanged(QString)),
            &currentViewer(), SLOT(setAtomSymbol(QString)));
  }
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
  currentViewer().fileMol().generate();

  FileMolGeneratorDialog dialog(&currentViewer().fileMol(), this);
  dialog.setWindowTitle(currentViewer().fileMol().fileName());
  dialog.exec();
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
  //  ui_.actionStructureRemoveHydrogens->setEnabled(true);
  //  ui_.actionStructureConformations->setEnabled(true);
  ui_.actionStructureExportImage->setEnabled(true);
}

void MainWindow::startOptimization()
{
  ui_.centralWidget->setEnabled(false);
  ui_.menuBar->setEnabled(false);
  ui_.toolBar->setEnabled(false);
  QCoreApplication::processEvents();
  currentViewer().fileMol().molecule().optimize(
      ui_.comboBoxForceField->currentText(),
      ui_.spinBoxMaxStepsCount->value(),
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
  currentViewer().updateGL();
}

//void MainWindow::startConformationalSearch()
//{
//  ui_.centralWidget->setEnabled(false);
//  ui_.menuBar->setEnabled(false);
//  ui_.toolBar->setEnabled(false);
//  QCoreApplication::processEvents();
//  currentViewer().fileMol().molecule().searchConformers(
//      ui_.comboBoxForceField->currentText(),
//      (Render::SearchType)ui_.comboBoxSearchType->currentIndex(),
//      ui_.spinBoxConformersCount->value(),
//      ui_.spinBoxMaxStepsCount->value(),
//      &os_);
//}

//void MainWindow::onConformationalSearchFinished()
//{
//  ui_.centralWidget->setEnabled(true);
//  ui_.menuBar->setEnabled(true);
//  ui_.toolBar->setEnabled(true);
//  addToLog(QString::fromStdString(os_.str()));
//  os_.clear();
//  os_.seekp(0);
//  if (currentViewer().fileMol().molecule().conformersCount() > 1)
//  {
//    fillConformersTable(ui_.doubleSpinBox->value());
//    ui_.toolBox->setCurrentIndex(2);
//  }
//}

//void MainWindow::onTableWidgetConformersCurrentCellChanged(int currentRow)
//{
//  bool ok;
//
//  if (currentRow >= 0)
//  {
//    quint16 n = ui_.tableWidgetConformers->item(currentRow, 0)->text().toUInt(&ok);
//    if (ok)
//      currentViewer().displayConformer(n - 1);
//  }
//}

void MainWindow::onSearchTypeChanged(int index)
{
  if (index == 0)
  {
    ui_.spinBoxConformersCount->setEnabled(false);
    ui_.labelConformersCount->setEnabled(false);
  }
  else
  {
    ui_.spinBoxConformersCount->setEnabled(true);
    ui_.labelConformersCount->setEnabled(true);
  }
}

//----------

void MainWindow::on_actionStructureExportImage_triggered()
{
  currentViewer().saveSnapshot(false, false);
}

void MainWindow::on_actionExit_triggered()
{
  if (QMessageBox::information(
      this,
      QCoreApplication::applicationName(),
      QString("Exit %1?").arg(QCoreApplication::applicationName()),
      QMessageBox::Ok | QMessageBox::Cancel,
      QMessageBox::Cancel) == QMessageBox::Ok)
  {
    close();
  }
}

void MainWindow::setObForceFieldName(const QString& obForceFieldName)
{
  obForceFieldName_ = obForceFieldName;
}

void MainWindow::showFontDialogForLabelsOnAtoms()
{
  currentViewer().setLabelsOnAtomsFont(QFontDialog::getFont(
      0, currentViewer().labelsOnAtomsFont()));
}

void MainWindow::showFontDialogForLabelsOnBonds()
{
  currentViewer().setLabelsOnBondsFont(QFontDialog::getFont(
      0, currentViewer().labelsOnBondsFont()));
}

void MainWindow::onTabCloseRequested(int index)
{
  //  if (index > 0)
  if (QMessageBox::information(
      this,
      QCoreApplication::applicationName(),
      QString("Close tab %1?").arg(ui_.tabWidget->tabText(index)),
      QMessageBox::Ok | QMessageBox::Cancel,
      QMessageBox::Cancel) == QMessageBox::Ok)
  {
    ui_.tabWidget->widget(index)->deleteLater();
    ui_.tabWidget->removeTab(index);
  }
}

//--------------------------------

Render::Viewer& MainWindow::currentViewer()
{
  return *ui_.tabWidget->currentWidget()->findChild<Render::Viewer*>();
}

Render::Viewer& MainWindow::viewer(int index)
{
  if (index < 0)
    index = ui_.tabWidget->count() + index;
  return *ui_.tabWidget->widget(index)->findChild<Render::Viewer*>();
}
