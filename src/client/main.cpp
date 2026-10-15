#include "seed/config.hpp"
#include "seed/protocol.hpp"
#include "seed/socket.hpp"
#include "seed/ui.hpp"
#include <iostream>
int main(int argc,char**argv){try{seed::boot_loader();auto c=seed::Config::load(argc>1?argv[1]:"config/seed.ini");for(;;){std::cout<<"\n1 Register  2 Login  3 Generate demo world  4 Logout  0 Exit\n> ";std::string x;if(!std::getline(std::cin,x)||x=="0")break;std::string q;if(x=="1"||x=="2"){std::string u,p;std::cout<<"Username: ";std::getline(std::cin,u);std::cout<<"Password (visible in starter): ";std::getline(std::cin,p);q=(x=="1"?"REGISTER\t":"LOGIN\t")+seed::clean_field(u)+"\t"+seed::clean_field(p);}else if(x=="3"){std::string s,n;std::cout<<"Seed: ";std::getline(std::cin,s);std::cout<<"Planet count <=1000: ";std::getline(std::cin,n);q="WORLD\t"+s+"\t"+n;}else if(x=="4")q="LOGOUT";else{std::cout<<"Unknown choice\n";continue;}std::cout<<seed::tcp_request(c.host,c.port,q)<<"\n";}return 0;}catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}}
