#include "url_encoded_form_parser.hpp"
#include "../app/externs.hpp"
#include <ctype.h>
#include <stdlib.h>


http::UrlEncodedFormParser::UrlEncodedFormParser() {}
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

char http::UrlEncodedFormParser::strToBin(char *pString)
{
    char szBuffer[2];
    char ch;
    szBuffer[0] = CharToInt(pString[0]); //make the B to 11 -- 00001011
    szBuffer[1] = CharToInt(pString[1]); //make the 0 to 0 -- 00000000
    ch = (szBuffer[0] << 4) | szBuffer[1]; //to change the BO to 10110000
    return ch;
}

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
            strResult += StrToBin(szTemp);
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
/* end of external code use */

std::unordered_map<std::string, std::string> http::UrlEncodedFormParser::get_parameter(std::istream& input_stream, int content_leng) {
  std::unordered_map<std::string, std::string> parameters_map;
  if (content_leng != 0) {
    std::string content = this->input_stream_reader.read(input_stream, content_leng);
    std::istringstream ss(content);
    ss >> std::ws;
    if (content.length() == 0) {
      return parameters_map;
    }
    //_DEBUG("content: ", content);

    std::string url_decoded_query;
    url_decoded_query = urlDecode(content);
   //_DEBUG("url decoded query: ", url_decoded_query);
    std::vector<std::string> params_for_content = split(url_decoded_query, '&');

    for (auto key_value_string : params_for_content) {
      //_DEBUG("key_value_string: ", key_value_string);
      auto loc = key_value_string.find("=", 0);
      if (loc == std::string::npos || loc == 0) {
        _DEBUG("malformed url query string: ", key_value_string);
        continue;
        //soft failure: skip current parameter
        //throw "malformed url query string";

      } else {
        std::string key = key_value_string.substr(0, loc);
        //std::replace(key.begin(), key.end(), '+', ' ');
        std::string value = key_value_string.substr(loc+1, key_value_string.length());

        //std::replace(value.begin(), value.end(), '+', ' ');
        //_DEBUG("param key:",key,":",value);
        parameters_map.insert(std::make_pair(key, value));
      }
    }
  }
  return parameters_map;
}

std::vector<std::string> http::UrlEncodedFormParser::split(std::string str, char delimiter) {
  std::vector<std::string> result;
  std::istringstream ss(str);
  std::string tok;
  while(getline(ss, tok, delimiter)) {
    result.push_back(tok);
  }
  return result;
}
