#include "Tape.h"

#include <algorithm>

Tape::Tape(char blank_) : headIndex(0), blank(blank_) {
  cells.push_back(blank);
}

char Tape::read() const { return cells[headIndex]; }

void Tape::write(char c) { cells[headIndex] = c; }

void Tape::moveLeft() {
  if (headIndex == 0) {
    // insert blank at beginning and keep head at the same logical cell
    cells.insert(cells.begin(), blank);
    // head stays at 0
  } else {
    --headIndex;
  }
}

void Tape::moveRight() {
  ++headIndex;
  if (headIndex >= (int)cells.size()) {
    cells.push_back(blank);
  }
}

std::string Tape::asString(size_t radius) const {
  int start = std::max(0, headIndex - (int)radius);
  int end = std::min((int)cells.size() - 1, headIndex + (int)radius);
  std::string s;
  for (int i = start; i <= end; ++i) {
    if (i == headIndex) {
      s.push_back('[');
      s.push_back(cells[i]);
      s.push_back(']');
    } else {
      s.push_back(cells[i]);
    }
  }
  return s;
}
