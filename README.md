# Graph_algorithm
Fiduccia-Mattheyses algorithm

# Компиляция

```
# Если работает cmake
mkdir build
cmake -S . -B build
cmake --build build

# Если не работает cmake
g++ -O3 all.cpp Alg/common.cpp Alg/base.cpp Alg/mod.cpp -o bin/FM
```