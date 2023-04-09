#include "apply.hpp"
#include <chrono>


int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cout << "Select input file \n";
    return 1;  
}

  std::ifstream File{argv[1]};
  std::ofstream OutFile{std::string(argv[1]) + ".part.2"};

  HGraph Graph{File};
  Partitions Prt{Graph};

  // Время до
  auto const Start = std::chrono::steady_clock::now();
  // Выполняем проход о графу
  unsigned const Iterations = FM(Graph, Prt);
  // Время после
  auto const End = std::chrono::steady_clock::now();

  // Вывод в формате: входной файл | вид ребер в графе | вид вершин в графе | счетчик разбиений | время работы алгоритма | итерации
  OutFile << argv[1] << ' ' << Graph.getEdges().size() << ' '
        << Graph.getVertices().size() << ' ' << Prt.getCost() << ' '
        << (End - Start).count()<< ' ' << Iterations << '\n';
  return 0;
}