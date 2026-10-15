#include "seed/protocol.hpp"
#include <algorithm>
namespace seed {
std::vector<std::string> split_tabs(const std::string&s){std::vector<std::string>o;size_t b=0;for(;;){auto p=s.find('\t',b);o.push_back(s.substr(b,p==std::string::npos?p:p-b));if(p==std::string::npos)break;b=p+1;}return o;}
std::string clean_field(std::string s){s.erase(std::remove_if(s.begin(),s.end(),[](char c){return c=='\r'||c=='\n'||c=='\t';}),s.end());return s;}
std::string response(bool ok,const std::string&m){return std::string(ok?"OK\t":"ERR\t")+clean_field(m);}
}
