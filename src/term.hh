#ifndef OB_TERM_HH
#define OB_TERM_HH

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>

namespace OB
{

class Term
{
public:
  static size_t height()
  {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
  }

  static size_t width()
  {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
  }

  static void size(size_t &width, size_t &height)
  {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    width = w.ws_col;
    height = w.ws_row;
  }
}; // class Term

} // namespace OB

#endif // OB_TERM_HH
