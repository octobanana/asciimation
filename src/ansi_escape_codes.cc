#include "ansi_escape_codes.hh"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace OB
{

namespace ANSI_Escape_Codes
{

  // standard escaped characters
  std::string const nl {"\n"};
  std::string const cr {"\r"};
  std::string const tab {"\t"};
  std::string const alert {"\a"};

  // escape code sequence
  std::string const esc {"\033["};

  // clears all attributes
  std::string const reset {esc + "0m"};

  // style
  std::string const bold {esc + "1m"};
  std::string const dim {esc + "2m"};
  std::string const italic {esc + "3m"};
  std::string const underline {esc + "4m"};
  std::string const blink {esc + "5m"};
  std::string const rblink {esc + "6m"};
  std::string const reverse {esc + "7m"};
  std::string const conceal {esc + "8m"};
  std::string const cross {esc + "9m"};

  // erasing
  std::string const erase_end {esc + "K"};
  std::string const erase_start {esc + "1K"};
  std::string const erase_line {esc + "2K"};
  std::string const erase_down {esc + "J"};
  std::string const erase_up {esc + "1J"};
  std::string const erase_screen {esc + "2J"};

  // cursor visibility
  std::string const cursor_hide {esc + "?25l"};
  std::string const cursor_show {esc + "?25h"};

  // cursor movement
  std::string const cursor_home {esc + "0;0H"};
  std::string const cursor_up {esc + "1A"};
  std::string const cursor_down {esc + "1B"};
  std::string const cursor_right {esc + "1C"};
  std::string const cursor_left {esc + "1D"};
  std::string const cursor_save {"\0337"};
  std::string const cursor_load {"\0338"};

  // foreground color
  std::string const fg_black {esc + "30m"};
  std::string const fg_red {esc + "31m"};
  std::string const fg_green {esc + "32m"};
  std::string const fg_yellow {esc + "33m"};
  std::string const fg_blue {esc + "34m"};
  std::string const fg_magenta {esc + "35m"};
  std::string const fg_cyan {esc + "36m"};
  std::string const fg_white {esc + "37m"};

  // background color
  std::string const bg_black {esc + "40m"};
  std::string const bg_red {esc + "41m"};
  std::string const bg_green {esc + "42m"};
  std::string const bg_yellow {esc + "43m"};
  std::string const bg_blue {esc + "44m"};
  std::string const bg_magenta {esc + "45m"};
  std::string const bg_cyan {esc + "46m"};
  std::string const bg_white {esc + "47m"};

  std::string fg_256(std::string x)
  {
    auto n = std::stoi(x);
    if (n < 0 || n > 256) return {};
    std::stringstream ss;
    ss << esc << "38;5;" << x << "m";
    return ss.str();
  }

  std::string bg_256(std::string x)
  {
    auto n = std::stoi(x);
    if (n < 0 || n > 256) return {};
    std::stringstream ss;
    ss << esc << "48;5;" << x << "m";
    return ss.str();
  }

  std::string htoi(std::string x)
  {
    std::stringstream ss;
    ss << x;
    unsigned int n;
    ss >> std::hex >> n;
    return std::to_string(n);
  }

  std::string fg_true(std::string x)
  {
    if (x.size() != 6) return {};
    std::string h1 {x.substr(0, 2)};
    std::string h2 {x.substr(2, 2)};
    std::string h3 {x.substr(4, 2)};
    std::stringstream ss; ss
    << esc << "38;2;"
    << htoi(h1) << ";"
    << htoi(h2) << ";"
    << htoi(h3) << "m";
    return ss.str();
  }

  std::string bg_true(std::string x)
  {
    if (x.size() != 6) return {};
    std::string h1 {x.substr(0, 2)};
    std::string h2 {x.substr(2, 2)};
    std::string h3 {x.substr(4, 2)};
    std::stringstream ss; ss
    << esc << "48;2;"
    << htoi(h1) << ";"
    << htoi(h2) << ";"
    << htoi(h3) << "m";
    return ss.str();
  }

  std::string cursor_set(size_t x, size_t y)
  {
    std::stringstream ss;
    ss << esc << y << ";" << x << "H";
    return ss.str();
  }

} // namespace ANSI_Escape_Codes

} // namespace OB
