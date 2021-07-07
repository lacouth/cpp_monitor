#ifndef PROCESS_H
#define PROCESS_H

#include <iomanip>
#include <string>

#include "linux_parser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(long pid);
  int Pid();                   // TODO: See src/process.cpp
  std::string User();          // TODO: See src/process.cpp
  std::string Command();       // TODO: See src/process.cpp
  float CpuUtilization();      // TODO: See src/process.cpp
  std::string Ram();           // TODO: See src/process.cpp
  long int UpTime();           // TODO: See src/process.cpp
  bool operator<(Process &a);  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  long _pid;
};

#endif