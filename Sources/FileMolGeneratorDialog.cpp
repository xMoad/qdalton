#include "FileMolGeneratorDialog.h"
#include "ui_FileMolGeneratorDialog.h"

FileMolGeneratorDialog::FileMolGeneratorDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::FileMolGeneratorDialog)
{
    ui_->setupUi(this);
}

FileMolGeneratorDialog::~FileMolGeneratorDialog()
{
    delete ui_;
}

void FileMolGeneratorDialog::setText(const QString& text)
{
  ui_->textEdit->setText(text);
}
