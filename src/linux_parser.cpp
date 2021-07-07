#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

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
  string os, kernel, version;
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
  string line;
  string key;
  float memTotal;
  float memFree;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream(line) >> key >> memTotal;
    std::getline(filestream, line);
    std::istringstream(line) >> key >> memFree;
  }
  return ((memTotal - memFree) / memTotal);
}

long LinuxParser::UpTime() {
  long upTime;
  long idleTime;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    filestream >> upTime >> idleTime;
  }
  return upTime + idleTime;
}

long LinuxParser::Jiffies() {
  auto values = LinuxParser::CpuUtilization();
  long jiffies = 0;
  for (int i = kUser_; i < kSteal_; ++i) {
    jiffies += stol(values[i]);
  }
  return jiffies;
}

// Read and return the number of active jiffies for a PID
// ref:
// https://stackoverflow.com/questions/1420426/how-to-calculate-the-cpu-usage-of-a-process-by-pid-in-linux-from-c
long LinuxParser::ActiveJiffies(int pid) {
  string value;
  long jiffies = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  int i = 0;
  while (stream >> value) {
    if (i >= 13 && i <= 16) {
      jiffies += stol(value);
    }
    ++i;
  }
  return jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[kIdle_]) + stol(jiffies[kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() {
  string value;
  string line;
  vector<string> cpuUtilization;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    while (linestream >> value) {
      cpuUtilization.emplace_back(value);
    }
  }
  return cpuUtilization;
}

long LinuxParser::GetValueFromStat(std::string key) {
  string lKey;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> lKey >> value;
      if (lKey == key) {
        return stol(value);
      }
    }
  }
  return 0;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return LinuxParser::GetValueFromStat(filterProcesses);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return LinuxParser::GetValueFromStat(filterRunningProcesses);
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }
  return string();
}

string LinuxParser::GetInfoFromProcess(int pid, string info) {
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (std::getline(stream, line)) {
    std::stringstream(line) >> key >> value;
    if (key == info) {
      return value;
    }
  }
  return string();
}

// Read and return the memory used by a process
// as suggested, the use of VmRSS is more accurate than VmSize
string LinuxParser::Ram(int pid) {
  return LinuxParser::GetInfoFromProcess(pid, filterProcMem);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  return LinuxParser::GetInfoFromProcess(pid, filterUID);
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uId = LinuxParser::Uid(pid);
  string line;
  string userName;
  string pass;
  string id;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::stringstream linestream(line);
      std::getline(linestream, userName, ':');
      std::getline(linestream, pass, ':');
      std::getline(linestream, id, ':');
      if (id == uId) {
        return userName;
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string value;
  int i = 0;
  if (stream.is_open()) {
    while (i < 22) {
      stream >> value;
      ++i;
    }
    return UpTime() - stol(value) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}
