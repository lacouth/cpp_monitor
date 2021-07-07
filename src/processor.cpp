#include "processor.h"

// Return the aggregate CPU utilization
double Processor::Utilization() {
  auto total_1 = LinuxParser::Jiffies();
  auto active_1 = LinuxParser::ActiveJiffies();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  auto total_2 = LinuxParser::Jiffies();
  auto active_2 = LinuxParser::ActiveJiffies();

  auto total_f = total_2 - total_1;
  auto active_f = active_2 - active_1;

  if (total_f <= 0) return 0.0;

  return active_f / static_cast<double>(total_f);
}