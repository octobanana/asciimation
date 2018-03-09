#include "asciimation.hh"

#include "color.hh"
namespace Cl = OB::Color;

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>


namespace OB
{

Asciimation::Asciimation()
{
  std::cout
  << Cl::c_hide
  << Cl::c_h
  << Cl::e_d
  << std::endl;
}

Asciimation::~Asciimation()
{
  std::cout
  << Cl::c_h
  << Cl::e_d
  << Cl::c_show
  << std::endl;
}

Asciimation& Asciimation::set_debug(bool debug)
{
  debug_ = debug;
  return *this;
}

Asciimation& Asciimation::set_delay(int delay)
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

  std::string content;
  content.assign((std::istreambuf_iterator<char>(ifile)),
    (std::istreambuf_iterator<char>()));
  ifile.close();

  size_t n {0};
  auto frames = delimit(content, delim_);
  for (auto const& e : frames)
  {
    ++n;
    std::cout
    << Cl::c_h
    << Cl::e_d;
    if (debug_)
    {
      std::cout << n << "\n\n";
    }
    else
    {
      std::cout << "\n";
    }
    std::cout
    << e
    << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_));
  }
}

std::vector<std::string> Asciimation::delimit(std::string const& str, std::string const delim)
{
  std::vector<std::string> vtok;
  auto start = 0U;
  auto end = str.find(delim);
  while (end != std::string::npos) {
    vtok.emplace_back(str.substr(start, end - start));
    start = end + delim.length();
    end = str.find(delim, start);
  }
  vtok.emplace_back(str.substr(start, end));
  return vtok;
}


} // namespace OB
