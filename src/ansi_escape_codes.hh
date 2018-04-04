#ifndef OB_ANSI_ESCAPE_CODES_HH
#define OB_ANSI_ESCAPE_CODES_HH

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace OB
{

namespace ANSI_Escape_Codes
{
  // standard escaped characters
  extern std::string const nl;
  extern std::string const cr;
  extern std::string const tab;
  extern std::string const alert;

  // escape code sequence
  extern std::string const esc;

  // clears all attributes
  extern std::string const reset;

  // style
  extern std::string const bold;
  extern std::string const dim;
  extern std::string const italic;
  extern std::string const underline;
  extern std::string const blink;
  extern std::string const rblink;
  extern std::string const reverse;
  extern std::string const conceal;
  extern std::string const cross;

  // erasing
  extern std::string const erase_end;
  extern std::string const erase_start;
  extern std::string const erase_line;
  extern std::string const erase_down;
  extern std::string const erase_up;
  extern std::string const erase_screen;

  // cursor visibility
  extern std::string const cursor_hide;
  extern std::string const cursor_show;

  // cursor movement
  extern std::string const cursor_home;
  extern std::string const cursor_up;
  extern std::string const cursor_down;
  extern std::string const cursor_right;
  extern std::string const cursor_left;
  extern std::string const cursor_save;
  extern std::string const cursor_load;

  // foreground color
  extern std::string const fg_black;
  extern std::string const fg_red;
  extern std::string const fg_green;
  extern std::string const fg_yellow;
  extern std::string const fg_blue;
  extern std::string const fg_magenta;
  extern std::string const fg_cyan;
  extern std::string const fg_white;

  // background color
  extern std::string const bg_black;
  extern std::string const bg_red;
  extern std::string const bg_green;
  extern std::string const bg_yellow;
  extern std::string const bg_blue;
  extern std::string const bg_magenta;
  extern std::string const bg_cyan;
  extern std::string const bg_white;

  // prototypes
  std::string fg_256(std::string x);
  std::string bg_256(std::string x);
  std::string htoi(std::string x);
  std::string fg_true(std::string x);
  std::string bg_true(std::string x);
  std::string cursor_set(size_t x, size_t y);

  template<class T>
  std::string wrap(T const val, std::string col)
  {
    std::string x {std::to_string(val)};
    if (x.size() != 6) return {};
    std::string h1 {x.substr(0, 2)};
    std::string h2 {x.substr(2, 2)};
    std::string h3 {x.substr(4, 2)};
    std::stringstream ss; ss
    << esc << "38;2;"
    << htoi(h1) << ";"
    << htoi(h2) << ";"
    << htoi(h3) << "m"
    << val
    << reset;
    return ss.str();
  }

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

} // namespace ANSI_Escape_Codes

} // namespace OB

#endif // OB_ANSI_ESCAPE_CODES_HH
