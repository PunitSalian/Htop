#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include <iostream>
#include "linux_parser.h"
#include "process.h"
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}
int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  long activejiff = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);

  long starttime = LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK);

  long jiffies = LinuxParser::Jiffies() / sysconf(_SC_CLK_TCK);

  long totalelapsedtime = jiffies - starttime;

  cpuutilize_ = ((float)activejiff) / ((float)totalelapsedtime);

  return cpuutilize_;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() {
  return LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK);
}

bool Process::operator>(Process const& a) const {
  return cpuutilize_ > a.cpuutilize_;
}