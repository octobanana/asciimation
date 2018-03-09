#include "asciimation.hh"
#include "term.hh"

#include "parg.hh"
using Parg = OB::Parg;

#include "color.hh"
namespace Cl = OB::Color;

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <csignal>

int program_options(Parg& pg);
void clean_shutdown();
void cb_signal(int signal);
void register_signals();
void cursor_hide();
void cursor_show();

void clean_shutdown()
{
  cursor_show();
}

void cb_signal(int signal)
{
  std::cerr << "Signal: " << signal << " Received" << '\n';
  clean_shutdown();
  exit(1);
}

void register_signals()
{
  std::signal(SIGINT, cb_signal);
  std::signal(SIGABRT, cb_signal);
  std::signal(SIGFPE, cb_signal);
  std::signal(SIGILL, cb_signal);
  std::signal(SIGINT, cb_signal);
  std::signal(SIGSEGV, cb_signal);
  std::signal(SIGTERM, cb_signal);
}

int program_options(Parg& pg)
{
  pg.name("asciimation").version("0.1.0 (08.03.2018)");
  pg.description("ascii animations for the terminal");
  pg.usage("[flags] [options] [--] [arguments]");
  pg.usage("[-f 'input_file'] [-d 'delim'] [-t time_delay] [-w min_width] [-h min_height] [--debug]");
  pg.usage("[-v|--version]");
  pg.usage("[-h|--help]");
  pg.info("Exit Codes", {"0 -> normal", "1 -> error"});
  pg.info("Examples", {
    "asciimation -f 'test' -d 'END' -t 80",
    "asciimation --help",
    "asciimation --version",
  });
  pg.author("Brett Robinson (octobanana) <octobanana.dev@gmail.com>");

  pg.set("help,h", "print the help output");
  pg.set("version,v", "print the program version");
  pg.set("file,f", "", "file_name", "the input file");
  pg.set("delim,d", "END", "str", "the frame delimiter");
  pg.set("time,t", "250", "int", "the time delay between frames in milliseconds");
  pg.set("debug", "show debug output");
  pg.set("width,w", "80", "int", "set the minimum screen width");
  pg.set("height,h", "20", "int", "set the minimum screen height");
  pg.set("loop,l", "set the animation to loop");

  // pg.set_pos();
  // pg.set_stdin();

  int status {pg.parse()};
  // uncomment if at least one argument is expected
  if (status > 0 && pg.get_stdin().empty())
  {
    std::cout << pg.print_help() << "\n";
    std::cout << "Error: " << "expected arguments" << "\n";
    return -1;
  }
  if (status < 0)
  {
    std::cout << pg.print_help() << "\n";
    std::cout << "Error: " << pg.error() << "\n";
    return -1;
  }
  if (pg.get<bool>("help"))
  {
    std::cout << pg.print_help();
    return 1;
  }
  if (pg.get<bool>("version"))
  {
    std::cout << pg.print_version();
    return 1;
  }
  return 0;
}

void cursor_hide()
{
  std::cout
  << Cl::c_hide
  << Cl::c_h
  << Cl::e_d
  << std::endl;
}

void cursor_show()
{
  std::cout
  << Cl::c_h
  << Cl::e_d
  << Cl::c_show
  << std::endl;
}

int main(int argc, char *argv[])
{
  Parg pg {argc, argv};
  int pstatus {program_options(pg)};
  if (pstatus > 0) return 0;
  if (pstatus < 0) return 1;

  register_signals();

  size_t twidth {0};
  size_t theight {0};
  OB::Term::size(twidth, theight);

  if (pg.get<size_t>("width") > twidth)
  {
    std::cerr << "Error: screen width is too small" << std::endl;
    return 1;
  }

  if (pg.get<size_t>("height") > theight)
  {
    std::cerr << "Error: screen height is too small" << std::endl;
    return 1;
  }

  OB::Asciimation am;
  am.set_debug(pg.get<bool>("debug"));
  am.set_loop(pg.get<bool>("loop"));
  am.set_delay(pg.get<int>("time"));
  am.set_delim(pg.get("delim"));
  cursor_hide();
  am.run(pg.get("file"));
  cursor_show();

  return 0;
}
