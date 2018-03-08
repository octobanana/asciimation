//
// MIT License
//
// Copyright (c) 2018 Brett Robinson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <cstdlib>
#include <cassert>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <unistd.h>
#include <sys/ioctl.h>
#include <streambuf>
#include <iomanip>

namespace OB
{
class Parg
{
private:
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

  class Widthbuf: public std::streambuf
  {
  public:
    Widthbuf(size_t w, std::streambuf* s):
      indent_width {0},
      def_width {w},
      width {w},
      sbuf {s},
      count {0}
    {
    }

    ~Widthbuf()
    {
      overflow('\n');
    }

    void set_indent(size_t w)
    {
      if (w == 0)
      {
        prefix.clear();
        indent_width = 0;
        width = def_width;
      }
      else
      {
        indent_width += w;
        prefix = std::string(indent_width, ' ');
        width -= w;
      }
    }

  private:
    using string = std::basic_string<char_type>;

    int_type overflow(int_type c)
    {
      if (traits_type::eq_int_type(traits_type::eof(), c))
      {
        return traits_type::not_eof(c);
      }

      switch (c)
      {
        case '\n':
        case '\r':
        {
          buffer += c;
          count = 0;
          sbuf->sputn(prefix.c_str(), static_cast<std::streamsize>(indent_width));
          int_type rc = sbuf->sputn(buffer.c_str(), static_cast<std::streamsize>(buffer.size()));
          buffer.clear();
          return rc;
        }
        case '\a':
          return sbuf->sputc(c);
        case '\t':
          buffer += c;
          count += tab_width - count % tab_width;
          return c;
        default:
          if (count >= width)
          {
            size_t wpos = buffer.find_last_of(" \t");
            if (wpos != string::npos)
            {
              sbuf->sputn(prefix.c_str(), static_cast<std::streamsize>(indent_width));
              sbuf->sputn(buffer.c_str(), static_cast<std::streamsize>(wpos));
              count = buffer.size()-wpos-1;
              buffer = string(buffer, wpos+1);
            }
            else
            {
              sbuf->sputn(prefix.c_str(), static_cast<std::streamsize>(indent_width));
              sbuf->sputn(buffer.c_str(), static_cast<std::streamsize>(buffer.size()));
              buffer.clear();
              count = 0;
            }
            sbuf->sputc('\n');
          }
          buffer += c;
          ++count;
          return c;
      }
    }

    size_t indent_width;
    size_t def_width;
    size_t width;
    std::streambuf* sbuf;
    size_t count;
    size_t tab_count;
    static const int tab_width {4};
    std::string prefix;
    string buffer;
  }; // class widthbuf

  class Widthstream : public std::ostream
  {
  public:
    Widthstream(size_t width, std::ostream &os):
      std::ostream {&buf},
      buf {width, os.rdbuf()}
    {
    }

    Widthstream& indent(size_t w)
    {
      buf.set_indent(w);
      return *this;
    }

  private:
    Widthbuf buf;
  }; // class Widthstream

public:
  Parg(int _argc, char** _argv):
    argc_ {_argc}
  {
    argvf(_argv);
  }

  Parg& name(std::string const _name)
  {
    name_ = _name;
    return *this;
  }

  Parg& version(std::string const _version)
  {
    version_ = _version;
    return *this;
  }

  Parg& usage(std::string const _usage)
  {
    usage_ += "  " + name_ + " " + _usage + "\n";
    return *this;
  }

  Parg& description(std::string const _description)
  {
    description_ = _description;
    return *this;
  }

  Parg& info(std::string const _title, std::vector<std::string> const _text)
  {
    info_.emplace_back(info_pair{_title, _text});
    return *this;
  }

  Parg& author(std::string const _author)
  {
    author_ = _author;
    return *this;
  }

  std::string print_name() const
  {
    return name_;
  }

  std::string print_version() const
  {
    return name_ + " v" + version_ + "\n";
  }

  std::string print_usage() const
  {
    return usage_;
  }

  std::string print_help() const
  {
    auto width {Term::width()};
    std::stringstream out;
    if (! description_.empty())
    {
      out << name_ << ":" << "\n";
      std::stringstream ss;
      Widthstream wout {width, ss};
      wout.indent(2) << description_ << "\n";
      out << ss.str() << "\n";
    }

    if (! usage_.empty())
    {
      out << "Usage: " << "\n"
        << usage_ << "\n";
    }

    if (! modes_.empty())
    {
      out << "Flags: " << "\n"
        << modes_;
    }

    if (! options_.empty())
    {
      out << "\nOptions: " << "\n"
        << options_;
    }

    if (! info_.empty())
    {
      for (auto const& e : info_)
      {
        out << "\n" << e.title << ":" << "\n";

        for (auto const& t : e.text)
        {
          Widthstream wout {width, out};
          wout.indent(2) << t;
        }
      }
    }

    if (! author_.empty())
    {
      out << "\nAuthor: " << "\n";
      std::stringstream ss;
      Widthstream wout {width, ss};
      wout.indent(2) << author_ << "\n";
      out << ss.str();
    }

    return out.str();
  }

  int parse()
  {
    if (is_stdin_)
    {
      pipe_stdin();
    }
    status_ = parse_args(argc_, argv_);
    return status_;
  }

  void set(std::string _name, std::string _info)
  {
    // sets a flag
    std::string delim {","};
    if (_name.find(delim) != std::string::npos)
    {
      bool has_short {false};
      std::string _long;
      std::string _short;

      auto const names = delimit(_name, delim);
      assert(names.size() >= 1 && names.size() <= 2);
      _long = names.at(0);

      if (names.size() == 2) has_short = true;
      if (has_short)
      {
        // short name must be one char
        assert(names.at(1).size() == 1);
        _short = names.at(1);
      }

      flags_[_short] = _long;
      data_[_long].long_ = _long;
      data_[_long].short_ = _short;
      data_[_long].mode_ = true;
      data_[_long].value_ = "0";
      modes_.append("  -" + _short + ", --" + _long + "\n");
    }
    else
    {
      data_[_name].long_ = _name;
      data_[_name].mode_ = true;
      data_[_name].value_ = "0";
      modes_.append("  --" + _name + "\n");
    }

    std::stringstream out;
    Widthstream wout {Term::width(), out};
    wout.indent(4) << _info << "\n";
    modes_.append(out.str());
  }

  void set(std::string _name, std::string _default, std::string _arg, std::string _info)
  {
    // sets an option
    std::string delim {","};
    if (_name.find(delim) != std::string::npos)
    {
      bool has_short {false};
      std::string _long;
      std::string _short;

      auto const names = delimit(_name, delim);
      assert(names.size() >= 1 && names.size() <= 2);
      _long = names.at(0);

      if (names.size() == 2) has_short = true;
      if (has_short)
      {
        // short name must be one char
        assert(names.at(1).size() == 1);
        _short = names.at(1);
      }

      flags_[_short] = _long;
      data_[_long].long_ = _long;
      data_[_long].short_ = _short;
      data_[_long].mode_ = false;
      data_[_long].value_ = _default;
      options_.append("  -" + _short + ", --" + _long + "=<" + _arg + ">\n");
    }
    else
    {
      data_[_name].long_ = _name;
      data_[_name].mode_ = false;
      data_[_name].value_ = _default;
      options_.append("  --" + _name + "=<" + _arg + ">\n");
    }

    std::stringstream out;
    Widthstream wout {Term::width(), out};
    wout.indent(4) << _info << "\n";
    options_.append(out.str());
  }

  template<class T>
  T get(std::string const _key)
  {
    std::stringstream ss;
    ss << data_[_key].value_;
    T val;
    ss >> val;
    return val;
  }

  std::string get(std::string const _key)
  {
    return data_[_key].value_;
  }

  bool find(std::string const _key) const
  {
    // key must exist
    assert(data_.find(_key) != data_.end());
    return data_.at(_key).seen_;
  }

  Parg& set_pos(bool const _positional = true)
  {
    is_positional_ = _positional;
    return *this;
  }

  std::string get_pos() const
  {
    return positional_;
  }

  Parg& set_stdin(bool const _stdin = true)
  {
    is_stdin_ = _stdin;
    return *this;
  }

  std::string get_stdin() const
  {
    return stdin_;
  }

  int status() const
  {
    return status_;
  }

  std::string error() const
  {
    return error_;
  }

  struct Option
  {
    std::string short_;
    std::string long_;
    bool mode_;
    std::string value_;
    bool seen_ {false};
  };

  struct info_pair
  {
    std::string title;
    std::vector<std::string> text;
  };

private:
  int argc_;
  std::vector<std::string> argv_;
  std::string name_;
  std::string version_;
  std::string usage_;
  std::string description_;
  std::string modes_;
  std::string options_;
  int options_indent_;
  std::vector<info_pair> info_;
  std::string author_;
  std::map<std::string, Option> data_;
  std::map<std::string, std::string> flags_;
  bool is_positional_ {false};
  std::string positional_;
  std::string stdin_;
  bool is_stdin_;
  int status_ {0};
  std::string error_;

  void argvf(char** _argv)
  {
    // std::cout << "argc: " << argc_ << std::endl;
    // removes first arg
    if (argc_ < 1) return;
    for (int i = 1; i < argc_; ++i)
    {
      argv_.emplace_back(_argv[i]);
      // std::cout << "argv: " << i << " -> " << argv_.at(i) << std::endl;
    }
    --argc_;
  }

  int pipe_stdin()
  {
    if (! isatty(STDIN_FILENO))
    {
      stdin_.assign((std::istreambuf_iterator<char>(std::cin)),
        (std::istreambuf_iterator<char>()));
      return 0;
    }
    stdin_ = "";
    return -1;
  }

  std::vector<std::string> delimit(const std::string str, const std::string delim) const
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

  int parse_args(int _argc, std::vector<std::string> _argv)
  {
    if (_argc < 1) return 1;

    bool dashdash {false};

    // loop through arg vector
    for (int i = 0; i < _argc; ++i)
    {
      std::string const& tmp {_argv.at(static_cast<size_t>(i))};
      // std::cout << "ARG: " << i << " -> " << tmp << std::endl;

      if (dashdash)
      {
        if (positional_.size() == 0)
        {
          positional_.append(tmp);
        }
        else
        {
          positional_.append(" " + tmp);
        }
        continue;
      }

      if (tmp.size() > 1 && tmp.at(0) == '-' && tmp.at(1) != '-')
      {
        // short
        // std::cout << "SHORT: " << tmp << std::endl;

        std::string c {tmp.at(1)};
        if (flags_.find(c) != flags_.end() && !(data_.at(flags_.at(c)).mode_))
        {
          // short arg
          // std::cout << "SHORT: arg -> " << c << std::endl;

          if (data_.at(flags_.at(c)).seen_)
          {
            // error
            error_ = "flag '-" + c + "' has already been seen";
            return -1;
          }

          if (tmp.size() > 2 && tmp.at(2) != '=')
          {
            data_.at(flags_.at(c)).value_ = tmp.substr(2, tmp.size() - 1);
            data_.at(flags_.at(c)).seen_ = true;
          }
          else if (tmp.size() > 3 && tmp.at(2) == '=')
          {
            data_.at(flags_.at(c)).value_ = tmp.substr(3, tmp.size() - 1);
            data_.at(flags_.at(c)).seen_ = true;
          }
          else if (i + 1 < _argc)
          {
            data_.at(flags_.at(c)).value_ = _argv.at(static_cast<size_t>(i + 1));
            data_.at(flags_.at(c)).seen_ = true;
            ++i;
          }
          else
          {
            // error
            error_ = "flag '-" + c + "' requires an arg";
            return -1;
          }
        }
        else
        {
          // short mode
          for (size_t j = 1; j < tmp.size(); ++j)
          {
            std::string s {tmp.at(j)};

            if (flags_.find(s) != flags_.end() && data_.at(flags_.at(s)).mode_)
            {
              // std::cout << "SHORT: mode -> " << c << std::endl;

              if (data_.at(flags_.at(s)).seen_)
              {
                // error
                error_ = "flag '-" + s + "' has already been seen";
                return -1;
              }

              data_.at(flags_.at(s)).value_ = "1";
              data_.at(flags_.at(s)).seen_ = true;
            }
            else
            {
              // error
              error_ = "invalid flag '" + tmp + "'";
              return -1;
            }
          }
        }
      }
      else if (tmp.size() > 2 && tmp.at(0) == '-' && tmp.at(1) == '-')
      {
        // long || --
        // std::cout << "LONG: " << tmp << std::endl;
        std::string c {tmp.substr(2, tmp.size() - 1)};
        std::string a;

        auto const delim = c.find("=");
        if (delim != std::string::npos)
        {
          c = tmp.substr(2, delim);
          a = tmp.substr(3 + delim, tmp.size() - 1);
        }

        if (data_.find(c) != data_.end())
        {
          if (data_.at(c).seen_)
          {
            // error
            error_ = "option '--" + c + "' has already been seen";
            return -1;
          }

          if (data_.at(c).mode_ && a.size() == 0)
          {
            // std::cout << "LONG: mode -> " << c << std::endl;
            data_.at(c).value_ = "1";
            data_.at(c).seen_ = true;
          }
          else
          {
            // std::cout << "LONG: arg -> " << c << std::endl;
            if (a.size() > 0)
            {
              data_.at(c).value_ = a;
              data_.at(c).seen_ = true;
            }
            else if (i + 1 < _argc)
            {
              data_.at(c).value_ = _argv.at(static_cast<size_t>(i + 1));
              data_.at(c).seen_ = true;
              ++i;
            }
            else
            {
              // error
              error_ = "option '--" + c + "' requires an arg";
              return -1;
            }
          }
        }
        else
        {
          // error
          error_ = "invalid option '" + tmp + "'";
          return -1;
        }
      }
      else if (tmp.size() > 0 && is_positional_)
      {
        // positional
        // std::cout << "POS: " << tmp << std::endl;
        if (tmp == "--")
        {
          dashdash = true;
        }
        else
        {
          if (positional_.size() == 0)
          {
            positional_.append(tmp);
          }
          else
          {
            positional_.append(" " + tmp);
          }
        }
      }
      else
      {
        // error
        error_ = "no match for '" + tmp + "'";
        return -1;
      }
    }

    return 0;
  }
}; // class Parg
} // namespace OB
