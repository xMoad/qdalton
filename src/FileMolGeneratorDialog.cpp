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

#include "FileMolGeneratorDialog.h"


FileMolGeneratorDialog::FileMolGeneratorDialog(File::Mol* fileMol,
                                               QWidget* parent) :
    QDialog(parent),
    ui_(),
    fileMol_(fileMol)
{
  ui_.setupUi(this);

  QFont font;

#ifdef Q_WS_MAC
  font.setPointSize(12);
  font.setFamily("Monaco");
#else
#ifdef Q_WS_X11
  font.setPointSize(9);
  font.setFamily("Monospace");
#else
  font.setPointSize(10);
  font.setFamily("Courier");
#endif
#endif

  ui_.textEdit->setFont(font);

  if (fileMol_->molecule().charge() != 0)
    ui_.spinBoxCharge->setValue(fileMol_->molecule().charge());
  if (fileMol_->unitOfLength() == UnitOfLengthAngstrom)
    ui_.comboBoxUnitOfLength->setCurrentIndex(1);
  ui_.textEdit->setText(fileMol_->text());

  connect(ui_.spinBoxCharge, SIGNAL(valueChanged(int)),
          this, SLOT(chargeChanged(int)));
  connect(ui_.comboBoxUnitOfLength, SIGNAL(currentIndexChanged(int)),
          this, SLOT(unitOfLengthChanged(int)));
  connect(ui_.comboBoxGaussiansType, SIGNAL(currentIndexChanged(int)),
          this, SLOT(gaussiansTypeChanged(int)));
}

void FileMolGeneratorDialog::chargeChanged(int charge)
{
  fileMol_->molecule().setCharge(charge);
  fileMol_->generate();
  ui_.textEdit->setText(fileMol_->text());
}

void FileMolGeneratorDialog::unitOfLengthChanged(int index)
{
  fileMol_->setUnitOfLength((UnitOfLength) index);
  fileMol_->generate();
  ui_.textEdit->setText(fileMol_->text());
}

void FileMolGeneratorDialog::gaussiansTypeChanged(int index)
{
  fileMol_->setGaussiansType((GaussiansType)index);
  fileMol_->generate();
  ui_.textEdit->setText(fileMol_->text());
}
