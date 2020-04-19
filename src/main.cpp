#include <iostream>
#include "linux_parser.h"
#include "ncurses_display.h"
#include "process.h"
#include "system.h"

using namespace std;
int main() {
  System system;

  // cout << system.MemoryUtilization();
  NCursesDisplay::Display(system);
}