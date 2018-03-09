#ifndef OB_COLOR_HH
#define OB_COLOR_HH

#include <string>
#include <vector>
#include <sstream>

namespace OB
{
namespace Color
{
  std::string const reset {"\033[0m"};
  std::string const bold {"\033[1m"};
  std::string const dim {"\033[2m"};
  std::string const italic {"\033[3m"};
  std::string const underline {"\033[4m"};
  std::string const blink {"\033[5m"};
  std::string const rblink {"\033[6m"};
  std::string const reverse {"\033[7m"};
  std::string const conceal {"\033[8m"};
  std::string const cross {"\033[9m"};

  std::string const e_eol {"\033[K"};
  std::string const e_sol {"\033[1K"};
  std::string const e_l {"\033[2K"};
  std::string const e_d {"\033[J"};
  std::string const e_u {"\033[1J"};
  std::string const e_s {"\033[2J"};

  std::string const c_h {"\033[0;0H"};
  std::string const c_u {"\033[0A"};
  std::string const c_d {"\033[0B"};
  std::string const c_f {"\033[0C"};
  std::string const c_b {"\033[0D"};

  std::string const c_hide {"\033[?25l"};
  std::string const c_show {"\033[?25h"};

  std::string const fg_black {"\033[30m"};
  std::string const fg_red {"\033[31m"};
  std::string const fg_green {"\033[32m"};
  std::string const fg_yellow {"\033[33m"};
  std::string const fg_blue {"\033[34m"};
  std::string const fg_magenta {"\033[35m"};
  std::string const fg_cyan {"\033[36m"};
  std::string const fg_white {"\033[37m"};

  std::string const bg_black {"\033[40m"};
  std::string const bg_red {"\033[41m"};
  std::string const bg_green {"\033[42m"};
  std::string const bg_yellow {"\033[43m"};
  std::string const bg_blue {"\033[44m"};
  std::string const bg_magenta {"\033[45m"};
  std::string const bg_cyan {"\033[46m"};
  std::string const bg_white {"\033[47m"};

  template<class T>
  std::string wrap(T const val, std::vector<std::string> const col)
  {
    std::stringstream ss;
    for (auto const& e : col)
    {
      ss << e;
    }
    ss << val << reset;
    return ss.str();
  }
} // namespace Color
} // namespace OB

#endif // OB_COLOR_HH
