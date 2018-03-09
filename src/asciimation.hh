#ifndef OB_ASCIIMATION_HH
#define OB_ASCIIMATION_HH

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

namespace OB
{

class Asciimation
{
public:
  Asciimation();
  ~Asciimation();

  Asciimation& set_debug(bool debug);
  Asciimation& set_delay(int delay);
  Asciimation& set_delim(std::string delim);
  void run(std::string file_name);

private:
  bool debug_ {false};
  int delay_ {250};
  std::string delim_ {"END"};

  std::vector<std::string> delimit(std::string const& str, std::string const delim);

}; // class Asciimation

} // namespace OB

#endif // OB_ASCIIMATION_HH
