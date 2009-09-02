#ifndef CHEMISTRY_BOND_H
#define CHEMISTRY_BOND_H

namespace Chemistry
{
  class Bond
  {
  public:
    Bond(int beginIndex, int endIndex);
    int beginIndex() const;
    int endIndex() const;
  private:
    int beginIndex_;
    int endIndex_;
  };
}
#endif // CHEMISTRY_BOND_H
