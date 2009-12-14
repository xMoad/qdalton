#include <QFileDialog>

#include <openbabel/obconversion.h>

#include "MainWindow.h"
#include "ImportStructureFromFileDialog.h"

bool obFormatCompare(OpenBabel::OBFormat* first,
                   OpenBabel::OBFormat* second)
{
  return QString(first->Description()) < QString(second->Description());
}

ImportStructureFromFileDialog::ImportStructureFromFileDialog(
    MainWindow* mainWindow) :
    QDialog(mainWindow),
    mainWindow_(mainWindow),
    fileName_(""),
    currentObFormatIndex_(0)
{
  ui_.setupUi(this);

  fillComboBoxFileType();

  connect(ui_.comboBoxFileType, SIGNAL(currentIndexChanged(int)),
          this, SLOT(setCurrentObFormatIndex(int)));
  connect(ui_.pushButtonChoose, SIGNAL(clicked()),
          this, SLOT(showOpenFileDialog()));
  connect(ui_.buttonBoxOkCancel->button(QDialogButtonBox::Ok),
          SIGNAL(clicked()),
          this, SLOT(accepted()));
}

// Slots.

void ImportStructureFromFileDialog::accepted()
{
  if (QFile::exists(fileName_))
  {
    mainWindow_->importMoleculeFromFile(fileName_,
                                        obFormatList_[currentObFormatIndex_]);
  }
}

void ImportStructureFromFileDialog::setCurrentObFormatIndex(int index)
{
  currentObFormatIndex_ = index;
}

void ImportStructureFromFileDialog::showOpenFileDialog()
{
  OpenBabel::OBFormat* obFormat;
  int obFormatIndex;
  QString fileName = QFileDialog::getOpenFileName(
      this,
      tr("Choose file"),
      mainWindow_->getWorkDir());

  if (fileName != 0)
  {
    fileName_ = fileName;
    ui_.lineEditFileName->setText(fileName);
    obFormat = OpenBabel::OBConversion::FormatFromExt(
        fileName.section('.', -1, -1).toAscii().data());
    obFormatIndex = obFormatList_.indexOf(obFormat);
    if (obFormatIndex != -1)
    {
      ui_.comboBoxFileType->setCurrentIndex(obFormatIndex);
    }
  }
}

void ImportStructureFromFileDialog::fillComboBoxFileType()
{
  // Don't delete obConversion!
  OpenBabel::OBConversion obConversion;
  OpenBabel::OBFormat* obFormat;
  OpenBabel::Formatpos formapos;
  const char* str = NULL;

  while (OpenBabel::OBConversion::GetNextFormat(formapos, str, obFormat))
  {
    if (!obFormat || (obFormat->Flags() & NOTREADABLE))
    {
      continue;
    }

    if (!obFormatList_.contains(obFormat))
    {
      obFormatList_.append(obFormat);
    }
  }

  qSort(obFormatList_.begin(), obFormatList_.end(), obFormatCompare);

  QListIterator<OpenBabel::OBFormat*> i(obFormatList_);
  while (i.hasNext())
  {
    QString description(i.next()->Description());
    // There can be multiple lines in the description -- we only want one
    int lineEnding = description.indexOf('\n');
    if (lineEnding != -1)
      description.truncate(lineEnding);
    // remove any remaining initial or ending whitespace
    description = description.trimmed();
    ui_.comboBoxFileType->addItem(description);
  }
}
