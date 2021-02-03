# yavom
Yet Another Variation of Myers for generic containers (for example std::vector)

This is an implementation of a quasi-Myers algorithm to determine the differences between two generic containers.
Everything is included in *diff.h*. Its usage is quite simple, for example:
```c
using namespace syscall::yavom;
std::vector<std::string> arrayOne;
std::vector<std::string> arrayTwo;
// Fill the arrays
auto moves = myers(arrayOne,arrayTwo);
```
The return value *moves* is a vector of *Move* objects. The *myers* function accepts an additional *long* parameter which defines the maximum
time (in *milliseconds*) that the algorithm is allowed to spend at each iteration (an iteration is one step *d* in Myers algorithm): this is
useful when dealing with very large containers (by default the value is -1, meaning that no time limit is set).
You can apply moves to the array as follows:
```c
for (const auto& m : moves) {
  apply_move(m, arrayOne);
}
// now arrayOne contents are the same as arrayTwo
```
You can serialize / deserialize *Move* objects as you deem necessary. To do so please consider the following definitions (found in *diff.h*):
```c
enum class OP {INSERT, DELETE};
using Point=std::tuple<long,long>;

template<typename K>
using Move=std::tuple<OP,Point,Point,std::vector<K>>;
```
The *std::vector<K>* field stores the values to be inserted.

If you are interested in knowning how many moves will be necessary but do not want to generate complete moves (with complete insert data), you can
use the *myers_unfilled* function:
```c
auto moves = myers_unfilled(a,b, 1);
std::cerr << moves.size() << " moves\n";
``` 
Subsequently you can fill the insertion data:
```c
myers_fill(a, b, moves);
```

## Credits & License
This code is Copyright (C) 2021 Amos Brocco (contact@amosbrocco.ch)

BSD 3-Clause License
