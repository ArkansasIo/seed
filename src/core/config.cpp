#include "seed/config.hpp"
#include <fstream>
#include <stdexcept>
#include <unordered_map>
namespace seed {
Config Config::load(const std::string& path){
 Config c; std::ifstream in(path); if(!in)return c;
 std::string line,section; std::unordered_map<std::string,std::string> v;
 while(std::getline(in,line)){auto p=line.find_first_not_of(" \t\r");if(p==std::string::npos||line[p]=='#'||line[p]==';')continue;line=line.substr(p);if(line.front()=='['&&line.back()==']'){section=line.substr(1,line.size()-2);continue;}auto e=line.find('=');if(e!=std::string::npos)v[section+"."+line.substr(0,e)]=line.substr(e+1);}
 auto get=[&](std::string k,std::string d){auto i=v.find(k);return i==v.end()?d:i->second;};
 auto num=[&](std::string k,unsigned d){try{return static_cast<unsigned>(std::stoul(get(k,std::to_string(d))));}catch(...){return d;}};
 c.host=get("server.host",c.host);c.port=static_cast<std::uint16_t>(num("server.port",c.port));c.max_clients=static_cast<int>(num("server.max_clients",c.max_clients));
 c.db_host=get("database.host",c.db_host);c.db_port=static_cast<std::uint16_t>(num("database.port",c.db_port));c.db_name=get("database.name",c.db_name);c.db_user=get("database.user",c.db_user);c.db_password=get("database.password",c.db_password);c.connect_timeout_seconds=num("database.connect_timeout_seconds",c.connect_timeout_seconds);c.session_ttl_minutes=num("security.session_ttl_minutes",c.session_ttl_minutes);c.pbkdf2_iterations=num("security.pbkdf2_iterations",c.pbkdf2_iterations);
 if(!c.port||c.max_clients<1||c.pbkdf2_iterations<10000)throw std::runtime_error("Invalid configuration values");return c;
}}
