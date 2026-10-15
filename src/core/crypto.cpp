#include "seed/crypto.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <array>
#include <climits>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>
namespace seed {
namespace {
std::vector<unsigned char> unhex(std::string_view s){std::vector<unsigned char> o;if(s.size()%2)return {};auto n=[](char c){if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;return -1;};for(size_t i=0;i<s.size();i+=2){int a=n(s[i]),b=n(s[i+1]);if(a<0||b<0)return {};o.push_back(static_cast<unsigned char>((a<<4)|b));}return o;}
std::string hex(const unsigned char*p,size_t n){std::ostringstream o;o<<std::hex<<std::setfill('0');for(size_t i=0;i<n;++i)o<<std::setw(2)<<unsigned(p[i]);return o.str();}
}
std::string random_salt_hex(size_t n){std::vector<unsigned char>s(n);if(RAND_bytes(s.data(),static_cast<int>(n))!=1)throw std::runtime_error("RAND_bytes failed");return hex(s.data(),s.size());}
std::string password_hash(std::string_view p,std::string_view sh,unsigned it){auto s=unhex(sh);if(s.empty()||it<10000||p.size()>INT_MAX)throw std::invalid_argument("Invalid PBKDF2 parameters");std::array<unsigned char,32>d{};if(PKCS5_PBKDF2_HMAC(p.data(),static_cast<int>(p.size()),s.data(),static_cast<int>(s.size()),static_cast<int>(it),EVP_sha256(),d.size(),d.data())!=1)throw std::runtime_error("PBKDF2 failed");return hex(d.data(),d.size());}
bool constant_time_equal(std::string_view a,std::string_view b)noexcept{size_t n=std::max(a.size(),b.size());unsigned d=unsigned(a.size()^b.size());for(size_t i=0;i<n;++i)d|=(i<a.size()?a[i]:0)^(i<b.size()?b[i]:0);return d==0;}
}
