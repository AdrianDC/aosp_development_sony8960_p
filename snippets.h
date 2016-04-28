// AUTO GENERATED - ruby snippets.rb
#include <map>
#include <string>
auto cmp = [](const std::string& a, const std::string&b) {
   return a.compare(b) < 0; };
typedef std::map<std::string, std::string> SnipMap;
typedef std::map<std::string, SnipMap> SnipMapMap;
extern SnipMapMap snippets_cpp;
