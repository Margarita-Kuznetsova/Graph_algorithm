#pragma once 

#include "common.hpp"

namespace Base {

unsigned Alg(HGraph const& HG , Partitions& Prt);

class GainContainer
{
  using RLSide = std::map<int , std::set<int>>;
  using Move = std::pair<unsigned , int>;

  RLSide Left;
  RLSide Right;

  std::vector<int> VertGain;
  std::set<unsigned> IsDeleted;

  RLSide& getNeededSide (bool Side);
  RLSide const& getNeededSide (bool Side) const;

  public:
  GainContainer (HGraph const& HG , Partitions const& Prt);
  bool isEmpty (bool Check) const;
  Move bestFeasibleMove (bool Side);
  void update (unsigned Vertex , bool Side , int Value);
  void erase (unsigned Vertex , bool Side);
  void updateDeleted (unsigned Vertex);

  void dump (std::ostream& Out = std::cout) const;
};
}

