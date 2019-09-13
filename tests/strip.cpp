#include <iostream>
#include "../src/util.cpp"

void TestString(std::string str) {
  std::cout << "===" << std::endl;
  std::cout << '"' << str << "\" => <" << Strip(str) << '>' << std::endl;
}

int main(int args, char** argv) {
  TestString("Clean string");
  TestString("    \t     \t   ");
  TestString("Test ending   \n   ");
  TestString("  \r\n   Whitespaces \t\t in \nbetween");
  TestString(" \t!!\t Bang!  !!   \r\n\r\n ");
  TestString("     o    ");
  TestString("     o");
  TestString("o    ");
  TestString("\n\nNew lines\n\n");

  return 1;
}

