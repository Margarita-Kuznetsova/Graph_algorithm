#include "mod.hpp"

namespace Mod
{

    static int FMPass (GainContainer& GC , Partitions& Prt , HGraph const& HG);
  static void applyMove (GainContainer& GC , Partitions& Prt , HGraph const& HG ,
                        unsigned MoveVertex , unsigned it);

  unsigned Alg (HGraph const& HG , Partitions& Prt)
  {
    unsigned Iteration = 0;
    while (1)
    {
      ++Iteration;
      GainContainer GC { HG, Prt };
      int const Best = FMPass (GC , Prt , HG);
      if (Best == Prt.getCost ())
        break;

      Prt.setCost (Best);
    }
    return Iteration;
  }

  int FMPass (GainContainer& GC , Partitions& Prt , HGraph const& HG)
  {
    int Cost = Prt.getCost ();
    int BestCost = Cost;

    std::vector<unsigned> VertToChange;

    unsigned it = 0;
    while (!GC.isEmpty ())
    {
      auto [Vertex , Gain] = GC.bestFeasibleMove (Prt.getSide ());
      VertToChange.push_back (Vertex);
      Cost -= Gain;
      if (Cost < BestCost)
      {
        BestCost = Cost;
        VertToChange.clear ();
      }
      applyMove (GC , Prt , HG , Vertex , it);
    }

    for (auto Vertex : VertToChange)
      Prt.update (Vertex);

    return BestCost;
  }

  void applyMove (GainContainer& GC , Partitions& Prt , HGraph const& HG ,
                 unsigned MoveVertex , unsigned it)
  {
    for (auto Edge : HG.getVertices ().at (MoveVertex))
    {
      bool IsNoVInDst = true;
      bool IsOneVInSrc = true;

      int CountInDst = 0;
      unsigned VertDst = 0;

      int CountInSrc = 0;
      unsigned VertSrc = 0;

      for (auto Vertex : HG.getEdges ().at (Edge))
      {
        if (Prt.getPart ().at (MoveVertex))
        {
          if (!Prt.getPart ().at (Vertex))
          {
            IsNoVInDst = false;
            CountInDst++;
            VertDst = Vertex;
          }
          else if (Vertex != MoveVertex)
          {
            CountInSrc++;
            VertSrc = Vertex;
          }
        }
        else
        {
          if (Prt.getPart ().at (Vertex))
          {
            IsNoVInDst = false;
            CountInDst++;
            VertDst = Vertex;
          }
          else if (Vertex != MoveVertex)
          {
            CountInSrc++;
            VertSrc = Vertex;
          }
        }

        if (Prt.getPart ().at (MoveVertex) == Prt.getPart ().at (Vertex) && Vertex != MoveVertex)
          IsOneVInSrc = false;
      }

      int const UpdateVal = IsNoVInDst ? 1 : IsOneVInSrc ? -1 : 0;

      if (UpdateVal)
        for (auto Vertex : HG.getEdges ().at (Edge))
          GC.getDeltas ().at (Vertex) += UpdateVal;

      if (CountInSrc == 1)
        ++GC.getDeltas ().at (VertSrc);
      if (CountInDst == 1)
        --GC.getDeltas ().at (VertDst);
    }

    GC.erase (MoveVertex , Prt.getPart ().at (MoveVertex));
    GC.updateDeleted (MoveVertex);

    for (unsigned Edge : HG.getVertices ().at (MoveVertex))
      for (unsigned Vertex : HG.getEdges ().at (Edge))
        if (GC.getDeltas ().at (Vertex))
        {
          GC.update (Vertex , Prt.getPart ().at (Vertex) , GC.getDeltas ().at (Vertex));
          GC.getDeltas ().at (Vertex) = 0;
        }

    Prt.update (MoveVertex);
  }

  GainContainer::GainContainer (HGraph const& HG , Partitions const& Prt)
  {
    unsigned const VertSize = Prt.getPart ().size ();
    VertGain.resize (VertSize);
    IsDeleted.resize (VertSize);
    Deltas.resize (VertSize);
    Iterators.resize (VertSize);

    unsigned SkipFirst = 0;
    for (auto&& VecEdges : HG.getEdges ())
    {
      if (!SkipFirst++)
        continue;
      int VLeft = 0 , VRight = 0;
      unsigned VLeftId = 0 , VRightId = 0;
      for (unsigned Vertex : VecEdges)
        if (!Prt.getPart ().at (Vertex))
        {
          VLeft++;
          VLeftId = Vertex;
        }
        else
        {
          VRight++;
          VRightId = Vertex;
        }

      if ((VRight == 0 && VLeft != 1) || (VLeft == 0 && VRight != 1))
        for (unsigned Vertex : VecEdges)
          --VertGain[Vertex];

      if (VLeft == 1 && VRight != 0)
        ++VertGain[VLeftId];
      if (VRight == 1 && VLeft != 0)
        ++VertGain[VRightId];
    }

    for (unsigned i = 1; i != Prt.getPart ().size (); ++i)
    {
      auto&& Side = getNeededSide (Prt.getPart ().at (i));
      auto& Item = Side[VertGain[i]];
      Item.push_front (i);
      Iterators[i] = Item.begin ();
    }
  }

  GainContainer::RLSide& GainContainer::getNeededSide (bool Side)
  {
    return Side ? Right : Left;
  }
  GainContainer::RLSide const& GainContainer::getNeededSide (bool Side) const
  {
    return Side ? Right : Left;
  }

  bool GainContainer::isEmpty () const
  {
    return Left.empty () || Right.empty ();
  }

  GainContainer::Move GainContainer::bestFeasibleMove (int Displacement)
  {

    const int GainR = Right.rbegin ()->first;
    const int GainL = Left.rbegin ()->first;
    const bool IsSideR = GainL < GainR || Displacement >= 0;

    auto& SizeToUpd = getNeededSide (IsSideR);

    auto&& [Gain , Vertices] = *SizeToUpd.rbegin ();
    unsigned Vertex = *Vertices.begin ();
    Vertices.pop_front ();
    IsDeleted[Vertex] = true;
    if (Vertices.empty ())
      SizeToUpd.erase (Gain);

    return std::make_pair (Vertex , Gain);
  }

  void GainContainer::update (unsigned Vertex , bool Side , int Value)
  {
    if (IsDeleted[Vertex])
      return;
    erase (Vertex , Side);
    VertGain[Vertex] += Value;
    auto& SizeToUpd = getNeededSide (Side);
    SizeToUpd[VertGain[Vertex]].push_front (Vertex);
    Iterators[Vertex] = SizeToUpd[VertGain[Vertex]].begin ();
  }

  void GainContainer::erase (unsigned Vertex , bool Side)
  {
    if (IsDeleted[Vertex])
      return;
    auto& SizeToUpd = getNeededSide (Side);
    int const Gain = VertGain.at (Vertex);
    SizeToUpd[Gain].erase (Iterators[Vertex]);
    if (SizeToUpd[Gain].empty ())
      SizeToUpd.erase (Gain);
  }

  void GainContainer::updateDeleted (unsigned Vertex) { IsDeleted[Vertex] = true; }

  std::vector<int> const& GainContainer::getDeltas () const { return Deltas; }
  std::vector<int>& GainContainer::getDeltas () { return Deltas; }

  void GainContainer::dump (std::ostream& Out) const
  {
    Out << "Left\n";
    for (auto&& Item : Left)
    {
      Out << Item.first << ": ";
      for (auto Vert : Item.second)
        Out << Vert << ' ';
      Out << '\n';
    }
    Out << "Right\n";
    for (auto&& Item : Right)
    {
      Out << Item.first << ": ";
      for (auto Vert : Item.second)
        Out << Vert << ' ';
      Out << '\n';
    }
    Out << "Deltas:\n";
    for (auto x : Deltas)
      Out << x << ' ';
    Out << "\n";
  }
}