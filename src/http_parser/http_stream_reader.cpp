#include "http_stream_reader.hpp"
#include "../app/externs.hpp"

/** @file http_stream_reader.cpp
 * \ingroup http
 * @brief HttpStreamReader class implementation
 */

/**
** @brief a helper function to determine if we have reached the end of a http request line
** @param next: the next character of the stream casted to an integer
** @param input_stream is the input stream we are currently processing
** @return a boolean indicating whether we have reached the end of the line
*/
bool http::HttpStreamReader::is_end_of_line(int next, std::istream& input_stream) {
  return next == line_feed || (next == carraige_return && eat_next_char_if_line_feed(input_stream));
}

/**
** @brief a helper function that reads the next character if the current character is '\n'
** @param input_stream is the input stream we are currently processing
** @return a boolean indicating whether the next character is line feed
*/
bool http::HttpStreamReader::eat_next_char_if_line_feed(std::istream& input_stream) {

      if (input_stream.peek() == line_feed && line_feed != -1) {
        input_stream.get();
      }
      return true;
}


/**
** @brief given an input stream, returns the current line deliminted by '\r\n'
** @param input_stream is the input stream we are currently processing
** @return a string representing the next line in the input_stream
*/
std::string http::HttpStreamReader::get_next_line(std::istream& input_stream) {
  std::string line;
  int next;
  while((next = input_stream.get(), input_stream.good()) && (next != -1) && (!is_end_of_line(next, input_stream))) {
    line += (char)next;
  }
  return line;
}

/**
** @brief given an input stream, converts it to string
** @param input_stream is the input stream we are currently processing
** @return the entire input stream as a string
*/
std::string http::HttpStreamReader::to_string(std::istream& input_stream) {
  std::stringbuf buffer;
  input_stream.get(buffer);
  return buffer.str();
}

std::string http::HttpStreamReader::read_util(std::istream& input_stream, int character) {
  int next;
  std::string buffer;
  while ((next = input_stream.get(), input_stream.good()) && next != character) {
        buffer += (char)next;
    }
  return buffer;
}

/**
** @brief given an input stream and number of length bytes we want to read from the input stream,
** converts number of length bytes + 1 from input stream to a string
** @param input_stream is the input stream we are currently processing
** @param length: number of bytes we would like to read from the input stream
*/
std::string http::HttpStreamReader::read(std::istream& input_stream, int length) {
  char buffer[length+1];
  input_stream.get(buffer, length+1);
  std::string str;
  str = buffer;
  return buffer;
}

/**
** @brief removes white spaces from input stream
** @param input_stream is the input stream we are currently processing
*/
void http::HttpStreamReader::eat_white_space(std::istream& input_stream) {
  input_stream >> std::ws;
}
