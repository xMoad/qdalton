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

#ifndef IMPORT_MOLECULE_FROM_FILE_DIALOG_H
#define IMPORT_MOLECULE_FROM_FILE_DIALOG_H

#include "ui_ImportMoleculeFromFileDialog.h"

namespace OpenBabel
{
  class OBFormat;
}

class MainWindow;

class ImportMoleculeFromFileDialog : public QDialog
{
  Q_OBJECT

public:
  ImportMoleculeFromFileDialog(MainWindow* mainWindow);

private slots:
  void accepted();
  void fillComboBoxFileType();
  void setCurrentObFormatIndex(int index);
  void showOpenFileDialog();


private:
  Ui::ImportMoleculeFromFileDialog ui_;
  MainWindow* mainWindow_;
  QString fileName_;
  QList<OpenBabel::OBFormat*> obFormatList_;
  int currentObFormatIndex_;
};

#endif // IMPORT_MOLECULE_FROM_FILE_DIALOG_H
