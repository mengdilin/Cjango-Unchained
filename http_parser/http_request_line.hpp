#include <unordered_map>
#include <string>

namespace http {
class HttpRequestLine {
  public:
    std::string action;
    std::string uri;
    std::unordered_map<std::string, std::string> parameters;
    std::string protocolVersion;

    HttpRequestLine(std::string action, std::string uri, std::string protocolVersion, std::unordered_map<std::string, std::string>& parameters);
};
}

