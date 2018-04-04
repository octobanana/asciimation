#ifndef OB_ASCIIMATION_HH
#define OB_ASCIIMATION_HH

#include <string>
#include <vector>
#include <map>

namespace OB
{

class Asciimation
{
public:
  Asciimation();
  ~Asciimation();

  Asciimation& set_debug(bool debug);
  Asciimation& set_loop(size_t loop);
  Asciimation& set_delay(size_t delay);
  Asciimation& set_delim(std::string delim);
  void run(std::string file_name);

private:
  bool debug_ {false};
  size_t loop_ {false};
  size_t delay_ {250};
  std::string delim_ {"END\n"};
  std::string begin_ {"BEGIN"};

  void main_loop(std::vector<std::string>& frames);
  void flush() const;
  void clear_screen(size_t num) const;
  size_t str_count(std::string const& str, std::string const& s) const;
  void check_window_size(std::map<std::string, std::string>& headers) const;
  std::vector<std::string> delimit(std::string const& str, std::string const delim) const;

}; // class Asciimation

} // namespace OB

#endif // OB_ASCIIMATION_HH
