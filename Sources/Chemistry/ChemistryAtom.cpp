#include "ChemistryAtom.h"

Chemistry::Atom::Atom()
{
  protons_ = 0;
  neutrons_ = 0;
}

Chemistry::Atom::Atom(const Chemistry::Atom& atom)
{
  protons_ = atom.protons_;
  neutrons_ = atom.neutrons_;
  centre_ = atom.centre_;
  bondedIndexes_ = bondedIndexes_;
}

int Chemistry::Atom::protons() const
{
  return protons_;
}

void Chemistry::Atom::setProtons(int protons)
{
  protons_ = protons;
}

int Chemistry::Atom::relativeAtomicMass() const
{
  return protons_ + neutrons_;
}

void Chemistry::Atom::setRelativeAtomicMass(int mass)
{
  neutrons_ = mass - protons_;
}

const Eigen::Vector3f& Chemistry::Atom::centre() const
{
  return centre_;
}

void Chemistry::Atom::setCentre(const Eigen::Vector3f& point)
{
  centre_ = point;
}

void Chemistry::Atom::bondWith(int index)
{
  bondedIndexes_ << index;
}

bool Chemistry::Atom::isBondedWith(int index) const
{
  return bondedIndexes_.contains(index);
}

float Chemistry::Atom::covalentRadius() const
{
  return (float) covalentRadii_[protons_] / 1000.0f;
}

short Chemistry::Atom::covalentRadii_[] =
{
    0,    //   0  Xx does not bond
    230,  //   1  H
    930,  //   2  He
    680,  //   3  Li
    350,  //   4  Be
    830,  //   5  B
    680,  //   6  C
    680,  //   7  N
    680,  //   8  O
    640,  //   9  F
    1120, //  10  Ne
    970,  //  11  Na
    1100, //  12  Mg
    1350, //  13  Al
    1200, //  14  Si
    750,  //  15  P
    1020, //  16  S
    990,  //  17  Cl
    1570, //  18  Ar
    1330, //  19  K
    990,  //  20  Ca
    1440, //  21  Sc
    1470, //  22  Ti
    1330, //  23  V
    1350, //  24  Cr
    1350, //  25  Mn
    1340, //  26  Fe
    1330, //  27  Co
    1500, //  28  Ni
    1520, //  29  Cu
    1450, //  30  Zn
    1220, //  31  Ga
    1170, //  32  Ge
    1210, //  33  As
    1220, //  34  Se
    1210, //  35  Br
    1910, //  36  Kr
    1470, //  37  Rb
    1120, //  38  Sr
    1780, //  39  Y
    1560, //  40  Zr
    1480, //  41  Nb
    1470, //  42  Mo
    1350, //  43  Tc
    1400, //  44  Ru
    1450, //  45  Rh
    1500, //  46  Pd
    1590, //  47  Ag
    1690, //  48  Cd
    1630, //  49  In
    1460, //  50  Sn
    1460, //  51  Sb
    1470, //  52  Te
    1400, //  53  I
    1980, //  54  Xe
    1670, //  55  Cs
    1340, //  56  Ba
    1870, //  57  La
    1830, //  58  Ce
    1820, //  59  Pr
    1810, //  60  Nd
    1800, //  61  Pm
    1800, //  62  Sm
    1990, //  63  Eu
    1790, //  64  Gd
    1760, //  65  Tb
    1750, //  66  Dy
    1740, //  67  Ho
    1730, //  68  Er
    1720, //  69  Tm
    1940, //  70  Yb
    1720, //  71  Lu
    1570, //  72  Hf
    1430, //  73  Ta
    1370, //  74  W
    1350, //  75  Re
    1370, //  76  Os
    1320, //  77  Ir
    1500, //  78  Pt
    1500, //  79  Au
    1700, //  80  Hg
    1550, //  81  Tl
    1540, //  82  Pb
    1540, //  83  Bi
    1680, //  84  Po
    1700, //  85  At
    2400, //  86  Rn
    2000, //  87  Fr
    1900, //  88  Ra
    1880, //  89  Ac
    1790, //  90  Th
    1610, //  91  Pa
    1580, //  92  U
    1550, //  93  Np
    1530, //  94  Pu
    1510, //  95  Am
    1500, //  96  Cm
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
