#pragma once
#include <cstdint>
#include <functional>
#include <string>
namespace seed {
using ClientHandler=std::function<std::string(const std::string&)>;
bool run_tcp_server(const std::string&,std::uint16_t,int,const ClientHandler&);
std::string tcp_request(const std::string&,std::uint16_t,const std::string&);
}
