#pragma once
#include <string>
#include <vector>

// A transition for a multi-tape Turing Machine. Each vector has one entry per
// tape.
struct Transition {
  std::string from;
  std::vector<char> read;  // symbols to read on each tape
  std::string to;
  std::vector<char> write;  // symbols to write on each tape
  std::vector<char> move;   // 'L', 'R' or 'S' for each tape
};
