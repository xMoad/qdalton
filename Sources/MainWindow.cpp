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
#include "File/FileDal.h"
#include "File/FileMol.h"
#include "Render/RenderAtom.h"
#include "Render/RenderColor.h"
#include "Render/RenderViewer.h"

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    viewer_(0),
    obForceField_(0),
    workDir_(QDir::currentPath())
{
  ui_.setupUi(this);

  newTab("untitled.mol");
  goToTab(0);

  setActionsViewEnabled(true);

#ifdef Q_OS_MAC
  ui_.toolBar->setIconSize(QSize(24, 24));
#endif

  ui_.logPlainTextEdit->hide();
  ui_.widgetConformers->hide();

  QWidget* widget = new QWidget(statusBar());
  QHBoxLayout* layout = new QHBoxLayout(widget);
  QLabel* labelStatus = new QLabel(workDir_, widget);
  layout->addWidget(labelStatus, 1);
  widget->setLayout(layout);
  layout->setContentsMargins(QMargins());
  statusBar()->addWidget(widget, 1);
  statusBar()->setSizeGripEnabled(false);

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

  connect(ui_.actionHelpAbout, SIGNAL(triggered()),
          this, SLOT(showAboutDialog()));
  connect(ui_.actionHelpAboutQt, SIGNAL(triggered()),
          qApp, SLOT(aboutQt()));

  connect(ui_.pushButtonSearch, SIGNAL(clicked()),
          this, SLOT(startConformationalSearch()));

  connect(ui_.tabWidget, SIGNAL(currentChanged(int)),
          this, SLOT(goToTab(int)));
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
  Chemistry::Molecule molecule;

  if (!fileName.isEmpty())
  {
    addToLog(QString("Import structure from %1 started.").arg(fileName));
    if (molecule.importFromFile(fileName, obFormat))
    {
      addToLog("Import succeeded.");
      newTab(QFileInfo(fileName).baseName() + ".mol*");
      ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
      viewer_->setMolecule(molecule);
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

// Slots.

void MainWindow::newMolecule()
{
  newTab("untitled.mol");
  ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
}

void MainWindow::openMolecule()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Open *.mol file",
      getWorkDir(), "*.mol files(*.mol)");

  if (!fileName.isEmpty())
  {
    addToLog(QString("Reading from *.mol file started. \nFileName: %1").arg(fileName));
    File::Mol molFile(fileName);
    molFile.read();

    if (molFile.parse(true))
    {
      addToLog("Reading succeeded.");
      setWorkDir(molFile.filePath());
      newTab(molFile.fileName());
      viewer_->setMolecule(molFile.molecule());
      ui_.tabWidget->setCurrentIndex(ui_.tabWidget->count() - 1);
    }
    else
    {
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

void MainWindow::fillConformersTable(double threshold)
{
  QList<bool> list;

  ui_.tableWidgetConformers->setRowCount(0);

  for (int i = 0; i < viewer_->molecule()->conformersCount(); ++i)
    list.append(true);

  for (quint16 i = 0; i < list.count(); ++i)
    if (list[i])
      for (quint16 j = i + 1; j < list.count(); ++j)
        if (list[j] && qAbs(viewer_->molecule()->conformerEnergy(i) -
                            viewer_->molecule()->conformerEnergy(j)) <= threshold)
          list[j] = false;

  for (int i = 0; i < viewer_->molecule()->conformersCount(); ++i)
    if (list[i])
    {
      ui_.tableWidgetConformers->insertRow(
          ui_.tableWidgetConformers->rowCount());
      QTableWidgetItem* newItem = new QTableWidgetItem(
          QString::number(i + 1).rightJustified(3, '0'));
      ui_.tableWidgetConformers->setItem(
          ui_.tableWidgetConformers->rowCount() - 1, 0, newItem);

      newItem = new QTableWidgetItem(QString::number(
          viewer_->molecule()->conformerEnergy(i)));
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
    viewer_->molecule()->disconnect();
    disconnect(ui_.actionStructureAddHydrogens, SIGNAL(triggered()),
            viewer_->molecule(), SLOT(addHydrogensAndBuild()));
    disconnect(ui_.actionStructureRemoveHydrogens, SIGNAL(triggered()),
            viewer_->molecule(), SLOT(removeHydrogens()));

  }
  viewer_ = ui_.tabWidget->currentWidget()->findChild<Render::Viewer*>();
  // Set connections.
  connect(viewer_->molecule(), SIGNAL(becameEmpty()),
          this, SLOT(updateActionsForEmptyMolecule()));
  connect(viewer_->molecule(), SIGNAL(becameNonempty()),
          this, SLOT(updateActionsForNonemptyMolecule()));
  connect(viewer_->molecule(), SIGNAL(geometryChanged()),
          viewer_, SLOT(updateMolecule()));
  connect(viewer_->molecule(), SIGNAL(conformationalSearchFinished()),
          this, SLOT(onConformationalSearchFinished()));
  connect(ui_.actionStructureAddHydrogens, SIGNAL(triggered()),
          viewer_->molecule(), SLOT(addHydrogensAndBuild()));
  connect(ui_.actionStructureRemoveHydrogens, SIGNAL(triggered()),
          viewer_->molecule(), SLOT(removeHydrogens()));
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
}

void MainWindow::startConformationalSearch()
{
  ui_.centralWidget->setEnabled(false);
  ui_.menuBar->setEnabled(false);
  ui_.toolBar->setEnabled(false);
  QFuture<void> future_ = QtConcurrent::run(
      viewer_->molecule(),
      &Chemistry::Molecule::searchConformers,
      obForceField_,
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
  fillConformersTable(ui_.doubleSpinBox->value());
  ui_.actionViewConformersTable->setChecked(true);
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
    ui_.tabWidget->currentWidget()->findChild<Render::Viewer*>()->setAxes(true, ui_.doubleSpinBoxAxesSize->value());
  }
  else
  {
    ui_.doubleSpinBoxAxesSize->setEnabled(false);
    ui_.tabWidget->currentWidget()->findChild<Render::Viewer*>()->setAxes(false, ui_.doubleSpinBoxAxesSize->value());
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

void MainWindow::on_pushButtonOptimize_clicked()
{
  std::ostringstream os;

  setCursor(Qt::WaitCursor);
  ui_.pushButtonOptimize->setText(tr("Running..."));
  qApp->processEvents();
  ui_.pushButtonOptimize->setEnabled(false);
  viewer_->molecule()->optimize(obForceField_,
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

void MainWindow::on_tableWidgetConformers_cellClicked(int row, int column)
{
  bool ok;

  quint16 n = ui_.tableWidgetConformers->item(row, 0)->text().toUInt(&ok);
  if (ok)
    viewer_->displayConformer(n - 1);
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
