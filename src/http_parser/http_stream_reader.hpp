#ifndef LIBHttpStreamReader
#define LIBHttpStreamReader
#include <iostream>
#include <fstream>
#include <sstream>      // std::stringbuf
#include <string>

/** @file http_stream_reader.hpp
 * \ingroup http
 * @brief HttpStreamReader class declaration
 */

namespace http {
class HttpStreamReader {
  public:
    static const int carraige_return = '\r';
    static const int line_feed = '\n';
  private:
    bool is_end_of_line(int next, std::istream& input_stream);
    bool eat_next_char_if_line_feed(std::istream& input_stream);
  public:
    std::string get_next_line(std::istream& input_stream);

    std::string to_string(std::istream& input_stream);

    std::string read_util(std::istream& input_stream, int character);

    std::string read(std::istream& input_stream, int length);

    void eat_white_space(std::istream& input_stream);

};
}
#endif
