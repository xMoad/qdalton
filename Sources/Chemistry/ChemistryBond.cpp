#include "ChemistryBond.h"

Chemistry::Bond::Bond(int beginIndex, int endIndex)
{
  beginIndex_ = beginIndex;
  endIndex_ = endIndex;
}

int Chemistry::Bond::beginIndex() const
{
  return beginIndex_;
}

int Chemistry::Bond::endIndex() const
{
  return endIndex_;
}
