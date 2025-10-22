#pragma once
#include <string>
#include <vector>

// Tape represents an infinite tape to the left and right using a vector.
class Tape {
 public:
  Tape(char blank = '.');
  char read() const;
  void write(char c);
  void moveLeft();
  void moveRight();
  std::string asString(size_t radius = 20) const;
  void setHeadIndex(int index) { headIndex = index; }
  void reset() {
    cells.clear();
    cells.push_back(blank);
    headIndex = 0;
  }

 private:
  std::vector<char> cells;
  int headIndex;  // index into deque
  char blank;
};
