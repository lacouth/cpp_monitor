#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

Process::Process(long pid) : _pid(pid) {}

// Return this process's ID
int Process::Pid() { return _pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
  auto total_jiffies = LinuxParser::ActiveJiffies(_pid);
  auto process_time = LinuxParser::UpTime(_pid);
  auto system_time = LinuxParser::UpTime();
  float diff = system_time - process_time;

  if (diff <= 0) return 0.0;

  return (total_jiffies / sysconf(_SC_CLK_TCK)) / diff;
}

// Return the command that generated this process
string Process::Command() {
  string cmd = LinuxParser::Command(_pid);
  return cmd.size() < 40 ? cmd : cmd.substr(0, 40) + "...";
}

// Return this process's memory utilization
string Process::Ram() {
  string ram_str = LinuxParser::Ram(_pid);
  if (ram_str.size()) {
    int ram = stoi(ram_str) / 1024;
    return to_string(ram);
  }
  return "0";
}

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(_pid); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(_pid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process &a) {
  return stof(a.Ram()) < stof(this->Ram());
}