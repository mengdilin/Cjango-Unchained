#ifndef HttpRequestHeader
#define HttpRequestHeader
#include <unordered_map>
class HttpRequest {
  public:
    string action; //method
    string url; //path
    string version; //scheme
    std::unordered_map<string, string> request_headers;
    std::unordered_map<string, string> parameters; //get request param

    HttpRequest(string action, string url, string version, std::unordered_map<string, string>& request_headers, std::unordered_map<string, string>& parameters) {
      this->action = action;
      this->url = url;
      this->version = version;
      this->request_headers = request_headers;
      this->parameters = parameters;
    }

    std::ostream & operator<<(std::ostream & Str, HttpRequest const & v) {
        string result = "action: " + action + "\n"
            + "url: " + url  + "\n"
            + "version: " + version + "\n";
        result += "request headers: \n";
        for (auto entry : request_headers) {
          result += entry.first + ":" + entry.second + "\n"''
        }
        result += "parameters: \n";
        for (auto entry : parameters) {
          result += entry.first + ":" + entry.second + "\n"''
        }
        return Str << result;
    }
}
#endif
