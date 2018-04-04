#ifndef OB_TERM_HH
#define OB_TERM_HH

#include "ansi_escape_codes.hh"
namespace AEC = OB::ANSI_Escape_Codes;

#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <thread>
#include <chrono>

namespace OB
{

class Term
{
public:
  Term()
  {
    cursor_hide();
    set_raw();
  }

  ~Term()
  {
    cursor_show();
    set_cooked();
  }

  void set_cooked()
  {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_) == -1)
    {
      throw std::runtime_error("tcsetattr failed");
    }
  }

  void set_raw()
  {
    if (tcgetattr(STDIN_FILENO, &old_) == -1)
    {
      throw std::runtime_error("tcgetattr failed");
    }

    raw_ = old_;
    raw_.c_lflag &= ~(ECHO | ICANON);
    raw_.c_cc[VMIN]  = 0;
    raw_.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_) == -1)
    {
      throw std::runtime_error("tcsetattr failed");
    }
  }

  static void cursor_hide()
  {
    std::cout << AEC::cursor_hide << std::flush;
  }

  static void cursor_show()
  {
    std::cout << AEC::cursor_show << std::flush;
  }

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

  static int cursor_get(size_t& width, size_t& height)
  {
    std::cout << "\033[6n" << std::flush;
    char buf[32];
    uint8_t i = 0;
    for (;i < sizeof(buf) -1; ++i)
    {
      while (read(STDIN_FILENO, &buf[i], 1) != 1)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      if (buf[i] == 'R') break;
    }
    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    int x;
    int y;
    if (sscanf(&buf[2], "%d;%d", &y, &x) != 2) return -1;
    width = x;
    height = y;
    return 0;
  }

private:
  termios old_;
  termios raw_;

}; // class Term

} // namespace OB

#endif // OB_TERM_HH
