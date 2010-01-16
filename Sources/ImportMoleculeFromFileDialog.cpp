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

#include <QFileDialog>

#include <openbabel/obconversion.h>

#include "MainWindow.h"
#include "ImportMoleculeFromFileDialog.h"

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
