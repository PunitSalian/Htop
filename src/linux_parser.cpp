#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

#define UPTIMEINDEX 22
#define USERTIMEINDEX 14
using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string total_mem;
  string free_mem;
  string line;
  string key;
  float memutilization = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key >> total_mem;

    std::getline(filestream, line);
    std::istringstream linestream2(line);
    linestream2 >> key >> free_mem;
    float totalmem = stol(total_mem);
    float freemem = stol(free_mem);

    memutilization = (totalmem - freemem) / totalmem;
  }

  return memutilization;
}

long LinuxParser::UpTime() {
  string system_uptime;
  string line;
  long uptime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> system_uptime;
    uptime = stol(system_uptime);
  }
  return uptime;
}

long LinuxParser::Jiffies() {
  return LinuxParser::IdleJiffies() + LinuxParser::ActiveJiffies();
}

long LinuxParser::ActiveJiffies(int pid) {
  string utime;
  string stime;
  string cutime;
  string cstime;
  string line;
  long activejiffies = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);

    std::istringstream linestream(line);

    int y = USERTIMEINDEX;
    while (y-- > 0 && linestream >> utime)
      ;
    linestream >> stime;
    linestream >> cutime;
    linestream >> cstime;
    activejiffies = stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
  }

  return activejiffies;
}

long LinuxParser::ActiveJiffies() {
  string line;
  string key;
  string user;
  string nice;
  string system;
  string dummy1;
  string dummy2;
  string irq;
  string softirq;
  string steal;
  long activejiffirsystem = 0;
  // ignoring softirq and irq to simplify calculation

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          linestream >> user >> nice >> system >> dummy1 >> dummy2 >> irq >>
              softirq >> steal;
          activejiffirsystem = stol(user) + stol(nice) + stol(system) +
                               stol(irq) + stol(softirq) + stol(steal);
        }
      }
    }
  }
  return activejiffirsystem;
}
long LinuxParser::IdleJiffies() {
  string line;
  string key;
  string iowait;
  string idle;
  long idlejiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          int y = 4;
          while (y-- > 0 && linestream >> idle)
            ;

          linestream >> iowait;

          idlejiffies = stol(iowait) + stol(idle);
        }
      }
    }
  }

  return idlejiffies;
}

vector<string> LinuxParser::CpuUtilization() { return {}; }

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value = "0";
  int totprocs = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          totprocs = stoi(value);
        } else {
          break;
        }
      }
    }
  }
  return totprocs;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value = "0";
  int runprocs = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          runprocs = stoi(value);
        } else {
          break;
        }
      }
    }
  }
  return runprocs;
}

= string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }

  return line;
}

string LinuxParser::Ram(int pid) {
  string line = "";
  string key = "";
  string value = "";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          int x = stoi(value) / 1000;
          return to_string(x);
        }
      }
    }
  }

  return value;
}

string LinuxParser::Uid(int pid) {
  string line = "";
  string key = "";
  string value = "";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);

  string user, dummy1, curruid, line;

  std::ifstream filestream(kPasswordPath);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> dummy1 >> curruid) {
        if (curruid == uid) {
          return user;
        }
      }
    }
  }

  return user;
}

long LinuxParser::UpTime(int pid) {
  string line;
  long uptime = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);

    std::istringstream linestream(line);

    string value;

    int y = UPTIMEINDEX;

    while (y-- > 0 && linestream >> value)
      ;

    uptime = stol(value);
  }
  return uptime;
}
