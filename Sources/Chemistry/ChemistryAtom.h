#ifndef CHEMISTRY_ATOM_H
#define CHEMISTRY_ATOM_H

#include <QList>

#include <Eigen/Array>

namespace Chemistry
{
  class Atom
  {
  public:
    Atom(unsigned char protons, unsigned char neutrons = 0);
    Atom(const Chemistry::Atom& atom);
    
    int protons() const;
    int neutrons() const;
    
    int relativeAtomicMass() const;
    
    const Eigen::Vector3f& centre() const;
    void setCentre(const Eigen::Vector3f& centre);
    
    float covalentRadius() const;

  private:
    unsigned char protons_;
    unsigned char neutrons_;
    Eigen::Vector3f centre_;

    /**
     * Default table of covalent Radii
     * stored as a short mar ... Milli Angstrom Radius
     * Values taken from OpenBabel.
     * @see <a href="http://openbabel.sourceforge.net">openbabel.sourceforge.net</a>
     */
    static short covalentRadii_[];
  };
}

#endif // CHEMISTRY_ATOM_H
