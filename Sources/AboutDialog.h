#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui/QDialog>

namespace Ui
{
  class AboutDialog;
}

class AboutDialog : public QDialog
{
  Q_OBJECT
public:
  AboutDialog(QWidget *parent = 0);
  ~AboutDialog();
  void setHeaderLabelText(const QString& text);

private:
  Ui::AboutDialog* ui;
};

#endif // ABOUTDIALOG_H
