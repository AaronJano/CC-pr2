#pragma once
#include <string>
#include <unordered_set>
#include <vector>

#include "Tape.h"
#include "Transition.h"

class TuringMachine {
 public:
  // Create a multi-tape Turing machine with `nTapes`. A single-tape machine is
  // nTapes=1.
  explicit TuringMachine(size_t nTapes = 1);
  void setStates(const std::vector<std::string>& states);
  void setInputAlphabet(const std::vector<char>& alpha);
  void setTapeAlphabet(const std::vector<char>& alpha);
  void setStartState(const std::string& s);
  void setBlank(char b);
  void setAcceptStates(const std::vector<std::string>& acc);
  void addTransition(const Transition& t);
  // load input on tape 0 (for single-tape compatibility). Other tapes are
  // blank.
  void loadTape(const std::string& input);
  bool step();
  bool run(size_t maxSteps = 10000);
  std::string currentState() const;
  // snapshot for tape i (default 0)
  std::string tapeSnapshot(size_t tapeIndex = 0, size_t radius = 20) const;
  void reset() {
    state = startState;
    for (auto& tape : tapes) tape.reset();
  }

 private:
  std::unordered_set<std::string> states;
  std::unordered_set<std::string> acceptStates;
  std::vector<Transition> transitions;
  std::vector<Tape> tapes;
  std::string startState;
  std::string state;
  char blank;
  bool isAcceptState(const std::string& s) const;
  size_t nTapes() const { return tapes.size(); }
};
