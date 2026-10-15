#include "seed/socket.hpp"
#include <iostream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
using sock_t=SOCKET; constexpr sock_t bad=INVALID_SOCKET; void close_sock(sock_t s){closesocket(s);} struct Init{Init(){WSADATA w;WSAStartup(MAKEWORD(2,2),&w);}~Init(){WSACleanup();}};
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using sock_t=int; constexpr sock_t bad=-1; void close_sock(sock_t s){close(s);} struct Init{};
#endif
namespace seed {
static bool sendall(sock_t s,const std::string& x){size_t n=0;while(n<x.size()){
#ifdef _WIN32
int r=send(s,x.data()+n,int(x.size()-n),0);
#else
auto r=send(s,x.data()+n,x.size()-n,0);
#endif
if(r<=0)return false;n+=size_t(r);}return true;}
static std::string readline(sock_t s){std::string x;char c;while(x.size()<8192){
#ifdef _WIN32
int n=recv(s,&c,1,0);
#else
auto n=recv(s,&c,1,0);
#endif
if(n<=0||c=='\n')break;if(c!='\r')x+=c;}return x;}
bool run_tcp_server(const std::string& host,uint16_t port,int,const ClientHandler& h){Init init;sock_t s=socket(AF_INET,SOCK_STREAM,0);if(s==bad)return false;int yes=1;setsockopt(s,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&yes),sizeof(yes));sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(port);a.sin_addr.s_addr=host=="0.0.0.0"?INADDR_ANY:inet_addr(host.c_str());if(bind(s,reinterpret_cast<sockaddr*>(&a),sizeof(a))<0||listen(s,32)<0){close_sock(s);return false;}std::cout<<"Listening on "<<host<<":"<<port<<" (plain TCP; localhost only)\n";for(;;){sock_t c=accept(s,nullptr,nullptr);if(c==bad)continue;auto reply=h(readline(c));sendall(c,reply+"\n");close_sock(c);}close_sock(s);return true;}
std::string tcp_request(const std::string& host,uint16_t port,const std::string& req){Init init;sock_t s=socket(AF_INET,SOCK_STREAM,0);if(s==bad)return "ERR\tSocket error";sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(port);a.sin_addr.s_addr=inet_addr(host.c_str());if(connect(s,reinterpret_cast<sockaddr*>(&a),sizeof(a))<0){close_sock(s);return "ERR\tConnect failed";}sendall(s,req+"\n");auto r=readline(s);close_sock(s);return r;}
}
