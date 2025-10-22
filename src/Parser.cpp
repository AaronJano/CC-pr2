#include "Parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

static std::vector<std::string> split_ws(const std::string& s) {
  std::istringstream iss(s);
  std::vector<std::string> out;
  std::string tok;
  while (iss >> tok) out.push_back(tok);
  return out;
}

static std::vector<std::string> split_comma(const std::string& s) {
  std::vector<std::string> out;
  std::string cur;
  for (char c : s) {
    if (c == ',') {
      out.push_back(cur);
      cur.clear();
    } else
      cur.push_back(c);
  }
  if (!cur.empty() || s.size() == 0) out.push_back(cur);
  return out;
}

bool Parser::parseFile(const std::string& path, TuringMachine& tm,
                       std::string& error) {
  std::ifstream f(path);
  if (!f) {
    error = "cannot open file: " + path;
    return false;
  }

  std::string line;
  int step = 0;
  std::vector<std::string> states;
  std::vector<char> inputAlpha;
  std::vector<char> tapeAlpha;
  std::string start;
  std::string blank;
  std::vector<std::string> accepts;
  std::size_t numberOfTapes = 1;
  std::vector<std::string> transLines;

  while (std::getline(f, line)) {
    // trim left/right
    auto first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) continue;
    line = line.substr(first);
    if (line.empty()) continue;
    if (line[0] == '#') continue;

    auto tokens = split_ws(line);
    if (tokens.empty()) continue;

    if (step == 0) {
      states = tokens;
    } else if (step == 1) {
      for (auto& t : tokens)
        if (t.size() == 1) {
          inputAlpha.push_back(t[0]);
        } else {
          error = "invalid symbol in input alphabet: " + t;
          return false;
        }
    } else if (step == 2) {
      for (auto& t : tokens)
        if (t.size() == 1) {
          tapeAlpha.push_back(t[0]);
        } else {
          error = "invalid symbol in tape alphabet: " + t;
          return false;
        }
    } else if (step == 3) {
      if (!tokens.empty()) {
        start = tokens[0];
      }
      bool found = false;
      for (auto& s : states) {
        if (s == start) {
          found = true;
          break;
        }
      }

      if (!found) {
        error = "start state not in states: " + start;
        return false;
      }

    } else if (step == 4) {
      if (!tokens.empty()) {
        blank = tokens[0];
      }
      bool found = false;
      for (char& c : inputAlpha) {
        if (c == blank[0]) {
          found = true;
        }
      }
      if (found) {
        error = "blank symbol cannot be in input alphabet: " + blank;
        return false;
      }
    } else if (step == 5) {
      for (auto& t : tokens) {
        bool found = false;
        for (auto& s : states) {
          if (s == t) {
            found = true;
            break;
          }
        }
        if (!found) {
          error = "accept state not in states: " + t;
          return false;
        }
      }
      accepts = tokens;
    } else if (step == 6) {
      if (tokens.size() == 1) {
        try {
          numberOfTapes = std::stoul(tokens[0]);
        } catch (const std::exception& e) {
          error = "invalid number of tapes: " + tokens[0];
          return false;
        }
      } else {
        error = "invalid input in number of tapes line";
        return false;
      }
    } else {
      transLines.push_back(line);
    }
    ++step;
  }

  if (states.empty()) {
    error = "no states found";
    return false;
  }

  // assign parsed elements to TuringMachine
  tm = TuringMachine(numberOfTapes);
  tm.setStates(states);
  if (!start.empty()) tm.setStartState(start);
  if (!blank.empty()) tm.setBlank(blank[0]);
  tm.setAcceptStates(accepts);

  // parse transitions
  for (auto& l : transLines) {
    auto toks = split_ws(l);
    if (toks.size() < 5) continue;
    std::string from = toks[0];
    std::string readS = toks[1];
    std::string to = toks[2];
    std::string writeS = toks[3];
    std::string moveS = toks[4];

    auto reads = split_comma(readS);
    auto writes = split_comma(writeS);
    auto moves = split_comma(moveS);

    bool found = false;
    for (std::string& state : states) {
      if (state == from) {
        found = true;
        break;
      }
    }

    if (!found) {
      error = "transition from unknown state: " + from +
              " in transition line: " + l;
      return false;
    }

    for (std::string& r : reads) {
      if (r.size() != 1) {
        error = "invalid read symbol in transition line: " + l;
        return false;
      }

      bool found = false;
      for (char& c : tapeAlpha) {
        if (c == r[0]) {
          found = true;
          break;
        }
      }
      if (!found) {
        error = "transition reads invalid symbol: " + std::string(1, r[0]) +
                " in transition line: " + l;
        return false;
      }
    }

    found = false;
    for (std::string& state : states) {
      if (state == to) {
        found = true;
        break;
      }
    }

    if (!found) {
      error = "transition to unknown state: " + to;
      return false;
    }

    for (std::string& w : writes) {
      if (w.size() != 1) {
        error = "invalid write symbol in transition line: " + l;
        return false;
      }

      bool found = false;
      for (char& c : tapeAlpha) {
        if (c == w[0]) {
          found = true;
          break;
        }
      }
      if (!found) {
        error = "transition writes invalid symbol: " + std::string(1, w[0]) +
                " in transition line: " + l;
        return false;
      }
    }

    for (std::string& m : moves) {
      if (m.size() != 1 || (m[0] != 'L' && m[0] != 'R' && m[0] != 'S')) {
        error = "invalid move symbol in transition line: " + l;
        return false;
      }
    }

    Transition new_transition;
    new_transition.from = from;
    new_transition.to = to;
    new_transition.read.assign(numberOfTapes, '.');
    new_transition.write.assign(numberOfTapes, '.');
    new_transition.move.assign(numberOfTapes, 'R');

    for (size_t i = 0; i < reads.size(); ++i)
      if (!reads[i].empty()) new_transition.read[i] = reads[i][0];
    for (size_t i = 0; i < writes.size(); ++i)
      if (!writes[i].empty()) new_transition.write[i] = writes[i][0];
    for (size_t i = 0; i < moves.size(); ++i)
      if (!moves[i].empty()) new_transition.move[i] = moves[i][0];

    tm.addTransition(new_transition);
  }

  return true;
}
