#include "asciimation.hh"

#include "parg.hh"
using Parg = OB::Parg;

#include "ansi_escape_codes.hh"
namespace AEC = OB::ANSI_Escape_Codes;

#include <string>
#include <iostream>
#include <csignal>

void clean_shutdown();
void cb_signal(int signal);
void register_signals();
int program_options(Parg& pg);

void clean_shutdown()
{
  std::cout << AEC::cursor_show << std::flush;
}

void cb_signal(int signal)
{
  std::cerr << "\nSignal: " << signal << " Received" << "\n";
  std::cerr << "Exiting\n";
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
  pg.name("asciimation").version("0.4.0 (03.04.2018)");
  pg.description("ascii animation interpreter");
  pg.usage("[flags] [options] [--] [arguments]");
  pg.usage("[-f|--file input_file] [-d|--delim delim] [-t|--time time_delay_ms] [-l|--loop loop_number] [--debug]");
  pg.usage("[-v|--version]");
  pg.usage("[-h|--help]");
  pg.info("Runtime Keybindings", {
    "h -> show the help text",
    "q -> quit the asciimation",
    "d -> toggle debug output",
    "j -> decrease speed by 5",
    "J -> decrease speed by 50",
    "k -> increase speed by 5",
    "K -> increase speed by 50",
    "space -> pause the animation",
  });
  pg.info("Exit Codes", {"0 -> normal", "1 -> error"});
  pg.info("Examples", {
    "asciimation -f './test' -d 'END' -t 80 -l 3",
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
  pg.set("loop,l", "0", "int", "set the animation to loop n times, if n is 0, it will loop infinitely");

  int status {pg.parse()};
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

int main(int argc, char *argv[])
{
  Parg pg {argc, argv};
  int pstatus {program_options(pg)};
  if (pstatus > 0) return 0;
  if (pstatus < 0) return 1;

  register_signals();

  try
  {
    OB::Asciimation am;
    am.set_debug(pg.get<bool>("debug"));
    am.set_loop(pg.get<size_t>("loop"));
    am.set_delay(pg.get<size_t>("time"));
    am.set_delim(pg.get("delim"));
    am.run(pg.get("file"));
  }
  catch (std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
