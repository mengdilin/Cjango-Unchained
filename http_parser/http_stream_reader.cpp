#include "http_stream_reader.hpp"

bool http::HttpStreamReader::is_end_of_line(int next, std::istream& input_stream) {
  std::cout << "inside is_end_of_line" << std::endl;
  std::cout << "next: " << next  << std::endl;
  std::cout << "line_feed: " << line_feed  << std::endl;
  std::cout << "carriage_return: " << carraige_return << std::endl;
    return next == line_feed || (next == carraige_return && eat_next_char_if_line_feed(input_stream));
}
bool http::HttpStreamReader::eat_next_char_if_line_feed(std::istream& input_stream) {
  //std::cout << "peak: " << input_stream.peek() << std::endl;

      if (input_stream.peek() == line_feed && line_feed != -1) {
        std::cout << "peak == line_feed " << std::endl;
        std::cout << "peak: " << input_stream.peek() << std::endl;
        input_stream.get();
        std::cout << "here" << std::endl;
      }
      /*
      auto cur_pos = input_stream.tellg();
      if (input_stream.get() != line_feed) {
        input_stream.seekg (0, input_stream.beg);
        input_stream.seekg(cur_pos);
      }
      */
      std::cout << "return" << std::endl;
      return true;
}
std::string http::HttpStreamReader::get_next_line(std::istream& input_stream) {
  std::string line;
  int next;
  //next = input_stream.get();
  //std::cout << "next1: " << next << std::endl;
  //std::cout << "stream.good(): " << input_stream.good() << std::endl;
  while((next = input_stream.get(), input_stream.good()) && (next != -1) && (!is_end_of_line(next, input_stream))) {
    std::cout << "get_next_line: " << (char)next;
    std::cout << "Next: " << next << std::endl;
    line += (char)next;
  }
  std::cout << line << std::endl;
  std::cout << "return from get_next_line" << std::endl;

  return line;
}

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

std::string http::HttpStreamReader::read(std::istream& input_stream, int length) {
  char buffer[length];
  input_stream.get(buffer, length);
  std::string str;
  str = buffer;
  return buffer;
}

void http::HttpStreamReader::eat_white_space(std::istream& input_stream) {
  input_stream >> std::ws;
}
