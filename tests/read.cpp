#include <iostream>
#include "../src/util.cpp"

int main(int args, char** argv) {
  std::vector<std::string> names = ReadLines("data/names.txt");
  if (names.size() > 0) {
//    for (std::string nm: names) std::cout << nm << ' ';
//    std::cout << std::endl;

    std::set<int> exc;
    for (int i = 0; i < names.size() + 4; i++)
      std::cout << RandomChoice(names, &exc) << std::endl;
    std::cout << exc.size() << "\n=====" << std::endl;

    for (int i = 0; i < 5; i++)
      std::cout << RandomChoice(names) << std::endl;
  }
}

