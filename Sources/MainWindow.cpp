#include "MainWindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSplitter>

#include "AboutDialog.h"
#include "Render/RenderAtom.h"
#include "Render/RenderColor.h"
#include "File/FileDal.h"
#include "File/FileMol.h"

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    molecule_(),
    dalFile_(0),
    molFile_(0),
    workDir_(QDir::currentPath())
{
  ui_->setupUi(this);

  // Hide all tabs
  ui_->tabWidget->hide();

  ui_->logPlainTextEdit->hide();
  ui_->widgetConformers->hide();

  ui_->statusbar->addWidget(&statusLabel_, 1);
  statusLabel_.setText(workDir_);

  QStringList headerLabels;
  headerLabels << " Number" << "Energy";
  ui_->tableWidgetConformers->setHorizontalHeaderLabels(headerLabels);
  ui_->tableWidgetConformers->horizontalHeader()->setVisible(true);

  connect(&molecule_, SIGNAL(formulaChanged()),
          this, SLOT(updateActions()));
  connect(&molecule_, SIGNAL(geometryChanged()),
          ui_->viewer, SLOT(updateMolecule()));
}

MainWindow::~MainWindow()
{
  delete ui_;
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
  ui_->logPlainTextEdit->appendHtml(QString("<b>%1</b>").arg(dateTime_.time().toString()));
  ui_->logPlainTextEdit->appendPlainText(string);
  ui_->logPlainTextEdit->appendPlainText("");
}

void MainWindow::updateActions()
{
  if (molecule_.atomsCount() != 0)
  {
    ui_->actionStructureBuild->setEnabled(true);
    ui_->actionStructureAddHydrogens->setEnabled(true);
    ui_->actionStructureRemoveHydrogens->setEnabled(true);
    ui_->actionStructureBuild->setEnabled(true);
    ui_->actionStructureConformations->setEnabled(true);
    ui_->actionStructureExportImage->setEnabled(true);
    ui_->actionStructureExportXyz->setEnabled(true);
  }
  else
  {
    ui_->actionStructureBuild->setEnabled(false);
    ui_->actionStructureAddHydrogens->setEnabled(false);
    ui_->actionStructureRemoveHydrogens->setEnabled(false);
    ui_->actionStructureBuild->setEnabled(false);
    ui_->actionStructureConformations->setEnabled(false);
    ui_->actionStructureExportImage->setEnabled(false);
    ui_->actionStructureExportImage->setEnabled(false);
  }
}

void MainWindow::on_doubleSpinBoxAxesSize_valueChanged(double value)
{
  ui_->viewer->setAxes(true, value);
}

void MainWindow::on_actionJobImportDalton_triggered()
{
  QString fileName;

  if (molecule_.atomsCount() != 0)
  {
    QMessageBox::StandardButton reply;
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
          ui_->plainTextEditDal->setPlainText(dalFile.text());
          ui_->plainTextEditMol->setPlainText(molFile.text());
          // Set groupBoxes titles
          ui_->groupBoxDalFile->setTitle("Dalton input file: "
                                        + dalFile.fileName());
          ui_->groupBoxMolFile->setTitle("Molecule input file: "
                                        + molFile.fileName());
          switch (dalFile.jobType())
          {
          case File::Dal::JobTypeOptimize:
            ui_->comboBoxJobType->setCurrentIndex(0);
            break;
          case File::Dal::JobTypeWalk:
            ui_->comboBoxJobType->setCurrentIndex(1);
            break;
          case File::Dal::JobTypeRun:
            ui_->comboBoxJobType->setCurrentIndex(2);
            break;
          }
          switch (molFile.basisType())
          {
          case File::Mol::BasisTypeAtombasis:
            ui_->comboBoxBasisType->setCurrentIndex(0);
            break;
          case File::Mol::BasisTypeBasis:
            ui_->comboBoxBasisType->setCurrentIndex(1);
            break;
          case File::Mol::BasisTypeIntgrl:
            ui_->comboBoxBasisType->setCurrentIndex(2);
            break;
          }
          ui_->tabWidget->setTabText(0, "Job ()");
          ui_->tabWidget->setTabText(1, QString("DALTON Input (%1)").arg(dalFile.fileName()));
          ui_->tabWidget->setTabText(2, QString("MOLECULE Input (%1)").arg(molFile.fileName()));
          ui_->tabWidget->setTabText(3, QString("Viewer (%1)").arg("Input geometry"));
          ui_->welcomeWidget->hide();
          ui_->tabWidget->show();

          ui_->tabWidget->setCurrentIndex(3);
          molecule_ = molFile.molecule();
          ui_->viewer->setMolecule(&molecule_);
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
    ui_->viewer->setView(Render::Viewer::ViewBallsAndBonds);
    break;
  case 1:
    ui_->viewer->setView(Render::Viewer::ViewBallsAndSticks);
    break;
  case 2:
    ui_->viewer->setView(Render::Viewer::ViewSticks);
    break;
  case 3:
    ui_->viewer->setView(Render::Viewer::ViewVdWSpheres);
    break;
  }
}

void MainWindow::on_toolBox_currentChanged(int index)
{
  switch (index)
  {
  case 0:
  case 2:
    ui_->viewer->setMode(ui_->viewer->ModeView);
    break;
  case 1:
    ui_->viewer->setMode(ui_->viewer->ModeEdit);
    break;
  }
}

void MainWindow::on_actionJobNew_triggered()
{
  ui_->tabWidget->setTabText(0, "Job");
  ui_->tabWidget->setTabText(1, QString("DALTON Input"));
  ui_->tabWidget->setTabText(2, QString("MOLECULE Input"));
  ui_->tabWidget->setTabText(3, QString("Viewer (%1)").arg("Input geometry"));
  ui_->welcomeWidget->hide();
  ui_->tabWidget->show();
  ui_->viewer->setMolecule(&molecule_);
}

void MainWindow::on_comboBoxAtom_currentIndexChanged(QString s)
{
  QByteArray byteArray = s.toLatin1();
  const char* s_char = byteArray.data();
  ui_->viewer->setAtomicNumber(OpenBabel::etab.GetAtomicNum(s_char));
}

void MainWindow::on_actionStructureImportGaussianOutput_triggered()
{
  QString fileName;

  fileName = QFileDialog::getOpenFileName(this,
                                          "Choose Gaussian output file",
                                          getWorkDir(),
                                          "Gaussian output files (*.g98 *.g03);;Any file (*.*)");
  if (!fileName.isEmpty())
  {
    addToLog(QString("Import from Gaussian output file started. \nFileName: %1").arg(fileName));
    if (molecule_.importFromFile(Chemistry::FormatGaussianOutput,
                                fileName))
    {
      addToLog("Import succeeded.");
      ui_->welcomeWidget->hide();
      ui_->tabWidget->show();
      ui_->tabWidget->setCurrentIndex(3);
      ui_->viewer->setMolecule(&molecule_);
    }
    else
    {
      addToLog("failure.");
      QMessageBox::critical(this, QCoreApplication::applicationName(), "Can't import file " + fileName);
    }
  }
}

void MainWindow::on_actionStructureExportImage_triggered()
{
  ui_->viewer->saveSnapshot(false, false);
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
    ui_->doubleSpinBoxAxesSize->setEnabled(true);
    ui_->viewer->setAxes(true, ui_->doubleSpinBoxAxesSize->value());
  }
  else
  {
    ui_->doubleSpinBoxAxesSize->setEnabled(false);
    ui_->viewer->setAxes(false, ui_->doubleSpinBoxAxesSize->value());
  }
}

void MainWindow::on_checkBoxDebugInfo_clicked(bool checked)
{
  ui_->viewer->setDebugInfoVisibility(checked);
}

void MainWindow::on_actionHelpAboutQt_triggered()
{
  QApplication::aboutQt();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
  if (index == 3)
  {
    ui_->actionStructureExportImage->setEnabled(true);
  }
  else
  {
    ui_->actionStructureExportImage->setEnabled(false);
  }
}

void MainWindow::on_actionHelpAbout_triggered()
{
  AboutDialog dialog;
  dialog.setHeaderLabelText("<h1>" + QCoreApplication::applicationName() +
                            " " + QCoreApplication::applicationVersion() + "</h1>");
  dialog.exec();
}

void MainWindow::on_pushButtonRun_clicked()
{
  std::ostringstream os;

  setCursor(Qt::WaitCursor);
  ui_->pushButtonRun->setText(tr("Running..."));
  qApp->processEvents();
  ui_->pushButtonRun->setEnabled(false);
  molecule_.optimize((Chemistry::ForceField)ui_->comboBoxForceField->currentIndex(),
                       (Chemistry::Algorithm)ui_->comboBoxAlgorithm->currentIndex(),
                       powf(10, -ui_->spinBoxConvergence->value()),
                       ui_->spinBoxMaxSteps->value(),
                       ui_->spinBoxStepsPerUpdate->value(),
                       &os);
  addToLog(QString::fromStdString(os.str()));
  ui_->pushButtonRun->setEnabled(true);
  ui_->pushButtonRun->setText(tr("Run"));
  setCursor(Qt::ArrowCursor);
}

void MainWindow::on_actionStructureImportSMILES_triggered()
{
  bool ok;
  QString text;

  text = QInputDialog::getText(this,
                               tr("QDalton"),
                               tr("Input SMILES:"),
                               QLineEdit::Normal,
                               "",
                               &ok);
  if (ok)
  {
    if (molecule_.importFromString(Chemistry::FormatSmiles,
                                  text))
    {
      ui_->tabWidget->setCurrentIndex(3);
      ui_->viewer->setMolecule(&molecule_);
    }
  }
}

void MainWindow::on_actionStructureImportInChI_triggered()
{
  bool ok;
  QString text;

  text = QInputDialog::getText(this,
                               tr("QDalton"),
                               tr("Input InChI:"),
                               QLineEdit::Normal,
                               "",
                               &ok);
  if (ok)
  {
    if (molecule_.importFromString(Chemistry::FormatInchi,
                                  text))
    {
      ui_->tabWidget->setCurrentIndex(3);
      ui_->viewer->setMolecule(&molecule_);
    }
  }
}

void MainWindow::on_actionStructureConformations_triggered()
{
  ui_->viewer->conformationalSearch(ui_->tableWidgetConformers);
  ui_->actionViewConformersTable->setChecked(true);
}

void MainWindow::on_tableWidgetConformers_cellClicked(int row, int column)
{
  bool ok;

  quint16 n = ui_->tableWidgetConformers->item(row, 0)->text().toUInt(&ok);
  if (ok)
  {
    ui_->viewer->displayConformer(n - 1);
  }
}

void MainWindow::on_actionViewLog_toggled(bool checked)
{
  if (checked)
  {
    ui_->logPlainTextEdit->show();
  }
  else
  {
    ui_->logPlainTextEdit->hide();
  }
}

void MainWindow::on_actionViewToolbox_toggled(bool checked)
{
  if (checked)
  {
    ui_->toolBox->show();
  }
  else
  {
    ui_->toolBox->hide();
  }
}

void MainWindow::on_actionViewConformersTable_toggled(bool checked)
{
  if (checked)
  {
    ui_->widgetConformers->show();
  }
  else
  {
    ui_->widgetConformers->hide();
  }
}

void MainWindow::on_pushButtonOK_clicked()
{
  ui_->plainTextEditMol->setPlainText(molecule_.toString(Chemistry::FormatXyz));
}

void MainWindow::on_actionStructureExportXyz_triggered()
{
  ui_->plainTextEditMol->setPlainText(molecule_.toString(Chemistry::FormatXyz));
}
