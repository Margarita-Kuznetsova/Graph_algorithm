#include "common.hpp"

HGraph::HGraph (std::ifstream& FIn)
{
  assert (FIn.is_open ());

  unsigned EdgesNo = 0 , VerticesNo = 0;
  FIn >> EdgesNo >> VerticesNo;

  Vertices.resize (VerticesNo + 1);
  Edges.resize (EdgesNo + 1);

  unsigned CurrentVert { 0 };
  std::string Line;
  unsigned EdgeC { 0 };
  while (getline (FIn , Line))
  {
    std::istringstream Stream (Line);
    while (Stream >> CurrentVert)
    {
      Vertices[CurrentVert].push_back (EdgeC);
      Edges[EdgeC].push_back (CurrentVert);
    }
    ++EdgeC;
  }
}

void HGraph::dump (std::ostream& Out) const
{
  Out << "HGraph dumping:\nVerticesNo: " << Vertices.size () - 1
    << " EdgesNo: " << Edges.size () - 1 << '\n';
  Out << "Vetices:\n";
  unsigned SkipFirst = 0;
  for (auto&& VertVec : Vertices)
  {
    if (!SkipFirst++)
      continue;
    for (auto&& Vert : VertVec)
      Out << Vert << ' ';
    Out << '\n';
  }
  Out << "Edges:\n";
  for (auto&& EdgesVec : Edges)
  {
    for (auto&& Edge : EdgesVec)
      Out << Edge << ' ';
    Out << '\n';
  }
}


void Partitions::calculateCost ()
{
  unsigned Skip = 0;
  for (auto&& EdgesVec : Graph.getEdges ())
  {
    if (!Skip++)
      continue;
    bool const CurrentPartition = VertPartitions[EdgesVec[0]];
    bool const Inc =
      std::any_of (EdgesVec.begin () + 1 , EdgesVec.end () ,
                  [&CurrentPartition , this](int Edge) {
                    return VertPartitions[Edge] != CurrentPartition;
                  });
    if (Inc)
      ++Cost;
  }
}

Partitions::Partitions (HGraph const GraphIn): Graph { GraphIn }
{
  unsigned const Size = Graph.getVertices ().size () - 1;
  VertPartitions.resize (Size + 1);
  // TODO remake static partition initialization,
  for (unsigned i = 1; i <= Size / 2; ++i)
    VertPartitions[i] = false;
  for (unsigned i = Size / 2 + 1; i <= Size; ++i)
    VertPartitions[i] = true;

  Side = Size % 2;
  calculateCost ();
}

void Partitions::update (unsigned Vertex)
{
  int const Inc = VertPartitions.at (Vertex) ? -1 : 1;
  Side += Inc;
  VertPartitions.at (Vertex) = !VertPartitions.at (Vertex);
}

void Partitions::dump (std::ostream& Out) const
{
  Out << "Left: ";
  unsigned const Size = VertPartitions.size ();
  for (unsigned i = 1; i != Size; ++i)
    if (!VertPartitions[i])
      Out << i << " ";
  Out << "\nRight: ";
  for (unsigned i = 1; i != Size; ++i)
    if (VertPartitions[i])
      Out << i << " ";
  Out << "\nSolution cost: " << Cost << '\n';
}

void Partitions::out (std::ostream& Out) const
{
  unsigned Skip = 0;
  for (auto&& Part : VertPartitions)
  {
    if (!Skip++)
      continue;
    Out << Part << '\n';
  }
}