#include "apply.hpp"
#include <chrono>

int main(int argc, char *argv[])
{
  if (argc == 1 || argc >= 4) {
    std::cout << "Select input file and optionally apply -m\n";
    return 1;
  }

  char *file = nullptr;
  bool isMod = false;
  if (argc == 2)
    file = argv[1];
  else { // argc == 3
    if (std::string(argv[1]) == "-m")
      file = argv[2];
    else if (std::string(argv[2]) == "-m")
      file = argv[1];
    else {
      std::cout << "Waited for -m option and file\n";
      return 2;
    }
    isMod = true;
  }
  std::ifstream File{file};
  std::ofstream OutFile{std::string(file) + ".part.2"};

  HGraph hgraph{File};
  Partitions partitions{hgraph};

  // Время до
  auto const Start = std::chrono::steady_clock::now();
  // Выполняем проход о графу
  unsigned const Iterations = isMod ? AlgMod::FMMod(hgraph, partitions) : FM(hgraph, partitions);
  // Время после
  auto const End = std::chrono::steady_clock::now();

  // Итоговый вывод
  //partitions.out(OutFile);

  // Вывод в формате: входной файл | вид ребер в графе | вид вершин в графе | счетчик разбиений | время работы алгоритма | итерации
  // используется для parse.py скрипта и оценки работы алгоритма
   OutFile << argv[1] << ' ' << hgraph.getEdges().size() << ' '
         << hgraph.getVertices().size() << ' ' << partitions.getCost() << ' '
         << (End - Start).count()<< ' ' << Iterations << '\n';
  return 0;
}