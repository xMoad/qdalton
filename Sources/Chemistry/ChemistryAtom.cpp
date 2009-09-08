/**********************************************************************
  Copyright (C) 2008, 2009 Anton Simakov

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

#include "Chemistry/ChemistryAtom.h"
#include "Chemistry/ChemistryMolecule.h"

Chemistry::Atom::Atom(quint8 protons,
                      quint8 neutrons)
{
  protons_ = protons;
  // TODO If neutrons == 0 then take value from default isotope table! Not 1!
  neutrons_ = (neutrons == 0) ? 1 : neutrons;
}

Chemistry::Atom::Atom(const Chemistry::Atom& atom):
    protons_(atom.protons_),
    neutrons_(atom.neutrons_),
    centre_(atom.centre_)
{
}

void Chemistry::Atom::setParentMolecule(Chemistry::Molecule* molecule)
{
  molecule_ = molecule;
}

quint8 Chemistry::Atom::protons() const
{
  return protons_;
}

quint8 Chemistry::Atom::neutrons() const
{
  return neutrons_;
}

quint16 Chemistry::Atom::relativeAtomicMass() const
{
  return protons_ + neutrons_;
}

const Eigen::Vector3f& Chemistry::Atom::centre() const
{
  return centre_;
}

void Chemistry::Atom::setCentre(const Eigen::Vector3f& point)
{
  centre_ = point;
}

float Chemistry::Atom::covalentRadius() const
{
  return (float) covalentRadii_[protons_] / 1000.0f;
}

const quint16 Chemistry::Atom::covalentRadii_[] =
{
    0,    //   0  Xx does not bond
    310, //   1  H
    280, //   2  He
    1280, //   3  Li
    960, //   4  Be
    840, //   5  B
    730, //   6  C
    710, //   7  N
    660, //   8  O
    570, //   9  F
    580, //  10  Ne
    1660, //  11  Na
    1410, //  12  Mg
    1210, //  13  Al
    1110, //  14  Si
    1070, //  15  P
    1050, //  16  S
    1020, //  17  Cl
    1060, //  18  Ar
    2029, //  19  K
    1760, //  20  Ca
    1700, //  21  Sc
    1600, //  22  Ti
    1530, //  23  V
    1390, //  24  Cr
    1500, //  25  Mn
    1400, //  26  Fe
    1380, //  27  Co
    1240, //  28  Ni
    1320, //  29  Cu
    1220, //  30  Zn
    1220, //  31  Ga
    1200, //  32  Ge
    1190, //  33  As
    1200, //  34  Se
    1200, //  35  Br
    1160, //  36  Kr
    2200, //  37  Rb
    1950, //  38  Sr
    1900, //  39  Y
    1750, //  40  Zr
    1640, //  41  Nb
    1540, //  42  Mo
    1470, //  43  Tc
    1460, //  44  Ru
    1420, //  45  Rh
    1390, //  46  Pd
    1450, //  47  Ag
    1440, //  48  Cd
    1420, //  49  In
    1390, //  50  Sn
    1390, //  51  Sb
    1380, //  52  Te
    1390, //  53  I
    1400, //  54  Xe
    2440, //  55  Cs
    2150, //  56  Ba
    2070, //  57  La
    2040, //  58  Ce
    2029, //  59  Pr
    2009, //  60  Nd
    1990, //  61  Pm
    1980, //  62  Sm
    1980, //  63  Eu
    1960, //  64  Gd
    1940, //  65  Tb
    1920, //  66  Dy
    1920, //  67  Ho
    1890, //  68  Er
    1900, //  69  Tm
    1870, //  70  Yb
    1870, //  71  Lu
    1750, //  72  Hf
    1700, //  73  Ta
    1620, //  74  W
    1510, //  75  Re
    1440, //  76  Os
    1410, //  77  Ir
    1360, //  78  Pt
    1360, //  79  Au
    1320, //  80  Hg
    1450, //  81  Tl
    1460, //  82  Pb
    1480, //  83  Bi
    1400, //  84  Po
    1500, //  85  At
    1500, //  86  Rn
    2600, //  87  Fr
    2210, //  88  Ra
    2150, //  89  Ac
    2060, //  90  Th
    2000, //  91  Pa
    1960, //  92  U
    1900, //  93  Np
    1870, //  94  Pu
    1800, //  95  Am
    1690, //  96  Cm last data from article!
    1500, //  97  Bk
    1500, //  98  Cf
    1500, //  99  Es
    1500, // 100  Fm
    1500, // 101  Md
    1500, // 102  No
    1500, // 103  Lr
    1600, // 104  Rf
    1600, // 105  Db
    1600, // 106  Sg
    1600, // 107  Bh
    1600, // 108  Hs
    1600, // 109  Mt
  };
