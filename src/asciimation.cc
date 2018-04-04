#include "asciimation.hh"
#include "term.hh"

#include "ansi_escape_codes.hh"
namespace AEC = OB::ANSI_Escape_Codes;

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <regex>
#include <chrono>
#include <thread>

namespace OB
{

Asciimation::Asciimation()
{
}

Asciimation::~Asciimation()
{
}

Asciimation& Asciimation::set_debug(bool debug)
{
  debug_ = debug;
  return *this;
}

Asciimation& Asciimation::set_loop(size_t loop)
{
  loop_ = loop;
  return *this;
}

Asciimation& Asciimation::set_delay(size_t delay)
{
  delay_ = delay;
  return *this;
}

Asciimation& Asciimation::set_delim(std::string delim)
{
  delim_ = delim + "\n";
  return *this;
}

void Asciimation::run(std::string file_name)
{
  std::ifstream ifile {file_name};
  if (! ifile.is_open())
  {
    throw std::runtime_error("could not open input file");
  }

  // parse headers
  std::map<std::string, std::string> headers;
  std::string line;
  bool begin_found {false};
  while (std::getline(ifile, line))
  {
    if (line == begin_)
    {
      begin_found = true;
      break;
    }
    std::smatch m;
    if (std::regex_match(line, m, std::regex("^(.+?)\\s*:\\s*(.+)$")))
    {
      headers[std::string(m[1])] = std::string(m[2]);
    }
    else
    {
      throw std::runtime_error("invalid header syntax");
    }
  }

  if (! begin_found)
  {
    throw std::runtime_error("begin identifier not found");
  }

  check_window_size(headers);

  size_t offset = ifile.tellg();
  ifile.seekg(0, std::ios::end);
  size_t size = ifile.tellg();
  size -= offset;
  std::string content (size, ' ');
  ifile.seekg(offset);
  ifile.read(&content[0], size);
  ifile.close();

  auto frames = delimit(content, delim_);

  OB::Term term;

  main_loop(frames);
}

void Asciimation::main_loop(std::vector<std::string>& frames)
{
  std::cout << AEC::erase_screen << AEC::cursor_home;

  bool exit {false};
  size_t loop_count {loop_};
  size_t line_num {0};
  size_t frame_num {0};

  while (! exit && (loop_ == 0 || loop_count >= 1))
  {
    frame_num = 0;
    for (auto const& e : frames)
    {
      if (exit) break;

      line_num = 0;
      ++frame_num;
      std::stringstream out;
      if (debug_)
      {
        line_num += 2;
        if (loop_ == 0)
        {
          out << "L | ";
        }
        else
        {
          out << loop_count << " | ";
        }
        out
        << delay_ << " | "
        << frame_num << "/" << frames.size() << "\n\n";
      }

      auto line = delimit(e, "\n");
      if (line.size() > 1)
      {
        for (size_t i = 0; i < line.size() - 2; ++i)
        {
          out << line.at(i) << "\n";
          ++line_num;
        }
        out << line.at(line.size() - 2);
      }
      else if (line.size() == 1)
      {
        out << line.at(0);
        ++line_num;
      }
      std::cout << out.str() << std::flush;

      std::this_thread::sleep_for(std::chrono::milliseconds(delay_));

      // ----------------------------------------------------

      bool reset {false};
      int num_read {0};
      char c {0};

      while ((num_read = read(STDIN_FILENO, &c, 1)) == 1)
      {
        if (num_read == -1 && errno != EAGAIN)
        {
          throw std::runtime_error("read failed");
        }

        if (static_cast<int>(c) == (static_cast<int>('c') & 0x1f))
        {
          throw std::runtime_error("program interrupt");
        }
        else if (static_cast<int>(c) == (static_cast<int>('q') & 0x1f))
        {
          exit = true;
          break;
        }
        else if (static_cast<int>(c) == (static_cast<int>('d') & 0x1f))
        {
          reset = true;
          break;
        }
        else if (c == 'q')
        {
          exit = true;
          break;
        }
        else if (c == 'd')
        {
          debug_ = ! debug_;
        }
        else if (c == 'j')
        {
          if (delay_ > 5)
          {
            delay_ -= 5;
          }
        }
        else if (c == 'k')
        {
          if (delay_ < 1000)
          {
            delay_ += 5;
          }
        }
        else if (c == 'J')
        {
          if (delay_ > 50)
          {
            delay_ -= 50;
          }
        }
        else if (c == 'K')
        {
          if (delay_ < 1000)
          {
            delay_ += 50;
          }
        }
        else if (c == ' ')
        {
          size_t x = 0;
          size_t y = 0;
          Term::cursor_get(x, y);
          std::cout
          << AEC::cursor_set(0, 0)
          << AEC::wrap("||", {AEC::bold, AEC::reverse})
          << AEC::cursor_set(x, y);
          flush();
          while ((num_read = read(STDIN_FILENO, &c, 1)) != 1)
          {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
          }
        }
        else if (c == 'h' || c == '?')
        {
          clear_screen(line_num);
          std::cout
          << "Help:\n"
          << "h -> show the help text\n"
          << "q -> quit the asciimation\n"
          << "d -> toggle debug output\n"
          << "j -> decrease speed by 5\n"
          << "J -> decrease speed by 50\n"
          << "k -> increase speed by 5\n"
          << "K -> increase speed by 50\n"
          << "space -> pause the animation\n"
          << "Press any key to continue";
          flush();
          line_num += 9;
          while ((num_read = read(STDIN_FILENO, &c, 1)) != 1)
          {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
          }
        }
      }

      clear_screen(line_num);
      flush();

      if (reset) break;
    }
    --loop_count;
  }
}

void Asciimation::flush() const
{
  std::cout << std::flush;
}

void Asciimation::clear_screen(size_t num) const
{
  std::stringstream clr;
  clr << AEC::erase_line << AEC::cr;
  while (num > 0)
  {
    clr << AEC::cursor_up << AEC::erase_line;
    --num;
  }
  std::cout << clr.str();
}

size_t Asciimation::str_count(std::string const& str, std::string const& s) const
{
  size_t count {0};
  size_t pos {0};

  for (;;)
  {
    pos = str.find(s, pos);
    if (pos == std::string::npos) break;
    ++count;
    ++pos;
  }
  return count;
}

void Asciimation::check_window_size(std::map<std::string, std::string>& headers) const
{
  size_t twidth {0};
  size_t theight {0};
  OB::Term::size(twidth, theight);

  if (headers.find("x") != headers.end())
  {
    if (std::stoul(headers["x"]) > twidth)
    {
      throw std::runtime_error("screen width is too small, requires a minimum width of " + headers["x"]);
    }
  }

  if (headers.find("y") != headers.end())
  {
    if (std::stoul(headers["y"]) > theight)
    {
      throw std::runtime_error("screen height is too small, requires a minimum height of " + headers["y"]);
    }
  }
}

std::vector<std::string> Asciimation::delimit(std::string const& str, std::string const delim) const
{
  std::vector<std::string> vtok;
  size_t start {0};
  size_t end = str.find(delim);
  while (end != std::string::npos)
  {
    vtok.emplace_back(str.substr(start, end - start));
    start = end + delim.length();
    end = str.find(delim, start);
  }
  vtok.emplace_back(str.substr(start, end));
  return vtok;
}

} // namespace OB
