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
The return value *moves* is a vector of *Move* objects: you can apply those moves to the array as follows:
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

## Credits & License
This code is Copyright (C) 2021 Amos Brocco (contact@amosbrocco.ch)

BSD 3-Clause License
