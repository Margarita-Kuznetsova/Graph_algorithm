#include "base.hpp"

namespace Base {

static int FMPass (GainContainer& GC , Partitions& Prt , HGraph const& HG);
static void applyMove (GainContainer& GC , Partitions& Prt , HGraph const& HG ,
                      unsigned MoveVertex);

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

  std::set<unsigned> VertToChange;

  while (!GC.isEmpty (Prt.getSide ()))
  {
    auto [Vertex , Gain] = GC.bestFeasibleMove (Prt.getSide ());
    VertToChange.insert (Vertex);
    Cost -= Gain;
    if (Cost < BestCost)
    {
      BestCost = Cost;
      VertToChange.clear ();
    }
    applyMove (GC , Prt , HG , Vertex);
  }

  for (auto Vertex : VertToChange)
    Prt.update (Vertex);
  return BestCost;
}

void applyMove (GainContainer& GC , Partitions& Prt , HGraph const& HG ,
               unsigned MoveVertex)
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
      if ((Prt.getSide () && !Prt.getPart ().at (Vertex)) ||
          (!Prt.getSide () && Prt.getPart ().at (Vertex)))
      {
        IsNoVInDst = false;
        ++CountInDst;
        VertDst = Vertex;
      }
      else if (Vertex != MoveVertex)
      {
        ++CountInSrc;
        VertSrc = Vertex;
      }
      if (Prt.getPart ().at (MoveVertex) == Prt.getPart ().at (Vertex) &&
          Vertex != MoveVertex)
        IsOneVInSrc = false;
    }

    int const UpdateVal = IsNoVInDst ? 1 : IsOneVInSrc ? -1 : 0;

    if (UpdateVal)
      for (auto Vertex : HG.getEdges ().at (Edge))
        GC.update (Vertex , Prt.getPart ().at (Vertex) , UpdateVal);

    if (CountInSrc == 1)
      GC.update (VertSrc , Prt.getPart ().at (VertSrc) , 1);
    if (CountInDst == 1)
      GC.update (VertDst , Prt.getPart ().at (VertDst) , -1);
  }

  GC.erase (MoveVertex , Prt.getPart ().at (MoveVertex));
  GC.updateDeleted (MoveVertex);
  Prt.update (MoveVertex);
}

GainContainer::GainContainer (HGraph const& HG , Partitions const& Prt)
{
  unsigned const VertSize = Prt.getPart ().size ();
  VertGain.resize (VertSize);

  for (unsigned i = 0; i != VertSize; ++i)
  {
    int CurrentGain = 0;
    bool CurrentPart = Prt.getPart ().at (i);

    for (unsigned EdgeId : HG.getVertices ().at (i))
    {
      bool IsAlone = true;
      bool IsEntirely = true;

      for (unsigned VertId : HG.getEdges ().at (EdgeId))
      {
        if (CurrentPart != Prt.getPart ().at (VertId))
          IsEntirely = false;
        if (CurrentPart == Prt.getPart ().at (VertId) && VertId != i)
          IsAlone = false;
      }

      if (IsAlone)
        ++CurrentGain;
      if (IsEntirely)
        --CurrentGain;
    }

    auto& SizeToUpd = getNeededSide (CurrentPart);
    SizeToUpd[CurrentGain].insert (i);
    VertGain[i] = CurrentGain;
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

bool GainContainer::isEmpty (bool Side) const
{
  auto const& SizeToUpd = getNeededSide (Side);
  return SizeToUpd.empty ();
}

GainContainer::Move GainContainer::bestFeasibleMove (bool Side)
{
  auto& SizeToUpd = getNeededSide (Side);

  auto&& [Gain , Vertices] = *SizeToUpd.rbegin ();
  unsigned Vertex = *Vertices.begin ();
  Vertices.erase (Vertex);

  if (Vertices.empty ())
    SizeToUpd.erase (Gain);

  return std::make_pair (Vertex , Gain);
}

void GainContainer::update (unsigned Vertex , bool Side , int Value)
{
  if (IsDeleted.count (Vertex))
    return;
  erase (Vertex , Side);
  VertGain[Vertex] += Value;
  auto& SizeToUpd = getNeededSide (Side);
  SizeToUpd[VertGain[Vertex]].insert (Vertex);
}

void GainContainer::erase (unsigned Vertex , bool Side)
{
  auto& SizeToUpd = getNeededSide (Side);
  int const Gain = VertGain.at (Vertex);
  SizeToUpd[Gain].erase (Vertex);
  if (SizeToUpd[Gain].empty ())
    SizeToUpd.erase (Gain);
}

void GainContainer::updateDeleted (unsigned Vertex) { IsDeleted.insert (Vertex); }

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
}
}