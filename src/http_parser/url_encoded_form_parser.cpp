#include "url_encoded_form_parser.hpp"
#include "../app/externs.hpp"
#include <ctype.h>
#include <stdlib.h>

/** @file url_encoded_form_parser.cpp
 * \ingroup http
 * @brief UrlEncodedFormParser class implementation
 */

http::UrlEncodedFormParser::UrlEncodedFormParser() {}

/**
 * @brief checks if cotent_type in question can be parsed by the current parser
 * @param content_type: a string representing the content type of a http request
 * @return: a boolean indicating whether content_type is supported
 */
bool http::UrlEncodedFormParser::can_parse_content_type(std::string content_type) {
  return content_type == supported_content_type;
}

/* UrlDecoder codes found online:
 * https://github.com/int2e/UrlEncoder/blob/master/Encoder.cpps
 *
 */
char http::UrlEncodedFormParser::charToInt(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return (char)(ch - '0');
    }
    if (ch >= 'a' && ch <= 'f')
    {
        return (char)(ch - 'a' + 10);
    }
    if (ch >= 'A' && ch <= 'F')
    {
        return (char)(ch - 'A' + 10);
    }
    return -1;
}

/* UrlDecoder codes found online:
 * https://github.com/int2e/UrlEncoder/blob/master/Encoder.cpps
 *
 */
char http::UrlEncodedFormParser::strToBin(char *pString)
{
    char szBuffer[2];
    char ch;
    szBuffer[0] = charToInt(pString[0]); //make the B to 11 -- 00001011
    szBuffer[1] = charToInt(pString[1]); //make the 0 to 0 -- 00000000
    ch = (szBuffer[0] << 4) | szBuffer[1]; //to change the BO to 10110000
    return ch;
}

/* UrlDecoder codes found online:
 * https://github.com/int2e/UrlEncoder/blob/master/Encoder.cpps
 *
 */
std::string http::UrlEncodedFormParser::urlDecode(const std::string &str)
{
    std::string strResult;
    char szTemp[2];
    size_t i = 0;
    size_t nLength = str.length();
    while (i < nLength)
    {
        if (str[i] == '%')
        {
            szTemp[0] = str[i + 1];
            szTemp[1] = str[i + 2];
            strResult += strToBin(szTemp);
            i = i + 3;
        }
        else if (str[i] == '+')
        {
            strResult += ' ';
            i++;
        }
        else
        {
            strResult += str[i];
            i++;
        }
    }
    return strResult;
}

/**
** @brief given current input stream which contains get or post params
** encoded as a single string, parses the stream into a map
** @param input_stream is the input stream we are currently processing which contains get or post params
** @param content_leng: length of the input_stream
** @return a map containing http request's parameters
*/
std::unordered_map<std::string, std::string> http::UrlEncodedFormParser::get_parameter(std::istream& input_stream, int content_leng) {
  std::unordered_map<std::string, std::string> parameters_map;
  if (content_leng != 0) {
    std::string content = this->input_stream_reader.read(input_stream, content_leng);
    if (content.at(content.length()-1) == '\r') {
      content = content.substr(0, content.length()-1);
    }
    std::istringstream ss(content);
    ss >> std::ws;
    if (content.length() == 0) {
      return parameters_map;
    }

    std::string url_decoded_query;
    url_decoded_query = urlDecode(content);
    std::vector<std::string> params_for_content = split(url_decoded_query, '&');

    for (auto key_value_string : params_for_content) {
      auto loc = key_value_string.find("=", 0);
      if (loc == std::string::npos || loc == 0) {
        _DEBUG("malformed url query string: ", key_value_string);
        continue;
        //soft failure: skip current parameter
        //throw "malformed url query string";

      } else {
        std::string key = key_value_string.substr(0, loc);
        std::string value = key_value_string.substr(loc+1, key_value_string.length());
        parameters_map.insert(std::make_pair(key, value));
      }
    }
  }
  return parameters_map;
}

/**
** @brief splits a string by a character delimiter and returns the result as a vector
** @param str: string to be split
** @param delimiter: a character delimiter to split the string by
** @return the split string in a vector
*/
std::vector<std::string> http::UrlEncodedFormParser::split(std::string str, char delimiter) {
  std::vector<std::string> result;
  std::istringstream ss(str);
  std::string tok;
  while(getline(ss, tok, delimiter)) {
    result.push_back(tok);
  }
  return result;
}
