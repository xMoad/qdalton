#ifndef FILEMOLGENERATORDIALOG_H
#define FILEMOLGENERATORDIALOG_H

#include <QDialog>

namespace Ui
{
  class FileMolGeneratorDialog;
}

class FileMolGeneratorDialog : public QDialog
{
  Q_OBJECT

public:
  FileMolGeneratorDialog(QWidget* parent = 0);
  ~FileMolGeneratorDialog();

  void setText(const QString& text);

private:
  Ui::FileMolGeneratorDialog* ui_;
};

#endif // FILEMOLGENERATORDIALOG_H
