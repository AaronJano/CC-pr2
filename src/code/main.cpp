#include <iostream>

#include "Parser.h"
#include "TuringMachine.h"

int main(int argc, char** argv) {
  if (argc > 2 || argc < 1) {
    std::cerr << "Usage: " << argv[0] << " [path/to/turing_machine.txt]"
              << std::endl;
    return 1;
  }

  std::string path = argv[1];

  TuringMachine tm;
  std::string err;
  if (!Parser::parseFile(path, tm, err)) {
    std::cerr << "Error parsing file: " << err << std::endl;
    return 1;
  }

  do {
    std::string input = "";
    std::cout << "Enter input string for the Turing Machine (.q to exit): ";
    std::cin >> input;
    if (input == ".q") {
      break;
    }
    try {
      tm.loadTape(input);
    } catch (const std::invalid_argument& e) {
      std::cerr << "Invalid input: " << e.what() << std::endl;
      continue;
    }
    bool accepted = tm.run(10000);
    std::cout << "Final state: " << tm.currentState() << std::endl;
    std::cout << "Accepted: " << (accepted ? "yes" : "no") << std::endl;
    std::cout << "Tape snapshot: " << tm.tapeSnapshot(0, 30) << std::endl;
  } while (true);
  return 0;
}