#include <cmath>
#include <QColor>

#include "Render/RenderAtom.h"
#include "Render/RenderArrow.h"
#include "Render/RenderBond.h"
#include "Render/RenderSphere.h"

const GLfloat Render::Atom::SELECTON_RADIUS = 0.15f;

Render::Atom::Atom(const Render::Atom& renderAtom) :
    chemistryAtom_(renderAtom.chemistryAtom_)
{
  isSelected_ = renderAtom.isSelected_;
  isMovable_ = renderAtom.isMovable_;
}

Render::Atom::Atom(const Chemistry::Atom& chemistryAtom) :
    chemistryAtom_(chemistryAtom)
{
//  label_ = obatom->Get
  isSelected_ = false;
  isMovable_ = false;
}

GLfloat Render::Atom::drawRadius() const
{
  return cbrt(chemistryAtom_.relativeAtomicMass()) / 10.0f + 0.2f;
}

GLfloat Render::Atom::vanderwaalsRadius() const
{
  return (GLfloat) vanderwaalsRadii_[4 *(chemistryAtom_.protons())] / 1000.0f;
}

Render::Color Render::Atom::color() const
{
  QColor color(colors[chemistryAtom_.protons()]);
  return Render::Color(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

void Render::Atom::draw(Render::Atom::DrawStyle style, Render::Quality quality) const
{
  Material material(color(), true);
  Sphere sphere(chemistryAtom_.centre(), drawRadius(), material);

  switch (style)
  {
  case AS_ATOM:
    break;
  case AS_CONNECTOR:
    sphere.setRadius(Bond::DEFAULT_THIKNESS);
    break;
  case AS_VDW:
    sphere.setRadius(vanderwaalsRadius());
    break;
  }

  sphere.draw(STYLE_FILL, quality);

  if (isMovable_)
  {
    GLfloat size = 1.0f;
    glTranslatef(chemistryAtom_.centre().x(), chemistryAtom_.centre().y(), chemistryAtom_.centre().z());
    Arrow x(Eigen::Vector3f(0.0f, 0.0f, 0.0f),
            Eigen::Vector3f(size, 0.0f, 0.0f),
            0.04f,
            Material::red());
    x.draw(STYLE_FILL, QUALITY_LOW);

    Arrow y(Eigen::Vector3f(0.0f, 0.0f, 0.0f),
            Eigen::Vector3f(0.0f, size, 0.0f),
            0.05f,
            Material::green());
    y.draw(Render::STYLE_FILL, QUALITY_LOW);

    Arrow z(Eigen::Vector3f(0.0f, 0.0f, 0.0f),
            Eigen::Vector3f(0.0f, 0.0f, size),
            0.05f,
            Material::blue());
    z.draw(STYLE_FILL, QUALITY_LOW);
  }
}

void Render::Atom::drawSelection(Atom::DrawStyle style, Quality quality) const
{
  Material material(Color::selection(), true);
  Sphere sphere(chemistryAtom_.centre(),
                drawRadius() + SELECTON_RADIUS,
                material);
  if (style == AS_CONNECTOR)
    sphere.setRadius(Bond::DEFAULT_THIKNESS + SELECTON_RADIUS);
  if (style == AS_VDW)
    sphere.setRadius(vanderwaalsRadius() + SELECTON_RADIUS * 2);
  // Enable blending
  glEnable(GL_BLEND);
  //  glDisable(GL_DEPTH_TEST);
  sphere.draw(STYLE_FILL, quality);
  glDisable(GL_BLEND);
  //  glEnable(GL_DEPTH_TEST);
}

const Eigen::Vector3f& Render::Atom::centre() const
{
  return chemistryAtom_.centre();
}

void Render::Atom::setCentre(const Eigen::Vector3f& point)
{
  chemistryAtom_.setCentre(point);
}

bool Render::Atom::isSelected() const
{
  return isSelected_;
}

void Render::Atom::setSelected(bool selected)
{
  if (selected == true)
    isSelected_ = true;
  else
    isSelected_ = false;
}

bool Render::Atom::isMovable() const
{
  return isMovable_;
}

void Render::Atom::setMovable(bool movable)
{
  if (movable == true)
    isMovable_ = true;
  else
    isMovable_ = false;
}

const QString& Render::Atom::label() const
{
//  return label_;
  return "Atom";
}

const short Render::Atom::vanderwaalsRadii_[] =
{
  //Jmol,openBabel,openRasmol,reserved
    1000,1000,1000,0, // XX 0
    1200,1200,1100,0, // H 1
    1400,1400,2200,0, // He 2
    1820,2200,1220,0, // Li 3
    1700,1900, 628,0, // Be 4
    2080,1800,1548,0, // B 5
    1950,1700,1548,0, // C 6
    1850,1600,1400,0, // N 7
    1700,1550,1348,0, // O 8
    1730,1500,1300,0, // F 9
    1540,1540,2020,0, // Ne 10
    2270,2400,2200,0, // Na 11
    1730,2200,1500,0, // Mg 12
    2050,2100,1500,0, // Al 13
    2100,2100,2200,0, // Si 14
    2080,1950,1880,0, // P 15
    2000,1800,1808,0, // S 16
    1970,1800,1748,0, // Cl 17
    1880,1880,2768,0, // Ar 18
    2750,2800,2388,0, // K 19
    1973,2400,1948,0, // Ca 20
    1700,2300,1320,0, // Sc 21
    1700,2150,1948,0, // Ti 22
    1700,2050,1060,0, // V 23
    1700,2050,1128,0, // Cr 24
    1700,2050,1188,0, // Mn 25
    1700,2050,1948,0, // Fe 26
    1700,2000,1128,0, // Co 27
    1630,2000,1240,0, // Ni 28
    1400,2000,1148,0, // Cu 29
    1390,2100,1148,0, // Zn 30
    1870,2100,1548,0, // Ga 31
    1700,2100,3996,0, // Ge 32
    1850,2050, 828,0, // As 33
    1900,1900, 900,0, // Se 34
    2100,1900,1748,0, // Br 35
    2020,2020,1900,0, // Kr 36
    1700,2900,2648,0, // Rb 37
    1700,2550,2020,0, // Sr 38
    1700,2400,1608,0, // Y 39
    1700,2300,1420,0, // Zr 40
    1700,2150,1328,0, // Nb 41
    1700,2100,1748,0, // Mo 42
    1700,2050,1800,0, // Tc 43
    1700,2050,1200,0, // Ru 44
    1700,2000,1220,0, // Rh 45
    1630,2050,1440,0, // Pd 46
    1720,2100,1548,0, // Ag 47
    1580,2200,1748,0, // Cd 48
    1930,2200,1448,0, // In 49
    2170,2250,1668,0, // Sn 50
    2200,2200,1120,0, // Sb 51
    2060,2100,1260,0, // Te 52
    2150,2100,1748,0, // I 53
    2160,2160,2100,0, // Xe 54
    1700,3000,3008,0, // Cs 55
    1700,2700,2408,0, // Ba 56
    1700,2500,1828,0, // La 57
    1700,2480,1860,0, // Ce 58
    1700,2470,1620,0, // Pr 59
    1700,2450,1788,0, // Nd 60
    1700,2430,1760,0, // Pm 61
    1700,2420,1740,0, // Sm 62
    1700,2400,1960,0, // Eu 63
    1700,2380,1688,0, // Gd 64
    1700,2370,1660,0, // Tb 65
    1700,2350,1628,0, // Dy 66
    1700,2330,1608,0, // Ho 67
    1700,2320,1588,0, // Er 68
    1700,2300,1568,0, // Tm 69
    1700,2280,1540,0, // Yb 70
    1700,2270,1528,0, // Lu 71
    1700,2250,1400,0, // Hf 72
    1700,2200,1220,0, // Ta 73
    1700,2100,1260,0, // W 74
    1700,2050,1300,0, // Re 75
    1700,2000,1580,0, // Os 76
    1700,2000,1220,0, // Ir 77
    1720,2050,1548,0, // Pt 78
    1660,2100,1448,0, // Au 79
    1550,2050,1980,0, // Hg 80
    1960,2200,1708,0, // Tl 81
    2020,2300,2160,0, // Pb 82
    1700,2300,1728,0, // Bi 83
    1700,2000,1208,0, // Po 84
    1700,2000,1120,0, // At 85
    1700,2000,2300,0, // Rn 86
    1700,2000,3240,0, // Fr 87
    1700,2000,2568,0, // Ra 88
    1700,2000,2120,0, // Ac 89
    1700,2400,1840,0, // Th 90
    1700,2000,1600,0, // Pa 91
    1860,2300,1748,0, // U 92
    1700,2000,1708,0, // Np 93
    1700,2000,1668,0, // Pu 94
    1700,2000,1660,0, // Am 95
    1700,2000,1648,0, // Cm 96
    1700,2000,1640,0, // Bk 97
    1700,2000,1628,0, // Cf 98
    1700,2000,1620,0, // Es 99
    1700,2000,1608,0, // Fm 100
    1700,2000,1600,0, // Md 101
    1700,2000,1588,0, // No 102
    1700,2000,1580,0, // Lr 103
    1700,2000,1600,0, // Rf 104
    1700,2000,1600,0, // Db 105
    1700,2000,1600,0, // Sg 106
    1700,2000,1600,0, // Bh 107
    1700,2000,1600,0, // Hs 108
    1700,2000,1600,0, // Mt 109
  };

  /**
   * Default table of CPK atom colors.
   * ghemical colors with a few proposed modifications
   */
  const int Render::Atom::colors[] =
  {
    0xFFFF1493, // Xx 0
    0xFFFFFFFF, // H  1
    0xFFD9FFFF, // He 2
    0xFFCC80FF, // Li 3
    0xFFC2FF00, // Be 4
    0xFFFFB5B5, // B  5
    0xFF909090, // C  6 - changed from ghemical
    0xFF3050F8, // N  7 - changed from ghemical
    0xFFFF0D0D, // O  8
    0xFF90E050, // F  9 - changed from ghemical
    0xFFB3E3F5, // Ne 10
    0xFFAB5CF2, // Na 11
    0xFF8AFF00, // Mg 12
    0xFFBFA6A6, // Al 13
    0xFFF0C8A0, // Si 14 - changed from ghemical
    0xFFFF8000, // P  15
    0xFFFFFF30, // S  16
    0xFF1FF01F, // Cl 17
    0xFF80D1E3, // Ar 18
    0xFF8F40D4, // K  19
    0xFF3DFF00, // Ca 20
    0xFFE6E6E6, // Sc 21
    0xFFBFC2C7, // Ti 22
    0xFFA6A6AB, // V  23
    0xFF8A99C7, // Cr 24
    0xFF9C7AC7, // Mn 25
    0xFFE06633, // Fe 26 - changed from ghemical
    0xFFF090A0, // Co 27 - changed from ghemical
    0xFF50D050, // Ni 28 - changed from ghemical
    0xFFC88033, // Cu 29 - changed from ghemical
    0xFF7D80B0, // Zn 30
    0xFFC28F8F, // Ga 31
    0xFF668F8F, // Ge 32
    0xFFBD80E3, // As 33
    0xFFFFA100, // Se 34
    0xFFA62929, // Br 35
    0xFF5CB8D1, // Kr 36
    0xFF702EB0, // Rb 37
    0xFF00FF00, // Sr 38
    0xFF94FFFF, // Y  39
    0xFF94E0E0, // Zr 40
    0xFF73C2C9, // Nb 41
    0xFF54B5B5, // Mo 42
    0xFF3B9E9E, // Tc 43
    0xFF248F8F, // Ru 44
    0xFF0A7D8C, // Rh 45
    0xFF006985, // Pd 46
    0xFFC0C0C0, // Ag 47 - changed from ghemical
    0xFFFFD98F, // Cd 48
    0xFFA67573, // In 49
    0xFF668080, // Sn 50
    0xFF9E63B5, // Sb 51
    0xFFD47A00, // Te 52
    0xFF940094, // I  53
    0xFF429EB0, // Xe 54
    0xFF57178F, // Cs 55
    0xFF00C900, // Ba 56
    0xFF70D4FF, // La 57
    0xFFFFFFC7, // Ce 58
    0xFFD9FFC7, // Pr 59
    0xFFC7FFC7, // Nd 60
    0xFFA3FFC7, // Pm 61
    0xFF8FFFC7, // Sm 62
    0xFF61FFC7, // Eu 63
    0xFF45FFC7, // Gd 64
    0xFF30FFC7, // Tb 65
    0xFF1FFFC7, // Dy 66
    0xFF00FF9C, // Ho 67
    0xFF00E675, // Er 68
    0xFF00D452, // Tm 69
    0xFF00BF38, // Yb 70
    0xFF00AB24, // Lu 71
    0xFF4DC2FF, // Hf 72
    0xFF4DA6FF, // Ta 73
    0xFF2194D6, // W  74
    0xFF267DAB, // Re 75
    0xFF266696, // Os 76
    0xFF175487, // Ir 77
    0xFFD0D0E0, // Pt 78 - changed from ghemical
    0xFFFFD123, // Au 79 - changed from ghemical
    0xFFB8B8D0, // Hg 80 - changed from ghemical
    0xFFA6544D, // Tl 81
    0xFF575961, // Pb 82
    0xFF9E4FB5, // Bi 83
    0xFFAB5C00, // Po 84
    0xFF754F45, // At 85
    0xFF428296, // Rn 86
    0xFF420066, // Fr 87
    0xFF007D00, // Ra 88
    0xFF70ABFA, // Ac 89
    0xFF00BAFF, // Th 90
    0xFF00A1FF, // Pa 91
    0xFF008FFF, // U  92
    0xFF0080FF, // Np 93
    0xFF006BFF, // Pu 94
    0xFF545CF2, // Am 95
    0xFF785CE3, // Cm 96
    0xFF8A4FE3, // Bk 97
    0xFFA136D4, // Cf 98
    0xFFB31FD4, // Es 99
    0xFFB31FBA, // Fm 100
    0xFFB30DA6, // Md 101
    0xFFBD0D87, // No 102
    0xFFC70066, // Lr 103
    0xFFCC0059, // Rf 104
    0xFFD1004F, // Db 105
    0xFFD90045, // Sg 106
    0xFFE00038, // Bh 107
    0xFFE6002E, // Hs 108
    0xFFEB0026, // Mt 109
};
