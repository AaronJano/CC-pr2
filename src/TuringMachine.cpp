#include "TuringMachine.h"

#include <algorithm>
#include <iostream>

TuringMachine::TuringMachine(size_t nTapes) : tapes(), blank('.') {
  if (nTapes == 0) nTapes = 1;
  for (size_t i = 0; i < nTapes; ++i) tapes.emplace_back(blank);
}

void TuringMachine::setStates(const std::vector<std::string>& sts) {
  states.clear();
  for (auto& s : sts) states.insert(s);
}

void TuringMachine::setInputAlphabet(const std::vector<char>&) {}
void TuringMachine::setTapeAlphabet(const std::vector<char>&) {}

void TuringMachine::setStartState(const std::string& s) {
  startState = s;
  state = s;
}

void TuringMachine::setBlank(char b) {
  blank = b;
  for (auto& t : tapes) t = Tape(blank);
}

void TuringMachine::setAcceptStates(const std::vector<std::string>& acc) {
  acceptStates.clear();
  for (auto& s : acc) acceptStates.insert(s);
}

void TuringMachine::addTransition(const Transition& t) {
  // accept transitions with correct arity
  if (t.read.size() == nTapes() && t.write.size() == nTapes() &&
      t.move.size() == nTapes())
    transitions.push_back(t);
  else {
    // ignore mismatched transitions for now
    std::cerr << "Warning: transition arity does not match number of tapes; "
                 "ignored\n";
  }
}

void TuringMachine::loadTape(const std::string& input) {
  // load input into tape 0; other tapes blank
  for (size_t i = 0; i < nTapes(); ++i) tapes[i] = Tape(blank);
  // write input starting at head position on tape 0
  for (size_t i = 0; i < input.size(); ++i) {
    tapes[0].write(input[i]);
    if (i + 1 < input.size()) tapes[0].moveRight();  // check
  }

  tapes[0].setHeadIndex(0);  // reset head to start of input

  state = startState;
}

bool TuringMachine::isAcceptState(const std::string& s) const {
  return acceptStates.find(s) != acceptStates.end();
}

bool TuringMachine::step() {
  // Current symbols on each tape
  std::vector<char> cur(nTapes());
  for (size_t i = 0; i < nTapes(); ++i) cur[i] = tapes[i].read();

  for (auto& t : transitions) {
    if (t.from != state) continue;
    if (t.read == cur) {
      // apply writes and moves
      for (size_t i = 0; i < nTapes(); ++i) tapes[i].write(t.write[i]);
      for (size_t i = 0; i < nTapes(); ++i) {
        if (t.move[i] == 'L') {
          tapes[i].moveLeft();
        } else if (t.move[i] == 'R') {
          tapes[i].moveRight();
        }
      }
      state = t.to;
      return true;
    }
  }
  return false;  // no transition
}

bool TuringMachine::run(size_t maxSteps) {
  for (size_t i = 0; i < maxSteps; ++i) {
    if (isAcceptState(state)) return true;
    if (!step()) return false;
  }
  return false;  // exceeded steps
}

std::string TuringMachine::currentState() const { return state; }

std::string TuringMachine::tapeSnapshot(size_t tapeIndex, size_t radius) const {
  if (tapeIndex >= nTapes()) return "";
  return this->blank + tapes[tapeIndex].asString(radius) + this->blank;
}
