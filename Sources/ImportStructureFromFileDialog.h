#ifndef IMPORT_STRUCTURE_FROM_FILEDIALOG_H
#define IMPORT_STRUCTURE_FROM_FILEDIALOG_H

#include "ui_ImportStructureFromFileDialog.h"

namespace OpenBabel
{
  class OBFormat;
}

class MainWindow;

class ImportStructureFromFileDialog : public QDialog
{
  Q_OBJECT

public:
  ImportStructureFromFileDialog(MainWindow* mainWindow);

private slots:
  void accepted();
  void fillComboBoxFileType();
  void setCurrentObFormatIndex(int index);
  void showOpenFileDialog();


private:
  Ui::ImportStructureFromFileDialog ui_;
  MainWindow* mainWindow_;
  QString fileName_;
  QList<OpenBabel::OBFormat*> obFormatList_;
  int currentObFormatIndex_;
};

#endif // IMPORT_STRUCTURE_FROM_FILEDIALOG_H
