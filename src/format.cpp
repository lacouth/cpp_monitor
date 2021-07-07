#include "format.h"

#include <string>

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
  long hour = seconds / 3600;
  seconds -= hour * 3600;
  long minutes = seconds / 60;
  seconds -= minutes * 60;
  string hour_str = to_string(hour);
  string minutes_str = to_string(minutes);
  string seconds_str = to_string(seconds);

  hour_str = hour_str.size() == 2 ? hour_str : "0" + hour_str;
  minutes_str = minutes_str.size() == 2 ? minutes_str : "0" + minutes_str;
  seconds_str = seconds_str.size() == 2 ? seconds_str : "0" + seconds_str;

  return hour_str + ":" + minutes_str + ":" + seconds_str;
}