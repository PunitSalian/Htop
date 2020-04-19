#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
  int hour = seconds / 3600;

  int minutes = (seconds % 3600) / 60;

  int secs = (seconds % 3600) % 60;

  string temp = std::to_string(hour) + ":" + std::to_string(minutes) + ":" +
                std::to_string(secs);

  return temp;
}