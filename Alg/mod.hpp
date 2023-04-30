#pragma once 

#include "common.hpp"

namespace Mod
{
  unsigned Alg(HGraph const& , Partitions&);

  class GainContainer
  {

    using RLSide = std::map<int , std::list<int>>;
    using Move = std::pair<unsigned , int>;

    RLSide Left;
    RLSide Right;

    std::vector<int> VertGain;
    std::vector<bool> IsDeleted;
    std::vector<int> Deltas;
    std::vector<std::list<int>::iterator> Iterators;

    RLSide& getNeededSide (bool Side);
    RLSide const& getNeededSide (bool Side) const;

    public:
    GainContainer (HGraph const& HG , Partitions const& Prt);
    bool isEmpty () const;
    Move bestFeasibleMove (int Displacement);
    void update (unsigned Vertex , bool Side , int Value);
    void erase (unsigned Vertex , bool Side);
    void updateDeleted (unsigned Vertex);
    std::vector<int>& getDeltas ();
    std::vector<int> const& getDeltas () const;

    void dump (std::ostream& Out = std::cout) const;
  };
}