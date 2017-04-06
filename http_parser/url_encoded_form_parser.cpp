#include "url_encoded_form_parser.hpp"
#include "../app/externs.hpp"

http::UrlEncodedFormParser::UrlEncodedFormParser() {}
bool http::UrlEncodedFormParser::can_parse_content_type(std::string content_type) {
  return content_type == supported_content_type;
}

/*
 * using uri decode functionality found via codeguru.com
 * source: http://www.codeguru.com/cpp/cpp/algorithms/strings/article.php/c12759/URI-Encoding-and-Decoding.htm
 */
std::string http::UrlEncodedFormParser::uri_decode(const std::string & sSrc) {
 // Note from RFC1630: "Sequences which start with a percent
 // sign but are not followed by two hexadecimal characters
 // (0-9, A-F) are reserved for future extension"
 const unsigned char *pSrc = (const unsigned char *)sSrc.c_str();
 const int SRC_LEN = sSrc.length();
 const unsigned char * const SRC_END = pSrc + SRC_LEN;
 // last decodable '%'
 const unsigned char * const SRC_LAST_DEC = SRC_END - 2;

 char * const pStart = new char[SRC_LEN];
 char * pEnd = pStart;

 while (pSrc < SRC_LAST_DEC)
 {
    if (*pSrc == '%')
    {
       char dec1, dec2;
       if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
          && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
       {
          *pEnd++ = (dec1 << 4) + dec2;
          pSrc += 3;
          continue;
       }
    }
    *pEnd++ = *pSrc++;
 }

 // the last 2- chars
 while (pSrc < SRC_END)
    *pEnd++ = *pSrc++;

 std::string sResult(pStart, pEnd);
 delete [] pStart;
 return sResult;
}

/* end of using url_encoding use from source*/

std::unordered_map<std::string, std::string> http::UrlEncodedFormParser::get_parameter(std::istream& input_stream, int content_leng) {
  std::unordered_map<std::string, std::string> parameters_map;
  if (content_leng != 0) {
    std::string content = this->input_stream_reader.read(input_stream, content_leng);
    std::string url_decoded_query = uri_decode(content);
    _DEBUG("url decoded query: ", url_decoded_query);
    std::vector<std::string> params_for_content = split(url_decoded_query, '&');

    for (auto key_value_string : params_for_content) {
      auto loc = key_value_string.find("=", 0);
      if (loc == std::string::npos || loc == 0) {
        _DEBUG("malformed url query string: ", key_value_string);
        throw "malformed url query string";
      } else {
        std::string key = key_value_string.substr(0, loc);
        std::string value = key_value_string.substr(loc+1, key_value_string.length());
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
