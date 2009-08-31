#ifndef CHEMISTRY_ATOM_H
#define CHEMISTRY_ATOM_H

#include <QList>

#include <Eigen/Array>

namespace Chemistry
{
  class Atom
  {
  public:
    Atom();
    Atom(const Chemistry::Atom& atom);
    
    int protons() const;
    void setProtons(int protons);
    
    int relativeAtomicMass() const;
    void setRelativeAtomicMass(int mass);
    
    const Eigen::Vector3f& centre() const;
    void setCentre(const Eigen::Vector3f& centre);

    void bondWith(int index);
    bool isBondedWith(int index) const;
    
    float covalentRadius() const;

  private:
    int protons_;
    int neutrons_;
    Eigen::Vector3f centre_;
    QList<int> bondedIndexes_;

    /**
     * Default table of covalent Radii
     * stored as a short mar ... Milli Angstrom Radius
     * Values taken from OpenBabel.
     * @see <a href="http://openbabel.sourceforge.net">openbabel.sourceforge.net</a>
     */
    static short covalentRadii_[];
  };
} // namespace Chemistry

#endif // CHEMISTRY_ATOM_H
