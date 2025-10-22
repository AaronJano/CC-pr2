#pragma once
#include <string>

#include "TuringMachine.h"

// Simple parser for the text format used in data/*.txt files.
class Parser {
 public:
  static bool parseFile(const std::string& path, TuringMachine& tm,
                        std::string& error);
};
