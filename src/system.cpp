#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  vector<int> processpid(LinuxParser::Pids());

  set<int> curr_pids;
  for (auto proc : processes_) {
    curr_pids.insert(proc.Pid());
  }

  for (auto procpid : processpid) {
    if (curr_pids.find(procpid) == curr_pids.end()) {
      processes_.emplace_back(procpid);
    }
  }

  // arrange the process with top utilized process
  for (auto& proc : processes_) {
    proc.CpuUtilization();
  }

  sort(processes_.begin(), processes_.end(), std::greater<Process>());
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }